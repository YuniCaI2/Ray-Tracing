//
// Created by cai on 24-12-12.
//

#ifndef LIGHT_H
#define LIGHT_H
#include<Eigen/Dense>
class Light
{
public:
    Light(const Eigen::Vector3f &p, const Eigen::Vector3f &i) : position(p), intensity(i) {}
    virtual ~Light() = default;

    Eigen::Vector3f position;
    Eigen::Vector3f intensity;
};
#endif //LIGHT_H
