#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include "Framework.h"

class Root;

class InputManager
{
public:
    InputManager(Root* root);
    void handleInput();

private:
    Root* root;
    aiVector3D v;
    bool mouseCaptureEnabled;
    int mouseX0, mouseY0, mouseX, mouseY;
    float yaw, pitch;
    float lenxz, lenxy;
    float delta;
    float dPitch;
    float dYaw;
    bool prevDepthOfField;

protected:


};


#endif
