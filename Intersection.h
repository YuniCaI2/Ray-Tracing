#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <Eigen/Dense>
#include <limits>

// 前向声明
class Triangle;
class Texture;

struct Intersection
{
    Intersection(){
        happened = false;
        coords = Eigen::Vector3f();
        normal = Eigen::Vector3f();
        distance = std::numeric_limits<double>::max();
        triangle = nullptr;
        texCoord = Eigen::Vector2f();
        texture = nullptr;
    }

    bool happened;
    Eigen::Vector3f coords;
    Eigen::Vector3f normal;
    double distance;
    Triangle* triangle;
    Eigen::Vector2f texCoord;
    Texture* texture;
};

#endif //INTERSECTION_H