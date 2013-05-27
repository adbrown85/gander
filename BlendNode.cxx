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
#include "BlendNode.h"

/**
 * Constructs a blend node.
 *
 * @param enabled Whether blending should be enabled
 */
BlendNode::BlendNode(const bool enabled) : enabled(enabled) {
    // empty
}

/**
 * Destructs a blend node.
 */
BlendNode::~BlendNode() {
    // empty
}

void BlendNode::visit(RapidGL::State& state) {
    if (enabled) {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
    } else {
        glDisable(GL_BLEND);
    }
}
