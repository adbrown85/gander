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
#ifndef GANDER_VOLUME_NODE_H
#define GANDER_VOLUME_NODE_H
#include <string>
#include <glycerin/AxisAlignedBoundingBox.hxx>
#include <RapidGL/Intersectable.h>
#include <RapidGL/Node.h>
#include <RapidGL/State.h>


/**
 * Node representing a volume.
 *
 * Note that `Volume` may not be used for all types of volume rendering.
 */
class VolumeNode : public RapidGL::Node, public RapidGL::Intersectable {
public:
// Methods
    VolumeNode(const std::string& textureId);
    virtual ~VolumeNode();
    std::string getTextureId() const;
    virtual double intersect(const Glycerin::Ray& ray) const;
    virtual void postVisit(RapidGL::State& state);
    virtual void preVisit(RapidGL::State& state);
    virtual void visit(RapidGL::State& state);
private:
// Constants
    static const Glycerin::AxisAlignedBoundingBox BOUNDING_BOX;
// Attributes
    const std::string textureId;
// Methods
    static Glycerin::AxisAlignedBoundingBox createBoundingBox();
};

#endif
