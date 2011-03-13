#ifndef CANNONPARTICLE_H
#define CANNONPARTICLE_H

#include "Framework.h"
#include "Utility.h"
#include "kdTree.h"

class Root;

class CannonParticle{
public:
    CannonParticle(aiVector3D& p, aiVector3D& v, Root* root);
    bool update(vector<BoundingBox>& bbList);
    void setSpeed(float, float, float);

    aiVector3D position;
    aiVector3D velocity;
    GLfloat alpha;
    GLfloat life;
    int lasting;

    Root* root;
};


#endif
