#ifndef KDTREE_H
#define KDTREE_H

#include <algorithm>
#include <vector>

#include "Framework.h"
#include "Utility.h"
#include "BoundingBox.h"

struct TreeNode{
    TreeNode():left(0),right(0) {}
    BoundingBox BB;
    std::vector<BoundingBox> BBList;
    TreeNode *left, *right;
    int axis;
};

class kdTree {
    
public:
    kdTree(std::vector<aiVector3D>& triVertices, std::vector<aiVector3D>& triNormals);
    float detectCollision(aiVector3D& start, aiVector3D& end);
    int findIntersection(TreeNode* current, aiVector3D& start, aiVector3D& end);
    float intersectDist(int idx, aiVector3D& r0, aiVector3D& d);

    TreeNode* root;
    std::vector<aiVector3D>* triangles;
    std::vector<aiVector3D>* normals;

private:
    std::vector<bool> tested;
    BoundingBox genBoundingBoxList(std::vector<aiVector3D>& triVertices, std::vector<BoundingBox>& BBList);
    BoundingBox genBoundingBox(std::vector<BoundingBox>& BBList);
    void splitNode(TreeNode* node, int axis);
    void getRange(aiVector3D* start, aiVector3D* end, int axis, float& min, float& max);
    int longestDim(TreeNode* node);
    int _findIntersection(TreeNode* current, aiVector3D& start, aiVector3D& end);
};

#endif
