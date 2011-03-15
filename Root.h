#ifndef ROOT_H
#define ROOT_H

#include "Framework.h"
#include "SceneManager.h"
#include "RenderManager.h"
#include "InputManager.h"
#include "SoundManager.h"

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
    Root(string configFilename);
    void run();

    string mConfigFilename;
    string mRootPath;
    string mFontPath;
    int    mNumOfScene;
    int    mNumOfShaders;
    int    mNumOfLights;
    int    mNumOfParticles;
    bool   mEnableAirplane;
    bool   mEnableMusic;
    bool   mHighSpeed;
    bool   mAirplaneCrash;
    bool   mDepthOfField;
    int    mPlayerScore;
    sf::Font mFont;
    int    mMinute;
    int    mSecond;
    int    mLevel;
    int    mNumOfTargets;

    // Camera parameters
    aiVector3D eye;
    aiVector3D target;
    aiVector3D up;

    ViewMode viewMode;

    // Light parameters
    vector<Light> light;

    // Airplane parameters
    SceneNode* airplane;

    SceneManager*  mSceneManager;
    RenderManager* mRenderManager;
    InputManager*  mInputManager;
    SoundManager*  mSoundManager;

private:
    bool loadConfigFile();


protected:


};


#endif
