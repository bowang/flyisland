#ifndef SCENENODE_H
#define SCENENODE_H

#include "Framework.h"
#include "Utility.h"

struct TextureSet {
    sf::Image diffuse, specular, normal;
    bool diffuseExist, specularExist, normalExist;
};

class SceneNode
{
public:
    SceneNode();

    aiVector3D mPosition;
    aiVector3D mRotate;
    float      mRotateAngle;
    aiVector3D mScale;
    const aiScene* mScene;
    Assimp::Importer mImporter;
    vector<TextureSet> mTexture;
    vector< vector<unsigned> >   mIndexBuffer;

    bool ReadFile(const std::string &pFile, unsigned int pFlags);
    void initialize(const char* sceneName, const char* configFileName);
    int  numShaders();
    bool useShader(int i);
    void rotate(float ra);
    void setRotateAxis(float rx, float ry, float rz);

private:
    string mSceneName;
    vector<unsigned> mShaderList;

    void buildIndexBuffer();
    void loadTextures();
    void loadParameters(const char* sceneName, const char* configFileName);

protected:


};

#endif
