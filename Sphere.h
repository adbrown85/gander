#ifndef GANDER_SPHERE_H
#define GANDER_SPHERE_H
#include <glycerin/Ray.hxx>


/**
 * Sphere that can be intersected by a ray.
 */
class Sphere {
public:
// Methods
    Sphere(double radius);
    double intersectedByRay(const Glycerin::Ray& ray) const;
// Attributes
    double radius;
};

#endif
