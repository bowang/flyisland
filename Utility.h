#ifndef UTILITY_H
#define UTILITY_H

#include "Framework.h"

#define Random(x) (2.f*(float)rand()/RAND_MAX-1.f)*x
#define aiVector3DRandom(x) aiVector3D(Random(x),Random(x),Random(x))

aiVector3D cross(aiVector3D a, aiVector3D b);

GLfloat dot(aiVector3D a, aiVector3D b);

GLfloat DOT(aiVector3D a, aiVector3D b);

void multMatrixf(GLfloat* out, GLfloat* in1, GLfloat* in2);

void loadIdentity(GLfloat* m);

void loadIdentity(aiMatrix4x4& m);

void debugShowMatrix(const GLfloat* m, const char* msg);

void debugShowVertex(const GLfloat* ver, const char* msg);

void debugShowVertex(const aiVector3D& ver, const char* msg);

float min(float, float);

float max(float, float);

float GetPrivateProfileFloat(const char* section, const char* key, float def_value, const char* filename);

bool  GetPrivateProfileBool (const char* section, const char* key, bool def_value, const char* filename);

#endif
