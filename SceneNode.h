#ifndef SCENENODE_H
#define SCENENODE_H

#include "Framework.h"
#include "Utility.h"

struct TextureSet {
    sf::Image diffuse, specular, normal;
    bool diffuseExist, specularExist, normalExist;
    TextureSet():diffuseExist(false), specularExist(false), normalExist(false){}
};

class SceneNode
{
public:
    SceneNode();

    aiVector3D mPosition;
    aiVector3D mRotate;
    float      mRotateAngle;
    aiVector3D mScale;
    aiVector3D mVelocity;
    const aiScene* mScene;
    Assimp::Importer mImporter;
    vector<TextureSet> mTexture;
    vector< vector<unsigned> >   mIndexBuffer;
    int index;
    bool fixed;
    bool target;
    bool hit;
    int  score;
    char name[BUFFER_SIZE];
    sf::Clock reappearClock;

    bool ReadFile(const std::string &pFile, unsigned int pFlags);
    void initialize(int i, const char* configFilename);
    int  numShaders();
    bool useShader(int i);
    void rotate(float angle);
    void rotateIncrease(float angleIncrement);
    void setRotateAxis(float rx, float ry, float rz);
    void setRotateAxis(aiVector3D v);

private:
    string mSceneName;
    vector<unsigned> mShaderList;

    void buildIndexBuffer();
    void loadTextures();
    void loadParameters(const char* sceneName, const char* configFilename);

protected:


};

#endif
