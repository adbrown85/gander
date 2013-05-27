/*
 * Gander - Scene renderer for prototyping OpenGL and GLSL
 * Copyright (C) 2013  Andrew Brown
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
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
