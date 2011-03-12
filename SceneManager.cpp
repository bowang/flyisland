#include "SceneManager.h"
#include "Root.h"

SceneManager::SceneManager(Root* root)
{
    this->root = root;
    mSceneNodes.resize(root->mNumOfScene);
    airscrewTopAngle = 0.0f;
    airscrewTailAngle = 0.0f;
}

char skyName[5][6] = {"Front", "Left", "Back", "Right", "Top"};

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

        mSceneNodes[i].initialize(i, root->mConfigFileName.c_str());
    }

    // build kdTree
    
    int j;
    if(root->mEnableAirplane) j = 2;
    else j = 0;
    for( ; j < root->mNumOfScene; j++){
        if(mSceneNodes[j].fixed){
            genTriangleVertices(mSceneNodes[j]);
            printf("[SceneManager] generate triangles for scene[%d]\n", j);
        }
    }
    kdtree = new kdTree(triVertices, triNormals);
    printf("[SceneManager] kdTree is built.\n");

    // load images for skybox
    for(int i = 0; i < 5; i++){
        char fileName[BUFFER_SIZE];
        char skyPath[2*BUFFER_SIZE];
        GetPrivateProfileString("Sky", skyName[i], "", fileName, BUFFER_SIZE, root->mConfigFileName.c_str());
        strcpy(skyPath, root->mRootPath.c_str());    strcat(skyPath, fileName);
        bool success = mSkybox[i].LoadFromFile(skyPath);
        if(!success){
            printf("Failed to load skybox (top) from %s\n", skyPath);
            exit(-1);
        }
        else printf("[SceneManager] Loading sky(top): %s\n", skyPath);
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

    if(root->mEnableAirplane){
        root->airplane = &(mSceneNodes[0]);
        flyDirection.x = GetPrivateProfileFloat("Initialization", "flyDirectionX", 1.0f, root->mConfigFileName.c_str());
        flyDirection.y = GetPrivateProfileFloat("Initialization", "flyDirectionY", 0.0f, root->mConfigFileName.c_str());
        flyDirection.z = GetPrivateProfileFloat("Initialization", "flyDirectionZ", 0.0f, root->mConfigFileName.c_str());
        flyDirection.x += root->airplane->mPosition.x;
        flyDirection.y += root->airplane->mPosition.y;
        flyDirection.z += root->airplane->mPosition.z;
        flySpeed      = GetPrivateProfileFloat("Initialization", "flySpeed", 1.0f, root->mConfigFileName.c_str());
        rotationSpeed = GetPrivateProfileFloat("Initialization", "rotationSpeed", 11.0f, root->mConfigFileName.c_str());
    }

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

    airplaneClock.Reset();
    cameraClock.Reset();
    collisionClock.Reset();
}

void SceneManager::updateWorld()
{
    // update airplane
    if(root->mEnableAirplane && airplaneClock.GetElapsedTime() > 0.001f){
        updateAirplane();
        airplaneClock.Reset();
    }

    // update camera
    if(root->mEnableAirplane && cameraClock.GetElapsedTime() > 0.001f){
        if(root->viewMode==FollowView){
            aiVector3D d = root->target - root->airplane->mPosition;
            root->target = root->airplane->mPosition;
            root->eye -= d;
        }
        else if(root->viewMode==FlyView){
            aiVector3D v = flyDirection - root->airplane->mPosition;
            v.Normalize();
            root->target = root->airplane->mPosition;
            root->eye = root->airplane->mPosition - v*10.f;
        }
        cameraClock.Reset();
    }

}

void SceneManager::updateAirplane()
{
    // translate airplane and airscrew
    aiVector3D v = flyDirection - root->airplane->mPosition;
    v.Normalize();
    aiVector3D h = aiVector3D(v.x, 0.0f, v.z);
    h.Normalize();
    pitchAxis = cross(v, root->up);
    pitch = 0.0f;
    if(v.y != 0.0f){
        pitch = fabs(v.y)/v.y*acos(dot(v,h))/Pi*180.0f;
    }
    yawAxis = root->up;
    yaw = 0.0f;
    if(v.z != 0.0f)
        yaw = -fabs(v.z)/v.z*acos(dot(h,aiVector3D(1.f,0.f,0.f)))/Pi*180.0f;

    float speed = flySpeed;
    if(root->mHighSpeed) speed *= 7;
    v *= speed;

    // collision detection
    if(root->mEnableAirplane && collisionClock.GetElapsedTime() > 0.001f){    

        aiVector3D position = root->airplane->mPosition;
        aiVector3D newPosition = position + 2.f*v;
        int triIdx = kdtree->findIntersection(kdtree->root, position, newPosition);
        if(triIdx >= 0){
            float dist = kdtree->intersectDist(triIdx, position, v);
            PRINTF("[SceneManager] collision detection: triIdx=%d dist=%f\n", triIdx, dist);
            if(dist < max(2.f*v.Length(), 1.f)){
                root->mAirplaneCrash = true;
                printf("[Info] Airplane crashed!\n");
            }
        }
        collisionClock.Reset();
    }

    if(!root->mAirplaneCrash){
        root->airplane->mPosition += v;
        flyDirection += v;

        // rotate airscrew
        airscrewTopAngle += rotationSpeed/180.f*Pi;
        airscrewTailAngle += rotationSpeed/180.f*Pi;

        // rotate airplane
        // root->airplane->setRotateAxis(v);
        // root->airplane->rotate(-yaw);
    }

}

void SceneManager::genTriangleVertices(SceneNode& scene){

    aiMatrix4x4 translate, rotate, scale;
    translate.Translation(scene.mPosition, translate);
    //rotate.Rotation(scene.mRotateAngle/180.f*Pi, scene.mRotate, rotate);
    rotate.Rotation(scene.mRotateAngle/180.f*Pi, aiVector3D(0.0f,0.0f,1.0f), rotate);
    scale.Scaling(scene.mScale,scale);
    loadIdentity(modelMatrix);
    modelMatrix *= translate;
    modelMatrix *= rotate;
    modelMatrix *= scale;
    genTriangle(scene.mScene, scene.mScene->mRootNode);
}

void SceneManager::genTriangle(const aiScene* scene, aiNode* node){

    modelMatrixStack.push(modelMatrix);
    modelMatrix = node->mTransformation * modelMatrix;
    aiMatrix4x4 m = modelMatrix;
    m.Inverse().Transpose();

    for(unsigned i = 0; i < node->mNumMeshes; i++){
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        for(unsigned j = 0; j < mesh->mNumFaces; j++){
            aiFace& face = mesh->mFaces[j];
            aiVector3D p1 = modelMatrix * mesh->mVertices[face.mIndices[0]];
            aiVector3D p2 = modelMatrix * mesh->mVertices[face.mIndices[1]];
            aiVector3D p3 = modelMatrix * mesh->mVertices[face.mIndices[2]];
            triVertices.push_back(p1);
            triVertices.push_back(p2);
            triVertices.push_back(p3);
            aiVector3D n  = cross((p2-p1),(p3-p2));
            n.Normalize();
            triNormals.push_back(n);
        }
    }

    for(unsigned i = 0; i < node->mNumChildren; i++){
        genTriangle(scene, node->mChildren[i]);
    }

    modelMatrix = modelMatrixStack.top();
    modelMatrixStack.pop();
}


