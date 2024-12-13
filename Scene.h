#ifndef SCENE_H
#define SCENE_H

#include <Eigen/Dense>
#include <vector>

// 前向声明
class BVH;
class Light;
class Model;
class Triangle;
class Ray;
class Intersection;
class Texture;

class Scene {
public:
    int width = 1280;
    int height = 960;
    double fov = 90.0;
    Eigen::Vector3f backgroundColor = Eigen::Vector3f(0.235294f, 0.67451f, 0.843137f);
    int maxDepth = 5;

    std::vector<Light*> lights;
    std::vector<Model*> models;
    std::vector<Triangle*> triangles;

    Scene(int w, int h);
    ~Scene(); // 添加析构函数以管理内存

    void AddModel(const std::vector<Model*>& modelsToAdd); // 使用原始指针
    void AddLight(const std::vector<Light*>& lightsToAdd); // 使用原始指针
    void BuildBVH();

    BVH* bvh;

    Eigen::Vector3f castRay(const Ray& ray, int depth) const;

    // 反射函数
    Eigen::Vector3f reflect(const Eigen::Vector3f& I, const Eigen::Vector3f& N) const;

    // 折射函数
    Eigen::Vector3f refract(const Eigen::Vector3f& I, const Eigen::Vector3f& N, const float& ior) const;

    // 菲涅尔方程
    void fresnel(const Eigen::Vector3f& I, const Eigen::Vector3f& N, const float& ior, float& kr) const;
};

#endif // SCENE_H