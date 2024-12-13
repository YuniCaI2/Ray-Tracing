//
// Created by cai on 24-12-11.
//

#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H
#include <Eigen/Dense>
#include "Ray.h"

class BoundingBox {
public:
    Eigen::Vector3f pMax, pMin;
    //两个角点关于包围盒

    BoundingBox();
    BoundingBox(Eigen::Vector3f pMin, Eigen::Vector3f pMax);

    Eigen::Vector3f Diagonal() const;

    Eigen::Vector3f GetCenter() const;

    BoundingBox Merge(const BoundingBox &other) const;

    bool Inside(const Eigen::Vector3f &p) const;
    bool IntersectP(const Ray& ray, const Eigen::Vector3f& invDir,
                                const std::array<int, 3>& dirIsNeg) const;
    int maxExtent() const;
};


inline BoundingBox Union(const BoundingBox& b1, const BoundingBox& b2)
{
    BoundingBox ret;
    ret.pMin = Eigen::Vector3f(std::min(b1.pMin[0], b2.pMin[0]),std::min(b1.pMin[1],b2.pMin[1]),
                                            std::min(b1.pMin[2], b2.pMin[2]));
    ret.pMax = Eigen::Vector3f(std::max(b1.pMax[0], b2.pMax[0]),std::max(b1.pMax[1],b2.pMax[1]),
                                            std::max(b1.pMax[2], b2.pMax[2]));
    return ret;
}

inline BoundingBox Union(const BoundingBox& b, const Eigen::Vector3f& p)
{
    BoundingBox ret;
    ret.pMin = Eigen::Vector3f(std::min(b.pMin[0], p[0]),std::min(b.pMin[1], p[1]),std::min(b.pMin[2],p[2]));
    ret.pMax = Eigen::Vector3f(std::max(b.pMax[0], p[0]),std::max(b.pMax[1], p[1]),std::max(b.pMax[2],p[2]));
    return ret;
}


#endif //BOUNDINGBOX_H
