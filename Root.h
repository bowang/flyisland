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

enum ViewMode{
    FlyView = 0,
    FollowView = 1,
    FreeView = 2
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
    bool   mHighSpeed;
    bool   mAirplaneCrash;

    // Camera parameters
    aiVector3D eye;
    aiVector3D target;
    aiVector3D up;
    ViewMode viewMode;

    // Light parameters
    vector<Light> light;

    // Airplane parameters
    SceneNode* airplane;
    SceneNode* airscrew;

    SceneManager*  mSceneManager;
    RenderManager* mRenderManager;
    InputManager*  mInputManager;

private:
    bool loadConfigFile();


protected:


};


#endif
