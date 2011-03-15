#ifndef DATA_STRUCT_H
#define DATA_STRUCT_H

#include"Framework.h"
#define PI 3.1415926535897932384
#define SQUART2 1.414213562373095
#define SQUART1_2 0.7071067811865475

double gaussrand();

class vector3{
public:
	float x, y, z;
	vector3();
	vector3(float x_, float y_, float z_);
	vector3 cross_product(vector3 & b);
	vector3 operator-(vector3 &b);
	vector3 operator+(vector3 &b);
	vector3 operator*(float a);
	void normalize();
	float operator*(vector3 &b);
	float & operator[](int i);
};

class vector2{
public:
	float x, y;
	vector2();
	vector2(float x_, float y_);
	vector2 operator-(vector2 &b);
	vector2 operator+(vector2 &b);
	vector2 operator*(float a);
	void normalize();
	float norm2();
	float norm1();
	float operator*(vector2 &b);
	float cross_product(vector2 &b);
	float & operator[](int i);
};

class my2Darray{
private:
	int row, col;
	vector<float> data;
public:
	my2Darray(int row_, int col_);
	float & operator()(int row_, int col_);
};

class my2Darray_vector2{
private:
	int row, col;
public:
	vector<vector2> data;
	my2Darray_vector2(int row_, int col_);
	vector2 & operator()(int row_, int col_);
};

class my2Darray_vector3{
private:
	int row, col;
public:
	vector<vector3> data;
	my2Darray_vector3(int row_, int col_);
	vector3 & operator()(int row_, int col_);
};

class complex{
public:
	float re, im;
	complex();
	complex(float re_, float im_);
/*	float & Re();
	float & Im();*/
	complex operator+(complex & c);
	complex operator-(complex & c);
	complex operator*(complex & c);
	complex operator*(float a);
	complex conj();	//return the conj
	complex & operator=(complex & c);
	complex & operator+=(complex & c);
	complex & operator*=(float a);
};

class my2Darray_complex{
private:
	int row, col;
	vector<complex> data;
public:
	my2Darray_complex(int row_, int col_);
	complex & operator()(int row_, int col_);
};

#endif
