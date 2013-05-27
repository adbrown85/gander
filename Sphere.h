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
