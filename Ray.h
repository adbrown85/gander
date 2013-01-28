#ifndef GANDER_RAY_H
#define GANDER_RAY_H
#include <m3d/Vec3.h>


/**
 * Ray with an origin and direction.
 */
class Ray {
public:
// Methods
    Ray(const M3d::Vec3& o, const M3d::Vec3& d) : o(o), d(d) { }
// Attributes
    M3d::Vec3 o;
    M3d::Vec3 d;
};

#endif
