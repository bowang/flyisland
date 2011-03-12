#ifndef RENDERMANAGER_H
#define RENDERMANAGER_H

#include "Framework.h"
#include "Shader.h"
#include "SceneNode.h"
#include "DepthRenderTarget.h"

class Root;

class RenderManager
{
public:
    RenderManager(Root* root);
    void initOpenGL();
    void preprocess();
    void renderFrame();

private:
    Root* root;
    vector<Shader*> shaders;
    DepthRenderTarget* depthBuffer;
    GLfloat inverseView[16];

    void loadShaders();
    void renderObjects();
    void renderScene(SceneNode& scene, int shaderIdx);
    void renderNode(aiNode* node, SceneNode& scene, Shader* shader);
    void renderAirplane(aiNode* node, SceneNode& scene, Shader* shader);
    void renderParticles(Shader* shader);
    void setMaterial(const aiScene* scene, aiMesh* mesh, Shader* shader);
    void setTextures(vector<TextureSet> &textures, aiMesh* mesh, Shader* shader);
    void setMeshData(aiMesh* mesh, Shader* shader);
    void setCamera();
    void setLight();

    void renderCubeFaces(SceneNode &scene);
    void setCameraForCube(GLuint face);
    void renderSkyBox();
    void renderTriangles(Shader* shader);

    GLuint tColorCubeMap;

protected:


};

#endif
