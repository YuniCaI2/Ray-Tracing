//
// Created by cai on 24-12-11.
//

#ifndef BVH_H
#define BVH_H
#include "BoundingBox.h"
#include "Intersection.h"
#include "Model.h"


class BVH {
public:
    BVH(const std::vector<Model*> & models, const int& maxModelInNode);

    struct BVHBuildNode {
        BoundingBox bounds;
        BVHBuildNode *left;
        BVHBuildNode *right;
        Triangle* triangle;

    public:
        int splitAxis=0, firstPrimOffset=0, nPrimitives=0;
        BVHBuildNode(){
            bounds = BoundingBox();
            left = nullptr;right = nullptr;
            triangle = nullptr;
        }
    };

    BVHBuildNode* buildBVH(const std::vector<Triangle*> & triangles);

    int maxModelInNode;
    std::vector<Model*> models;
    std::vector<Triangle*> triangles;
    BVHBuildNode* root;

    Intersection GetIntersection(const Ray& ray,BVHBuildNode* node);
    Intersection BVHIntersection(const Ray& ray);
};




#endif //BVH_H
