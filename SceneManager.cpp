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
        GetPrivateProfileString(sceneName, "FileName", "", modelName, BUFFER_SIZE, root->mConfigFilename.c_str());
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

        mSceneNodes[i].initialize(i, root->mConfigFilename.c_str());
    }
    printf("\n");

    // build kdTree
    int j = 0;
    if(root->mEnableAirplane) j = 1;
    for( ; j < root->mNumOfScene; j++){
        if(mSceneNodes[j].fixed){
            genTriangleVertices(mSceneNodes[j]);
            printf("[SceneManager] generate triangles for scene[%d]\n", j);
        }
    }
    kdtree = new kdTree(triVertices, triNormals);
    printf("[SceneManager] kdTree is built.\n");

    // build bounding box list for target(movable) scenes
    j = 0;
    if(root->mEnableAirplane) j = 1;
    for( ; j < root->mNumOfScene; j++){
        if(mSceneNodes[j].target==true && mSceneNodes[j].hit==false){
            genBoundingBox(j);
            printf("[SceneManager] bounding box built for scene[%d]\n", j);
        }
    }

    // load images for skybox
    for(int i = 0; i < 5; i++){
        char fileName[BUFFER_SIZE];
        char skyPath[2*BUFFER_SIZE];
        GetPrivateProfileString("Sky", skyName[i], "", fileName, BUFFER_SIZE, root->mConfigFilename.c_str());
        strcpy(skyPath, root->mRootPath.c_str());    strcat(skyPath, fileName);
        bool success = mSkybox[i].LoadFromFile(skyPath);
        if(!success){
            printf("Failed to load skybox (top) from %s\n", skyPath);
            exit(-1);
        }
        else printf("[SceneManager] Loading sky(top): %s\n", skyPath);
    }

    // load particles
    mParticleTypes.resize(root->mNumOfParticles);
    for(int i = 0; i < root->mNumOfParticles; i++){
        char fileName[BUFFER_SIZE];
        char particleName[BUFFER_SIZE];
        char particlePath[2*BUFFER_SIZE];
        sprintf(particleName, "Particle %d", i);
        GetPrivateProfileString(particleName, "FileName", "", fileName, BUFFER_SIZE, root->mConfigFilename.c_str());
        strcpy(particlePath, root->mRootPath.c_str());    strcat(particlePath, fileName);
        bool success = mParticleTypes[i].LoadFromFile(particlePath);
        if(success){
            printf("[SceneManager] Particle[%d] loaded: %s\n", i, particlePath);
        }
        else{
            printf("Failed to load particle[%d]\n", i);
        }
        mParticleTypes[i].speed = GetPrivateProfileFloat(particleName, "speed", 2.0f, root->mConfigFilename.c_str());
        mParticleTypes[i].size  = GetPrivateProfileFloat(particleName, "size", 1.0f, root->mConfigFilename.c_str());

    }
}

void SceneManager::genBoundingBox(int sceneIdx)
{
    min_x = FLT_MAX;
    min_y = FLT_MAX;
    min_z = FLT_MAX;
    max_x =-FLT_MAX;
    max_y =-FLT_MAX;
    max_z =-FLT_MAX;

    loadIdentity(modelMatrix);
    findBox(mSceneNodes[sceneIdx].mScene->mRootNode, mSceneNodes[sceneIdx].mScene);
    
    BoundingBox box(min_x, min_y, min_z, max_x, max_y, max_z);
    box.index = sceneIdx;
    originalBoundingBox.push_back(box);
}

void SceneManager::findBox(aiNode* node, const aiScene* scene)
{
    modelMatrixStack.push(modelMatrix);
    modelMatrix = modelMatrix * node->mTransformation;

    for(unsigned i = 0; i < node->mNumMeshes; i++){
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        for(unsigned j = 0; j < mesh->mNumFaces; j++){
            aiFace& face = mesh->mFaces[j];
            aiVector3D p1 = modelMatrix * mesh->mVertices[face.mIndices[0]];
            aiVector3D p2 = modelMatrix * mesh->mVertices[face.mIndices[1]];
            aiVector3D p3 = modelMatrix * mesh->mVertices[face.mIndices[2]];
            testMaxMin(p1);
            testMaxMin(p2);
            testMaxMin(p3);
        }
    }

    for(unsigned i = 0; i < node->mNumChildren; i++){
        findBox(node->mChildren[i], scene);
    }

    modelMatrix = modelMatrixStack.top();
    modelMatrixStack.pop();
}

void SceneManager::testMaxMin(aiVector3D& v)
{
    if(v.x > max_x) max_x = v.x;
    else if(v.x < min_x) min_x = v.x;
    if(v.y > max_y) max_y = v.y;
    else if(v.y < min_y) min_y = v.y;
    if(v.z > max_z) max_z = v.z;
    else if(v.z < min_z) min_z = v.z;
}

void SceneManager::buildBoundingBoxBuffer()
{
    bbEdges.clear();
    for(unsigned i = 0; i < mBoundingBox.size(); i++){
        BoundingBox &bb = mBoundingBox[i];
        
        bbEdges.push_back(aiVector3D(bb.min[0],bb.min[1],bb.min[2]));
        bbEdges.push_back(aiVector3D(bb.max[0],bb.min[1],bb.min[2]));
        bbEdges.push_back(aiVector3D(bb.min[0],bb.min[1],bb.min[2]));
        bbEdges.push_back(aiVector3D(bb.min[0],bb.min[1],bb.max[2]));
        bbEdges.push_back(aiVector3D(bb.min[0],bb.min[1],bb.min[2]));
        bbEdges.push_back(aiVector3D(bb.min[0],bb.max[1],bb.min[2]));

        bbEdges.push_back(aiVector3D(bb.max[0],bb.min[1],bb.max[2]));
        bbEdges.push_back(aiVector3D(bb.min[0],bb.min[1],bb.max[2]));
        bbEdges.push_back(aiVector3D(bb.max[0],bb.min[1],bb.max[2]));
        bbEdges.push_back(aiVector3D(bb.max[0],bb.min[1],bb.min[2]));
        bbEdges.push_back(aiVector3D(bb.max[0],bb.min[1],bb.max[2]));
        bbEdges.push_back(aiVector3D(bb.max[0],bb.max[1],bb.max[2]));

        bbEdges.push_back(aiVector3D(bb.min[0],bb.max[1],bb.max[2]));
        bbEdges.push_back(aiVector3D(bb.min[0],bb.max[1],bb.min[2]));
        bbEdges.push_back(aiVector3D(bb.min[0],bb.max[1],bb.max[2]));
        bbEdges.push_back(aiVector3D(bb.min[0],bb.min[1],bb.max[2]));
        bbEdges.push_back(aiVector3D(bb.min[0],bb.max[1],bb.max[2]));
        bbEdges.push_back(aiVector3D(bb.max[0],bb.max[1],bb.max[2]));

        bbEdges.push_back(aiVector3D(bb.max[0],bb.max[1],bb.min[2]));
        bbEdges.push_back(aiVector3D(bb.min[0],bb.max[1],bb.min[2]));
        bbEdges.push_back(aiVector3D(bb.max[0],bb.max[1],bb.min[2]));
        bbEdges.push_back(aiVector3D(bb.max[0],bb.min[1],bb.min[2]));
        bbEdges.push_back(aiVector3D(bb.max[0],bb.max[1],bb.min[2]));
        bbEdges.push_back(aiVector3D(bb.max[0],bb.max[1],bb.max[2]));
    }
}


void SceneManager::initializeWorld()
{
    root->eye.x = GetPrivateProfileFloat("Initialization", "eyeX", 0.0f, root->mConfigFilename.c_str());
    root->eye.y = GetPrivateProfileFloat("Initialization", "eyeY", 0.0f, root->mConfigFilename.c_str());
    root->eye.z = GetPrivateProfileFloat("Initialization", "eyeZ", 0.0f, root->mConfigFilename.c_str());
    root->target.x = GetPrivateProfileFloat("Initialization", "targetX", -1.0f, root->mConfigFilename.c_str());
    root->target.y = GetPrivateProfileFloat("Initialization", "targetY", 0.0f, root->mConfigFilename.c_str());
    root->target.z = GetPrivateProfileFloat("Initialization", "targetZ", 0.0f, root->mConfigFilename.c_str());
    root->up.x = GetPrivateProfileFloat("Initialization", "upX", 0.0f, root->mConfigFilename.c_str());
    root->up.y = GetPrivateProfileFloat("Initialization", "upY", 1.0f, root->mConfigFilename.c_str());
    root->up.z = GetPrivateProfileFloat("Initialization", "upZ", 0.0f, root->mConfigFilename.c_str());

    if(root->mEnableAirplane){
        root->airplane = &(mSceneNodes[0]);
        flyDirection.x = GetPrivateProfileFloat("Initialization", "flyDirectionX", 1.0f, root->mConfigFilename.c_str());
        flyDirection.y = GetPrivateProfileFloat("Initialization", "flyDirectionY", 0.0f, root->mConfigFilename.c_str());
        flyDirection.z = GetPrivateProfileFloat("Initialization", "flyDirectionZ", 0.0f, root->mConfigFilename.c_str());
        flyDirection.x += root->airplane->mPosition.x;
        flyDirection.y += root->airplane->mPosition.y;
        flyDirection.z += root->airplane->mPosition.z;
        flySpeed      = GetPrivateProfileFloat("Initialization", "flySpeed", 1.0f, root->mConfigFilename.c_str());
        rotationSpeed = GetPrivateProfileFloat("Initialization", "rotationSpeed", 11.0f, root->mConfigFilename.c_str());
    }

    for(int i = 0; i < root->mNumOfLights; i++){
        Light l;
        char lightName[17];
        sprintf(lightName, "Light %d", i);
        l.position[0] = GetPrivateProfileFloat(lightName, "x", 0.0f, root->mConfigFilename.c_str());
        l.position[1] = GetPrivateProfileFloat(lightName, "y", 1.0f, root->mConfigFilename.c_str());
        l.position[2] = GetPrivateProfileFloat(lightName, "z", 0.0f, root->mConfigFilename.c_str());
        l.position[3] = GetPrivateProfileFloat(lightName, "w", 0.0f, root->mConfigFilename.c_str());
        l.ambient[0] = GetPrivateProfileFloat(lightName, "ambientR", 0.1f, root->mConfigFilename.c_str());
        l.ambient[1] = GetPrivateProfileFloat(lightName, "ambientG", 0.1f, root->mConfigFilename.c_str());
        l.ambient[2] = GetPrivateProfileFloat(lightName, "ambientB", 0.1f, root->mConfigFilename.c_str());
        l.ambient[3] = GetPrivateProfileFloat(lightName, "ambientA", 1.0f, root->mConfigFilename.c_str());
        l.diffuse[0] = GetPrivateProfileFloat(lightName, "diffuseR", 0.3f, root->mConfigFilename.c_str());
        l.diffuse[1] = GetPrivateProfileFloat(lightName, "diffuseG", 0.3f, root->mConfigFilename.c_str());
        l.diffuse[2] = GetPrivateProfileFloat(lightName, "diffuseB", 0.3f, root->mConfigFilename.c_str());
        l.diffuse[3] = GetPrivateProfileFloat(lightName, "diffuseA", 1.0f, root->mConfigFilename.c_str());
        l.specular[0] = GetPrivateProfileFloat(lightName, "specularR", 0.2f, root->mConfigFilename.c_str());
        l.specular[1] = GetPrivateProfileFloat(lightName, "specularG", 0.2f, root->mConfigFilename.c_str());
        l.specular[2] = GetPrivateProfileFloat(lightName, "specularB", 0.2f, root->mConfigFilename.c_str());
        l.specular[3] = GetPrivateProfileFloat(lightName, "specularA", 1.0f, root->mConfigFilename.c_str());
        root->light.push_back(l);
    }

    airplaneClock.Reset();
    cameraClock.Reset();
    collisionClock.Reset();
    fireClock.Reset();
}

void SceneManager::updateWorld()
{
    // update airplane
    if(root->mEnableAirplane && airplaneClock.GetElapsedTime() > 0.05f){
        updateAirplane();
        airplaneClock.Reset();
    }

    // update camera
    if(root->mEnableAirplane && cameraClock.GetElapsedTime() > 0.05f){
        if(root->viewMode==FollowView){
            aiVector3D d = root->target - root->airplane->mPosition;
            root->target = root->airplane->mPosition;
            root->eye -= d;
        }
        else if(root->viewMode==FlyView){
            aiVector3D v = flyDirection - root->airplane->mPosition;
            if(v.Length() > EPSILON) v.Normalize();
            root->target = root->airplane->mPosition;
            root->eye = root->airplane->mPosition - v*7.f;
        }
        cameraClock.Reset();
    }

    // update time
    if(timeClock.GetElapsedTime() > 1.f) {
        root->mSecond ++;
        if(root->mSecond == 60){
            root->mSecond = 0;
            root->mMinute ++;
        }
        timeClock.Reset();
    }

    // update particle
    updateParticles();
    
    // update bounding box
    updateBoundingBox();
    // buildBoundingBoxBuffer();

    // update targets
    updateTargets();

}

void SceneManager::updateAirplane()
{
    // translate airplane and airscrew
    aiVector3D v = flyDirection - root->airplane->mPosition;
    if(v.Length() > EPSILON) v.Normalize();
    aiVector3D h = aiVector3D(v.x, 0.0f, v.z);
    if(h.Length() > EPSILON) h.Normalize();
    pitchAxis = cross(v, root->up);
    pitch = 0.0f;
    if(fabs(v.y) > EPSILON){
        pitch = fabs(v.y)/v.y*acos(DOT(v,h))/Pi*180.0f;
    }
    yawAxis = root->up;
    yaw = 0.0f;
    if(fabs(v.z) > EPSILON)
        yaw = -fabs(v.z)/v.z*acos(DOT(h,aiVector3D(1.f,0.f,0.f)))/Pi*180.0f;

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
        if(root->airplane->mPosition.y < -0.5f){
            root->mAirplaneCrash = true;
            // root->airplane->mPosition.y = 0.f;
        }

        // rotate airscrew
        airscrewTopAngle += rotationSpeed/180.f*Pi;
        airscrewTailAngle += rotationSpeed/180.f*Pi;

        // rotate airplane
        // root->airplane->setRotateAxis(v);
        // root->airplane->rotate(-yaw);
    }
}

void SceneManager::updateParticles(){

    // update cannon particles
    if(cannonClock.GetElapsedTime() > 0.05f){
        for(unsigned i = 0; i < mCannonParticles.size(); i++){
            if(!mCannonParticles[i].update(mBoundingBox)){
                mCannonParticles.erase(mCannonParticles.begin()+i);
                i--;
            }
        }
        cannonClock.Reset();
    }

    // update fire particles
    if(fireClock.GetElapsedTime() > 0.05f) {
        for(unsigned i = 0; i < mFireParticles.size(); i++){
            if(!mFireParticles[i].update()){
                mFireParticles.erase(mFireParticles.begin()+i);
                i--;
            }
        }
        fireClock.Reset();
    }
}

void SceneManager::genTriangleVertices(SceneNode& scene){

    aiMatrix4x4 translate, rotate, scale;
    translate.Translation(scene.mPosition, translate);
    rotate.Rotation(scene.mRotateAngle/180.f*Pi, scene.mRotate, rotate);
    scale.Scaling(scene.mScale,scale);
    loadIdentity(modelMatrix);
    modelMatrix *= translate;
    modelMatrix *= rotate;
    modelMatrix *= scale;
    genTriangle(scene.mScene, scene.mScene->mRootNode);
}

void SceneManager::genTriangle(const aiScene* scene, aiNode* node){

    modelMatrixStack.push(modelMatrix);
    modelMatrix = modelMatrix * node->mTransformation;

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

void SceneManager::genCannonParticle()
{
    aiVector3D v = flyDirection - root->airplane->mPosition;
    v.Normalize();
    v *= mParticleTypes[0].speed;
    CannonParticle ball(root->airplane->mPosition, v, root);
    mCannonParticles.push_back(ball);
    for(int i = 1; i < root->mLevel; i++){
        CannonParticle ball(root->airplane->mPosition, v, root);
        ball.position.x += Random(2.f);
        ball.position.y += Random(2.f);
        ball.position.z += Random(2.f);
        mCannonParticles.push_back(ball);
    }
}

void SceneManager::genFireParticle(aiVector3D position)
{
    FireParticle fire(position);
    mFireParticles.push_back(fire);
}

void SceneManager::updateBoundingBox()
{
    mBoundingBox.clear();
    mBoundingBox.reserve(originalBoundingBox.size());
    for(unsigned i = 0; i < originalBoundingBox.size(); i++){
        BoundingBox bb = originalBoundingBox[i];
        SceneNode& scene = mSceneNodes[bb.index];
        if(scene.hit==true) continue;
        aiMatrix4x4 translate, rotate, scale, matrix;
        aiVector3D new_position = aiVector3D(scene.mPosition.x, scene.mPosition.y, scene.mPosition.z);
        translate.Translation(new_position, translate);
        rotate.Rotation(scene.mRotateAngle/180.f*Pi, scene.mRotate, rotate);
        scale.Scaling(scene.mScale,scale);
        matrix *= translate;
        matrix *= rotate;
        matrix *= scale;
        bb.multiMatrix(matrix);
        mBoundingBox.push_back(bb);
    }
}

void SceneManager::updateTargets()
{
    if(targetClock.GetElapsedTime() < 0.05f)
        return;
    else
        targetClock.Reset();

    for(unsigned i = 0; i < mSceneNodes.size(); i++){
        SceneNode &scene = mSceneNodes[i];
        if(scene.target==true){
            if(scene.hit==false){
                if(strcmp(scene.name,"balloon")==0){
                    aiVector3D d = scene.mPosition - root->airplane->mPosition;
                    float dl = d.Length();
                    float dd = dl*dl;
                    // if balloon is too far away from the airplane, destroy it!
                    if(dd > 80000.f) scene.hit = true;
                }
                if(strcmp(scene.name,"ship")==0){
                    // add update for ship here
                    aiVector3D d = scene.mPosition - root->airplane->mPosition;
                    float dl = d.Length();
                    float dd = dl*dl;
                    // if ship is too far away from the airplane, destroy it!
                    if(dd > 80000.f) scene.hit = true;
                }
                scene.mPosition += scene.mVelocity;
            }
            else if(scene.reappearClock.GetElapsedTime() > 10.f){
                if(strcmp(scene.name,"balloon")==0){
                    scene.hit = false;
                    aiVector3D position;
                    position.x = root->airplane->mPosition.x + Random(70.f);
                    position.y = (float)fabs(Random(15.f))+10.f;
                    position.z = root->airplane->mPosition.z + Random(70.f);
                    scene.mPosition = position;
                    aiVector3D distance = root->airplane->mPosition - position;
                    if(distance.Length() > EPSILON) distance.Normalize();
                    scene.mVelocity = distance * flySpeed * min((float)root->mLevel, 10.f);
                    scene.mVelocity.x += Random(0.3f) * flySpeed;
                    scene.mVelocity.z += Random(0.3f) * flySpeed;
                    scene.mVelocity.y = 0.f;
                }
                if(strcmp(scene.name,"ship")==0){
                    // add regeneration for ship here
                    scene.hit = false;
                    scene.mPosition.x = 0.f;
                    scene.mPosition.y = 0.f;
                    scene.mPosition.z = -50.f;
                    scene.mVelocity.x = 0.f;
                    scene.mVelocity.y = 0.f;
                    scene.mVelocity.z = flySpeed;
                }
            }
        }
        if(strcmp(scene.name,"island")==0){
            aiVector3D d = scene.mPosition - root->airplane->mPosition;
            float dl = d.Length();
            float dd = dl*dl;
            if(dd > 320000.f){
                aiVector3D v = flyDirection - root->airplane->mPosition;
                scene.mPosition = root->airplane->mPosition + 300.f*v;
                scene.mPosition.x += Random(100.f);
                scene.mPosition.y = 0.f;
                scene.mPosition.z += Random(300.f);
            }
        }
    }
}

void SceneManager::hitTarget(int i)
{
    if(mSceneNodes[i].hit==false){
        mSceneNodes[i].hit = true;
        mSceneNodes[i].reappearClock.Reset();
        root->mSoundManager->play(Hit);
        root->mPlayerScore += mSceneNodes[i].score;
        int newLevel = root->mPlayerScore/50 + 1;
        if(newLevel > root->mLevel){
            root->mLevel = newLevel;
            root->mSoundManager->play(Upgrade);
        }
        if(root->mLevel == 10){
            root->mSuccess = true;
        }
        genFireParticle(mSceneNodes[i].mPosition);
    }
}
