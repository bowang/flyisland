#include "Framework.h"
#include "Utility.h"
#include "kdTree.h"

class CannonParticle{
public:
    CannonParticle(aiVector3D& p, aiVector3D& v);
    bool update(vector<BoundingBox>& bbList);
    void setSpeed(float, float, float);

    aiVector3D position;
    aiVector3D velocity;
    GLfloat alpha;
    GLfloat life;
    int lasting;
};

