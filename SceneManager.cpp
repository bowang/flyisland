#include "SceneManager.h"
#include "Root.h"

SceneManager::SceneManager(Root* root)
{
    this->root = root;
    mSceneNodes.resize(root->mNumOfScene);
}

void SceneManager::loadAssets() 
{
    for(int i = 0; i < root->mNumOfScene; i++){
        char modelName[BUFFER_SIZE];
        char sceneName[17];
        sprintf(sceneName, "Scene %d", i);
        GetPrivateProfileString(sceneName, "FileName", "", modelName, BUFFER_SIZE, root->mConfigFileName.c_str());
        char modelPath[2*BUFFER_SIZE];
        strcpy(modelPath, root->mRootPath.c_str());
        strcat(modelPath, modelName);
        printf("\n[SceneManager] Loading asset: %s\n", modelPath);

        bool success = mSceneNodes[i].ReadFile(modelPath,
            aiProcess_CalcTangentSpace |
            aiProcess_Triangulate |
            aiProcess_SortByPType |
            aiProcess_JoinIdenticalVertices |
            aiProcessPreset_TargetRealtime_Quality);

        if (!success || mSceneNodes[i].mScene->mNumMeshes <= 0) {
            std::cerr << mSceneNodes[i].mImporter.GetErrorString() << std::endl;
            exit(-1);
        }

        mSceneNodes[i].initialize(sceneName, root->mConfigFileName.c_str());
    }
}

void SceneManager::initializeWorld()
{
    root->eye.x = GetPrivateProfileFloat("Initialization", "eyeX", 0.0f, root->mConfigFileName.c_str());
    root->eye.y = GetPrivateProfileFloat("Initialization", "eyeY", 0.0f, root->mConfigFileName.c_str());
    root->eye.z = GetPrivateProfileFloat("Initialization", "eyeZ", 0.0f, root->mConfigFileName.c_str());
    root->target.x = GetPrivateProfileFloat("Initialization", "targetX", -1.0f, root->mConfigFileName.c_str());
    root->target.y = GetPrivateProfileFloat("Initialization", "targetY", 0.0f, root->mConfigFileName.c_str());
    root->target.z = GetPrivateProfileFloat("Initialization", "targetZ", 0.0f, root->mConfigFileName.c_str());
    root->up.x = GetPrivateProfileFloat("Initialization", "upX", 0.0f, root->mConfigFileName.c_str());
    root->up.y = GetPrivateProfileFloat("Initialization", "upY", 1.0f, root->mConfigFileName.c_str());
    root->up.z = GetPrivateProfileFloat("Initialization", "upZ", 0.0f, root->mConfigFileName.c_str());

    for(int i = 0; i < root->mNumOfLights; i++){
        Light l;
        char lightName[17];
        sprintf(lightName, "Light %d", i);
        l.position[0] = GetPrivateProfileFloat(lightName, "x", 0.0f, root->mConfigFileName.c_str());
        l.position[1] = GetPrivateProfileFloat(lightName, "y", 1.0f, root->mConfigFileName.c_str());
        l.position[2] = GetPrivateProfileFloat(lightName, "z", 0.0f, root->mConfigFileName.c_str());
        l.position[3] = GetPrivateProfileFloat(lightName, "w", 0.0f, root->mConfigFileName.c_str());
        l.ambient[0] = GetPrivateProfileFloat(lightName, "ambientR", 0.1f, root->mConfigFileName.c_str());
        l.ambient[1] = GetPrivateProfileFloat(lightName, "ambientG", 0.1f, root->mConfigFileName.c_str());
        l.ambient[2] = GetPrivateProfileFloat(lightName, "ambientB", 0.1f, root->mConfigFileName.c_str());
        l.ambient[3] = GetPrivateProfileFloat(lightName, "ambientA", 1.0f, root->mConfigFileName.c_str());
        l.diffuse[0] = GetPrivateProfileFloat(lightName, "diffuseR", 0.3f, root->mConfigFileName.c_str());
        l.diffuse[1] = GetPrivateProfileFloat(lightName, "diffuseG", 0.3f, root->mConfigFileName.c_str());
        l.diffuse[2] = GetPrivateProfileFloat(lightName, "diffuseB", 0.3f, root->mConfigFileName.c_str());
        l.diffuse[3] = GetPrivateProfileFloat(lightName, "diffuseA", 1.0f, root->mConfigFileName.c_str());
        l.specular[0] = GetPrivateProfileFloat(lightName, "specularR", 0.2f, root->mConfigFileName.c_str());
        l.specular[1] = GetPrivateProfileFloat(lightName, "specularG", 0.2f, root->mConfigFileName.c_str());
        l.specular[2] = GetPrivateProfileFloat(lightName, "specularB", 0.2f, root->mConfigFileName.c_str());
        l.specular[3] = GetPrivateProfileFloat(lightName, "specularA", 1.0f, root->mConfigFileName.c_str());
        root->light.push_back(l);
    }

}

void SceneManager::updateWorld()
{
    
}
