#ifndef	RENDER_WARTER_H
#define RENDER_WATRER_H

#include"Framework.h"
#include"Data_struct.h"
#include"Shader.h"

void fft(complex * to_fft, int level, int k_points);
void fft_inverse(complex * to_fft, int level, int k_points);

class ocean_wave{
private:
	int k_num;	//k_point num from
	float Lx, Ly;
	my2Darray P_k;
	my2Darray_complex h_k;
	my2Darray omega;
	float time, gravity;
	float A, lambda_L, lambda_cutoff;	//this is due to wind
	vector2 wind_dir;
public:
	my2Darray_vector2 k;
	my2Darray_complex h_k_t;
	ocean_wave(int k_num_, float Lx_, float Ly_, float A_, float lambda_L_, vector2 & wind_dir_);
	void cal_h_k_t(float elapsed);
	int val_k_num();
};

class ocean_mesh{
private:
	float Lx, Lz;
public:
	int N;
	float max_height;
	my2Darray_vector3 mesh;
	my2Darray_vector2 e_x_t;
	vector<unsigned int> indexBuffer[5];	//3 levels of index buffer, 0 is the finest and 2 is the roughest
	ocean_mesh(int N_, float Lx_, float Lz_);
	void cal_ocean_mesh(ocean_wave & wave_set);
	void fft_cal_ocean_mesh(ocean_wave & wave_set);
	void fft_cal_slope_mesh(ocean_wave & wave_set);
};

struct quat_tree_node{
	int tree_level;
	vector2 bbox_node[4];	//bbox 
};

class quat_tree_ocean{
private:
	float Lx, Lz;	//size of an ocean_path
	vector2 cam_position, left_sight_norm, right_sight_norm, sight_dir, left_sight_dir, right_sight_dir;//its 2D
	float cam_height;
	vector2 relative_origin;
	int tree_level;	//for level ==1, root node bbox is the same as the ocean path
	//for level = 2, root node bbox contain 2^2=4 ocean path, level=3, root node bbox contain 2^3 = 8 pathes
	int sub_level;	//for sub_level, continuing divide ocean pathes,

	//-------------collide according to the frustum
	vector3 frustum_normal[6];
	float frustum_b[6];

public:
	vector<quat_tree_node> quat_tree; //this is actually a breadth priority tree
	vector<vector2> ocean_path_center;	//this is the ocean path to render
	vector<int> detail_level;	//0 is the most detailed one
	quat_tree_ocean(float Lx_, float Lz_, vector3 & cam_pos, float r_l_angle);
	quat_tree_ocean(float Lx_, float Lz_);
	void reset_cam_pos(vector3 & cam_pos, float r_l_angle);
	void build_root_tree();
	void build_tree_leaf();
	void render_how_many();
	void set_frustum(aiMatrix4x4 & pj_mv);
	bool bbox_collide_frustum(float x_min, float x_max, float z_min, float z_max);
	void build_by_frustum();
};

bool line_seg_arrow_collide(vector2 & v0, vector2 & v1, vector2 & o, vector2 & d);

class render_ocean_class{
private:
	std::auto_ptr<Shader> ocean_shader;
	vector3 frustum_normal[6];
	float frustum_b[6];
public:
	ocean_wave wave_set;
	ocean_mesh mesh_set;
	quat_tree_ocean space_tree_for_ocean;
	render_ocean_class(int sample_points, float mesh_Lx_, float mesh_Ly_, float Amplitude_, 
		float largest_wave_, vector2 & wind_dir);
	void initial_ocean_shader();
	void reset_cam_pos(vector3 & cam_pos, float r_l_angle);
	void set_frustum(aiMatrix4x4 & pj_mv);
	void render_ocean(float elapsed_time, vector3 & cam_pos_, float r_l_angle_);
	void render_ocean(float elapsed_time, vector3 & cam_pos_, aiMatrix4x4 & pj_mv_);

	bool bbox_collide_frustum(float x_min, float x_max, float z_min, float z_max);

};
#endif