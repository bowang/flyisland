#ifndef RENDERMANAGER_H
#define RENDERMANAGER_H

#include "Framework.h"
#include "Shader.h"
#include "SceneNode.h"
#include "DepthRenderTarget.h"
#include "Render_water.h"
#include "Data_struct.h"

class Root;

class RenderManager
{
public:
    RenderManager(Root* root);
    void initOpenGL();
    void preprocess();
    void renderFrame(int j=0);

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

    void initializeCamera();
    void setMaterial(const aiScene* scene, aiMesh* mesh, Shader* shader);
    void setTextures(vector<TextureSet> &textures, aiMesh* mesh, Shader* shader);
    void setMeshData(aiMesh* mesh, Shader* shader);
    void setCamera();
    void setCameraDOF(float xoff=0.f, float yoff=0.f, float focus=1.f);
    void setCameraForCube(GLuint face);
    void setLight();

    void renderCubeFaces(SceneNode &scene);
    void renderSkyBox();
    void renderTriangles(Shader* shader);
    void renderBoundingBox(Shader* shader);
    void renderText();
    void renderOcean();

    GLuint tColorCubeMap;
    int renderPosition;
    render_ocean_class* oceanRender;
	//------------- need aiMatrix4x4 pj_mv
	aiMatrix4x4 pj_mv;

protected:


};

#endif
