#ifndef PARTICLE_H
#define PARTICLE_H

#include "Framework.h"

struct Particle{
    sf::Image image;
    float speed;
    float size;
    Particle(){}
    bool LoadFromFile(char* filename){
        bool success = image.LoadFromFile(filename);
        if(success)
            printf("[Particle] texture loaded from: %s\n", filename);
        else
            printf("Failed to load particle texture: %s\n", filename);
        return success;
    }
    void Bind(){
        image.Bind();
    }
};

#endif
