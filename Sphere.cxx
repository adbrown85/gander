#include "config.h"
#include "Sphere.h"

Sphere::Sphere(const double radius) : radius(radius) {
    // empty
}

double Sphere::intersectedByRay(const Ray& ray) const {

    const double a = dot(ray.d, ray.d);
    const double b = 2 * dot(ray.d, ray.o);
    const double c = dot(ray.o, ray.o) - (radius * radius);

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
