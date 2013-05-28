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
#include <stdexcept>
#include <m3d/Vec4.h>
#include "VolumeNode.h"

// Bounding box for intersection tests
const Glycerin::AxisAlignedBoundingBox VolumeNode::BOUNDING_BOX = VolumeNode::createBoundingBox();

/**
 * Constructs a `VolumeNode`.
 *
 * @param textureId Identifier of texture node
 * @throws std::invalid_argument if ID of texture node is `null`
 */
VolumeNode::VolumeNode(const std::string& textureId) : textureId(textureId) {
    // empty
}

/**
 * Destructs a `VolumeNode`.
 */
VolumeNode::~VolumeNode() {
    // empty
}

/**
 * Creates the bounding box the cube delegates to for intersection testing.
 */
Glycerin::AxisAlignedBoundingBox VolumeNode::createBoundingBox() {
    const M3d::Vec4 min(-0.5, -0.5, -0.5, 1.0);
    const M3d::Vec4 max(+0.5, +0.5, +0.5, 1.0);
    return Glycerin::AxisAlignedBoundingBox(min, max);
}

/**
 * Returns the identifier of the texture to use for this volume.
 *
 * @return Identifier of the texture to use for this volume
 */
std::string VolumeNode::getTextureId() const {
    return textureId;
}

double VolumeNode::intersect(const Glycerin::Ray& ray) const {
    return BOUNDING_BOX.intersect(ray);
}

void VolumeNode::postVisit(RapidGL::State& state) {
    // empty
}

void VolumeNode::preVisit(RapidGL::State& state) {
    // empty
}

void VolumeNode::visit(RapidGL::State& state) {
    // empty
}
