#include "config.h"
#include "Sphere.h"

Sphere::Sphere(const double radius) : radius(radius) {
    // empty
}

double Sphere::intersectedByRay(const Glycerin::Ray& ray) const {

    const M3d::Vec3 o = ray.origin.toVec3();
    const M3d::Vec3 d = ray.direction.toVec3();

    const double a = dot(d, d);
    const double b = 2 * dot(d, o);
    const double c = dot(o, o) - (radius * radius);

    double q;
    if (b < 0) {
        q = (-b + sqrt(b * b - 4.0 * a * c)) / 2.0;
    } else {
        q = (-b - sqrt(b * b - 4.0 * a * c)) / 2.0;
    }

    double t0 = q / a;
    double t1 = c / q;

    if (t0 > t1) {
        std::swap(t0, t1);
    }

    if (t0 < 0) {
        if (t1 < 0) {
            return -1;
        } else {
            return t1;
        }
    } else {
        return t0;
    }
}
