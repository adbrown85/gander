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
#include "SlicingVolumeRendererNode.h"
#include <algorithm>
#include <limits>
#include <stack>
#include <gloop/TextureTarget.hxx>
#include <RapidGL/AttributeNode.h>
#include <RapidGL/TextureNode.h>
#include <RapidGL/TransformNode.h>
#include <RapidGL/UseNode.h>

/**
 * Constructs a `SlicingVolumeRendererNode`.
 *
 * @param strategy Strategy to render with
 * @param numberOfSlices Number of slices to create for each volume
 * @throws std::invalid_argument if name of uniform is empty
 */
SlicingVolumeRendererNode::SlicingVolumeRendererNode(Strategy* const strategy,
                                                     const int numberOfSlices) :
        ready(false),
        arrayBuffer(Gloop::BufferTarget::arrayBuffer()),
        numberOfSlices(numberOfSlices),
        strategy(strategy),
        vao(Gloop::VertexArrayObject::generate()),
        vbo(Gloop::BufferObject::generate()) {
    // empty
}

/**
 * Destructs a `SlicingVolumeRendererNode`.
 */
SlicingVolumeRendererNode::~SlicingVolumeRendererNode() {
    vao.dispose();
    vbo.dispose();
}

/**
 * Compares two quads according to their depths.
 *
 * @param q1 First quad
 * @param q2 Second quad
 * @return `true` if the first quad should go before the second quad
 */
bool SlicingVolumeRendererNode::compare(const Quad& q1, const Quad& q2) {
    return q2.z > q1.z;
}

/**
 * Determines the minimum and maximum positions of a volume in eye space.
 *
 * @param volumeNode Volume to determine minimum and maximum of
 * @param viewMatrix Current view matrix, which transforms world coordinates to eye coordinates
 * @return Minimum and maximum of volume in eye space, as an `Extent`
 */
SlicingVolumeRendererNode::Extent SlicingVolumeRendererNode::findExtent(const VolumeNode* volumeNode,
                                                                        const M3d::Mat4& viewMatrix) {

    // Compute model view matrix
    const M3d::Mat4 modelMatrix = getModelMatrix(volumeNode);
    const M3d::Mat4 modelViewMatrix = viewMatrix * modelMatrix;

    // Create points
    const M3d::Vec4 points[] = {
        M3d::Vec4(-0.5, -0.5, -0.5, 1.0),
        M3d::Vec4(-0.5, -0.5, +0.5, 1.0),
        M3d::Vec4(-0.5, +0.5, -0.5, 1.0),
        M3d::Vec4(-0.5, +0.5, +0.5, 1.0),
        M3d::Vec4(+0.5, -0.5, -0.5, 1.0),
        M3d::Vec4(+0.5, -0.5, +0.5, 1.0),
        M3d::Vec4(+0.5, +0.5, -0.5, 1.0),
        M3d::Vec4(+0.5, +0.5, +0.5, 1.0)
    };

    // Transform points
    std::vector<M3d::Vec4> transformedPoints;
    for (int i = 0; i < 8; ++i) {
        transformedPoints.push_back(modelViewMatrix * points[i]);
    }

    // Calculate extent
    Extent extent;
    for (int i = 0; i < 3; ++i) {
        extent.min[i] = +std::numeric_limits<double>::infinity();
        extent.max[i] = -std::numeric_limits<double>::infinity();
        for (int j = 0; j < 8; ++j) {
            const double value = transformedPoints[j][i];
            extent.min[i] = std::min(extent.min[i], value);
            extent.max[i] = std::max(extent.max[i], value);
        }
    }

    // Return it
    return extent;
}

/**
 * Computes the model matrix at a node.
 *
 * @param node Node to compute model matrix for
 * @return Model matrix at the node
 */
M3d::Mat4 SlicingVolumeRendererNode::getModelMatrix(const RapidGL::Node* node) {

    // Find transform nodes
    std::stack<RapidGL::TransformNode*> transformNodes;
    RapidGL::Node* parent = node->getParent();
    while (parent != NULL) {
        RapidGL::TransformNode* const transformNode = dynamic_cast<RapidGL::TransformNode*>(parent);
        if (transformNode != NULL) {
            transformNodes.push(transformNode);
        }
        parent = parent->getParent();
    }

    // Accumulate matrices
    RapidGL::State state;
    while (!transformNodes.empty()) {
        RapidGL::TransformNode* const transformNode = transformNodes.top();
        transformNode->visit(state);
        transformNodes.pop();
    }

    // Return
    return state.getModelMatrix();
}

/**
 * Looks up the texture unit to use for a volume node.
 *
 * @param volumeNode Volume node to find texture unit for
 * @return Texture unit for volume node
 * @throws std::logic_error if could not find texture unit for volume node
 */
Gloop::TextureUnit SlicingVolumeRendererNode::getTextureUnit(VolumeNode* volumeNode) const {
    std::map<VolumeNode*,Gloop::TextureUnit>::const_iterator it = textureUnitsByVolumeNode.find(volumeNode);
    if (it == textureUnitsByVolumeNode.end()) {
        throw std::logic_error("[SlicingVolumeRendererNode] Could not find texture unit!");
    }
    return it->second;
}

/**
 * Sets up the renderer.
 */
void SlicingVolumeRendererNode::preVisit(RapidGL::State& state) {

    // Skip if already ready
    if (ready) {
        return;
    }

    // Find `UseNode`
    RapidGL::UseNode* const useNode = RapidGL::findAncestor<RapidGL::UseNode>(this);
    if (useNode == NULL) {
        throw std::runtime_error("[SlicingVolumeRendererNode] Could not find use node!");
    }

    // Find `ProgramNode`
    RapidGL::ProgramNode* const programNode = useNode->getProgramNode();
    if (programNode == NULL) {
        throw std::runtime_error("[SlicingVolumeRendererNode] Use node hasn't found program node yet!");
    }

    // Find volume nodes
    volumeNodes = findDescendants<VolumeNode>(this);
    if (volumeNodes.empty()) {
        throw std::runtime_error("[SlicingVolumeRendererNode] Could not find any volume nodes!");
    }

    // Store texture units
    for (std::vector<VolumeNode*>::const_iterator it = volumeNodes.begin(); it != volumeNodes.end(); ++it) {
        VolumeNode* const volumeNode = *it;
        const std::string textureId = volumeNode->getTextureId();
        RapidGL::TextureNode* textureNode = RapidGL::findAncestor<RapidGL::TextureNode>(this, textureId);
        if (textureNode == NULL) {
            throw std::runtime_error("[SlicingVolumeRendererNode] Could not find texture node!");
        }
        putTextureUnit(volumeNode, textureNode->getTextureUnit());
    }

    // Get program
    const Gloop::Program program = programNode->getProgram();

    // Find uniform locations
    strategy->findUniformLocations(program);

    // Bind
    vao.bind();
    arrayBuffer.bind(vbo);

    // Allocate buffer
    const GLsizei sizeOfQuad = strategy->getSizeOfQuad();
    const GLsizei sizeOfBuffer = sizeOfQuad * numberOfSlices * volumeNodes.size();
    arrayBuffer.data(sizeOfBuffer, NULL, GL_STREAM_DRAW);

    // Set up attributes
    strategy->setUpAttributes(programNode, vao);

    // Unbind VAO
    arrayBuffer.unbind(vbo);
    vao.unbind();

    // Now ready
    ready = true;
}

/**
 * Stores a texture unit by its volume node.
 *
 * @param volumeNode Volume node
 * @param textureUnit Texture unit to use for volume node
 */
void SlicingVolumeRendererNode::putTextureUnit(VolumeNode* volumeNode, const Gloop::TextureUnit& textureUnit) {
    textureUnitsByVolumeNode.insert(std::pair<VolumeNode*,Gloop::TextureUnit>(volumeNode, textureUnit));
}

/**
 * Renders the volumes.
 */
void SlicingVolumeRendererNode::visit(RapidGL::State& state) {

    // Get view matrix
    const M3d::Mat4 viewMatrix = state.getViewMatrix();

    // Make quads
    std::vector<Quad> quads;

    for (std::vector<VolumeNode*>::iterator it = volumeNodes.begin(); it != volumeNodes.end(); ++it) {
        VolumeNode* const volumeNode = *it;

        // Get texture unit
        const Gloop::TextureUnit textureUnit = getTextureUnit(volumeNode);

        // Find extent
        const Extent extent = findExtent(volumeNode, viewMatrix);
        const double dz = (extent.max.z - extent.min.z) / numberOfSlices;

        // Calculate inverse model view matrix
        const M3d::Mat4 modelMatrix = getModelMatrix(volumeNode);
        const M3d::Mat4 modelViewMatrix = viewMatrix * modelMatrix;
        const M3d::Mat4 inverseModelViewMatrix = M3d::inverse(modelViewMatrix);

        for (int i = 0; i < numberOfSlices; ++i) {

            // Make quad
            Quad quad;

            // Set texture unit
            quad.unit = textureUnit.toOrdinal();

            // Compute Z coordinate of quad
            quad.z = extent.min.z + (dz * i);

            // Store points of quad
            quad.v0.x = extent.max.x;
            quad.v0.y = extent.max.y;
            quad.v1.x = extent.min.x;
            quad.v1.y = extent.max.y;
            quad.v2.x = extent.min.x;
            quad.v2.y = extent.min.y;
            quad.v3.x = extent.max.x;
            quad.v3.y = extent.min.y;

            // Compute texture coordinates
            for (int j = 0; j < 4; ++j) {
                Vertex* const v = &(quad[j]);
                const M3d::Vec4 p1(v->x, v->y, quad.z, 1);
                const M3d::Vec4 p2 = inverseModelViewMatrix * p1;
                v->s = p2.x + 0.5;
                v->t = p2.y + 0.5;
                v->p = p2.z + 0.5;
            }

            // Store quad
            quads.push_back(quad);
        }
    }

    // Sort the quads
    std::sort(quads.begin(), quads.end(), &compare);

    // Bind
    vao.bind();
    arrayBuffer.bind(vbo);

    // Draw quads
    strategy->draw(quads);

    // Unbind
    arrayBuffer.unbind(vbo);
    vao.unbind();
}

// ==================
// ATTRIBUTE STRATEGY
// ==================

SlicingVolumeRendererNode::AttributeStrategy::AttributeStrategy() {
    // empty
}

void SlicingVolumeRendererNode::AttributeStrategy::draw(const std::vector<Quad>& quads) {

    const Gloop::BufferTarget arrayBuffer = Gloop::BufferTarget::arrayBuffer();

    const int floatsPerBuffer = FLOATS_PER_QUAD * quads.size();
    GLfloat data[floatsPerBuffer];

    // Draw the quads
    int count = 0;
    for (std::vector<Quad>::const_iterator it = quads.begin(); it != quads.end(); ++it) {

        int p = count * FLOATS_PER_QUAD;

        /* upper right */
        data[p++] = it->v0.x; data[p++] = it->v0.y; data[p++] = it->z;
        data[p++] = it->v0.s; data[p++] = it->v0.t; data[p++] = it->v0.p;
        data[p++] = it->unit;

        /* upper left */
        data[p++] = it->v1.x; data[p++] = it->v1.y; data[p++] = it->z;
        data[p++] = it->v1.s; data[p++] = it->v1.t; data[p++] = it->v1.p;
        data[p++] = it->unit;

        /* lower left */
        data[p++] = it->v2.x; data[p++] = it->v2.y; data[p++] = it->z;
        data[p++] = it->v2.s; data[p++] = it->v2.t; data[p++] = it->v2.p;
        data[p++] = it->unit;

        /* lower left */
        data[p++] = it->v2.x; data[p++] = it->v2.y; data[p++] = it->z;
        data[p++] = it->v2.s; data[p++] = it->v2.t; data[p++] = it->v2.p;
        data[p++] = it->unit;

        /* lower right */
        data[p++] = it->v3.x; data[p++] = it->v3.y; data[p++] = it->z;
        data[p++] = it->v3.s; data[p++] = it->v3.t; data[p++] = it->v3.p;
        data[p++] = it->unit;

        /* upper right */
        data[p++] = it->v0.x; data[p++] = it->v0.y; data[p++] = it->z;
        data[p++] = it->v0.s; data[p++] = it->v0.t; data[p++] = it->v0.p;
        data[p++] = it->unit;

        ++count;
    }
    if (count > 0) {
        arrayBuffer.subData(0, count * SIZE_OF_QUAD, data);
        glDrawArrays(GL_TRIANGLES, 0, count * VERTICES_PER_QUAD);
    }
}

void SlicingVolumeRendererNode::AttributeStrategy::findUniformLocations(const Gloop::Program& program) {
    // empty
}

GLsizei SlicingVolumeRendererNode::AttributeStrategy::getSizeOfQuad() {
    return SIZE_OF_QUAD;
}

void SlicingVolumeRendererNode::AttributeStrategy::setUpAttributes(RapidGL::ProgramNode* programNode,
                                                                   const Gloop::VertexArrayObject& vao) {

    // Get program from program node
    const Gloop::Program program = programNode->getProgram();

    // Find attribute names by usage
    std::map<RapidGL::AttributeNode::Usage,std::string> attributeNamesByUsage;
    const RapidGL::Node::node_range_t programNodeChildren = programNode->getChildren();
    for (RapidGL::Node::node_iterator_t it = programNodeChildren.begin; it != programNodeChildren.end; ++it) {
        const RapidGL::AttributeNode* attributeNode = dynamic_cast<RapidGL::AttributeNode*>(*it);
        if (attributeNode != NULL) {
            attributeNamesByUsage[attributeNode->getUsage()] = attributeNode->getName();
        }
    }

    // Make sure a position and coordinate were specified
    if (attributeNamesByUsage.count(RapidGL::AttributeNode::POSITION) == 0) {
        throw std::runtime_error("[SlicingVolumeRendererNode] Could not find attribute for position!");
    }
    if (attributeNamesByUsage.count(RapidGL::AttributeNode::TEXCOORD0) == 0) {
        throw std::runtime_error("[SlicingVolumeRendererNode] Could not find attribute for coordinate!");
    }
    if (attributeNamesByUsage.count(RapidGL::AttributeNode::COLOR) == 0) {
        throw std::runtime_error("[SlicingVolumeRendererNode] Could not find attribute for color!");
    }

    // Find locations
    const std::string positionName = attributeNamesByUsage[RapidGL::AttributeNode::POSITION];
    const GLint pointLocation = program.attribLocation(positionName);
    if (pointLocation < 0) {
        throw std::runtime_error("[SlicingVolumeRendererNode] Could not find location for position attribute!");
    }
    const std::string coordName = attributeNamesByUsage[RapidGL::AttributeNode::TEXCOORD0];
    const GLint coordLocation = program.attribLocation(coordName);
    if (coordLocation < 0) {
        throw std::runtime_error("[SlicingVolumeRendererNode] Could not find location for coordinate attribute!");
    }
    const std::string colorName = attributeNamesByUsage[RapidGL::AttributeNode::COLOR];
    const GLint colorLocation = program.attribLocation(colorName);
    if (colorLocation < 0) {
        throw std::runtime_error("[SlicingVolumeRendererNode] Could not find location for color attribute!");
    }

    // Set up attributes
    vao.enableVertexAttribArray(pointLocation);
    vao.vertexAttribPointer(Gloop::VertexAttribPointer()
            .index(pointLocation)
            .size(3)
            .type(GL_FLOAT)
            .stride(SIZE_OF_VERTEX)
            .offset(0));
    vao.enableVertexAttribArray(coordLocation);
    vao.vertexAttribPointer(Gloop::VertexAttribPointer()
            .index(coordLocation)
            .size(3)
            .type(GL_FLOAT)
            .stride(SIZE_OF_VERTEX)
            .offset(SIZE_OF_FLOAT * 3));
    vao.enableVertexAttribArray(colorLocation);
    vao.vertexAttribPointer(Gloop::VertexAttribPointer()
            .index(colorLocation)
            .size(1)
            .type(GL_FLOAT)
            .stride(SIZE_OF_VERTEX)
            .offset(SIZE_OF_FLOAT * 6));
}

// ================
// UNIFORM STRATEGY
// ================

SlicingVolumeRendererNode::UniformStrategy::UniformStrategy(const std::string& uniformName) :
        uniformName(uniformName) {
    if (uniformName.empty()) {
        throw std::invalid_argument("[SlicingVolumeRenderer] Uniform name is empty!");
    }
}

void SlicingVolumeRendererNode::UniformStrategy::draw(const std::vector<Quad>& quads) {

    const Gloop::BufferTarget arrayBuffer = Gloop::BufferTarget::arrayBuffer();

    const int floatsPerBuffer = FLOATS_PER_QUAD * quads.size();
    GLfloat data[floatsPerBuffer];

    // Draw the quads
    int count = 0;
    GLint lastUnit = 0;
    glUniform1i(uniformLocation, 0);
    for (std::vector<Quad>::const_iterator it = quads.begin(); it != quads.end(); ++it) {

        // Flush buffer if uniform changed
        if (it->unit != lastUnit) {
            arrayBuffer.subData(0, count * SIZE_OF_QUAD, data);
            glDrawArrays(GL_TRIANGLES, 0, count * VERTICES_PER_QUAD);
            glUniform1i(uniformLocation, it->unit);
            lastUnit = it->unit;
            count = 0;
        }

        int p = count * FLOATS_PER_QUAD;

        /* upper right */
        data[p++] = it->v0.x; data[p++] = it->v0.y; data[p++] = it->z;
        data[p++] = it->v0.s; data[p++] = it->v0.t; data[p++] = it->v0.p;

        /* upper left */
        data[p++] = it->v1.x; data[p++] = it->v1.y; data[p++] = it->z;
        data[p++] = it->v1.s; data[p++] = it->v1.t; data[p++] = it->v1.p;

        /* lower left */
        data[p++] = it->v2.x; data[p++] = it->v2.y; data[p++] = it->z;
        data[p++] = it->v2.s; data[p++] = it->v2.t; data[p++] = it->v2.p;

        /* lower left */
        data[p++] = it->v2.x; data[p++] = it->v2.y; data[p++] = it->z;
        data[p++] = it->v2.s; data[p++] = it->v2.t; data[p++] = it->v2.p;

        /* lower right */
        data[p++] = it->v3.x; data[p++] = it->v3.y; data[p++] = it->z;
        data[p++] = it->v3.s; data[p++] = it->v3.t; data[p++] = it->v3.p;

        /* upper right */
        data[p++] = it->v0.x; data[p++] = it->v0.y; data[p++] = it->z;
        data[p++] = it->v0.s; data[p++] = it->v0.t; data[p++] = it->v0.p;

        ++count;
    }
    if (count > 0) {
        arrayBuffer.subData(0, count * SIZE_OF_QUAD, data);
        glDrawArrays(GL_TRIANGLES, 0, count * VERTICES_PER_QUAD);
    }
}

void SlicingVolumeRendererNode::UniformStrategy::findUniformLocations(const Gloop::Program& program) {
    uniformLocation = program.uniformLocation(uniformName);
    if (uniformLocation < 0) {
        throw std::runtime_error("[SlicingVolumeRendererNode] Could not find uniform in program!");
    }
}

GLsizei SlicingVolumeRendererNode::UniformStrategy::getSizeOfQuad() {
    return SIZE_OF_QUAD;
}

void SlicingVolumeRendererNode::UniformStrategy::setUpAttributes(RapidGL::ProgramNode* programNode,
                                                                 const Gloop::VertexArrayObject& vao) {

    // Get program
    const Gloop::Program program = programNode->getProgram();

    // Find attribute names by usage
    std::map<RapidGL::AttributeNode::Usage,std::string> attributeNamesByUsage;
    const RapidGL::Node::node_range_t programNodeChildren = programNode->getChildren();
    for (RapidGL::Node::node_iterator_t it = programNodeChildren.begin; it != programNodeChildren.end; ++it) {
        const RapidGL::AttributeNode* attributeNode = dynamic_cast<RapidGL::AttributeNode*>(*it);
        if (attributeNode != NULL) {
            attributeNamesByUsage[attributeNode->getUsage()] = attributeNode->getName();
        }
    }

    // Make sure a position and coordinate were specified
    if (attributeNamesByUsage.count(RapidGL::AttributeNode::POSITION) == 0) {
        throw std::runtime_error("[SlicingVolumeRendererNode] Could not find attribute for position!");
    }
    if (attributeNamesByUsage.count(RapidGL::AttributeNode::TEXCOORD0) == 0) {
        throw std::runtime_error("[SlicingVolumeRendererNode] Could not find attribute for coordinate!");
    }

    // Find locations
    const std::string positionName = attributeNamesByUsage[RapidGL::AttributeNode::POSITION];
    const GLint pointLocation = program.attribLocation(positionName);
    if (pointLocation < 0) {
        throw std::runtime_error("[SlicingVolumeRendererNode] Could not find location for position attribute!");
    }
    const std::string coordName = attributeNamesByUsage[RapidGL::AttributeNode::TEXCOORD0];
    const GLint coordLocation = program.attribLocation(coordName);
    if (coordLocation < 0) {
        throw std::runtime_error("[SlicingVolumeRendererNode] Could not find location for coordinate attribute!");
    }

    // Set up attributes
    vao.enableVertexAttribArray(pointLocation);
    vao.vertexAttribPointer(Gloop::VertexAttribPointer()
            .index(pointLocation)
            .size(3)
            .type(GL_FLOAT)
            .stride(SIZE_OF_VERTEX)
            .offset(0));
    vao.enableVertexAttribArray(coordLocation);
    vao.vertexAttribPointer(Gloop::VertexAttribPointer()
            .index(coordLocation)
            .size(3)
            .type(GL_FLOAT)
            .stride(SIZE_OF_VERTEX)
            .offset(SIZE_OF_FLOAT * 3));
}
