#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include "Framework.h"

struct BoundingBox{
    BoundingBox(){
        max[0] = max[1] = max[2] = FLT_MIN;
        min[0] = min[1] = min[2] = FLT_MAX;
        index = -1;  // indicating it's not for a single triangle
        hit = false;
    }
    BoundingBox(float min_x, float min_y, float min_z,
                float max_x, float max_y, float max_z){
        min[0] = min_x;
        min[1] = min_y;
        min[2] = min_z;
        max[0] = max_x;
        max[1] = max_y;
        max[2] = max_z;
        index = -1;
    }
    float min[3];
    float max[3];
    int index;
    bool hit;
    float volume(){
        return (max[0]-min[0])*(max[1]-min[1])*(max[2]-min[2]);
    }
    float minLength(){
        float minLen = max[0]-min[0];
        minLen = (minLen>(max[1]-min[1]))?max[1]-min[1]:minLen;
        minLen = (minLen>(max[2]-min[2]))?max[2]-min[2]:minLen;
        return minLen;
    }
    void multiMatrix(aiMatrix4x4& m){
        float min_new[3];
        min_new[0] = min[0]*m.a1 + min[1]*m.a2 + min[2]*m.a3 + m.a4;
        min_new[1] = min[0]*m.b1 + min[1]*m.b2 + min[2]*m.b3 + m.b4;
        min_new[2] = min[0]*m.c1 + min[1]*m.c2 + min[2]*m.c3 + m.c4;
        min[0] = min_new[0];
        min[1] = min_new[1];
        min[2] = min_new[2];
        float max_new[3];
        max_new[0] = max[0]*m.a1 + max[1]*m.a2 + max[2]*m.a3 + m.a4;
        max_new[1] = max[0]*m.b1 + max[1]*m.b2 + max[2]*m.b3 + m.b4;
        max_new[2] = max[0]*m.c1 + max[1]*m.c2 + max[2]*m.c3 + m.c4;
        max[0] = max_new[0];
        max[1] = max_new[1];
        max[2] = max_new[2];
    }
    // following three functions are referred to 3D Programming Weekly
    // http://www.3dkingdoms.com/weekly/weekly.php?a=3
    bool isLineIntersectBox(aiVector3D& L1, aiVector3D& L2){
        aiVector3D hit;
        if (L2.x < min[0] && L1.x < min[0]) return false;
        if (L2.x > max[0] && L1.x > max[0]) return false;
        if (L2.y < min[1] && L1.y < min[1]) return false;
        if (L2.y > max[1] && L1.y > max[1]) return false;
        if (L2.z < min[2] && L1.z < min[2]) return false;
        if (L2.z > max[2] && L1.z > max[2]) return false;
        if (L1.x >= min[0] && L1.x <= max[0] &&
            L1.y >= min[1] && L1.y <= max[1] &&
            L1.z >= min[2] && L1.z <= max[2]) 
        {
            hit = L1;
            return true;
        }
        if ( (getIntersection( L1.x-min[0], L2.x-min[0], L1, L2, hit) && insideBoxFace( hit, min, max, 0 ))
          || (getIntersection( L1.y-min[1], L2.y-min[1], L1, L2, hit) && insideBoxFace( hit, min, max, 1 )) 
          || (getIntersection( L1.z-min[2], L2.z-min[2], L1, L2, hit) && insideBoxFace( hit, min, max, 2 )) 
          || (getIntersection( L1.x-max[0], L2.x-max[0], L1, L2, hit) && insideBoxFace( hit, min, max, 0 )) 
          || (getIntersection( L1.y-max[1], L2.y-max[1], L1, L2, hit) && insideBoxFace( hit, min, max, 1 )) 
          || (getIntersection( L1.z-max[2], L2.z-max[2], L1, L2, hit) && insideBoxFace( hit, min, max, 2 )))
            return true;
        return false;
    }
    bool inline getIntersection( float fDst1, float fDst2, aiVector3D& P1, aiVector3D& P2, aiVector3D& hit) {
        if ( (fDst1 * fDst2) > 0.0f) return false; // on the same side
        if ( fDst1 == fDst2) return false;         // parallel to one side of the box
        hit = P1 + (P2-P1) * ( -fDst1/(fDst2-fDst1) );
        return true;
    }
    bool inline insideBoxFace( aiVector3D& hit, float* min, float* max, const int axis) {
        if ( axis==0 && hit.z >= min[2] && hit.z <= max[2] && hit.y >= min[1] && hit.y <= max[1]) return true;
        if ( axis==1 && hit.z >= min[2] && hit.z <= max[2] && hit.x >= min[0] && hit.x <= max[0]) return true;
        if ( axis==2 && hit.x >= min[0] && hit.x <= max[0] && hit.y >= min[1] && hit.y <= max[1]) return true;
        return false;
    }
    bool isTriangleIntersectBox(aiVector3D& L1, aiVector3D& L2, aiVector3D& L3){
        return (isLineIntersectBox(L1,L2) 
             || isLineIntersectBox(L2,L3)
             || isLineIntersectBox(L3,L1));
    }
};

#endif
