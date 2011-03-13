#include "SceneNode.h"

SceneNode::SceneNode()
{
    index = -1;
    fixed = false;
    target = false;
    hit = false;
}

bool SceneNode::ReadFile(const std::string &pFile, unsigned int pFlags)
{
    mSceneName = pFile;
    mImporter.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_LINE|aiPrimitiveType_POINT);
    mScene = mImporter.ReadFile(pFile, pFlags);
    mImporter.ApplyPostProcessing(aiProcess_FlipUVs);
    return (NULL != mScene);
}

void SceneNode::loadTextures()
{
    char sceneFileName[BUFFER_SIZE];
    strcpy(sceneFileName, mSceneName.c_str());
    char* pch = strrchr(sceneFileName, 47);  // if '/' is used in path seperator
    if(pch==NULL)
        pch = strrchr(sceneFileName, 92);    // if '\' is used as path seperator
    unsigned length = pch-sceneFileName+1;
    char* prefix = new char[length+1];
    strncpy(prefix, sceneFileName, length);
    prefix[length] = '\0';

    mTexture.resize(mScene->mNumMaterials);
    aiString filename, path;

    for(unsigned i = 0; i < mScene->mNumMaterials; i++){
        aiMaterial* material = mScene->mMaterials[i];
        
        filename.Clear();
        material->GetTexture(aiTextureType_DIFFUSE, 0, &filename);
        path.Set(prefix); path.Append(filename.data); // path.Append("_d.jpg");
        mTexture[i].diffuseExist = mTexture[i].diffuse.LoadFromFile(path.data);
        if(mTexture[i].diffuseExist)
            printf("[SceneNode] material[%d] : diffuse texture loaded: %s\n", i, path.data);

        filename.Clear();
        material->GetTexture(aiTextureType_SPECULAR, 0, &filename);
        path.Set(prefix); path.Append(filename.data); // path.Append("_s.jpg");
        mTexture[i].specularExist = mTexture[i].specular.LoadFromFile(path.data);
        if(mTexture[i].specularExist)
            printf("[SceneNode] material[%d] : specular texture loaded: %s\n", i, path.data);

        filename.Clear();
        material->GetTexture(aiTextureType_NORMALS, 0, &filename);
        path.Set(prefix); path.Append(filename.data); // path.Append("_n.jpg");
        mTexture[i].normalExist = mTexture[i].normal.LoadFromFile(path.data);
        if(mTexture[i].normalExist)
            printf("[SceneNode] material[%d] : normal texture loaded: %s\n", i, path.data);
    }

    delete[] prefix;
}

void SceneNode::loadParameters(const char* sceneName, const char* configFileName)
{
    mPosition.x = GetPrivateProfileFloat(sceneName, "x", 0.0f, configFileName);
    mPosition.y = GetPrivateProfileFloat(sceneName, "y", 0.0f, configFileName);
    mPosition.z = GetPrivateProfileFloat(sceneName, "z", 0.0f, configFileName);

    mRotate.x = GetPrivateProfileFloat(sceneName, "rotateX", 0.0f, configFileName);
    mRotate.y = GetPrivateProfileFloat(sceneName, "rotateY", 0.0f, configFileName);
    mRotate.z = GetPrivateProfileFloat(sceneName, "rotateZ", 0.0f, configFileName);
    mRotateAngle = GetPrivateProfileFloat(sceneName, "rotateA", 0.0f, configFileName);

    mScale.x = GetPrivateProfileFloat(sceneName, "scaleX", 1.0f, configFileName);
    mScale.y = GetPrivateProfileFloat(sceneName, "scaleY", 1.0f, configFileName);
    mScale.z = GetPrivateProfileFloat(sceneName, "scaleZ", 1.0f, configFileName);

    mVelocity.x = GetPrivateProfileFloat(sceneName, "speedX", 0.0f, configFileName);
    mVelocity.y = GetPrivateProfileFloat(sceneName, "speedY", 0.0f, configFileName);
    mVelocity.z = GetPrivateProfileFloat(sceneName, "speedZ", 0.0f, configFileName);

    mShaderList.clear();
    char shaderList[BUFFER_SIZE];
    GetPrivateProfileString(sceneName, "shader", "0", shaderList, sizeof(shaderList), configFileName);
    istringstream iss(shaderList);
    printf("[SceneNode] %s uses shaders: ", sceneName);
    while(!iss.eof()){
        int num;
        iss >> num;
        mShaderList.push_back(num);
        printf("%d ", num);
    }
    printf("\n");

    fixed = GetPrivateProfileBool(sceneName, "fixed", false, configFileName);
    target = GetPrivateProfileBool(sceneName, "target", false, configFileName);
}

void SceneNode::buildIndexBuffer()
{
    // FILE* fout = fopen("out.txt", "wt");

    mIndexBuffer.reserve(mScene->mNumMeshes);
    for(unsigned i = 0; i < mScene->mNumMeshes; i++){
        aiMesh* mesh = mScene->mMeshes[i];
        std::vector<unsigned> iBuffer;
        iBuffer.reserve(mesh->mNumFaces * 3);
        for(unsigned j = 0; j < mesh->mNumFaces; j++){
            aiFace& face = mesh->mFaces[j];
            for(unsigned k = 0; k < face.mNumIndices; k++){
                iBuffer.push_back(face.mIndices[k]);
                // fprintf(fout, "%d ", face.mIndices[k]);
            }
        }
        mIndexBuffer.push_back(iBuffer);
        if(!mesh->HasTextureCoords(0)){
            printf("mesh[%d] has not texture coordinates.\n", i);
        }
    }
    printf("[SceneNode] Index buffers built: %s (%d meshes)\n", mSceneName.c_str(), mIndexBuffer.size());
}

void SceneNode::initialize(int i, const char* configFileName)
{
    index = i;
    buildIndexBuffer();
    loadTextures();
    char sceneName[17];
    sprintf(sceneName, "Scene %d", i);
    loadParameters(sceneName, configFileName);
}

int SceneNode::numShaders()
{
    return mShaderList.size();
}

bool SceneNode::useShader(int i)
{
    for(unsigned j = 0; j < mShaderList.size(); j++){
        if(i==mShaderList[j]) return true;
    }
    return false;
}

void SceneNode::rotateIncrease(float angleIncrement)
{
    mRotateAngle += angleIncrement;
    while(mRotateAngle > 360.0f)
        mRotateAngle -= 360.0f;
}

void SceneNode::rotate(float angle)
{
    mRotateAngle = angle;
    while(mRotateAngle > 360.0f)
        mRotateAngle -= 360.0f;
}

void SceneNode::setRotateAxis(float rx, float ry, float rz)
{
    mRotate.x = rx;
    mRotate.y = ry;
    mRotate.z = rz;
}

void SceneNode::setRotateAxis(aiVector3D v)
{
    mRotate = v;
}

