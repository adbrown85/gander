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
#ifndef GANDER_SLICING_VOLUME_RENDERER_NODE_H
#define GANDER_SLICING_VOLUME_RENDERER_NODE_H
#include <map>
#include <queue>
#include <stdexcept>
#include <string>
#include <vector>
#include <gloop/BufferObject.hxx>
#include <gloop/BufferTarget.hxx>
#include <gloop/Program.hxx>
#include <gloop/TextureUnit.hxx>
#include <gloop/VertexArrayObject.hxx>
#include <m3d/Vec4.h>
#include <m3d/Mat4.h>
#include <RapidGL/Node.h>
#include <RapidGL/State.h>
#include <RapidGL/ProgramNode.h>
#include "VolumeNode.h"


/**
 * Node rendering volumes using the slicing method.
 */
class SlicingVolumeRendererNode : public RapidGL::Node {
private:
// Constants
    static const int VERTICES_PER_QUAD = 6;
    static const GLsizei SIZE_OF_FLOAT = sizeof(GLfloat);
// Types
    struct Extent {
        M3d::Vec4 min;
        M3d::Vec4 max;
    };
    struct Vertex {
        double x, y;
        double s, t, p;
    };
    struct Quad {
    // Attributes
        GLint unit;
        double z;
        Vertex v0;
        Vertex v1;
        Vertex v2;
        Vertex v3;
    // Methods
        Vertex& operator[](int i) {
            switch (i) {
            case 0: return v0;
            case 1: return v1;
            case 2: return v2;
            case 3: return v3;
            default:
                throw std::out_of_range("Index is out of bounds!");
            }
        }
    };
public:
// Types
    class Strategy {
    public:
        virtual void draw(const std::vector<Quad>& quads) = 0;
        virtual void findUniformLocations(const Gloop::Program&) = 0;
        virtual GLsizei getSizeOfQuad() = 0;
        virtual void setUpAttributes(RapidGL::ProgramNode*, const Gloop::VertexArrayObject&) = 0;
    };
    class AttributeStrategy : public Strategy {
    public:
        AttributeStrategy();
        virtual void draw(const std::vector<Quad>& quads);
        virtual void findUniformLocations(const Gloop::Program&);
        virtual GLsizei getSizeOfQuad();
        virtual void setUpAttributes(RapidGL::ProgramNode*, const Gloop::VertexArrayObject&);
    private:
        static const int FLOATS_PER_VERTEX = 7;
        static const int FLOATS_PER_QUAD = VERTICES_PER_QUAD * FLOATS_PER_VERTEX;
        static const GLsizei SIZE_OF_VERTEX = SIZE_OF_FLOAT * FLOATS_PER_VERTEX;
        static const GLsizei SIZE_OF_QUAD = SIZE_OF_VERTEX * VERTICES_PER_QUAD;
    };
    class UniformStrategy : public Strategy {
    public:
        UniformStrategy(const std::string& uniformName);
        virtual void draw(const std::vector<Quad>& quads);
        virtual void findUniformLocations(const Gloop::Program& program);
        virtual GLsizei getSizeOfQuad();
        virtual void setUpAttributes(RapidGL::ProgramNode*, const Gloop::VertexArrayObject&);
    private:
        static const int FLOATS_PER_VERTEX = 6;
        static const int FLOATS_PER_QUAD = VERTICES_PER_QUAD * FLOATS_PER_VERTEX;
        static const GLsizei SIZE_OF_VERTEX = SIZE_OF_FLOAT * FLOATS_PER_VERTEX;
        static const GLsizei SIZE_OF_QUAD = SIZE_OF_VERTEX * VERTICES_PER_QUAD;
        const std::string uniformName;
        GLint uniformLocation;
    };
// Methods
    SlicingVolumeRendererNode(Strategy* strategy, const int numberOfSlices);
    virtual ~SlicingVolumeRendererNode();
    virtual void preVisit(RapidGL::State& state);
    virtual void visit(RapidGL::State& state);
private:
// Attributes
    bool ready;
    const Gloop::BufferTarget arrayBuffer;
    const Gloop::VertexArrayObject vao;
    const Gloop::BufferObject vbo;
    const int numberOfSlices;
    Strategy* const strategy;
    std::vector<VolumeNode*> volumeNodes;
    std::map<VolumeNode*,Gloop::TextureUnit> textureUnitsByVolumeNode;
// Methods
    static Extent findExtent(const VolumeNode* volumeNode, const M3d::Mat4& viewMatrix);
    template<typename T> static T* findDescendant(RapidGL::Node* node);
    template<typename T> static std::vector<T*> findDescendants(RapidGL::Node* node);
    static M3d::Mat4 getModelMatrix(const RapidGL::Node* node);
    Gloop::TextureUnit getTextureUnit(VolumeNode* volumeNode) const;
    void putTextureUnit(VolumeNode* volumeNode, const Gloop::TextureUnit& textureUnit);
    static bool compare(const Quad& q1, const Quad& q2);
};


/**
 * Finds a descendant node of a particular type.
 *
 * @param rootNode Node to search from
 * @return Pointer to the descendant node, or `NULL` if not found
 * @throw std::invalid_argument if root node is `NULL`
 */
template<typename T>
T* SlicingVolumeRendererNode::findDescendant(RapidGL::Node* rootNode) {

    if (rootNode == NULL) {
        throw std::invalid_argument("Root node is NULL!");
    }

    std::queue<RapidGL::Node*> nodes;
    nodes.push(rootNode);

    while (!nodes.empty()) {
        RapidGL::Node* const node = nodes.front();
        T* const t = dynamic_cast<T*>(node);
        if (t != NULL) {
            return t;
        }
        RapidGL::addToQueue(nodes, node->getChildren());
        nodes.pop();
    }

    return NULL;
}

/**
 * Finds all the descendants of a node of a particular type.
 *
 * @param rootNode Node to search from
 * @return Vector of the descendants of the specified type
 * @throw std::invalid_argument if root node is `NULL`
 */
template<typename T>
std::vector<T*> SlicingVolumeRendererNode::findDescendants(RapidGL::Node* const rootNode) {

    if (rootNode == NULL) {
        throw std::invalid_argument("Root node is NULL!");
    }

    std::vector<T*> ts;

    std::queue<RapidGL::Node*> q;
    q.push(rootNode);

    while (!q.empty()) {
        RapidGL::Node* const node = q.front();
        T* const t = dynamic_cast<T*>(node);
        if (t != NULL) {
            ts.push_back(t);
        }
        RapidGL::addToQueue(q, node->getChildren());
        q.pop();
    }

    return ts;
}

#endif
