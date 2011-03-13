#include "CannonParticle.h"
#include "Root.h"

CannonParticle::CannonParticle(aiVector3D& p, aiVector3D& v, Root* root)
    : position(p), velocity(v), root(root){
    alpha = 1.0f;
    life = 1.0f;
    lasting = 1000;
}

bool CannonParticle::update(vector<BoundingBox>& bbList){

    aiVector3D new_position = position+1.5f*velocity;
    
    int collision = -1;
    for(unsigned i = 0; i < bbList.size(); i++){
        if(bbList[i].isLineIntersectBox(position, new_position)){
            collision = bbList[i].index;
            break;
        }
    }

    if(collision>=0){
        printf("[CannonParticle] hit target[%d]\n", collision);
        root->mSceneManager->hitTarget(collision);
    }
    position += velocity;
    
    if(lasting > 0)
        lasting -= 1;
    else{
        alpha -= 0.05f;
        life  -= 0.05f;
    }
    if(life <= 0)
        return false;
    else
        return true;
}

void CannonParticle::setSpeed(float x, float y, float z){
    velocity.x = x;
    velocity.y = y;
    velocity.z = z;
}
