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
#ifndef GANDER_PICKER_H
#define GANDER_PICKER_H
#include <glycerin/Ray.hxx>
#include <RapidGL/Node.h>
#include <RapidGL/State.h>


struct Pick {
    RapidGL::Node* node;
    double depth;
};


class Picker {
public:
// Methods
    Picker(RapidGL::State* state);
    Pick pick(RapidGL::Node* root, int x, int y);
private:
// Attributes
    RapidGL::State* const state;
    RapidGL::Node* picked;
    double time;
// Methods
    Glycerin::Ray createRay(int x, int y);
    void pick(RapidGL::Node* node, const Glycerin::Ray& ray);
    static Glycerin::Ray transform(const M3d::Mat4& mat, const Glycerin::Ray& ray);
};

#endif
