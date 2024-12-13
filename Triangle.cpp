//
// Created by cai on 24-12-6.
//

#include "Triangle.h"

Triangle::Triangle() {
    vertex[0] << 0,0,0,1;
    vertex[1] << 0,0,0,1;
    vertex[2] << 0,0,0,1;

    color[0] << 0.0, 0.0, 0.0;
    color[1] << 0.0, 0.0, 0.0;
    color[2] << 0.0, 0.0, 0.0;

    tex[0] << 0.0, 0.0;
    tex[1] << 0.0, 0.0;
    tex[2] << 0.0, 0.0;

}

Eigen::Vector4f Triangle::a() {
    return vertex[0];
}

Eigen::Vector4f Triangle::b() {
    return vertex[1];
}

Eigen::Vector4f Triangle::c() {
    return vertex[2];
}

void Triangle::setVertex(int ind, const Eigen::Vector4f& ver) {
    vertex[ind] = ver;
    // std::cout << ver << std::endl;
}

void Triangle::setNormal(int ind,const Eigen::Vector3f& n) {
    normal[ind] = n;
}

void Triangle::setColor(int ind, float r, float g, float b) {
    color[ind] << r, g, b;
}

void Triangle::setNormals(const std::array<Eigen::Vector3f, 3> &normals) {
    normal[0] = normals[1];
    normal[1] = normals[2];
    normal[2] = normals[0];
    normal[0].normalize();
    normal[1].normalize();
    normal[2].normalize();
}

void Triangle::setColors(const std::array<Eigen::Vector3f, 3> &colors) {
    setColor(0, colors[0][0], colors[0][1], colors[0][2]);
    setColor(1, colors[1][0], colors[1][1], colors[1][2]);
    setColor(2, colors[2][0], colors[2][1], colors[2][2]);
}

void Triangle::setTexCoord(int ind, Eigen::Vector2f uv) {
    tex[ind] = uv;
}

void Triangle::setTexCoords(const std::array<Eigen::Vector2f, 3> &tex) {
    setTexCoord(0, tex[0]);
    setTexCoord(1, tex[1]);
    setTexCoord(2, tex[2]);
}

void Triangle::setVertexs(const std::array<Eigen::Vector4f, 3> &vertexs) {
    vertex[0] = vertexs[0];
    vertex[1] = vertexs[1];
    vertex[2] = vertexs[2];
}

void Triangle::LoadBoundingBox() {
    // 初始化最小顶点为正无穷大，最大顶点为负无穷大
    Eigen::Vector3f min_vert = Eigen::Vector3f::Constant(std::numeric_limits<float>::infinity());
    Eigen::Vector3f max_vert = Eigen::Vector3f::Constant(-std::numeric_limits<float>::infinity());

    // 假设 vertex 是一个包含三个 Eigen::Vector3f 的数组
    for(int i = 0; i < 3; ++i){
        // 使用 cwiseMin 和 cwiseMax 逐个比较并更新 min_vert 和 max_vert
        min_vert = min_vert.cwiseMin(vertex[i].head<3>());
        max_vert = max_vert.cwiseMax(vertex[i].head<3>());
    }

    // 构建包围盒
    bbox = BoundingBox(min_vert, max_vert);
}

BoundingBox Triangle::GetBoundingBox() const {
    return bbox;
}
Intersection Triangle::GetIntersection(const Ray &ray) const {
    const float EPSILON = 1e-4f;
    Intersection intersection;
    intersection.happened = false;
    intersection.distance = std::numeric_limits<float>::infinity();
    Eigen::Vector3f edge1 = vertex[1].head<3>() - vertex[0].head<3>();
    Eigen::Vector3f edge2 = vertex[2].head<3>() - vertex[0].head<3>();
    Eigen::Vector3f h = ray.direction.cross(edge2);
    float a = edge1.dot(h);
    if (a > -EPSILON && a < EPSILON)
        return intersection; // 没有交叉
    float f = 1.0f / a;
    Eigen::Vector3f s = ray.origin - vertex[0].head<3>();
    float u = f * s.dot(h);

    // 检查交点是否在三角形内部
    if (u < 0.0f || u > 1.0f)
        return intersection;

    Eigen::Vector3f q = s.cross(edge1);
    float v = f * ray.direction.dot(q);

    if (v < 0.0f || u + v > 1.0f)
        return intersection;

    float t = f * edge2.dot(q);

    if (t > EPSILON) {
        intersection.happened = true;
        intersection.distance = t;
        intersection.coords = ray.origin + t * ray.direction;

        float w = 1.0f - u - v;
        Eigen::Vector3f interpolatedNormal = w * normal[0] + u * normal[1] + v * normal[2];
        intersection.normal = interpolatedNormal.normalized();

        intersection.triangle = const_cast<Triangle*>(this); // 设置交叉的三角形指针
        intersection.texCoord = w * tex[0] + u * tex[1] + v * tex[2];
        intersection.texture = Tex;
        return intersection;
    }
    return intersection;

}
bool Triangle::Intersect(const Ray &ray, float &tNear) {
    const float EPSILON = 1e-8; // 浮点数误差阈值

    // 获取三角形顶点
    Eigen::Vector3f v0 = vertex[0].head<3>(); // 三角形的第一个顶点
    Eigen::Vector3f v1 = vertex[1].head<3>(); // 第二个顶点
    Eigen::Vector3f v2 = vertex[2].head<3>(); // 第三个顶点

    // 射线的起点和方向
    Eigen::Vector3f origin = ray.origin;
    Eigen::Vector3f direction = ray.direction;

    // 计算边向量
    Eigen::Vector3f edge1 = v1 - v0;
    Eigen::Vector3f edge2 = v2 - v0;

    // 计算 pvec = direction × edge2
    Eigen::Vector3f pvec = direction.cross(edge2);

    // 计算行列式
    float det = edge1.dot(pvec);

    // 检查行列式是否接近零（射线平行于三角形平面）
    float relativeEpsilon = EPSILON * edge1.norm() * edge2.norm();
    if (std::fabs(det) < relativeEpsilon)
        return false;

    // 计算行列式的倒数
    float invDet = 1.0f / det;

    // 计算 tvec = origin - v0
    Eigen::Vector3f tvec = origin - v0;

    // 计算 u 参数并检查范围
    float u = tvec.dot(pvec) * invDet;
    if (u < 0.0f || u > 1.0f)
        return false;

    // 计算 qvec = tvec × edge1
    Eigen::Vector3f qvec = tvec.cross(edge1);

    // 计算 v 参数并检查范围
    float v = direction.dot(qvec) * invDet;
    if (v < 0.0f || u + v > 1.0f)
        return false;

    // 计算 tNear（交点距离）
    tNear = edge2.dot(qvec) * invDet;

    // 如果 tNear 小于 0，说明交点在射线的反方向
    if (tNear < 0)
        return false;

    return true; // 射线与三角形相交
}

bool Triangle::Intersect(const Ray &ray) {
    return false;
}


