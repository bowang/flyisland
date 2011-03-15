#include "Render_water.h"

ocean_wave::ocean_wave(int k_num_, float Lx_, float Ly_, float A_, float lambda_L_, vector2 & wind_dir_)
    :k_num(k_num_),k(k_num_,k_num_), Lx(Lx_), Ly(Ly_), A(A_),lambda_L(lambda_L_),lambda_cutoff(lambda_L_/1000.0),
    P_k(k_num_,k_num_),	h_k(k_num_, k_num_), h_k_t(k_num_, k_num_),
    omega(k_num_, k_num_),wind_dir(wind_dir_), time(0.0), gravity(1.0)
{
    wind_dir.normalize();
    float lambda_L_sqr = pow(lambda_L, 2);	//L^2
    float kx_interval = 2 * PI /Lx;	//kx sample interval
    float ky_interval = 2 * PI /Ly;	//ky sample interval
    //---------generate kx, ky mesh, eps_r, eps_i mesh, P_k mesh, 
    for(int i = 0; i < k_num; i++)
        for(int j=0; j< k_num; j++){
            //----------calcuate k_mesh
            k(i, j).x = kx_interval * float(i- floor(k_num/2.0));// );
            k(i, j).y = ky_interval * float(j- floor(k_num/2.0));// );
            float k_norm2 = k(i, j).norm2();
            if(k_norm2 < 0.000000001) k_norm2 = 0.000000001;
            //---------calculate omega ---------
            omega(i, j)= sqrt(gravity * k(i, j).norm1());	//this is the easiest w-k relationship
        //	vector2 k_dir(k(i,j).x, k(i,j).y);
        //	k_dir.normalize();
        //	float cross_product_abs = k_dir * wind_dir;
            //----------calculate P_k mesh
            float cross_product_abs = fabs(k(i, j)* wind_dir);
            P_k(i, j)= A * exp(-1.0/(k_norm2 * lambda_L_sqr)) / pow(k_norm2, 3) * pow(cross_product_abs, 2)
                *exp(-k_norm2*pow(lambda_cutoff,2));
            float P_k_sqrt = sqrt(P_k(i,j));	//squar root
            // P_k = A exp(-1/(k*L)^2)/k^4 * (k_dir * w_dir)^2 = A exp(-1/(k*L)^2)/k^2 * (k * w_dir)^2

            //----------h_k_mesh
            float eps_r = gaussrand();	float eps_i = gaussrand();
            float h_k_Re = 1/sqrt(2.0) * eps_r * P_k_sqrt;
            float h_k_Im = 1/sqrt(2.0) * eps_i * P_k_sqrt;			
            h_k(i, j).re = h_k_Re;	h_k(i, j).im = h_k_Im;
        }
}

void ocean_wave::cal_h_k_t(float elapsed){
    this->time += elapsed;
    int k_mid = (k_num-1)/2;
    for(int i=0; i<k_mid*2+1; i++)
        for(int j=0; j<k_mid*2; j++){
            float omega_t = omega(i, j)*time;	//wt = w * t
            complex exp_iwt(cos(omega_t), sin(omega_t));//exp(iwt) = coswt + isinwt
            complex exp_iwt_conj = exp_iwt.conj();	//exp(-iwt) = coswt - isinwt
            h_k_t(i, j) = h_k(i, j) * exp_iwt + //h(k)*exp(iwt)+[h(-k)]conj * exp(-wt)
                h_k(k_num - i -1, k_num-j -1).conj() * exp_iwt_conj;	//-k = h_k(k_num - i, k_num -j)			
        }
}

int ocean_wave::val_k_num(){
    return k_num;
}

ocean_mesh::ocean_mesh(int N_, float Lx_, float Lz_)
    :N(N_), Lx(Lx_), Lz(Lz_), mesh(N_, N_), e_x_t(N_, N_), max_height(0.0)
{	//generate mesh grid of the ocean
    float x_interval = (float) Lx/(N-1.0); float z_interval = (float) Lz/(N-1.0);
    for(int i=0; i<N; i++)
        for(int j=0; j<N; j++){
            this->mesh(i, j).x = float(i-floor(N/2.0))* x_interval;//-
            this->mesh(i, j).z = float(j-floor(N/2.0))* z_interval;//
            this->mesh(i, j).y = 0.0;
        }
    for(int i=0; i<5; i++)
        indexBuffer[i].clear();
    //initiate index buffer for render, 0 is the finest
    for(int i=0; i<N-1; i++)	//e.g. N=33, there are 32*32 squares
        for(int j=0; j<N-1; j++){
            indexBuffer[0].push_back(i*N+j); indexBuffer[0].push_back(i*N+j+1);
            indexBuffer[0].push_back((i+1)*N+j+1); indexBuffer[0].push_back(i*N+j);
            indexBuffer[0].push_back((i+1)*N+j+1); indexBuffer[0].push_back((i+1)*N+j);
        }
    for(int i=0; i<N-1; i=i+2)	//e.g. N=33, there are 16*16 squares
        for(int j=0; j<N-1; j=j+2){
            indexBuffer[1].push_back(i*N+j); indexBuffer[1].push_back(i*N+j+2);
            indexBuffer[1].push_back((i+2)*N+j+2); indexBuffer[1].push_back(i*N+j);
            indexBuffer[1].push_back((i+2)*N+j+2); indexBuffer[1].push_back((i+2)*N+j);
        }
    //this is the coarseast level of rendering
    for(int i=0; i<N-1; i=i+4)	//e.g. N=33, only to render 8*8 squares
        for(int j=0; j<N-1; j=j+4){	
            indexBuffer[2].push_back(i*N+j); indexBuffer[2].push_back(i*N+j+4);
            indexBuffer[2].push_back((i+4)*N+j+4); indexBuffer[2].push_back(i*N+j);
            indexBuffer[2].push_back((i+4)*N+j+4); indexBuffer[2].push_back((i+4)*N+j);
        }
    for(int i=0; i<N-1; i=i+8)	//e.g. N=33, only to render 4*4 squares
        for(int j=0; j<N-1; j=j+8){	
            indexBuffer[3].push_back(i*N+j); indexBuffer[3].push_back(i*N+j+8);
            indexBuffer[3].push_back((i+8)*N+j+8); indexBuffer[3].push_back(i*N+j);
            indexBuffer[3].push_back((i+8)*N+j+8); indexBuffer[3].push_back((i+8)*N+j);
        }
    for(int i=0; i<N-1; i=i+16)	//e.g. N=33, only to render 4*4 squares
        for(int j=0; j<N-1; j=j+16){	
            indexBuffer[4].push_back(i*N+j); indexBuffer[4].push_back(i*N+j+16);
            indexBuffer[4].push_back((i+16)*N+j+16); indexBuffer[4].push_back(i*N+j);
            indexBuffer[4].push_back((i+16)*N+j+16); indexBuffer[4].push_back((i+16)*N+j);
        }
}

//This is DFT method to render ocean mesh
void ocean_mesh::cal_ocean_mesh(ocean_wave & wave_set){
    int k_num = wave_set.val_k_num();	//get dimensions of wave set
    int k_mid = (k_num-1)>>1;
    int N_mid = (N-1)>>1;
//	int k_num = N;
    for(int i=0; i<N; i++)
        for(int j=0; j<N; j++){//calculate every mesh point
            complex h_x_t(0.0,0.0);
            vector2 x(mesh(i, j).x, mesh(i, j).z);	//pay attention! x is (x,z)
            for(int m=0; m<k_mid*2+1; m++)	//h_k_t(-k, t) = h_k_t(+k,t).conj, only need cal half total k points
                for(int n=0; n<k_mid; n++){
                //	float kx = wave_set.k(m, n) * x;	
                // kx = (m-N/2)/Lx * 2PI, x=(i-N/2)/Lx
                    float kx = float((m-k_mid)*(i-N_mid)+(n-k_mid)*(j-N_mid))/float(N-1) * PI;
                    complex exp_ikx(cos(kx), sin(kx));
                    h_x_t += wave_set.h_k_t(m, n) * exp_ikx;
                }//sum for every k point
            mesh(i, j).y = 2* h_x_t.re;	//get the hight, save half calculation
/*			if(fabs(h_x_t.re) > this->max_height)
                max_height = fabs(h_x_t.re);*/
        }
}

void ocean_mesh::fft_cal_ocean_mesh(ocean_wave & wave_set){
    int k_num = wave_set.val_k_num();	//get dimensions of wave set

    //----------- this is for half of all k points
    int k_mid = (k_num-1)>>1; int k_points = k_mid<<1;
    int k_level = 0, sample_num=1;
    while(sample_num<k_points){
        sample_num*=2; k_level++;
    }
    my2Darray_complex to_FFT_pos(k_points, k_points);	

    for(int i=0; i<k_points; i++)
        for(int j=0; j<k_points; j++){
            to_FFT_pos(i, j) = wave_set.h_k_t(i, j); 
        }
    complex * h_k_t = new complex[k_points];

    for(int i=0; i<k_points; i++){
        for(int j=0; j<k_points; j++){
            h_k_t[j] = to_FFT_pos(i, j);
            if((j%2) == 1)	h_k_t[j] = h_k_t[j]* -1.0;
        }
        fft(h_k_t, k_level, k_points);
        for(int j=0; j<k_points; j++){
            if((j%2) == 1)	h_k_t[j] = h_k_t[j]* -1.0;
            to_FFT_pos(i,j) = h_k_t[j];
        }
    }
    
//----------- ky* y >0
    for(int j=0; j<k_points; j++){
        for(int i=0; i<k_points; i++){
            h_k_t[i] = to_FFT_pos(i, j);
            if((i%2)==1) h_k_t[i]*= -1.0;
        }
        fft(h_k_t, k_level, k_points);
        for(int i=0; i<k_points; i++){
            if((i%2)==1) h_k_t[i]*=-1.0;
            to_FFT_pos(i, j) = h_k_t[i];
        }
    }
/*
//------------------ do not new space, use original space
    for(int i=0; i<k_points; i++){
        for(int j=0; j<k_points; j++){
            h_k_t[j] = wave_set.h_k_t(i, j);
            if((j%2) == 1)
                h_k_t[j] = h_k_t[j]* -1.0;
        }
        fft(h_k_t, k_level, k_points);
        for(int j=0; j<k_points; j++){
            wave_set.h_k_t(i, j) = h_k_t[j];
            if((j%2) == 1)
                wave_set.h_k_t(i, j) = wave_set.h_k_t(i, j)*-1.0;
        }
    }
//	int j=0;
//----------- ky* y >0
    for(int j=0; j<k_points; j++){
        for(int i=0; i<k_points; i++){
            h_k_t[i] = wave_set.h_k_t(i, j);
            if((i%2) == 1)
                h_k_t[i] = h_k_t[i]* -1.0;
        }
        fft(h_k_t, k_level, k_points);
        for(int i=0; i<k_points; i++){
            wave_set.h_k_t(i, j) = h_k_t[i];
            if((i%2) == 1)
                wave_set.h_k_t(i, j) = wave_set.h_k_t(i, j)*-1.0;
        }
    }*/

    for(int x_i=0; x_i<k_points; x_i++)	//x_i<N-1
        for(int x_j=0; x_j<k_points; x_j++){	//x_j<N-1
            mesh(x_i,x_j).y =to_FFT_pos(x_i, x_j).re;//	mesh(x_i, x_j).y=  .im ==0 wave_set.h_k_t(x_i,x_j).re;
        }
//-------------- for smooth edges, need to do this-------------
    for(int j=0; j<k_points; j++)
        mesh(k_points, j).y = mesh(0, j).y;
    for(int i=0; i<k_points;i++)
        mesh(i, k_points).y = mesh(i, 0).y;
    mesh(k_points, k_points).y = mesh(0,0).y;

    delete [] h_k_t;
}

void ocean_mesh::fft_cal_slope_mesh(ocean_wave & wave_set){
    int k_num = wave_set.val_k_num();	//get dimensions of wave set
    //----------- this is for half of all k points
    int k_mid = (k_num-1)>>1; int k_points = k_mid<<1;
    int k_level = 0, sample_num=1;
    while(sample_num<k_points){
        sample_num*=2; k_level++;
    }
    my2Darray_complex x_slope_to_FFT_pos(k_points, k_points);
    my2Darray_complex y_slope_to_FFT_pos(k_points, k_points);

    for(int i=0; i<k_points; i++)
        for(int j=0; j<k_points; j++){
            vector2 & k_pos = wave_set.k(i, j);//(k_mid+i, k_mid+j)
            x_slope_to_FFT_pos(i, j) = wave_set.h_k_t(i, j)* complex(0, k_pos.x);	//ikx*h(k, t)(k_mid+i, k_mid+j)
            y_slope_to_FFT_pos(i, j) = wave_set.h_k_t(i, j)* complex(0, k_pos.y);	//iky*h(k, t)(k_mid+i, k_mid+j)
        }

    complex * e_x_t_x_pos = new complex [k_points];
    complex * e_x_t_y_pos = new complex [k_points];

//------------- for kx(k_x)>0, ky(k_y)>0, kx*x,ky*y
    for(int i=0; i<k_points; i++){	//------convolution in x direction,
        for(int j=0; j<k_points; j++){ //ikx*h(k,t), iky*h(k,t), k has kx and ky, sum for kx
            e_x_t_x_pos[j] = x_slope_to_FFT_pos(i, j);	//convolute in x direction
            e_x_t_y_pos[j] = y_slope_to_FFT_pos(i, j);	//convolute in x direction
            if((j%2) == 1)
            {	e_x_t_x_pos[j] *= -1.0; e_x_t_y_pos[j] *= -1.0;}
        }
        fft(e_x_t_x_pos, k_level, k_points);	//fft exp(i*2pi/N * kn)
        fft(e_x_t_y_pos, k_level, k_points);	//fft exp(i*2pi/N * kn)
        for(int j=0; j<k_points; j++){
            x_slope_to_FFT_pos(i, j) = e_x_t_x_pos[j];
            y_slope_to_FFT_pos(i, j) = e_x_t_y_pos[j];
            if((j%2) == 1)
            {	x_slope_to_FFT_pos(i, j) *= -1.0; y_slope_to_FFT_pos(i, j) *= -1.0;}
        }
    }
    for(int j=0; j<k_points; j++){	//convolution in y direction, col number is given
        for(int i=0; i<k_points; i++){	//convolute for every element in one col
            e_x_t_x_pos[i] = x_slope_to_FFT_pos(i, j);	//convolute in y direction
            e_x_t_y_pos[i] = y_slope_to_FFT_pos(i, j);	//convolute in y direction
            if((i%2) == 1)
            {	e_x_t_x_pos[i] *=-1.0; e_x_t_y_pos[i] *= -1.0;}
        }
        fft(e_x_t_x_pos, k_level, k_points);	//fft exp(i*2pi/N * kn)
        fft(e_x_t_y_pos, k_level, k_points);
        for(int i=0; i<k_points; i++){	//result is in one col, i is iterated
            x_slope_to_FFT_pos(i, j) = e_x_t_x_pos[i];
            y_slope_to_FFT_pos(i, j) = e_x_t_y_pos[i];
            if((i%2) == 1)
            {	x_slope_to_FFT_pos(i, j) *= -1.0;	y_slope_to_FFT_pos(i, j) *= -1.0;	}
        }
    }

//------------- write back the result
    for(int i=0; i<k_points; i++)
        for(int j=0; j<k_points; j++){
            e_x_t(i, j).x = x_slope_to_FFT_pos(i, j).re;
            e_x_t(i, j).y = x_slope_to_FFT_pos(i, j).re;
        }
    for(int j=0; j<k_points; j++) e_x_t(k_points, j) = e_x_t(0, j);	//periodic boundary
    for(int i=0; i<k_points; i++) e_x_t(i, k_points) = e_x_t(i, 0);	//periodic boundary
    e_x_t(k_points, k_points) = e_x_t(0, 0);
    delete [] e_x_t_x_pos;	//delete [] e_x_t_x_neg;	
    delete [] e_x_t_y_pos;	//delete [] e_x_t_y_neg;
}

void fft(complex * h_k_t, int k_level, int k_points){
    int i2 = k_points >> 1; int  j = 0, k;
    for(int i=0;i<k_points-1;i++) {
        if (i < j) {
            complex temp= h_k_t[i];
            h_k_t[i] = h_k_t[j];
            h_k_t[j] = temp;
        }      
        k = i2;
        while (k <= j) {
             j -= k;   k >>= 1;
        }
        j += k;
    }
    /*	//--------- This is what I wrote, the result is the same
    int stride = 1, group_num = k_points>>1;	//example k_points is 8, group_num is 4
    int group_space = 2;
    for(int level=0; level<k_level; level++){
        for(int i_group=0; i_group<group_num; i_group++){
            for(int j_ele=0; j_ele<stride; j_ele++){
                float phi_wk=2*PI/float(2*stride)*float(j_ele);
                complex wk(cos(phi_wk), sin(phi_wk));		   
                complex u0=h_k_t[i_group*group_space+j_ele];
                complex u1=h_k_t[i_group*group_space+j_ele+stride];
                h_k_t[i_group*group_space+j_ele] = u0 + wk*u1;
                h_k_t[i_group*group_space+j_ele+stride] = u0 - wk*u1; 
            }
        }
        stride >>=1; group_num <<=1; group_space >>= 1;
    }*/
     for (int ldm=1; ldm<=k_level; ++ldm)
    {
        const int m = (1<<ldm);
        const int mh = (m>>1);
        const double phi = PI / (double)mh;
        for (int j=0; j<mh; ++j)
        {
            complex w(cos(phi * (double)j), sin(phi * (double) j ));
            for (int r=0; r<k_points; r+=m)
            {
                int i0 = r + j;
                int i1 = i0 + mh;

                complex u = h_k_t[i0];
                complex v = h_k_t[i1] * w;

                h_k_t[i0] = u + v;
                h_k_t[i1] = u - v;
            }
        }
    }
}

void fft_inverse(complex * h_k_t, int k_level, int k_points){
    int i2 = k_points >> 1; int  j = 0, k;
    for(int i=0;i<k_points-1;i++) {
        if (i < j) {
            complex temp= h_k_t[i];
            h_k_t[i] = h_k_t[j];
            h_k_t[j] = temp;
        }      
        k = i2;
        while (k <= j) {
             j -= k;   k >>= 1;
        }
        j += k;
    }
     for (int ldm=1; ldm<=k_level; ++ldm)
    {
        const int m = (1<<ldm);
        const int mh = (m>>1);
        const double phi = -PI / (double)mh;
        for (int j=0; j<mh; ++j)
        {
            complex w(cos(phi * (double)j), sin(phi * (double) j ));
            for (int r=0; r<k_points; r+=m)
            {
                int i0 = r + j;
                int i1 = i0 + mh;

                complex u = h_k_t[i0];
                complex v = h_k_t[i1] * w;

                h_k_t[i0] = u + v;
                h_k_t[i1] = u - v;
            }
        }
    }
}

quat_tree_ocean::quat_tree_ocean(float Lx_, float Ly_, vector3 & cam_pos_, float l_r_angle):Lx(Lx_), Lz(Ly_){
    ocean_path_center.clear();
    quat_tree.clear();
    //-------------set cam_position, relative origin, and left_sight_view and right_sight_view
    cam_position.x = cam_pos_.x; cam_position.y = cam_pos_.z;
//	relative_origin.x = ceil((cam_position.x-Lx/2.0)/20.0)*20.0+10.0; relative_origin.y = floor((cam_position.y+10.0)/20.0)*20.0-10.0;
    relative_origin.x = ceil((cam_position.x-Lx/2.0)/Lx)*Lx+Lx/2.0; relative_origin.y = floor((cam_position.y+Lz/2.0)/Lz)*Lz-Lz/2.0;
    float left_phi = l_r_angle - 30.0; float right_phi = l_r_angle + 30.0;
    left_sight_norm =vector2(-cos(left_phi/180.0*PI), -sin(left_phi/180.0 *PI)); 
    right_sight_norm =vector2(-cos(right_phi/180.0*PI), -sin(right_phi/180.0*PI));
    sight_dir = vector2(-sin(l_r_angle/180.0*PI), cos(l_r_angle/180.0 *PI));
    left_sight_dir = vector2(-sin(left_phi/180.0*PI), cos(left_phi/180.0 *PI));
    right_sight_dir = vector2(-sin(right_phi/180.0*PI), cos(right_phi/180.0 *PI));
    sub_level = 0; tree_level = 4;	//3: highest level patch is 64*64 small patches
    detail_level.clear();
}

quat_tree_ocean::quat_tree_ocean(float Lx_, float Ly_):Lx(Lx_), Lz(Ly_){
    ocean_path_center.clear();
    quat_tree.clear();
    detail_level.clear();
    sub_level = 0; tree_level = 4;	//3: highest level patch is 64*64 small patches
}

void quat_tree_ocean::reset_cam_pos(vector3 & cam_pos_, float l_r_angle){

    //---------- reset cam_position
    cam_position.x = cam_pos_.x; cam_position.y = cam_pos_.z;
    cam_height = cam_pos_.y;
    relative_origin.x = ceil((cam_position.x-Lx/2.0)/Lx)*Lx+Lx/2.0; relative_origin.y = floor((cam_position.y+Lz/2.0)/Lz)*Lz-Lz/2.0;
    float left_phi = l_r_angle - 30.0; float right_phi = l_r_angle + 30.0;
    left_sight_norm =vector2(-cos(left_phi/180.0*PI), -sin(left_phi/180.0 *PI)); 
    right_sight_norm =vector2(-cos(right_phi/180.0*PI), -sin(right_phi/180.0*PI));
    sight_dir = vector2(-sin(l_r_angle/180.0*PI), cos(l_r_angle/180.0 *PI));
    left_sight_dir = vector2(-sin(left_phi/180.0*PI), cos(left_phi/180.0 *PI));
    right_sight_dir = vector2(-sin(right_phi/180.0*PI), cos(right_phi/180.0 *PI));
};

void quat_tree_ocean::set_frustum(aiMatrix4x4 & pj_mv){
    float m0[4] = {pj_mv.a1, pj_mv.b1, pj_mv.c1, pj_mv.d1};
    float m1[4] = {pj_mv.a2, pj_mv.b2, pj_mv.c2, pj_mv.d2};
    float m2[4] = {pj_mv.a3, pj_mv.b3, pj_mv.c3, pj_mv.d3};
    float m3[4] = {pj_mv.a4, pj_mv.b4, pj_mv.c4, pj_mv.d4};
    
    //---------- left plane
    this->frustum_normal[0] = vector3(-(m3[0]+m0[0]),-(m3[1]+m0[1]),-(m3[2]+m0[2]));
    this->frustum_b[0] = -(m3[3]+m0[3]);	//-(m3+m0)
    //---------- right plane
    this->frustum_normal[1] = vector3(-(m3[0]-m0[0]),-(m3[1]-m0[1]),-(m3[2]-m0[2]));
    this->frustum_b[1] = -(m3[3]-m0[3]);	//-(m3-m0)
    //---------- bottom plane
    this->frustum_normal[2] = vector3(-(m3[0]+m1[0]),-(m3[1]+m1[1]),-(m3[2]+m1[2]));
    this->frustum_b[2] = -(m3[3]+m1[3]);	//-(m3+m1)
    //---------- top plane
    this->frustum_normal[3] = vector3(-(m3[0]-m1[0]),-(m3[1]-m1[1]),-(m3[2]-m1[2]));
    this->frustum_b[3] = -(m3[3]-m1[3]);	//-(m3-m1)
    //---------- near plane
    this->frustum_normal[4] = vector3(-(m3[0]+m2[0]),-(m3[1]+m2[1]),-(m3[2]+m2[2]));
    this->frustum_b[4] = -(m3[3]+m2[3]);	//-(m3-m1)
    //---------- far plane
    this->frustum_normal[5] = vector3(-(m3[0]-m2[0]),-(m3[1]-m2[1]),-(m3[2]-m2[2]));
    this->frustum_b[5] = -(m3[3]-m2[3]);	//-(m3-m1)
}

bool quat_tree_ocean::bbox_collide_frustum(float x_min, float x_max, float z_min, float z_max){
    bool intersecting;
    for(int k=0; k<6; k++){
        vector3 v_min, v_max;
        if(this->frustum_normal[k].x >= 0.0){
            v_min.x = x_min;
            v_max.x = x_max;
        }else{
            v_min.x = x_max;
            v_max.x = x_min;
        }
        v_min.y = v_max.y = 0.0;
        if((frustum_normal[k] * v_min + frustum_b[k])>0.0)
            return false;
        if((frustum_normal[k] * v_max + frustum_b[k])>=0.0)
            return true;
    }
    return true;	//totally inside
}

void quat_tree_ocean::build_by_frustum(){
    if(quat_tree.size()==0){cout<<"failed build root node tree"<<endl; return;}
    bool collide = false;
    vector<quat_tree_node>::iterator i;
    i = quat_tree.begin();
    while(i->tree_level>=0){
        collide = false; vector2 point_dir;
        float x_max =i->bbox_node[0].x, x_min = i->bbox_node[0].x, z_max = i->bbox_node[0].y,
            z_min = i->bbox_node[0].y;
        for(int j=1; j<4; j++){	//for every node
            if(i->bbox_node[j].x > x_max)
                x_max = i->bbox_node[j].x;
            if(i->bbox_node[j].x < x_min)
                x_min = i->bbox_node[j].x;
            if(i->bbox_node[j].y > z_max)
                z_max = i->bbox_node[j].y;
            if(i->bbox_node[j].y < x_min)
                z_min = i->bbox_node[j].y;
        }
        collide = this->bbox_collide_frustum(x_min, x_max, z_min, z_max);
        if(collide==true && i->tree_level>0){
            quat_tree_node sub_node[2][2];
            int sub_node_level = i->tree_level-1; int sub_step = 1<<sub_node_level;
            for(int m=0; m<2; m++)
                for(int n=0; n<2; n++){
                    sub_node[m][n].tree_level = sub_node_level;	//first need to set level
                    //---------- first calculate the origin
                    sub_node[m][n].bbox_node[0] = i->bbox_node[0] + vector2(-Lx*(float)(sub_step*m), Lz*(float)(sub_step*n));
                    //---------- other points are shifted from the origin
                    sub_node[m][n].bbox_node[1] = sub_node[m][n].bbox_node[0] + vector2(-Lx*(float)sub_step, 0.0);
                    sub_node[m][n].bbox_node[2] = sub_node[m][n].bbox_node[0] + vector2(-Lx*(float)sub_step, Lz*(float)sub_step);
                    sub_node[m][n].bbox_node[3] = sub_node[m][n].bbox_node[0] + vector2(0.0, Lz*(float)sub_step);
                    this->quat_tree.push_back(sub_node[m][n]);	//when pushed, i is changed, so need to set i again
                    i = quat_tree.begin();
                }
        }//if there is no collide of the big bbox, then there is no need to continue to split
        else if(collide==true){
            vector2 patch_center = i->bbox_node[0]+vector2(-Lx/2.0, Lz/2.0);
            this->ocean_path_center.push_back(patch_center);
            vector2 dist = cam_position - patch_center;
            float max_dist = dist.norm2() + pow(this->cam_height,2);	//dist^2
            float patch_half_diag = pow(Lx,2)/2.0;	//assume Lx = Ly, this is Lx^2/2
            if(max_dist < patch_half_diag*9.0)
                this->detail_level.push_back(0);
            else if(max_dist < patch_half_diag * 49.0)
                this->detail_level.push_back(1);
            else if(max_dist < patch_half_diag * 121.0)
                this->detail_level.push_back(2);
            else if(max_dist < patch_half_diag * 196.0)
                this->detail_level.push_back(3);
            else
                this->detail_level.push_back(4);
        }
        quat_tree.erase(i);	//delete the first node, which has been traversed
        if(quat_tree.size()==0)
            break;
        i = quat_tree.begin();	//put it to the head
    }
//	ocean_path_center.push_back(vector2(0.0, 20.0));
}

void quat_tree_ocean::build_root_tree(){
    //----------clear all vectors 
    ocean_path_center.clear();
    quat_tree.clear();
    detail_level.clear();

    int step = 1<<tree_level;
    //------------ build root node
    quat_tree_node temp; temp.tree_level = this->tree_level;
    temp.bbox_node[0] = relative_origin;
    temp.bbox_node[1].x = relative_origin.x - Lx*(float)step; temp.bbox_node[1].y = relative_origin.y;
    temp.bbox_node[2].x = relative_origin.x - Lx*(float)step; temp.bbox_node[2].y = relative_origin.y + Lz*(float)step;
    temp.bbox_node[3].x = relative_origin.x; temp.bbox_node[3].y = relative_origin.y + Lz*(float)step;
    this->quat_tree.push_back(temp);
    //--------------- counter close wise, another node 
    quat_tree_node temp1; temp1.tree_level = this->tree_level;
    for(int i=0; i<4; i++)
        temp1.bbox_node[i] = temp.bbox_node[i] + vector2(Lx * (float)step,0.0);	//shift left
    quat_tree.push_back(temp1);
    //--------------- counter close wise, another node, temp2
    quat_tree_node temp2; temp2.tree_level = this->tree_level;
    for(int i=0; i<4; i++)
        temp2.bbox_node[i] = temp.bbox_node[i] + vector2((float)step*Lx,-(float)step*Lz);	//shift left
    quat_tree.push_back(temp2);
    //--------------- counter close wise, final node, temp3
    quat_tree_node temp3; temp3.tree_level = this->tree_level;
    for(int i=0; i<4; i++)
        temp3.bbox_node[i] = temp.bbox_node[i] - vector2(0,(float)step*Lz);	//shift left
    quat_tree.push_back(temp3);
    //---------------- root node has been build

}

void quat_tree_ocean::build_tree_leaf(){
    if(quat_tree.size()==0){cout<<"failed build root node tree"<<endl; return;}
    bool collide = false;
    vector<quat_tree_node>::iterator i;
    i = quat_tree.begin();
    while(i->tree_level>=0){
        collide = false; vector2 point_dir;
        for(int j=0; j<4; j++){	//for every node
            point_dir = i->bbox_node[j]-cam_position;
            if((point_dir*left_sight_norm)>-0.01 && (point_dir*right_sight_norm)<0.01){
                collide= true; 	break;
            }//detect a collide, break, need to split this bbox
        }
        if(collide == false){	//do the test for cross for every edge
            for(int k=0; k<4; k++)
                if(line_seg_arrow_collide(i->bbox_node[k], i->bbox_node[(k+1)%4], cam_position, left_sight_dir))
                {collide = true; break;}
            if(collide == false)
                for(int k=0; k<4; k++)
                    if(line_seg_arrow_collide(i->bbox_node[k], i->bbox_node[(k+1)%4], cam_position, right_sight_dir))
                    {collide = true; break;}
        }
        if(collide==true && i->tree_level>0){
            quat_tree_node sub_node[2][2];
            int sub_node_level = i->tree_level-1; int sub_step = 1<<sub_node_level;
            for(int m=0; m<2; m++)
                for(int n=0; n<2; n++){
                    sub_node[m][n].tree_level = sub_node_level;	//first need to set level
                    //---------- first calculate the origin
                    sub_node[m][n].bbox_node[0] = i->bbox_node[0] + vector2(-Lx*(float)(sub_step*m), Lz*(float)(sub_step*n));
                    //---------- other points are shifted from the origin
                    sub_node[m][n].bbox_node[1] = sub_node[m][n].bbox_node[0] + vector2(-Lx*(float)sub_step, 0.0);
                    sub_node[m][n].bbox_node[2] = sub_node[m][n].bbox_node[0] + vector2(-Lx*(float)sub_step, Lz*(float)sub_step);
                    sub_node[m][n].bbox_node[3] = sub_node[m][n].bbox_node[0] + vector2(0.0, Lz*(float)sub_step);
                    this->quat_tree.push_back(sub_node[m][n]);	//when pushed, i is changed, so need to set i again
                    i = quat_tree.begin();
                }
        }//if there is no collide of the big bbox, then there is no need to continue to split
        else if(collide==true){
            vector2 patch_center = i->bbox_node[0]+vector2(-Lx/2.0, Lz/2.0);
        //	this->ocean_path_center.push_back(i->bbox_node[0] + vector2(-Lx/2.0, Lz/2.0));//copy it to the vector to be rendered
            this->ocean_path_center.push_back(patch_center);
            vector2 dist = cam_position - patch_center;
            float max_dist = dist.norm2();	//dist^2
            float patch_half_diag = pow(Lx,2)/2.0;	//assume Lx = Ly, this is Lx^2/2
            if(max_dist < patch_half_diag*9.0)
                this->detail_level.push_back(0);
            else if(max_dist < patch_half_diag * 49.0)
                this->detail_level.push_back(1);
            else if(max_dist < patch_half_diag * 121.0)
                this->detail_level.push_back(2);
            else
                this->detail_level.push_back(3);
        }
        quat_tree.erase(i);	//delete the first node, which has been traversed
        if(quat_tree.size()==0)
            break;
        i = quat_tree.begin();	//put it to the head
    }
//	ocean_path_center.push_back(vector2(0.0, 20.0));
}

void quat_tree_ocean::render_how_many(){
    this->build_root_tree();
    this->build_tree_leaf();
}

bool line_seg_arrow_collide(vector2 & v0, vector2 & v1, vector2 & o, vector2 & d){
    vector2 e = v0 - v1;
    vector2 p = d*-1.0;
    float a = e.cross_product(p);
    if(a>-0.0000000001 && a<0.0000000001)
        return false;	//not collide
    float f = 1/a;
    vector2 s = o-v1;
    float u=f*s.cross_product(p);
    if(u<0.0 || u>1.0)
        return false;
    float t=f*e.cross_product(s);
    if(t<0.0)
        return false;
    return true;
}

render_ocean_class::render_ocean_class(int sample_points, float mesh_Lx_, float mesh_Ly_, float Amplitude_, 
        float largest_wave_, vector2 & wind_dir)
        :wave_set(sample_points, mesh_Lx_, mesh_Ly_, Amplitude_, largest_wave_, wind_dir),
        mesh_set(sample_points, mesh_Lx_, mesh_Ly_),
        space_tree_for_ocean(mesh_Lx_, mesh_Ly_)
{	// first initiate three object
    
}

void render_ocean_class::initial_ocean_shader(){
    ocean_shader.reset(new Shader("shaders/ocean"));
    if (!ocean_shader->loaded()) {
        std::cerr << "Ocean shader failed to load" << std::endl;
        std::cerr << ocean_shader->errors() << std::endl;
        exit(-1);
    }
}

void render_ocean_class::reset_cam_pos(vector3 & cam_pos, float r_l_angle){
    this->space_tree_for_ocean.reset_cam_pos(cam_pos, r_l_angle);
}

void render_ocean_class::set_frustum(aiMatrix4x4 & pj_mv){
    float m0[4] = {pj_mv.a1, pj_mv.b1, pj_mv.c1, pj_mv.d1};
    float m1[4] = {pj_mv.a2, pj_mv.b2, pj_mv.c2, pj_mv.d2};
    float m2[4] = {pj_mv.a3, pj_mv.b3, pj_mv.c3, pj_mv.d3};
    float m3[4] = {pj_mv.a4, pj_mv.b4, pj_mv.c4, pj_mv.d4};
    
    //---------- left plane
    this->frustum_normal[0] = vector3(-(m3[0]+m0[0]),-(m3[1]+m0[1]),-(m3[2]+m0[2]));
    this->frustum_b[0] = -(m3[3]+m0[3]);	//-(m3+m0)
    //---------- right plane
    this->frustum_normal[1] = vector3(-(m3[0]-m0[0]),-(m3[1]-m0[1]),-(m3[2]-m0[2]));
    this->frustum_b[1] = -(m3[3]-m0[3]);	//-(m3-m0)
    //---------- bottom plane
    this->frustum_normal[2] = vector3(-(m3[0]+m1[0]),-(m3[1]+m1[1]),-(m3[2]+m1[2]));
    this->frustum_b[2] = -(m3[3]+m1[3]);	//-(m3+m1)
    //---------- top plane
    this->frustum_normal[3] = vector3(-(m3[0]-m1[0]),-(m3[1]-m1[1]),-(m3[2]-m1[2]));
    this->frustum_b[3] = -(m3[3]-m1[3]);	//-(m3-m1)
    //---------- near plane
    this->frustum_normal[4] = vector3(-(m3[0]+m2[0]),-(m3[1]+m2[1]),-(m3[2]+m2[2]));
    this->frustum_b[4] = -(m3[3]+m2[3]);	//-(m3-m1)
    //---------- far plane
    this->frustum_normal[5] = vector3(-(m3[0]-m2[0]),-(m3[1]-m2[1]),-(m3[2]-m2[2]));
    this->frustum_b[5] = -(m3[3]-m2[3]);	//-(m3-m1)
}

bool render_ocean_class::bbox_collide_frustum(float x_min, float x_max, float z_min, float z_max){
    bool intersecting;
    for(int k=0; k<6; k++){
        vector3 v_min, v_max;
        if(this->frustum_normal[k].x >= 0.0){
            v_min.x = x_min;
            v_max.x = x_max;
        }else{
            v_min.x = x_max;
            v_max.x = x_min;
        }
        v_min.y = v_max.y = 0.0;
        if((frustum_normal[k] * v_min + frustum_b[k])>0.0)
            return false;
        if((frustum_normal[k] * v_max + frustum_b[k])>=0.0)
            return true;
    }
    return true;	//totally inside
}

void render_ocean_class::render_ocean(float elapsed_, vector3 & cam_pos_, float r_l_angle_){
    //------------- I assume that camera is set up 
    glUseProgram(this->ocean_shader->programID());
    this->wave_set.cal_h_k_t(elapsed_);
    this->space_tree_for_ocean.reset_cam_pos(cam_pos_, r_l_angle_);
    this->space_tree_for_ocean.render_how_many();
    this->mesh_set.fft_cal_ocean_mesh(this->wave_set);
    this->mesh_set.fft_cal_slope_mesh(this->wave_set);

    //----------now begin to render the mesh
    GLint positionIn = glGetAttribLocation(ocean_shader->programID(), "positionIn");
    glEnableVertexAttribArray(positionIn);
    glVertexAttribPointer(positionIn, 3, GL_FLOAT, 0, sizeof(vector3), &this->mesh_set.mesh.data[0]);

    GLint e_x_t_in = glGetAttribLocation(ocean_shader->programID(), "e_x_t");
    glEnableVertexAttribArray(e_x_t_in);
    glVertexAttribPointer(e_x_t_in, 3, GL_FLOAT, 0, sizeof(vector2), &this->mesh_set.e_x_t.data[0]);

    glMatrixMode(GL_MODELVIEW);
    for(int i=0; i<this->space_tree_for_ocean.ocean_path_center.size();i++){
        glPushMatrix();
        vector2 & center = this->space_tree_for_ocean.ocean_path_center[i];
        glTranslatef(center.x, 0.0, center.y);
        int level = this->space_tree_for_ocean.detail_level[i];
        glDrawElements(GL_TRIANGLES, this->mesh_set.indexBuffer[level].size(), 
            GL_UNSIGNED_INT, &this->mesh_set.indexBuffer[level][0]);
        glPopMatrix();
    }
//---------- disable these, incase shader conflict with each other
    glDisableVertexAttribArray(positionIn);
    glDisableVertexAttribArray(e_x_t_in);

}


void render_ocean_class::render_ocean(float elapsed_, vector3 & cam_pos_, aiMatrix4x4 &pj_mv_){
    //------------- I assume that camera is set up 
    glUseProgram(this->ocean_shader->programID());
    this->wave_set.cal_h_k_t(elapsed_);
    this->space_tree_for_ocean.reset_cam_pos(cam_pos_, 0.0);
    this->set_frustum(pj_mv_);

    this->space_tree_for_ocean.build_root_tree();
    this->space_tree_for_ocean.build_by_frustum();

    this->mesh_set.fft_cal_ocean_mesh(this->wave_set);
    this->mesh_set.fft_cal_slope_mesh(this->wave_set);

    //----------now begin to render the mesh
    GLint positionIn = glGetAttribLocation(ocean_shader->programID(), "positionIn");
    glEnableVertexAttribArray(positionIn);
    glVertexAttribPointer(positionIn, 3, GL_FLOAT, 0, sizeof(vector3), &this->mesh_set.mesh.data[0]);

    GLint e_x_t_in = glGetAttribLocation(ocean_shader->programID(), "e_x_t");
    glEnableVertexAttribArray(e_x_t_in);
    glVertexAttribPointer(e_x_t_in, 3, GL_FLOAT, 0, sizeof(vector2), &this->mesh_set.e_x_t.data[0]);

    glMatrixMode(GL_MODELVIEW);
    for(int i=0; i<this->space_tree_for_ocean.ocean_path_center.size();i++){
        glPushMatrix();
        vector2 & center = this->space_tree_for_ocean.ocean_path_center[i];
        glTranslatef(center.x, 0.0, center.y);
        int level = this->space_tree_for_ocean.detail_level[i];
        glDrawElements(GL_TRIANGLES, this->mesh_set.indexBuffer[level].size(), 
            GL_UNSIGNED_INT, &this->mesh_set.indexBuffer[level][0]);
        glPopMatrix();
    }
//---------- disable these, incase shader conflict with each other
    glDisableVertexAttribArray(positionIn);
    glDisableVertexAttribArray(e_x_t_in);

}