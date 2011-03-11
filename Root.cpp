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

    mSceneManager = new SceneManager(this);
    mRenderManager = new RenderManager(this);
    mInputManager = new InputManager(this);
    if(mEnableAirplane)
        viewMode = FlyView;
    else
        viewMode = FreeView;
}

bool Root::loadConfigFile()
{
    char rootPath[BUFFER_SIZE];
    GetPrivateProfileString("General", "RootPath", "./", rootPath, BUFFER_SIZE, mConfigFileName.c_str());
    mRootPath.assign(rootPath);
    printf("[Root] RootPath = %s\n", mRootPath.c_str());

    mNumOfScene = GetPrivateProfileInt("General", "NumOfScene", 0, mConfigFileName.c_str());
    printf("[Root] NumOfScene = %d\n", mNumOfScene);

    mNumOfShaders = GetPrivateProfileInt("General", "NumOfShaders", 0, mConfigFileName.c_str());
    printf("[Root] NumOfShaders = %d\n", mNumOfShaders);

    mNumOfLights = GetPrivateProfileInt("General", "NumOfLights", 0, mConfigFileName.c_str());
    printf("[Root] NumOfLights = %d\n", mNumOfLights);

    mEnableAirplane = (bool)GetPrivateProfileInt("General", "EnableAirplane", 0, mConfigFileName.c_str());
    printf("[Root] EnableAirplane = %d\n", (int)mEnableAirplane);

    return true;
};

void Root::run()
{
    mRenderManager->initOpenGL();

    // first loadAssets, then initializeWorld, otherwise flyDirection is not correct
    mSceneManager->loadAssets();
    mSceneManager->initializeWorld();

    mRenderManager->preprocess();

    printf("\n[Root] Begin rendering\n");
    while (window.IsOpened()) {
        mInputManager->handleInput();
        mSceneManager->updateWorld();
        mRenderManager->renderFrame();
        window.Display();
    }

}
