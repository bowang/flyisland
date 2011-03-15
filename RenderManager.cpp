#include "RenderManager.h"
#include "Root.h"
#include "Data_struct.h"

extern sf::RenderWindow window;

RenderManager::RenderManager(Root* root)
{
    this->root = root;
    oceanRender = new render_ocean_class(129, 20.f, 20.f, 0.002f, 1.f, vector2(1.f, 0.f));
    // depthBuffer = new DepthRenderTarget(window.GetWidth(), window.GetHeight());
}

void RenderManager::initOpenGL() 
{
    // Initialize GLEW on Windows, to make sure that OpenGL 2.0 is loaded
#ifdef FRAMEWORK_USE_GLEW
    GLint error = glewInit();
    if (GLEW_OK != error) {
        std::cerr << glewGetErrorString(error) << std::endl;
        exit(-1);
    }
    if (!GLEW_VERSION_2_0 || !GL_EXT_framebuffer_object) {
        std::cerr << "This program requires OpenGL 2.0 and FBOs" << std::endl;
        exit(-1);
    }
#endif

    glClearDepth(1.0f);
    glClearColor(99.f/255.f, 122.f/255.f, 192.f/255.f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glViewport(0, 0, window.GetWidth(), window.GetHeight());

    glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glHint(GL_TEXTURE_COMPRESSION_HINT, GL_NICEST);

    shaders.resize(root->mNumOfShaders);
    loadShaders();

}

void RenderManager::preprocess()
{
    // renderCubeFaces(root->mSceneManager->mSceneNodes[0]);
    oceanRender->initial_ocean_shader();
    initializeCamera();
}

void RenderManager::renderFrame(int j) 
{
    renderPosition = j;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // render skybox first
    renderSkyBox();

    // render ocean
    renderOcean();
    
    // render objects (airplane, island, etc)
    renderObjects();
    
    // render triangles for testing purpose
    // renderTriangles(shaders[3]);
    // renderBoundingBox(shaders[3]);
    
    // render particles here
    renderParticles(shaders[4]);
    
    // render text
    renderText();
    
}

void RenderManager::renderObjects()
{
    // render objects in the scene with different shaders
    for(int shaderIdx = 0; shaderIdx < root->mNumOfShaders; shaderIdx++){
        switch(shaderIdx){
        // Perform OpenGL initialization for the particular shader
        case 0:
            GL_CHECK(glUseProgram(shaders[shaderIdx]->programID()))
            GL_CHECK(glViewport(0, 0, window.GetWidth(), window.GetHeight()))
            GL_CHECK(setCameraDOF())
            GL_CHECK(setLight())
            for(unsigned i = 0; i < root->mSceneManager->mSceneNodes.size(); i++){
                if(root->mSceneManager->mSceneNodes[i].hit==false)
                    renderScene(root->mSceneManager->mSceneNodes[i], shaderIdx);
            }
            break;

        case 2:
            GL_CHECK(glUseProgram(shaders[shaderIdx]->programID()))
            GL_CHECK(glViewport(0, 0, window.GetWidth(), window.GetHeight()))
            GL_CHECK(setCameraDOF())
            for(int i = 0; i < root->mNumOfScene; i++){
                if(root->mSceneManager->mSceneNodes[i].hit==false)
                    renderScene(root->mSceneManager->mSceneNodes[i], shaderIdx);
            }
            break;

        default:
            break;
        }
    }
}

void RenderManager::renderScene(SceneNode& scene, int shaderIdx)
{
    GL_CHECK(glPushMatrix())
    // airplane need more rotations
    if(root->mEnableAirplane && &scene==root->airplane){
        GL_CHECK(glTranslatef(scene.mPosition.x, scene.mPosition.y, scene.mPosition.z))
        aiVector3D& pitchAxis = root->mSceneManager->pitchAxis;
        GL_CHECK(glRotatef(root->mSceneManager->pitch, pitchAxis.x, pitchAxis.y, pitchAxis.z))
        aiVector3D& yawAxis = root->mSceneManager->yawAxis;
        GL_CHECK(glRotatef(root->mSceneManager->yaw, yawAxis.x, yawAxis.y, yawAxis.z))
        GL_CHECK(glRotatef(scene.mRotateAngle, scene.mRotate.x, scene.mRotate.y, scene.mRotate.z))
        GL_CHECK(glScalef(scene.mScale.x, scene.mScale.y, scene.mScale.z))
        if(scene.useShader(shaderIdx))
            renderAirplane(scene.mScene->mRootNode, scene, shaders[shaderIdx]);

    }
    else {
        GL_CHECK(glTranslatef(scene.mPosition.x, scene.mPosition.y, scene.mPosition.z))
        GL_CHECK(glRotatef(scene.mRotateAngle, scene.mRotate.x, scene.mRotate.y, scene.mRotate.z))
        GL_CHECK(glScalef(scene.mScale.x, scene.mScale.y, scene.mScale.z))
        if(scene.useShader(shaderIdx))
            renderNode(scene.mScene->mRootNode, scene, shaders[shaderIdx]);
    }
    GL_CHECK(glPopMatrix())
}

void RenderManager::renderAirplane(aiNode* node, SceneNode& scene, Shader* shader)
{
    const char * airscrew = "nhprpsil";
    const char * airscrew_edge = "nhprpwht";
    const char * airtail = "nhbpblk";
    const char * airtail_edge = "nhbpwht";

    GL_CHECK(glPushMatrix())
    aiMatrix4x4 &m = node->mTransformation;
    GLfloat modelMatrix [] = {m.a1, m.b1, m.c1, m.d1,
                              m.a2, m.b2, m.c2, m.d2,
                              m.a3, m.b3, m.c3, m.d3,
                              m.a4, m.b4, m.c4, m.d4};
    GL_CHECK(glMatrixMode(GL_MODELVIEW))
    GL_CHECK(glMultMatrixf(modelMatrix))

    if ((strcmp(node->mName.data,airscrew)==0)||(strcmp(node->mName.data,airscrew_edge)==0)) {
        float angle = root->mSceneManager->airscrewTopAngle;
        float sintmp = sin(angle);
        float costmp = cos(angle);
        float airscrewRotate[16] = {costmp, sintmp, 0, 0, -sintmp, costmp, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1}; // Z-rotation
        glMultMatrixf(airscrewRotate);
    }
    
    if ((strcmp(node->mName.data,airtail) == 0)||(strcmp(node->mName.data,airtail_edge) == 0)) {
        float angle = root->mSceneManager->airscrewTailAngle;
        float sintmp = sin(angle);
        float costmp = cos(angle);
        float airscrewRotate[16] = {1, 0, 0, 0, 0, costmp, sintmp, 0, 0, -sintmp, costmp, 0, 0, 0, 0, 1}; // X-rotation
        glMultMatrixf(airscrewRotate);
    }

    for(unsigned i = 0; i < node->mNumMeshes; i++){
        aiMesh* mesh = scene.mScene->mMeshes[node->mMeshes[i]];
        GL_CHECK(setMaterial(scene.mScene, mesh, shader))
        GL_CHECK(setTextures(scene.mTexture, mesh, shader))
        GL_CHECK(setMeshData(mesh, shader))
        GL_CHECK(glDrawElements(GL_TRIANGLES, scene.mIndexBuffer[node->mMeshes[i]].size(),
                 GL_UNSIGNED_INT, &(scene.mIndexBuffer[node->mMeshes[i]][0])))
    }

    for(unsigned i = 0; i < node->mNumChildren; i++){
        renderAirplane(node->mChildren[i], scene, shader);
    }

    GL_CHECK(glPopMatrix())
}

void RenderManager::renderNode(aiNode* node, SceneNode& scene, Shader* shader)
{
    GL_CHECK(glPushMatrix())
    aiMatrix4x4 &m = node->mTransformation;
    GLfloat modelMatrix [] = {m.a1, m.b1, m.c1, m.d1,
                              m.a2, m.b2, m.c2, m.d2,
                              m.a3, m.b3, m.c3, m.d3,
                              m.a4, m.b4, m.c4, m.d4};
    GL_CHECK(glMatrixMode(GL_MODELVIEW))
    GL_CHECK(glMultMatrixf(modelMatrix))

    for(unsigned i = 0; i < node->mNumMeshes; i++){
        aiMesh* mesh = scene.mScene->mMeshes[node->mMeshes[i]];
        // TODO: Use Octree to cull meshes here
        GL_CHECK(setMaterial(scene.mScene, mesh, shader))
        GL_CHECK(setTextures(scene.mTexture, mesh, shader))
        GL_CHECK(setMeshData(mesh, shader))
        GL_CHECK(glDrawElements(GL_TRIANGLES, /*3*mesh->mNumFaces*/ scene.mIndexBuffer[node->mMeshes[i]].size(),
                 GL_UNSIGNED_INT, &(scene.mIndexBuffer[node->mMeshes[i]][0])))
    }

    for(unsigned i = 0; i < node->mNumChildren; i++){
        renderNode(node->mChildren[i], scene, shader);
    }

    GL_CHECK(glPopMatrix())
}

void RenderManager::setMaterial(const aiScene* scene, aiMesh* mesh, Shader* shader)
{
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    aiColor3D color;
    GLint ambient, diffuse, specular, shininess;

    if(shader->useAmbient){
        GL_CHECK(ambient = glGetUniformLocation(shader->programID(), "Ka"))
        material->Get(AI_MATKEY_COLOR_AMBIENT, color);
        GL_CHECK(glUniform3f(ambient, color.r, color.g, color.b))
    }

    if(shader->useDiffuse){
        GL_CHECK(diffuse = glGetUniformLocation(shader->programID(), "Kd"))
        material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        GL_CHECK(glUniform3f(diffuse, color.r, color.g, color.b))
    }

    if(shader->useSpecular){
        // set Ks
        GL_CHECK(specular = glGetUniformLocation(shader->programID(), "Ks"))
        material->Get(AI_MATKEY_COLOR_SPECULAR, color);
        GL_CHECK(glUniform3f(specular, color.r, color.g, color.b))
        // set alpha
        GL_CHECK(shininess = glGetUniformLocation(shader->programID(), "alpha"))
        float value;
        if (AI_SUCCESS == material->Get(AI_MATKEY_SHININESS, value)) {
            GL_CHECK(glUniform1f(shininess, value))
        } else {
            GL_CHECK(glUniform1f(shininess, 30))
        }
    }
}

void RenderManager::setTextures(vector<TextureSet> &textures, aiMesh* mesh, Shader* shader)
{
    TextureSet& tex = textures[mesh->mMaterialIndex];
    // printf("    mMaterialIndex = %d\n", mesh->mMaterialIndex);
    GLint diffuse, specular, normal, shadow, cube, inverseview;

    if(shader->useDiffuse){
        GL_CHECK(diffuse = glGetUniformLocation(shader->programID(), "diffuseMap"))
        GL_CHECK(glUniform1i(diffuse, 0))
        GL_CHECK(glActiveTexture(GL_TEXTURE0))
        if(tex.diffuseExist){
            tex.diffuse.Bind();
            GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR))
            GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR))
            GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT))
            GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT))
        }
        else{
            GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0))
        }
    }

    if(shader->useSpecular){
        GL_CHECK(specular = glGetUniformLocation(shader->programID(), "specularMap"))
        GL_CHECK(glUniform1i(specular, 1))
        GL_CHECK(glActiveTexture(GL_TEXTURE1))
        if(tex.specularExist){
            tex.specular.Bind();
            GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR))
            GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR))
            GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT))
            GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT))
        }
        else{
            GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0))
        }
    }

    if(shader->useNormal){
        GL_CHECK(normal = glGetUniformLocation(shader->programID(), "normalMap"))
        GL_CHECK(glUniform1i(normal, 2))
        GL_CHECK(glActiveTexture(GL_TEXTURE2))
        if(tex.normalExist){
            tex.normal.Bind();
            GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR))
            GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR))
            GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT))
            GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT))
        }
        else{
            GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0))
        }
    }

    if(shader->useShadow){
        GL_CHECK(shadow = glGetUniformLocation(shader->programID(), "shadowMap"))
        GL_CHECK(glUniform1i(shadow, 7))
        GL_CHECK(glActiveTexture(GL_TEXTURE7))
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, depthBuffer->textureID()))
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE))
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE))
        
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST))
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST))
    }

    if(shader->useCube){
        GL_CHECK(cube = glGetUniformLocation(shader->programID(), "cubeMap"))
        GL_CHECK(glUniform1i(cube, 3))
        GL_CHECK(glActiveTexture(GL_TEXTURE3))
        GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, tColorCubeMap))
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR))
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR))

        GL_CHECK(inverseview = glGetUniformLocation(shader->programID(), "inverseView"))
        GL_CHECK(glUniformMatrix4fv(inverseview, 1, false, inverseView))
    }

}

void RenderManager::setMeshData(aiMesh* mesh, Shader* shader)
{
    GLint position, texcoord, normal, tangent, bitangent;

    if(shader->usePosition){
        // Get a handle to the variables for the vertex data inside the shader.
        GL_CHECK(position = glGetAttribLocation(shader->programID(), "positionIn"))
        GL_CHECK(glEnableVertexAttribArray(position))
        GL_CHECK(glVertexAttribPointer(position, 3, GL_FLOAT, 0, sizeof(aiVector3D), mesh->mVertices))
    }

    if(shader->useDiffuse || shader->useSpecular){
        // Texture coords.  Note the [0] at the end, very important
        GL_CHECK(texcoord = glGetAttribLocation(shader->programID(), "texcoordIn"))
        GL_CHECK(glEnableVertexAttribArray(texcoord))
        GL_CHECK(glVertexAttribPointer(texcoord, 2, GL_FLOAT, 0, sizeof(aiVector3D), mesh->mTextureCoords[0]))
    }

    if(shader->useNormal){
        // Normals
        GL_CHECK(normal = glGetAttribLocation(shader->programID(), "normalIn"))
        GL_CHECK(glEnableVertexAttribArray(normal))
        GL_CHECK(glVertexAttribPointer(normal, 3, GL_FLOAT, 0, sizeof(aiVector3D), mesh->mNormals))
    }

    if(shader->useTangent){
        // Tangent
        GL_CHECK(tangent = glGetAttribLocation(shader->programID(), "tangentIn"))
        GL_CHECK(glEnableVertexAttribArray(tangent))
        GL_CHECK(glVertexAttribPointer(tangent, 3, GL_FLOAT, 0, sizeof(aiVector3D), mesh->mTangents))
    }

    if(shader->useBitangent){
        // Bitangent
        GL_CHECK(bitangent = glGetAttribLocation(shader->programID(), "bitangentIn"))
        GL_CHECK(glEnableVertexAttribArray(bitangent))
        GL_CHECK(glVertexAttribPointer(bitangent, 3, GL_FLOAT, 0, sizeof(aiVector3D), mesh->mBitangents))
    }
}

void RenderManager::loadShaders()
{
    for(int i = 0; i < root->mNumOfShaders; i++){
        char shaderPath[BUFFER_SIZE];
        char relativePath[BUFFER_SIZE];
        char shaderName[18];
        sprintf(shaderName, "Shader %d", i);
        GetPrivateProfileString(shaderName, "FileName", "", relativePath, BUFFER_SIZE, root->mConfigFilename.c_str());
        strcpy(shaderPath, root->mRootPath.c_str());
        strcat(shaderPath, relativePath);
        shaders[i] = new Shader(shaderPath);
        if(!shaders[i]->loaded()) {
            std::cerr << "shader[" << i << "] failed to load" << std::endl;
            std::cerr << shaders[i]->errors() << std::endl;
            exit(-1);
        }
        printf("[RenderManager] shader %d loaded\n", i);
        shaders[i]->initilize(shaderName, root->mConfigFilename.c_str());
    }
}

void RenderManager::setCamera() {
    // Set up the projection and model-view matrices
    GLfloat aspectRatio = (GLfloat)window.GetWidth()/window.GetHeight();
    GLfloat nearClip = 0.1f;
    GLfloat farClip = 500.0f;
    GLfloat fieldOfView = 45.0f; // Degrees
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fieldOfView, aspectRatio, nearClip, farClip);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(root->eye.x, root->eye.y, root->eye.z, 
              root->target.x, root->target.y, root->target.z, 
              root->up.x, root->up.y, root->up.z);

    GLfloat v[16];
    GL_CHECK(glGetFloatv(GL_MODELVIEW_MATRIX, v))
    aiMatrix4x4 vm(v[0],v[4],v[8],v[12],
                   v[1],v[5],v[9],v[13],
                   v[2],v[6],v[10],v[14],
                   v[3],v[7],v[11],v[15]);
    vm.Inverse();
    inverseView[0] = vm.a1; inverseView[4] = vm.a2; inverseView[8]  = vm.a3; inverseView[12] = vm.a4;
    inverseView[1] = vm.b1; inverseView[5] = vm.b2; inverseView[9]  = vm.b3; inverseView[13] = vm.b4;
    inverseView[2] = vm.c1; inverseView[6] = vm.c2; inverseView[10] = vm.c3; inverseView[14] = vm.c4;
    inverseView[3] = vm.d1; inverseView[7] = vm.d2; inverseView[11] = vm.d3; inverseView[15] = vm.d4;
}

void RenderManager::setCameraDOF(float xoff, float yoff, float focus) {
    // Set up the projection and model-view matrices
    GLfloat aspectRatio = (GLfloat)window.GetWidth()/window.GetHeight();
    GLfloat nearClip = 0.1f;
    GLfloat farClip = 500.0f;
    GLfloat fieldOfView = 45.0f; // Degrees

    float top = tan(fieldOfView*Pi/360.f)*nearClip;
    float bottom = -top;
    float left  = aspectRatio*bottom;
    float right = -left;
    float nearVal = nearClip;
    float farVal  = farClip;

    aiVector3D view = root->target - root->eye;
    xoff  = 0.03f;
    yoff  = 0.03f;
    focus = view.Length();

    switch(renderPosition){
        case 0: xoff =  0.0f; yoff =  0.0f; break;
        case 1: xoff =  xoff; yoff =  yoff; break;
        case 2: xoff =  xoff; yoff = -yoff; break;
        case 3: xoff = -xoff; yoff =  yoff; break;
        case 4: xoff = -xoff; yoff = -yoff; break;
        default: break;
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // gluPerspective(fieldOfView, aspectRatio, nearClip, farClip);
    // glFrustum(left, right, bottom, top, nearVal, farVal);
    glFrustum(left   - xoff*nearVal/focus, 
              right  - xoff*nearVal/focus,
              bottom - yoff*nearVal/focus,
              top    - yoff*nearVal/focus,
              nearVal, farVal);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(-xoff, -yoff, 0.0f);
    gluLookAt(root->eye.x, root->eye.y, root->eye.z, 
              root->target.x, root->target.y, root->target.z, 
              root->up.x, root->up.y, root->up.z);

    GLfloat v[16];
    GL_CHECK(glGetFloatv(GL_MODELVIEW_MATRIX, v))
    aiMatrix4x4 vm(v[0],v[4],v[8],v[12],
                   v[1],v[5],v[9],v[13],
                   v[2],v[6],v[10],v[14],
                   v[3],v[7],v[11],v[15]);
    vm.Inverse();
    inverseView[0] = vm.a1; inverseView[4] = vm.a2; inverseView[8]  = vm.a3; inverseView[12] = vm.a4;
    inverseView[1] = vm.b1; inverseView[5] = vm.b2; inverseView[9]  = vm.b3; inverseView[13] = vm.b4;
    inverseView[2] = vm.c1; inverseView[6] = vm.c2; inverseView[10] = vm.c3; inverseView[14] = vm.c4;
    inverseView[3] = vm.d1; inverseView[7] = vm.d2; inverseView[11] = vm.d3; inverseView[15] = vm.d4;
}

void RenderManager::setLight() {

    for(int i = 0; i < root->mNumOfLights; i++){
        glLightfv(GL_LIGHT0+i, GL_AMBIENT, root->light[i].ambient);
        glLightfv(GL_LIGHT0+i, GL_DIFFUSE, root->light[i].diffuse);
        glLightfv(GL_LIGHT0+i, GL_SPECULAR, root->light[i].specular);
        glLightfv(GL_LIGHT0+i, GL_POSITION, root->light[i].position);
    }
}

void RenderManager::setCameraForCube(GLuint face)
{
    GLfloat target[6][3] = {{1,0,0}, {-1,0,0}, {0,1,0}, 
                        {0,-1,0}, {0,0,1}, {0,0,-1}};
    GLfloat up[6][3] = {{0,-1,0}, {0,-1,0}, {0,0,1},
                        {0,0,-1}, {0,-1,0}, {0,-1,0}};

    // Set up the projection and model-view matrices
    GLfloat aspectRatio = 1.0f;
    GLfloat nearClip = 0.0001f;
    GLfloat farClip = 500.0f;
    GLfloat fieldOfView = 90.0f; // Degrees

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fieldOfView, aspectRatio, nearClip, farClip);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    GLfloat eye_x = 0.f;
    GLfloat eye_y = 2.f;
    GLfloat eye_z = 0.f;
    gluLookAt(eye_x, eye_y, eye_z, 
              eye_x+target[face][0], eye_y+target[face][1], eye_z+target[face][2],
              up[face][0], up[face][1], up[face][2]);
}

void RenderManager::renderCubeFaces(SceneNode &scene)
{
    unsigned cubeSize = 800;
    GL_CHECK(glUseProgram(shaders[1]->programID()))
    GL_CHECK(glEnable(GL_TEXTURE_CUBE_MAP))
    GL_CHECK(glGenTexturesEXT(1, &tColorCubeMap))
    GL_CHECK(glActiveTexture(GL_TEXTURE4))
    GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, tColorCubeMap))
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE))
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE))
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE))
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR))
    GL_CHECK(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR))

    for(GLuint face = 0; face < 6; face++){
        GL_CHECK(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+face, 0, GL_RGBA8, 
            cubeSize, cubeSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL))
        GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT))
        setCameraForCube(face);
        GL_CHECK(setLight())
        GL_CHECK(glViewport(0,0,cubeSize,cubeSize))
        renderNode(scene.mScene->mRootNode, scene, shaders[1]);
        GL_CHECK(glActiveTexture(GL_TEXTURE4))
        GL_CHECK(glBindTexture(GL_TEXTURE_CUBE_MAP, tColorCubeMap))
        GL_CHECK(glCopyTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+face,0,0,0,0,0,cubeSize,cubeSize))
    }

}

void RenderManager::renderSkyBox()
{
    GL_CHECK(glUseProgram(0))
    
    GL_CHECK(glViewport(0,0,window.GetWidth(),window.GetHeight()))
    GL_CHECK(glMatrixMode(GL_MODELVIEW))
    GL_CHECK(glPushMatrix())
    GL_CHECK(setCamera())
    GL_CHECK(glLoadIdentity())
    GL_CHECK(gluLookAt(0,0,0, root->target.x-root->eye.x, root->target.y-root->eye.y, root->target.z-root->eye.z, 0,1,0))

    GL_CHECK(glPushAttrib(GL_ENABLE_BIT))
    GL_CHECK(glEnable(GL_TEXTURE_2D))
    //GL_CHECK(glDisable(GL_DEPTH_TEST))
    GL_CHECK(glDepthMask(GL_FALSE))
    //GL_CHECK(glDisable(GL_LIGHTING))
    //GL_CHECK(glDisable(GL_BLEND))
    GL_CHECK(glColor4f(1,1,1,1))

    GL_CHECK(glActiveTexture(GL_TEXTURE0))
    root->mSceneManager->mSkybox[0].Bind();
    GL_CHECK(glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE))
    GL_CHECK(glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE))
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(  0.5f, -0.5f, -0.5f );
        glTexCoord2f(1, 0); glVertex3f( -0.5f, -0.5f, -0.5f );
        glTexCoord2f(1, 1); glVertex3f( -0.5f,  0.5f, -0.5f );
        glTexCoord2f(0, 1); glVertex3f(  0.5f,  0.5f, -0.5f );
    glEnd();

    root->mSceneManager->mSkybox[1].Bind();
    GL_CHECK(glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE))
    GL_CHECK(glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE))
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f(  0.5f, -0.5f,  0.5f );
        glTexCoord2f(1, 0); glVertex3f(  0.5f, -0.5f, -0.5f );
        glTexCoord2f(1, 1); glVertex3f(  0.5f,  0.5f, -0.5f );
        glTexCoord2f(0, 1); glVertex3f(  0.5f,  0.5f,  0.5f );
    glEnd();

    root->mSceneManager->mSkybox[2].Bind();
    GL_CHECK(glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE))
    GL_CHECK(glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE))
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( -0.5f, -0.5f,  0.5f );
        glTexCoord2f(1, 0); glVertex3f(  0.5f, -0.5f,  0.5f );
        glTexCoord2f(1, 1); glVertex3f(  0.5f,  0.5f,  0.5f );
        glTexCoord2f(0, 1); glVertex3f( -0.5f,  0.5f,  0.5f );
 
    glEnd();

    root->mSceneManager->mSkybox[3].Bind();
    GL_CHECK(glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE))
    GL_CHECK(glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE))
    glBegin(GL_QUADS);
        glTexCoord2f(0, 0); glVertex3f( -0.5f, -0.5f, -0.5f );
        glTexCoord2f(1, 0); glVertex3f( -0.5f, -0.5f,  0.5f );
        glTexCoord2f(1, 1); glVertex3f( -0.5f,  0.5f,  0.5f );
        glTexCoord2f(0, 1); glVertex3f( -0.5f,  0.5f, -0.5f );
    glEnd();

    root->mSceneManager->mSkybox[4].Bind();
    GL_CHECK(glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE))
    GL_CHECK(glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE))
    glBegin(GL_QUADS);
        glTexCoord2f(0, 1); glVertex3f( -0.5f,  0.5f, -0.5f );
        glTexCoord2f(0, 0); glVertex3f( -0.5f,  0.5f,  0.5f );
        glTexCoord2f(1, 0); glVertex3f(  0.5f,  0.5f,  0.5f );
        glTexCoord2f(1, 1); glVertex3f(  0.5f,  0.5f, -0.5f );
    glEnd();

    //GL_CHECK(glEnable(GL_LIGHTING))
    GL_CHECK(glDepthMask(GL_TRUE))
    GL_CHECK(glPopAttrib())
    GL_CHECK(glPopMatrix())
}

void RenderManager::renderTriangles(Shader* shader){
    GLint position;
    vector<aiVector3D>& triVertices = root->mSceneManager->triVertices;

    GL_CHECK(glMatrixMode(GL_MODELVIEW))
    GL_CHECK(glPushMatrix())
    GL_CHECK(setCamera())
    GL_CHECK(glLineWidth(1.0f))
    GL_CHECK(position = glGetAttribLocation(shader->programID(), "positionIn"))
    GL_CHECK(glEnableVertexAttribArray(position))
    GL_CHECK(glVertexAttribPointer(position, 3, GL_FLOAT, 0, sizeof(aiVector3D), &triVertices[0]))
    //GL_CHECK(glVertexAttribPointer(position, 3, GL_FLOAT, 0, 0, &triVertices[0]))

    /*
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    for(int i = 0; i < tree->intersected.size(); i++){
        GL_CHECK(glDrawArrays(GL_TRIANGLES, tree->intersected[i]*3, 1))
    }
    */

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, triVertices.size()))
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    /*
    // draw the forefront
    GL_CHECK(glPointSize(7.0f))
    GL_CHECK(glVertexAttribPointer(position, 3, GL_FLOAT, 0, sizeof(aiVector3D), &center))
    GL_CHECK(glDrawArrays(GL_POINTS, 0, 1))
    */

    GL_CHECK(glPopMatrix())

}

void RenderManager::renderParticles(Shader* shader){

    GL_CHECK(glEnable(GL_POINT_SPRITE))
    GL_CHECK(glEnable(GL_VERTEX_PROGRAM_POINT_SIZE))
    GL_CHECK(glDepthMask(GL_FALSE))
    GL_CHECK(glUseProgram(shader->programID()))

    GLuint position, map, alpha, baseSize;
    GL_CHECK(map = glGetUniformLocation(shader->programID(), "particleMap"))
    GL_CHECK(glUniform1i(map, 4))
    GL_CHECK(position = glGetAttribLocation(shader->programID(), "positionIn"))
    GL_CHECK(alpha = glGetAttribLocation(shader->programID(), "alphaIn"))
    GL_CHECK(baseSize = glGetUniformLocation(shader->programID(), "baseSize"))

    /*************** Render Canon ****************/
    unsigned numCannonParticles = root->mSceneManager->mCannonParticles.size();
    if(numCannonParticles > 0){
        GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE))
        GL_CHECK(glActiveTexture(GL_TEXTURE4))
        root->mSceneManager->mParticleTypes[0].Bind();
        GL_CHECK(glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE))
        GL_CHECK(glEnableVertexAttribArray(position))
        GL_CHECK(glEnableVertexAttribArray(alpha))
        GL_CHECK(glUniform1f(baseSize,  root->mSceneManager->mParticleTypes[0].size))
        GL_CHECK(glVertexAttribPointer(position, 3, GL_FLOAT, 0, sizeof(CannonParticle), &(root->mSceneManager->mCannonParticles[0].position)))
        GL_CHECK(glVertexAttribPointer(alpha, 3, GL_FLOAT, 0, sizeof(CannonParticle), &(root->mSceneManager->mCannonParticles[0].alpha)))
        GL_CHECK(glDrawArrays(GL_POINTS, 0, numCannonParticles))
    }

    /*************** Render Fire ****************/
    unsigned numFireParticles = root->mSceneManager->mFireParticles.size();
    if(numFireParticles > 0){
        GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE))
        GL_CHECK(glActiveTexture(GL_TEXTURE4))
        root->mSceneManager->mParticleTypes[1].Bind();
        GL_CHECK(glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE))
        GL_CHECK(glEnableVertexAttribArray(position))
        GL_CHECK(glEnableVertexAttribArray(alpha))
        GL_CHECK(glUniform1f(baseSize,  root->mSceneManager->mParticleTypes[1].size))
        GL_CHECK(glVertexAttribPointer(position, 3, GL_FLOAT, 0, sizeof(FireParticle), &(root->mSceneManager->mFireParticles[0].position)))
        GL_CHECK(glVertexAttribPointer(alpha, 3, GL_FLOAT, 0, sizeof(FireParticle), &(root->mSceneManager->mFireParticles[0].alpha)))
        GL_CHECK(glDrawArrays(GL_POINTS, 0, numFireParticles))
    }


    GL_CHECK(glDisable(GL_POINT_SPRITE))
    GL_CHECK(glDisable(GL_VERTEX_PROGRAM_POINT_SIZE))
    GL_CHECK(glDisable(GL_BLEND))
    GL_CHECK(glDepthMask(GL_TRUE))
    GL_CHECK(glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_FALSE))

}

void RenderManager::renderBoundingBox(Shader* shader)
{
    GLint position;

    GL_CHECK(glMatrixMode(GL_MODELVIEW))
    GL_CHECK(glPushMatrix())
    GL_CHECK(setCameraDOF())
    GL_CHECK(glUseProgram(shader->programID()))
    GL_CHECK(glViewport(0, 0, window.GetWidth(), window.GetHeight()))

    GL_CHECK(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE))
    GL_CHECK(glLineWidth(1.0f))
    GL_CHECK(position = glGetAttribLocation(shader->programID(), "positionIn"))
    GL_CHECK(glEnableVertexAttribArray(position))
    GL_CHECK(glVertexAttribPointer(position, 3, GL_FLOAT, 0, sizeof(aiVector3D), &(root->mSceneManager->bbEdges[0])))
    GL_CHECK(glDrawArrays(GL_LINES, 0, root->mSceneManager->bbEdges.size()))
    //GL_CHECK(glVertexAttribPointer(position, 3, GL_FLOAT, 0, sizeof(aiVector3D), &tri[0]))
    //GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, tri.size()))
    GL_CHECK(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL))

    /*
    // draw fore front
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    GL_CHECK(glPointSize(7.0f))
    GL_CHECK(glVertexAttribPointer(position, 3, GL_FLOAT, 0, sizeof(aiVector3D), &center))
    GL_CHECK(glDrawArrays(GL_POINTS, 0, 1))
    */

    GL_CHECK(glPopMatrix())
}

void RenderManager::renderText()
{
    // window.PreserveOpenGLStates(true);
    GL_CHECK(glActiveTexture(GL_TEXTURE0))
    GL_CHECK(glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE))
    GL_CHECK(glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE))
    GL_CHECK(glUseProgram(0))
    //GL_CHECK(glDepthMask(GL_FALSE))
    //GL_CHECK(glDisable(GL_DEPTH_TEST))

    // score
    char score[17];
    sprintf(score, "Score %d", root->mPlayerScore);
    sf::String scoreText(score, root->mFont, 50.f);
    scoreText.SetColor(sf::Color(0,0,255));
    scoreText.Move(800.f, 700.f);

    // time
    char time[9]; // 00:00:00
    if(root->mSecond < 10)
        sprintf(time, "%d:0%d", root->mMinute, root->mSecond);
    else
        sprintf(time, "%d:%d", root->mMinute, root->mSecond);
    sf::String timeText(time, root->mFont, 50.f);
    timeText.SetColor(sf::Color(255,0,0));
    timeText.Move(WINDOW_WIDTH/2-60, 0.f);

    // level
    char level[9]; // Level 10
    sprintf(level, "Level %d", root->mLevel);
    sf::String levelText(level, root->mFont, 50.f);
    levelText.SetColor(sf::Color(0,255,0));
    levelText.Move(WINDOW_WIDTH/2-90, 700.f);

    window.Draw(scoreText);
    window.Draw(timeText);
    window.Draw(levelText);

    //GL_CHECK(glEnable(GL_DEPTH_TEST))
    //GL_CHECK(glDepthMask(GL_TRUE))
}

void RenderManager::renderOcean()
{
    this->setCameraDOF();
    GL_CHECK(glEnable(GL_DEPTH_TEST))
    GL_CHECK(glDepthMask(GL_TRUE))
    aiVector3D v = root->target - root->eye;
    float r_l_angle; 
    if(v.z>0)
        r_l_angle = atan(-v.x/v.z)/Pi*180.f;
    else
        r_l_angle = atan(-v.x/v.z)/Pi*180.f + 180.f;
    oceanRender->render_ocean(0.01f, vector3(root->eye.x, root->eye.y, root->eye.z), r_l_angle);
}

void RenderManager::initializeCamera()
{
    aiVector3D v = root->target - root->eye;
    float lenxz = sqrt(v.x*v.x+v.z*v.z);
    float lenxy = sqrt(v.x*v.x+v.y*v.y);
    float yaw = asin(v.z/lenxz);
    float pitch = - asin(v.y/lenxy);
    root->eye.x = root->target.x - fabs(v.x)/v.x*lenxz*cos(yaw);
    root->eye.y = root->target.y + lenxy*sin(pitch);
    root->eye.z = root->target.z - lenxz*sin(yaw);
}

