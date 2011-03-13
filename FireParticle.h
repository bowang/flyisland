#ifndef FIREPARTICLE_H
#define FIREPARTICLE_H

#include "Framework.h"
#include "Utility.h"

class FireParticle{
public:
    FireParticle(aiVector3D position);
    void init();
    bool update();

    aiVector3D position;
    aiVector3D velocity;
    GLfloat alpha;
    GLfloat life;
};

#endif
