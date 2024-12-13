#include "BVH.h"
#include <algorithm>
#include <cassert>
#include <iostream>

BVH::BVH(const std::vector<Model *> &models, const int &maxModelInNode) {
    this->maxModelInNode = maxModelInNode;
    this->models = models;

    // 对每个模型和其中的三角形进行变换
    for (auto &model : this->models) {
        // 准备矩阵
        Eigen::Matrix4f modelMatrix = model->modelMatrix;
        Eigen::Matrix4f invTrans = model->modelMatrix.inverse().transpose();
        Eigen::Matrix3f normalMatrix = invTrans.block<3,3>(0,0);

        for (auto &triangle : model->triangleList) {
            for (int i = 0; i < 3; i++) {
                // 法线变换
                Eigen::Vector3f n = triangle->normal[i];   // 保证 normal[i] 是 Vector3f
                Eigen::Vector3f transformedNormal = normalMatrix * n;
                triangle->normal[i] = transformedNormal.normalized();

                // 顶点变换
                triangle->vertex[i] = modelMatrix * triangle->vertex[i];

                if (model->modelFlag)
                    triangle->Tex = &(model->texture);
                else
                    triangle->Tex = nullptr;
                triangle->LoadBoundingBox();
                // std::cout << "BBOX: Max:" << triangle->bbox.pMax << std::endl;
                // std::cout << "BBOX: Min:" << triangle->bbox.pMin << std::endl;

            }
        }
    }

    // 将所有三角形加入 BVH
    for (auto &model : this->models) {
        for (auto &triangle : model->triangleList) {
            // 确保 triangles 是 std::vector<Triangle*> 而非 const Triangle*
            triangles.push_back(triangle);
        }
    }

    root = buildBVH(triangles);

    std::cout << "BVH created !" << std::endl;
}

BVH::BVHBuildNode* BVH::buildBVH(const std::vector<Triangle*>& tris) {
    auto* node = new BVHBuildNode();

    // 若是叶子节点(根据maxModelInNode判断)
    if (tris.size() <= (size_t)maxModelInNode) {
        node->bounds = tris[0]->GetBoundingBox();
        node->triangle = tris[0];
        node->left = nullptr;
        node->right = nullptr;
        for (size_t i = 1; i < tris.size(); ++i) {
            node->bounds = Union(node->bounds, tris[i]->GetBoundingBox());
        }
        // std::cout << tris[0]->vertex[0] << " " << std::endl;
        return node;
    }

    // 计算包围盒
    BoundingBox bbox;
    for (auto &tri : tris) {
        bbox = Union(bbox, tri->GetBoundingBox());
    }
    node->bounds = bbox;

    // 计算质心盒
    BoundingBox centroidBounds;
    for (auto &tri : tris) {
        Eigen::Vector3f c = tri->GetBoundingBox().GetCenter();
        centroidBounds = Union(centroidBounds, BoundingBox(c, c));
    }

    int dim = centroidBounds.maxExtent();

    // 排序
    std::vector<Triangle*> sortedTris = tris;
    std::sort(sortedTris.begin(), sortedTris.end(), [dim](Triangle* f1, Triangle* f2) {
        Eigen::Vector3f c1 = f1->GetBoundingBox().GetCenter();
        Eigen::Vector3f c2 = f2->GetBoundingBox().GetCenter();
        return c1[dim] < c2[dim];
    });

    size_t mid = sortedTris.size() / 2;
    std::vector<Triangle*> leftshapes(sortedTris.begin(), sortedTris.begin() + mid);
    std::vector<Triangle*> rightshapes(sortedTris.begin() + mid, sortedTris.end());

    node->left = buildBVH(leftshapes);
    node->right = buildBVH(rightshapes);

    node->bounds = Union(node->left->bounds, node->right->bounds);

    return node;
}
Intersection BVH::GetIntersection(const Ray &ray, BVHBuildNode* node) {
    if (!node) return Intersection();

    std::array<int, 3> dirIsNeg{
        int(ray.direction.x() < 0),
        int(ray.direction.y() < 0),
        int(ray.direction.z() < 0)
    };

    Eigen::Vector3f invDir(1.0f / ray.direction.x(), 1.0f / ray.direction.y(), 1.0f / ray.direction.z());

    if (!node->bounds.IntersectP(ray, invDir, dirIsNeg)) {
        // std::cout << "bounds:" << node->bounds.pMax << " :::" << node->bounds.pMin << std::endl;
        return Intersection();
    }


    if (node->left == nullptr && node->right == nullptr) {
        // 叶节点
        return node->triangle->GetIntersection(ray);
    }

    Intersection hit1 = GetIntersection(ray, node->left);
    Intersection hit2 = GetIntersection(ray, node->right);

    return (hit1.distance < hit2.distance) ? hit1 : hit2;
}

Intersection BVH::BVHIntersection(const Ray &ray) {
    Intersection isect;
    if (!root)
        return isect;
    isect = GetIntersection(ray, root);
    return isect;
}