#include "kdTree.h"

kdTree::kdTree(std::vector<aiVector3D>& triVertices, std::vector<aiVector3D>& triNormals){
    if(triVertices.size()==0){
        root = NULL;
        return;
    }
    root = new TreeNode();
    root->BB = genBoundingBoxList(triVertices, root->BBList);
    root->axis = longestDim(root);
    triangles = &triVertices;
    normals   = &triNormals;
    tested.resize(triVertices.size()/3);
    splitNode(root, root->axis);
}

BoundingBox kdTree::genBoundingBoxList(std::vector<aiVector3D>& triVertices, 
                               std::vector<BoundingBox>& BBList){

    if(triVertices.size()%3!=0){
        printf("number of triangle vertices is not 3X : %d\n", (int)triVertices.size());
        system("pause");
        exit(-1);
    }
    BoundingBox mainBB;
    for(unsigned i = 0; i < triVertices.size(); i+=3){
        BoundingBox bb;
        bb.index = i/3;
        bb.max[0] = bb.min[0] = triVertices[i].x;
        bb.max[1] = bb.min[1] = triVertices[i].y;
        bb.max[2] = bb.min[2] = triVertices[i].z;

        if(triVertices[i+1].x > bb.max[0])
            bb.max[0] = triVertices[i+1].x;
        else if (triVertices[i+1].x < bb.min[0])
            bb.min[0] = triVertices[i+1].x;
        if(triVertices[i+1].y > bb.max[1])
            bb.max[1] = triVertices[i+1].y;
        else if (triVertices[i+1].y < bb.min[1])
            bb.min[1] = triVertices[i+1].y;
        if(triVertices[i+1].z > bb.max[2])
            bb.max[2] = triVertices[i+1].z;
        else if (triVertices[i+1].z < bb.min[2])
            bb.min[2] = triVertices[i+1].z;

        if(triVertices[i+2].x > bb.max[0])
            bb.max[0] = triVertices[i+2].x;
        else if (triVertices[i+2].x < bb.min[0])
            bb.min[0] = triVertices[i+2].x;
        if(triVertices[i+2].y > bb.max[1])
            bb.max[1] = triVertices[i+2].y;
        else if (triVertices[i+2].y < bb.min[1])
            bb.min[1] = triVertices[i+2].y;
        if(triVertices[i+2].z > bb.max[2])
            bb.max[2] = triVertices[i+2].z;
        else if (triVertices[i+2].z < bb.min[2])
            bb.min[2] = triVertices[i+2].z;

        BBList.push_back(bb);
        mainBB.max[0] = (mainBB.max[0]<bb.max[0])? bb.max[0]: mainBB.max[0];
        mainBB.max[1] = (mainBB.max[1]<bb.max[1])? bb.max[1]: mainBB.max[1];
        mainBB.max[2] = (mainBB.max[2]<bb.max[2])? bb.max[2]: mainBB.max[2];
        mainBB.min[0] = (mainBB.min[0]>bb.min[0])? bb.min[0]: mainBB.min[0];
        mainBB.min[1] = (mainBB.min[1]>bb.min[1])? bb.min[1]: mainBB.min[1];
        mainBB.min[2] = (mainBB.min[2]>bb.min[2])? bb.min[2]: mainBB.min[2];
    }
    return mainBB;
}

BoundingBox kdTree::genBoundingBox(std::vector<BoundingBox>& BBList){
    BoundingBox bb;
    for(unsigned i = 0; i < BBList.size(); i++){
        for(unsigned j = 0; j < 3; j++){
            if(bb.max[j] < BBList[i].max[j]) bb.max[j] = BBList[i].max[j];
            if(bb.min[j] > BBList[i].min[j]) bb.min[j] = BBList[i].min[j];
        }
    }
    return bb;
}

void kdTree::splitNode(TreeNode* node, int axis){
    float split_plane = (node->BB.max[axis]+node->BB.min[axis])/2.0f;
    std::vector<BoundingBox> BBLeft, BBRight;
    for(unsigned i = 0; i < node->BBList.size(); i++){
        if(node->BBList[i].max[axis]<=split_plane)
            BBLeft.push_back(node->BBList[i]);
        else if(node->BBList[i].min[axis]>=split_plane)
            BBRight.push_back(node->BBList[i]);
        else{
            BoundingBox bb_left, bb_right;
            bb_left = bb_right = node->BBList[i];
            bb_left.max[axis] = split_plane;
            bb_right.min[axis] = split_plane;
            
            // eliminate empty boxes
            aiVector3D& L1 = (*triangles)[bb_left.index*3];
            aiVector3D& L2 = (*triangles)[bb_left.index*3+1];
            aiVector3D& L3 = (*triangles)[bb_left.index*3+2];
            if(bb_left.isTriangleIntersectBox(L1,L2,L3))
                BBLeft.push_back(bb_left);
            if(bb_right.isTriangleIntersectBox(L1,L2,L3))
                BBRight.push_back(bb_right);
        }
    }
    node->left = new TreeNode();
    node->left->BB = genBoundingBox(BBLeft);
    node->left->BBList = BBLeft;
    
    node->right = new TreeNode();
    node->right->BB = genBoundingBox(BBRight);
    node->right->BBList = BBRight;

    if(BBLeft.size()>=node->BBList.size() || BBRight.size()>=node->BBList.size()){
        BBLeft.clear();
        BBRight.clear();
        return;
    }
    node->BBList.clear();

    if(node->left->BB.volume() > 0.001 /*&& node->left->BB.minLength() > 0.5*/ && node->left->BBList.size() > 1){
        node->left->axis = longestDim(node->left);
        splitNode(node->left, node->left->axis);
    }
    if(node->right->BB.volume() > 0.001 /*&& node->right->BB.minLength() > 0.5*/ && node->right->BBList.size() > 1){
        node->right->axis = longestDim(node->right);
        splitNode(node->right, node->right->axis);
    }
}

float kdTree::detectCollision(aiVector3D& start, aiVector3D& end){
    // intersected.clear();
    int idx = findIntersection(root, start, end);
    if(idx < 0)
        return 100.0f;
    else{
        aiVector3D d = end-start;
        return intersectDist(idx, start, d);
    }
}

int kdTree::findIntersection(TreeNode* current, aiVector3D& start, aiVector3D& end){
    tested.assign((*triangles).size()/3, false);
    return _findIntersection(root, start, end);
}

int kdTree::_findIntersection(TreeNode* current, aiVector3D& start, aiVector3D& end){

    // if both left and right child are null, search the BBList
    if(current->left==NULL && current->right==NULL){
        aiVector3D d = end-start;
        int min_idx = -1;
        float min_dist = FLT_MAX;
        for(unsigned i = 0; i < current->BBList.size(); i++){
            int idx = current->BBList[i].index;
            if(current->BBList[i].isLineIntersectBox(start, end)){
                if(tested[idx]==true) continue;
                    else tested[idx] = true;
                float dist = intersectDist(idx, start, d);
                if((dist < FLT_MAX) && (dist >= 0.0f) && (dist < min_dist)){
                    min_dist = dist;
                    min_idx = idx;
                    PRINTF("tested triangle[%d]:%f\n", idx, dist);
                }
            }
        }
        return min_idx;
    }

    // check if need to go to left or right
    int left_candidate = -1;
    int right_candidate = -1;
    if(current->left && current->left->BB.isLineIntersectBox(start, end))
        left_candidate = _findIntersection(current->left, start, end);
    
    if(current->right && current->right->BB.isLineIntersectBox(start, end))
        right_candidate = _findIntersection(current->right, start, end);
    
    // compare left and right candidates, return the one closer to start
    if(left_candidate==-1) return right_candidate;
    if(right_candidate==-1) return left_candidate;

    // when both left and right candidates are not -1
    float left_dist  = intersectDist(left_candidate,  start, end-start);
    float right_dist = intersectDist(right_candidate, start, end-start);
    if(left_dist < right_dist)
        return left_candidate;
    else
        return right_candidate;
}

void kdTree::getRange(aiVector3D* start, aiVector3D* end, int axis, float& min, float& max){
    switch(axis){
    case 0: if(start->x > end->x){
                max = start->x;
                min = end->x;
            }
            else {
                max = end->x;
                min = start->x;
            }
            break;
    case 1: if(start->y > end->y){
                max = start->y;
                min = end->y;
            }
            else {
                max = end->y;
                min = start->y;
            }
            break;
    case 2: if(start->z > end->z){
                max = start->z;
                min = end->z;
            }
            else {
                max = end->z;
                min = start->z;
            }
            break;
    };
}

int kdTree::longestDim(TreeNode* node){
    float length_x = node->BB.max[0] - node->BB.min[0];
    float length_y = node->BB.max[1] - node->BB.min[1];
    float length_z = node->BB.max[2] - node->BB.min[2];
    if(length_x > length_y && length_x > length_z)
        return 0;
    else if(length_y > length_x && length_y > length_z)
        return 1;
    else
        return 2;
}

float kdTree::intersectDist(int idx, aiVector3D& r0, aiVector3D& d){

    if(idx < 0) return FLT_MAX;

    d.Normalize();

    aiVector3D p1 = (*triangles)[idx*3];
    aiVector3D p2 = (*triangles)[idx*3+1];
    aiVector3D p3 = (*triangles)[idx*3+2];
    aiVector3D n = (*normals)[idx];

    float dn = dot(d,n);
    float t0 = dot(cross((p2-r0),(p1-r0)),d);
    float t1 = dot(cross((p3-r0),(p2-r0)),d);
    float t2 = dot(cross((p1-r0),(p3-r0)),d);
    
    if(dn!=0 && t0<0 && t1<0 && t2<0){
        // calculate the distance from start to the intersection point
        float k = dot(p1,n);
        float t = (k - dot(r0,n))/dot(d,n);
        PRINTF("intersected with positive triangle[%d]: %f\n", idx, t);
        return t;
    }

    if(dn!=0 && t0>0 && t1>0 && t2>0){
        // calculate the distance from start to the intersection point
        float k = dot(p1,n);
        float t = (k - dot(r0,n))/dot(d,n);
        PRINTF("intersected with negative triangle[%d]: %f\n", idx, t);
        return t;
    }

    return FLT_MAX;
}
