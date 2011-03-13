#include "FireParticle.h"

FireParticle::FireParticle(aiVector3D p){
    position = p;
    init();
}

void FireParticle::init(){
    velocity.x = Random(0.05f);
    velocity.y = fabs(Random(0.3f));
    velocity.z = Random(0.05f);
    life = Random(100.0f);
    alpha = 1.0f;
}

bool FireParticle::update(){
    position += velocity;
    float drift = position.x * (float)fabs(Random(0.1));
    velocity.x -= drift;
    drift = position.z * (float)fabs(Random(0.1));
    velocity.z -= drift;
    alpha = 0.01f*life;
    life  -= 0.1f;
    if(life <= 0) return false;
    else return true;
}
