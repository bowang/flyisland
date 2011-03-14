#include "Root.h"

extern sf::RenderWindow window;

Root::Root()
{
    Root("./");
}

Root::Root(string configFileName)
{
    mConfigFileName = configFileName;
    loadConfigFile();

    mSceneManager  = new SceneManager(this);
    mRenderManager = new RenderManager(this);
    mInputManager  = new InputManager(this);
    mSoundManager  = new SoundManager(this);
    if(mEnableAirplane)
        viewMode = FollowView;
    else
        viewMode = FreeView;
    mHighSpeed = false;
    mAirplaneCrash = false;
    mDepthOfField = false;
    mPlayerScore = 0;
    if(!mFont.LoadFromFile(mFontPath)){
        printf("Failed to load font: %s\n", mFontPath.c_str());
        exit(-1);
    }
    else{
        printf("[Root] Font loaded from %s\n", mFontPath.c_str());
    }
}

bool Root::loadConfigFile()
{
    char rootPath[BUFFER_SIZE];
    GetPrivateProfileString("General", "RootPath", "./", rootPath, BUFFER_SIZE, mConfigFileName.c_str());
    mRootPath.assign(rootPath);
    printf("[Root] RootPath = %s\n", mRootPath.c_str());

    char fontPath[BUFFER_SIZE];
    GetPrivateProfileString("General", "FontPath", "", fontPath, BUFFER_SIZE, mConfigFileName.c_str());
    mFontPath.assign(fontPath);
    printf("[Root] FontPath = %s\n", mFontPath.c_str());

    mNumOfScene = GetPrivateProfileInt("General", "NumOfScene", 0, mConfigFileName.c_str());
    printf("[Root] NumOfScene = %d\n", mNumOfScene);

    mNumOfShaders = GetPrivateProfileInt("General", "NumOfShaders", 0, mConfigFileName.c_str());
    printf("[Root] NumOfShaders = %d\n", mNumOfShaders);

    mNumOfLights = GetPrivateProfileInt("General", "NumOfLights", 0, mConfigFileName.c_str());
    printf("[Root] NumOfLights = %d\n", mNumOfLights);

    mNumOfParticles = GetPrivateProfileInt("General", "NumOfParticles", 0, mConfigFileName.c_str());
    printf("[Root] NumOfParticles = %d\n", mNumOfParticles);

    mEnableAirplane = GetPrivateProfileBool("General", "EnableAirplane", false, mConfigFileName.c_str());
    printf("[Root] EnableAirplane = %d\n", (int)mEnableAirplane);

    mEnableMusic = GetPrivateProfileBool("General", "EnableMusic", false, mConfigFileName.c_str());
    printf("[Root] EnableMusic = %d\n", (int)mEnableMusic);

    return true;
};

void Root::run()
{
    mRenderManager->initOpenGL();

    // first loadAssets, then initializeWorld, otherwise flyDirection is incorrect
    mSceneManager->loadAssets();
    mSceneManager->initializeWorld();

    mSoundManager->loadAssets();
    mSoundManager->play(Background);
    mSoundManager->loop(Background, true);

    mRenderManager->preprocess();

    printf("\n[Root] Begin rendering\n");
    while (window.IsOpened()) {
        mInputManager->handleInput();

        GL_CHECK(glClear(GL_ACCUM_BUFFER_BIT))
        int run = 1;
        if(mHighSpeed) run = 10;
        for(int i = 0; i < run; i++){
            mSceneManager->updateWorld();
            if(mDepthOfField){
                GL_CHECK(glClear(GL_ACCUM_BUFFER_BIT))
                for(int j = 0; j < 5; j++){
                    mRenderManager->renderFrame(j);
                    GL_CHECK(glAccum(GL_ACCUM, 0.1f))
                }
                GL_CHECK(glAccum(GL_RETURN, 1.f))
            }
            else{
                mRenderManager->renderFrame();
            }
            GL_CHECK(glAccum(GL_ACCUM, 1.f/float(run)))
        }
        GL_CHECK(glAccum(GL_RETURN, 1.f))

        window.Display();
    }

}
