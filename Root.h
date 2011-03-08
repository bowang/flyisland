#ifndef ROOT_H
#define ROOT_H

#include "Framework.h"
#include "SceneManager.h"
#include "RenderManager.h"
#include "InputManager.h"

struct Light{
    float position[4];
    float ambient[4];
    float diffuse[4];
    float specular[4];
};

class Root
{
public:
    Root();
    Root(string configFileName);
    void run();

    string mConfigFileName;
    string mRootPath;
    int    mNumOfScene;
    int    mNumOfShaders;
    int    mNumOfLights;
    bool   mEnableAirplane;

    // Camera parameters
    aiVector3D eye;
    aiVector3D target;
    aiVector3D up;

    // Light parameters
    vector<Light> light;

    SceneManager*  mSceneManager;
    RenderManager* mRenderManager;
    InputManager*  mInputManager;

private:
    bool loadConfigFile();


protected:


};


#endif
