#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include "Framework.h"
#include "SceneNode.h"
#include "BoundingBox.h"

class Root;

class SceneManager
{
public:
    SceneManager(Root* root);
    void loadAssets();
    void initializeWorld();
    void updateWorld();
    vector<SceneNode> mSceneNodes;
    vector<BoundingBox> mBoundingBox;
    sf::Image mSkybox[5];

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
    void updateAirplane();

protected:


};


#endif
