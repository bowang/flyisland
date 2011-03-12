#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "Framework.h"
#include "SceneNode.h"
#include "kdTree.h"
#include "Particle.h"
#include "CannonParticle.h"
#include <stack>

class Root;

class SceneManager
{
public:
    SceneManager(Root* root);
    void loadAssets();
    void initializeWorld();
    void updateWorld();
    void genCannonParticle();
    void buildBoundingBoxBuffer();

    vector<SceneNode> mSceneNodes;
    vector<Particle>  mParticleTypes;
    vector<CannonParticle> mCannonParticles;
    sf::Image mSkybox[5];

    vector<BoundingBox> mBoundingBox;
    std::vector<aiVector3D> triVertices;
    std::vector<aiVector3D> triNormals;
    kdTree* kdtree;
    vector<aiVector3D> bbEdges;

    float flySpeed;
    aiVector3D flyDirection;
    aiVector3D pitchAxis;
    aiVector3D yawAxis;
    float pitch;
    float yaw;
    float airscrewTopAngle;
    float airscrewTailAngle;

private:
    float rotationSpeed;
    Root* root;
    sf::Clock airplaneClock;
    sf::Clock cameraClock;
    sf::Clock collisionClock;
    sf::Clock cannonClock;
    std::stack<aiMatrix4x4> modelMatrixStack;
    aiMatrix4x4 modelMatrix;
    vector<BoundingBox> originalBoundingBox;
    float min_x, min_y, min_z,
          max_x, max_y, max_z;

    void updateAirplane();
    void updateParticles();
    void updateBoundingBox();
    void updateTargets();
    void genTriangleVertices(SceneNode& scene);
    void genTriangle(const aiScene* scene, aiNode* node);
    void genBoundingBox(int j);
    void findBox(aiNode* node, const aiScene* scene);
    void testMaxMin(aiVector3D& v);

protected:


};


#endif
