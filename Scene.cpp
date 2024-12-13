#include "Scene.h"
#include "Intersection.h"
#include "BVH.h"
#include "Ray.h"
#include "Light.h"
#include "Texture.h"
#include <algorithm>
#include <cmath>
#include <cstdio>

const float EPSILON = 0.001f;

Scene::Scene(int w, int h) : width(w), height(h), bvh(nullptr) {
    // 其他初始化逻辑如果有的话可以放这里
}

Scene::~Scene() {
    delete bvh;
}

void Scene::AddModel(const std::vector<Model *> &modelsToAdd) {
    this->models.insert(this->models.end(), modelsToAdd.begin(), modelsToAdd.end());
}

void Scene::AddLight(const std::vector<Light *> &lightsToAdd) {
    this->lights.insert(this->lights.end(), lightsToAdd.begin(), lightsToAdd.end());
}

void Scene::BuildBVH() {
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVH(models, 1);
}

Eigen::Vector3f Scene::castRay(const Ray &ray, int depth) const {
    if (depth > this->maxDepth) {
        return Eigen::Vector3f(0,0,0); // 返回背景色
    }

    // 如果 bvh 有可能为空，检查一下

    Intersection intersection = bvh->BVHIntersection(ray);

    // 没有交点直接返回背景色
    if (!intersection.happened) {
        // std::cout << "Triangle bbx:" << std::endl;
        // std::cout << intersection.triangle->bbox.pMax << " " << intersection.triangle->bbox.pMin << "\n";
        // std::cout << "Ray traced:" << std::endl;
        // std::cout << ray.origin << " -------" << ray.direction << "\n";
        return backgroundColor;
    }

    // 有交点，取得相关信息
    Triangle* hitTriangle = intersection.triangle; // 如果 intersection.happened=true，triangle应该有效
    Eigen::Vector3f hitPoint = intersection.coords;
    Eigen::Vector3f N = intersection.normal;

    // 判断是否有纹理
    bool hasTexture = (intersection.texture != nullptr);

    Eigen::Vector3f hitColor;
    if (!hasTexture) {
        Eigen::Vector3f reflectionDirection = reflect(ray.direction, N).normalized();
        Eigen::Vector3f refractionDirection = refract(ray.direction, N, 1.0f).normalized();

        Eigen::Vector3f reflectionRayOrig = (reflectionDirection.dot(N) < 0.0f) ?
                                             (hitPoint - N * EPSILON).eval() :
                                             (hitPoint + N * EPSILON).eval() ;

        Eigen::Vector3f refractionRayOrig = (refractionDirection.dot(N) < 0.0f) ?
                                             (hitPoint - N * EPSILON).eval() :
                                             (hitPoint + N * EPSILON).eval() ;

        // Fresnel计算反射比kr
        float kr;
        fresnel(ray.direction, N, 1.5f, kr);

        Eigen::Vector3f reflectionColor = castRay(Ray(reflectionRayOrig, reflectionDirection, 0.0f), depth + 1);
        Eigen::Vector3f refractionColor = castRay(Ray(refractionRayOrig, refractionDirection, 0.0f), depth + 1);
        kr = std::clamp(kr, 0.0f, 1.0f);
        hitColor = (reflectionColor * kr + refractionColor * (1.0f - kr)).cwiseMin(1.0f).cwiseMax(0.0f);
        hitColor = reflectionColor * kr + refractionColor * (1.0f - kr);
    } else {
        // 有纹理时计算阴影、光照和高光
        Eigen::Vector3f lightAmt = Eigen::Vector3f::Zero();
        Eigen::Vector3f specularColor = Eigen::Vector3f::Zero();
        Eigen::Vector3f shadowPointOrig = (ray.direction.dot(N) < 0.0f) ?
                                           (hitPoint + N * EPSILON).eval() :
                                           (hitPoint - N * EPSILON).eval() ;

        for (auto& light : lights) {
            Eigen::Vector3f lightDir = light->position - hitPoint;
            float lightDistance2 = lightDir.dot(lightDir); // 虽然未使用，但可用于光强衰减
            lightDir.normalize();
            float LdotN = std::max(0.0f, lightDir.dot(N));

            // 阴影测试
            bool inShadow = bvh->BVHIntersection(Ray(shadowPointOrig, lightDir, 0.0f)).happened;
            lightAmt += (1 - inShadow) * light->intensity * LdotN;

            Eigen::Vector3f reflectionDirection = reflect(-lightDir, N);
            specularColor += powf(std::max(0.0f, -reflectionDirection.dot(ray.direction)), 64.0f) * light->intensity;
        }

        // 采样纹理颜色
        Eigen::Vector3f textureColor = intersection.texture->getColorBilinear(intersection.texCoord.x(), intersection.texCoord.y());
        textureColor = textureColor / 255.0f;
        // std::cout << "纹理颜色:" << textureColor.x()/255 << " ::" << textureColor.y() << "::" <<textureColor.z() << std::endl;
        // 简单的光照模型：diffuse * 0.8 + specular * 0.2
        hitColor = (lightAmt * 0.8f).cwiseProduct(textureColor) + 0.2f * specularColor;
    }

    return hitColor;
}

Eigen::Vector3f Scene::reflect(const Eigen::Vector3f& I, const Eigen::Vector3f& N) const {
    return I - 2.0f * I.dot(N) * N;
}

Eigen::Vector3f Scene::refract(const Eigen::Vector3f& I, const Eigen::Vector3f& N, const float& ior) const {
    float cosi = I.dot(N);
    // 手动实现clamp
    cosi = std::max(-1.0f, std::min(1.0f, cosi));
    float etai = 1.0f, etat = ior;
    Eigen::Vector3f n = N;
    if (cosi < 0) {
        cosi = -cosi;
    } else {
        std::swap(etai, etat);
        n = -N;
    }
    float eta = etai / etat;
    float k = 1.0f - eta * eta * (1.0f - cosi * cosi);
    return (k < 0.0f) ? Eigen::Vector3f(0.0f, 0.0f, 0.0f) : eta * I + (eta * cosi - sqrtf(k)) * n;
}

void Scene::fresnel(const Eigen::Vector3f& I, const Eigen::Vector3f& N, const float& ior, float& kr) const {
    float cosi = I.dot(N);
    cosi = std::max(-1.0f, std::min(1.0f, cosi));
    float etai = 1.0f, etat = ior;
    if (cosi > 0.0f) { std::swap(etai, etat); }
    float sint = etai / etat * sqrtf(std::max(0.0f, 1.0f - cosi * cosi));
    if (sint >= 1.0f) {
        // 全反射
        kr = 1.0f;
    } else {
        float cost = sqrtf(std::max(0.0f, 1.0f - sint * sint));
        cosi = fabsf(cosi);
        float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
        float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
        kr = (Rs * Rs + Rp * Rp) / 2.0f;
    }
}