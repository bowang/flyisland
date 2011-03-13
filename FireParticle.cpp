#include "FireParticle.h"

FireParticle::FireParticle(aiVector3D p){
    position = p;
    position.y += 2.f;
    init();
}

void FireParticle::init(){
    velocity.x = 0.f;
    velocity.y = 0.03f;
    velocity.z = 0.f;
    life = 200.0f;
    alpha = 1.0f;
}

bool FireParticle::update(){
    position += velocity;
    alpha -= 1.f/life;
    if(alpha <= 0) return false;
    else return true;
}
