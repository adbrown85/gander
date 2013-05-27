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
#ifndef GANDER_BLEND_NODE_H
#define GANDER_BLEND_NODE_H
#include <RapidGL/Node.h>


/**
 * Node that enables or disables blending.
 */
class BlendNode : public RapidGL::Node {
public:
// Methods
    BlendNode(bool enabled);
    virtual ~BlendNode();
    virtual void visit(RapidGL::State& state);
private:
// Attributes
    const bool enabled;
};

#endif
