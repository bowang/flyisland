#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "Framework.h"
#include "SceneNode.h"
#include "BoundingBox.h"
#include "kdTree.h"
#include <stack>

class Root;

class SceneManager
{
public:
    SceneManager(Root* root);
    void loadAssets();
    void initializeWorld();
    void updateWorld();
    vector<SceneNode> mSceneNodes;
    sf::Image mSkybox[5];

    vector<BoundingBox> mBoundingBox;
    std::vector<aiVector3D> triVertices;
    std::vector<aiVector3D> triNormals;
    kdTree* kdtree;

    float rotationSpeed;
    float flySpeed;
    aiVector3D flyDirection;
    aiVector3D pitchAxis;
    aiVector3D yawAxis;
    float pitch;
    float yaw;

private:
    Root* root;
    sf::Clock airplaneClock;
    sf::Clock cameraClock;
    sf::Clock collisionClock;
    std::stack<aiMatrix4x4> modelMatrixStack;
    aiMatrix4x4 modelMatrix;

    void updateAirplane();
    void genTriangleVertices(SceneNode& scene);
    void genTriangle(const aiScene* scene, aiNode* node);

protected:


};


#endif
