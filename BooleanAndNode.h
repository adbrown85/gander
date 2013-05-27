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
#ifndef GANDER_BOOLEAN_AND_NODE_H
#define GANDER_BOOLEAN_AND_NODE_H
#include <string>
#include <gloop/BufferObject.hxx>
#include <gloop/VertexArrayObject.hxx>
#include <glycerin/BufferLayout.hxx>
#include <m3d/Vec4.h>
#include <RapidGL/CubeNode.h>
#include <RapidGL/Node.h>
#include <RapidGL/State.h>


/**
 * Node drawing the intersection between two cubes.
 */
class BooleanAndNode : public RapidGL::Node, public RapidGL::NodeListener {
public:
// Methods
    BooleanAndNode(const std::string& id1, const std::string& id2);
    virtual ~BooleanAndNode();
    virtual void nodeChanged(RapidGL::Node* node);
    virtual void preVisit(RapidGL::State& state);
    virtual void visit(RapidGL::State& state);
private:
// Types
    struct Extent {
        M3d::Vec4 min;
        M3d::Vec4 max;
    };
// Attributes
    bool ready;
    bool dirty;
    bool drawable;
    const std::string id1;
    const std::string id2;
    RapidGL::CubeNode* c1;
    RapidGL::CubeNode* c2;
    Gloop::BufferObject vbo;
    Glycerin::BufferLayout layout;
    std::map<Gloop::Program,Gloop::VertexArrayObject> vaos;
// Methods
    Glycerin::BufferLayout createLayout();
    Gloop::VertexArrayObject createVertexArrayObject(const Gloop::Program& program);
    Extent findExtent(RapidGL::CubeNode* cubeNode);
    Gloop::VertexArrayObject getVertexArrayObject(const Gloop::Program& program);
    bool isDrawable(const M3d::Vec4& min, const M3d::Vec4& max);
    void update();
};

#endif
