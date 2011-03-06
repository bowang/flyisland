#include "Utility.h"

aiVector3D cross(aiVector3D a, aiVector3D b){
    aiVector3D res;
    res.x = a.y*b.z - a.z*b.y;
    res.y = a.z*b.x - a.x*b.z;
    res.z = a.x*b.y - a.y*b.x;
    return res;
}

GLfloat dot(aiVector3D a, aiVector3D b){
    GLfloat res = a.x*b.x + a.y*b.y + a.z*b.z;
    return res;
}


// out = in1 * in2
void multMatrixf(GLfloat* out, GLfloat* in1, GLfloat* in2){
    GLfloat result[16];
    for (int i = 0; i < 4; ++i) {
       for (int j = 0; j < 4; ++j) {
           GLfloat sum = 0;
           for (int k = 0; k < 4; ++k) {
               sum += in1[k * 4 + j] * in2[i * 4 + k];
           }
           out[i * 4 + j] = sum;
       }
    }
    for(int i = 0; i < 16; i++)
        out[i] = result[i];
}

void loadIdentity(GLfloat* m){
    for (int i = 0; i < 16; ++i) {
        m[i] = (i % 5 == 0);
    }
}

void loadIdentity(aiMatrix4x4& m){
    m.a1 = m.b2 = m.c3 = m.d4 = 1.0f;
    m.a2 = m.a3 = m.a4 = 0.0f;
    m.b1 = m.b3 = m.b4 = 0.0f;
    m.c1 = m.c2 = m.c4 = 0.0f;
    m.d1 = m.d2 = m.d3 = 0.0f;
}

void debugShowMatrix(const GLfloat* m, const char* msg = NULL)
{
    if (msg) fprintf(stderr, "%s\n", msg);
    fprintf(stderr, "%f\t%f\t%f\t%f\n", m[0], m[4], m[8], m[12]);
    fprintf(stderr, "%f\t%f\t%f\t%f\n", m[1], m[5], m[9], m[13]);
    fprintf(stderr, "%f\t%f\t%f\t%f\n", m[2], m[6], m[10], m[14]);
    fprintf(stderr, "%f\t%f\t%f\t%f\n", m[3], m[7], m[11], m[15]);
    fprintf(stderr, "\n");
}

void debugShowVertex(const GLfloat* ver, const char* msg = NULL)
{
    if (msg) fprintf(stderr, "%s\n", msg);
    fprintf(stderr, "%f\t%f\t%f\n", ver[0], ver[1], ver[2]);
    fprintf(stderr, "\n");
}

void debugShowVertex(const aiVector3D& ver, const char* msg = NULL)
{
    if (msg) fprintf(stderr, "%s\n", msg);
    fprintf(stderr, "%f\t%f\t%f\n", ver.x, ver.y, ver.z);
    fprintf(stderr, "\n");
}

float GetPrivateProfileFloat(const char* section, const char* key, float def_value, const char* filename)
{   
     char buffer[64];   
     if(GetPrivateProfileString(section,key,"",buffer,sizeof(buffer),filename))
         return atof(buffer); 
     return def_value; 
} 

bool GetPrivateProfileBool(const char* section, const char* key, bool def_value, const char* filename)
{   
     char buffer[64];   
     if(GetPrivateProfileString(section,key,"",buffer,sizeof(buffer),filename))
         return (atoi(buffer)!=0);
     return def_value;
} 

float min(float x, float y){
    if(x > y) return y;
    else return x;
}

float max(float x, float y){
    if(x > y) return x;
    else return y;
}

