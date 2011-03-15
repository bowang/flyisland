#include"Data_struct.h"

double gaussrand()
	{
	    static double V1, V2, S;
	    static int phase = 0;
	    double X;

	    if(phase == 0) {
		do {
		    double U1 = (double)rand() / RAND_MAX;
		    double U2 = (double)rand() / RAND_MAX;

		    V1 = 2 * U1 - 1;
		    V2 = 2 * U2 - 1;
		    S = V1 * V1 + V2 * V2;
		} while(S >= 1 || S == 0);

		X = V1 * sqrt(-2 * log(S) / S);
	    } else
		X = V2 * sqrt(-2 * log(S) / S);

	    phase = 1 - phase;

	    return X;
	}

vector3::vector3(){
	x = 0.0; y=0.0; z=0.0;
}
vector3::vector3(float x_, float y_, float z_){
	x = x_; y= y_; z=z_;
}

vector3 vector3::operator-(vector3 & b){
	return vector3(x-b.x, y-b.y, z-b.z);
}

vector3 vector3::operator+(vector3 & b){
	return vector3(x+b.x, y+b.y, z+b.z);
}

float vector3::operator*(vector3 &b){
	return (x*b.x + y*b.y + z*b.z);
}

vector3 vector3::operator*(float a){
	return vector3(x*a, y*a, z*a);
}

void vector3::normalize(){
	float norm = sqrt(pow(x,2) + pow(y,2) + pow(z,2));
	if(norm < 1e-20)
		norm = 1e-20;
	x /= norm; y/= norm; z/= norm;
}

vector3 vector3::cross_product(vector3 & b){
	float x_ = y*b.z - z*b.y;
	float y_ = -(x*b.z - z*b.x);
	float z_ = x*b.y - y*b.x;
	return vector3(x_, y_, z_);
}

float & vector3::operator[](int i){
	if(i==0)	return x;
	else if(i==1)	return y;
	else if(i==2)	return z;
	else{
		cout<<"vector3[] overflow, clamped to x"<<endl;
		return x;
	}
}

vector2::vector2(){
	x = 0.0; y=0.0; 
}
vector2::vector2(float x_, float y_){
	x = x_; y= y_; 
}

vector2 vector2::operator-(vector2 & b){
	return vector2(x-b.x, y-b.y);
}

vector2 vector2::operator+(vector2 & b){
	return vector2(x+b.x, y+b.y);
}

float vector2::operator*(vector2 &b){
	return (x*b.x + y*b.y);
}

vector2 vector2::operator*(float a){
	return vector2(x*a, y*a);
}

void vector2::normalize(){
	float norm = sqrt(pow(x,2) + pow(y,2));
	if(norm < 0.0000000000001)
		norm = 0.0000000000001;
	x /= norm; y/= norm;
}

float vector2::norm2(){
	return pow(x,2) + pow(y,2);
}

float vector2::norm1(){
	return sqrt(pow(x,2) + pow(y,2));
}

float vector2::cross_product(vector2 & b){
	return x*b.y-y*b.x;
}

float & vector2::operator[](int i){
	if(i==0)	return x;
	else if(i==1)	return y;
	else{
		cout<<"vector2[] overflow, clamped to x"<<endl;
		return x;
	}
}

my2Darray_vector3::my2Darray_vector3(int row_, int col_): row(row_), col(col_){
	data.clear();
	for(int i=0; i<row; i++)
		for(int j=0; j<col; j++)
			data.push_back(vector3(0.0, 0.0, 0.0));
}

vector3 & my2Darray_vector3::operator()(int row_, int col_){
	return data.at(row_ * col + col_);
}

my2Darray::my2Darray(int row_, int col_): row(row_), col(col_){
	data.clear();
	for(int i=0; i<row; i++)
		for(int j=0; j<col; j++)
			data.push_back(0);
}

float & my2Darray::operator()(int row_, int col_){
	return data.at(row_*this->col + col_);
}

my2Darray_vector2::my2Darray_vector2(int row_, int col_): row(row_), col(col_){
	data.clear();
	for(int i=0; i<row; i++)
		for(int j=0; j<col; j++)
			data.push_back(vector2());
}

vector2 & my2Darray_vector2::operator()(int row_, int col_){
	return this->data.at(row_*this->col + col_);
}

//-------------- define computation of complex number --------------
complex::complex(){
	re = 0.0; im = 0.0;
}

complex::complex(float re_, float im_){
	re = re_; im = im_;
}
/*
float & complex::Re(){
	return this->re;
}
float & complex::Im(){
	return this->im;
}*/

complex complex::operator+(complex & c){
	return complex(re+c.re, im+c.im);
}

complex complex::operator-(complex & c){
	return complex(re-c.re, im-c.im);
}

complex complex::operator*(complex & c){
	return complex(re*c.re-im*c.im, re*c.im+ im*c.re);
}

complex complex::operator*(float a){
	return complex(re*a, im*a);
}

complex complex::conj(){
	return complex(re, -im);
}

complex & complex::operator=(complex & c){
	re = c.re; im = c.im;
	return *this;
}

complex & complex::operator+=(complex & c){
	re += c.re; im += c.im;
	return * this;
}

complex & complex::operator*=(float a){
	re *= a; im *= a;
	return * this;
}

//------------ define a 2D array to save complex num
my2Darray_complex::my2Darray_complex(int row_, int col_){
	row = row_; col = col_;
	data.clear();
	for(int i=0; i<row; i++)
		for(int j=0; j<col; j++)
			data.push_back(complex());
}

complex & my2Darray_complex::operator()(int i, int j){
	return data.at(i*col + j);
}