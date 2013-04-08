#include "config.h"
#include <algorithm>
#include <limits>
#include <vector>
#include <gloop/BufferTarget.hxx>
#include <glycerin/BufferLayoutBuilder.hxx>
#include <m3d/Mat4.h>
#include <m3d/Vec3.h>
#include <m3d/Vec4.h>
#include <RapidGL/TransformNode.h>
#include "BooleanAndNode.h"

BooleanAndNode::BooleanAndNode(const std::string& id1, const std::string& id2) :
        id1(id1), id2(id2), ready(false), dirty(true), drawable(false),
        vbo(Gloop::BufferObject::generate()),
        layout(createLayout()) {

    // Check IDs
    if (id1.empty() || id2.empty()) {
        throw std::invalid_argument("[BooleanAndNode] IDs cannot be empty!");
    }

    // Allocate buffer object
    const Gloop::BufferTarget arrayBuffer = Gloop::BufferTarget::arrayBuffer();
    arrayBuffer.bind(vbo);
    arrayBuffer.data(layout.sizeInBytes(), NULL, GL_DYNAMIC_DRAW);
    arrayBuffer.unbind(vbo);
}

BooleanAndNode::~BooleanAndNode() {
    vbo.dispose();
    for (std::map<Gloop::Program,Gloop::VertexArrayObject>::iterator it = vaos.begin(); it != vaos.end(); ++it) {
        it->second.dispose();
    }
}

Glycerin::BufferLayout BooleanAndNode::createLayout() {
    return Glycerin::BufferLayoutBuilder()
            .count(36)
            .components(3)
            .region("POSITION")
            .region("TEXCOORD0")
            .region("TEXCOORD1")
            .interleaved(true)
            .build();
}

Gloop::VertexArrayObject BooleanAndNode::createVertexArrayObject(const Gloop::Program& program) {

    // Generate VAO
    const Gloop::VertexArrayObject vao = Gloop::VertexArrayObject::generate();

    // Find program node for program
    const RapidGL::Node* root = RapidGL::findRoot(this);
    const RapidGL::ProgramNode* programNode = RapidGL::findProgramNode(root, program);
    if (programNode == NULL) {
        throw std::runtime_error("[BooleanAndNode] Could not find program node for program!");
    }

    // Store array buffer target
    const Gloop::BufferTarget arrayBuffer = Gloop::BufferTarget::arrayBuffer();

    // Store locations by region
    std::map<std::string,GLint> locationsByRegion;
    const RapidGL::Node::node_range_t programNodeChildren = programNode->getChildren();
    for (RapidGL::Node::node_iterator_t it = programNodeChildren.begin; it != programNodeChildren.end; ++it) {
        const RapidGL::AttributeNode* attributeNode = dynamic_cast<RapidGL::AttributeNode*>(*it);
        if (attributeNode != NULL) {
            const GLint location = attributeNode->getLocation();
            if (location >= 0) {
                const std::string usage = RapidGL::AttributeNode::formatUsage(attributeNode->getUsage());
                locationsByRegion[usage] = location;
            }
        }
    }

    // Bind VAO and VBO
    vao.bind();
    arrayBuffer.bind(vbo);

    // Enable attributes
    for (Glycerin::BufferLayout::const_iterator it = layout.begin(); it != layout.end(); ++it) {
        if (locationsByRegion.count(it->name()) > 0) {
            const GLint location = locationsByRegion.find(it->name())->second;
            vao.enableVertexAttribArray(location);
            vao.vertexAttribPointer(Gloop::VertexAttribPointer()
                    .index(location)
                    .size(it->components())
                    .offset(it->offset())
                    .stride(it->stride()));
        }
    }

    // Unbind VAO and VBO
    arrayBuffer.unbind(vbo);
    vao.unbind();

    // Return VAO
    return vao;
}

BooleanAndNode::Extent BooleanAndNode::findExtent(RapidGL::CubeNode* const cubeNode) {

    // Find the root
    const RapidGL::Node* root = RapidGL::findRoot(this);

    // Find all the nodes above it
    std::vector<RapidGL::Node*> nodes;
    RapidGL::Node* parent = cubeNode->getParent();
    while (parent != NULL) {
        nodes.push_back(parent);
        parent = parent->getParent();
    }

    // Reverse the nodes
    std::reverse(nodes.begin(), nodes.end());

    // Apply all their transformations
    RapidGL::State state;
    for (std::vector<RapidGL::Node*>::const_iterator it = nodes.begin(); it != nodes.end(); it++) {
        RapidGL::TransformNode* transformNode = dynamic_cast<RapidGL::TransformNode*>(*it);
        if (transformNode != NULL) {
            transformNode->visit(state);
        }
    }

    // Get model matrix
    const M3d::Mat4 modelMatrix = state.getModelMatrix();

    // Calculate extent
    Extent extent;
    extent.min = modelMatrix * M3d::Vec4(-0.5, -0.5, -0.5, 1.0);
    extent.max = modelMatrix * M3d::Vec4(+0.5, +0.5, +0.5, 1.0);
    return extent;
}

Gloop::VertexArrayObject BooleanAndNode::getVertexArrayObject(const Gloop::Program& program) {
    std::map<Gloop::Program,Gloop::VertexArrayObject>::iterator it = vaos.find(program);
    if (it == vaos.end()) {
        const Gloop::VertexArrayObject vao = createVertexArrayObject(program);
        vaos.insert(std::pair<Gloop::Program,Gloop::VertexArrayObject>(program, vao));
        return vao;
    } else {
        return it->second;
    }
}

bool BooleanAndNode::isDrawable(const M3d::Vec4& min, const M3d::Vec4& max) {
    for (int i = 0; i < 3; ++i) {
        if (max[i] - min[i] < 0) {
            return false;
        }
    }
    return true;
}

void BooleanAndNode::nodeChanged(RapidGL::Node* node) {
    dirty = true;
}

void BooleanAndNode::preVisit(RapidGL::State& state) {

    // Skip if already ready
    if (ready) {
        return;
    }

    // Find the root
    const RapidGL::Node* root = RapidGL::findRoot(this);

    // Search for first cube node
    c1 = dynamic_cast<RapidGL::CubeNode*>(RapidGL::findDescendant(root, id1));
    if (c1 == NULL) {
        throw std::runtime_error("[BooleanAndNode] Could not find first cube!");
    }

    // Search for second cube node
    c2 = dynamic_cast<RapidGL::CubeNode*>(RapidGL::findDescendant(root, id2));
    if (c2 == NULL) {
        throw std::runtime_error("[BooleanAndNode] Could not find second cube!");
    }

    // Observe transformations
    RapidGL::Node* cubes[] = { c1, c2 };
    for (int i = 0; i < 2; ++i) {
        RapidGL::Node* t = RapidGL::findAncestor<RapidGL::TransformNode>(cubes[i]);
        if (t != NULL) {
            t->addNodeListener(this);
        }
    }

    // Now ready
    ready = true;
}

void BooleanAndNode::update() {

    // Find extents of two cubes in world space
    const Extent e1 = findExtent(c1);
    const Extent e2 = findExtent(c2);

    // Find overlap
    const M3d::Vec4 min = M3d::max(e1.min, e2.min);
    const M3d::Vec4 max = M3d::min(e1.max, e2.max);

    // Check if drawable
    drawable = isDrawable(min, max);
    if (!drawable) {
        return;
    }

    // Store x, y, and z
    const GLfloat x1 = e1.min.x;
    const GLfloat y1 = e1.min.y;
    const GLfloat z1 = e1.min.z;
    const GLfloat x2 = e2.min.x;
    const GLfloat y2 = e2.min.y;
    const GLfloat z2 = e2.min.z;

    // Calculate width, height, and depth
    const GLfloat w1 = e1.max.x - e1.min.x;
    const GLfloat h1 = e1.max.y - e1.min.y;
    const GLfloat d1 = e1.max.z - e1.min.z;
    const GLfloat w2 = e2.max.x - e2.min.x;
    const GLfloat h2 = e2.max.y - e2.min.y;
    const GLfloat d2 = e2.max.z - e2.min.z;

    // Create data
    GLfloat data[] = {

        // Front face
        max.x, max.y, max.z,
        ((max.x - x1) / w1), ((max.y - y1) / h1), ((max.z - z1) / d1),
        ((max.x - x2) / w2), ((max.y - y2) / h2), ((max.z - z2) / d2),
        min.x, max.y, max.z,
        ((min.x - x1) / w1), ((max.y - y1) / h1), ((max.z - z1) / d1),
        ((min.x - x2) / w2), ((max.y - y2) / h2), ((max.z - z2) / d2),
        min.x, min.y, max.z,
        ((min.x - x1) / w1), ((min.y - y1) / h1), ((max.z - z1) / d1),
        ((min.x - x2) / w2), ((min.y - y2) / h2), ((max.z - z2) / d2),
        min.x, min.y, max.z,
        ((min.x - x1) / w1), ((min.y - y1) / h1), ((max.z - z1) / d1),
        ((min.x - x2) / w2), ((min.y - y2) / h2), ((max.z - z2) / d2),
        max.x, min.y, max.z,
        ((max.x - x1) / w1), ((min.y - y1) / h1), ((max.z - z1) / d1),
        ((max.x - x2) / w2), ((min.y - y2) / h2), ((max.z - z2) / d2),
        max.x, max.y, max.z,
        ((max.x - x1) / w1), ((max.y - y1) / h1), ((max.z - z1) / d1),
        ((max.x - x2) / w2), ((max.y - y2) / h2), ((max.z - z2) / d2),

        // Back face
        min.x, max.y, min.z,
        ((min.x - x1) / w1), ((max.y - y1) / h1), ((min.z - z1) / d1),
        ((min.x - x2) / w2), ((max.y - y2) / h2), ((min.z - z2) / d2),
        max.x, max.y, min.z,
        ((max.x - x1) / w1), ((max.y - y1) / h1), ((min.z - z1) / d1),
        ((max.x - x2) / w2), ((max.y - y2) / h2), ((min.z - z2) / d2),
        max.x, min.y, min.z,
        ((max.x - x1) / w1), ((min.y - y1) / h1), ((min.z - z1) / d1),
        ((max.x - x2) / w2), ((min.y - y2) / h2), ((min.z - z2) / d2),
        max.x, min.y, min.z,
        ((max.x - x1) / w1), ((min.y - y1) / h1), ((min.z - z1) / d1),
        ((max.x - x2) / w2), ((min.y - y2) / h2), ((min.z - z2) / d2),
        min.x, min.y, min.z,
        ((min.x - x1) / w1), ((min.y - y1) / h1), ((min.z - z1) / d1),
        ((min.x - x2) / w2), ((min.y - y2) / h2), ((min.z - z2) / d2),
        min.x, max.y, min.z,
        ((min.x - x1) / w1), ((max.y - y1) / h1), ((min.z - z1) / d1),
        ((min.x - x2) / w2), ((max.y - y2) / h2), ((min.z - z2) / d2),

        // Left face
        min.x, max.y, max.z,
        ((min.x - x1) / w1), ((max.y - y1) / h1), ((max.z - z1) / d1),
        ((min.x - x2) / w2), ((max.y - y2) / h2), ((max.z - z2) / d2),
        min.x, max.y, min.z,
        ((min.x - x1) / w1), ((max.y - y1) / h1), ((min.z - z1) / d1),
        ((min.x - x2) / w2), ((max.y - y2) / h2), ((min.z - z2) / d2),
        min.x, min.y, min.z,
        ((min.x - x1) / w1), ((min.y - y1) / h1), ((min.z - z1) / d1),
        ((min.x - x2) / w2), ((min.y - y2) / h2), ((min.z - z2) / d2),
        min.x, min.y, min.z,
        ((min.x - x1) / w1), ((min.y - y1) / h1), ((min.z - z1) / d1),
        ((min.x - x2) / w2), ((min.y - y2) / h2), ((min.z - z2) / d2),
        min.x, min.y, max.z,
        ((min.x - x1) / w1), ((min.y - y1) / h1), ((max.z - z1) / d1),
        ((min.x - x2) / w2), ((min.y - y2) / h2), ((max.z - z2) / d2),
        min.x, max.y, max.z,
        ((min.x - x1) / w1), ((max.y - y1) / h1), ((max.z - z1) / d1),
        ((min.x - x2) / w2), ((max.y - y2) / h2), ((max.z - z2) / d2),

        // Right face
        max.x, max.y, min.z,
        ((max.x - x1) / w1), ((max.y - y1) / h1), ((min.z - z1) / d1),
        ((max.x - x2) / w2), ((max.y - y2) / h2), ((min.z - z2) / d2),
        max.x, max.y, max.z,
        ((max.x - x1) / w1), ((max.y - y1) / h1), ((max.z - z1) / d1),
        ((max.x - x2) / w2), ((max.y - y2) / h2), ((max.z - z2) / d2),
        max.x, min.y, max.z,
        ((max.x - x1) / w1), ((min.y - y1) / h1), ((max.z - z1) / d1),
        ((max.x - x2) / w2), ((min.y - y2) / h2), ((max.z - z2) / d2),
        max.x, min.y, max.z,
        ((max.x - x1) / w1), ((min.y - y1) / h1), ((max.z - z1) / d1),
        ((max.x - x2) / w2), ((min.y - y2) / h2), ((max.z - z2) / d2),
        max.x, min.y, min.z,
        ((max.x - x1) / w1), ((min.y - y1) / h1), ((min.z - z1) / d1),
        ((max.x - x2) / w2), ((min.y - y2) / h2), ((min.z - z2) / d2),
        max.x, max.y, min.z,
        ((max.x - x1) / w1), ((max.y - y1) / h1), ((min.z - z1) / d1),
        ((max.x - x2) / w2), ((max.y - y2) / h2), ((min.z - z2) / d2),

        // Top face
        max.x, max.y, min.z,
        ((max.x - x1) / w1), ((max.y - y1) / h1), ((min.z - z1) / d1),
        ((max.x - x2) / w2), ((max.y - y2) / h2), ((min.z - z2) / d2),
        min.x, max.y, min.z,
        ((min.x - x1) / w1), ((max.y - y1) / h1), ((min.z - z1) / d1),
        ((min.x - x2) / w2), ((max.y - y2) / h2), ((min.z - z2) / d2),
        min.x, max.y, max.z,
        ((min.x - x1) / w1), ((max.y - y1) / h1), ((max.z - z1) / d1),
        ((min.x - x2) / w2), ((max.y - y2) / h2), ((max.z - z2) / d2),
        min.x, max.y, max.z,
        ((min.x - x1) / w1), ((max.y - y1) / h1), ((max.z - z1) / d1),
        ((min.x - x2) / w2), ((max.y - y2) / h2), ((max.z - z2) / d2),
        max.x, max.y, max.z,
        ((max.x - x1) / w1), ((max.y - y1) / h1), ((max.z - z1) / d1),
        ((max.x - x2) / w2), ((max.y - y2) / h2), ((max.z - z2) / d2),
        max.x, max.y, min.z,
        ((max.x - x1) / w1), ((max.y - y1) / h1), ((min.z - z1) / d1),
        ((max.x - x2) / w2), ((max.y - y2) / h2), ((min.z - z2) / d2),

        // Bottom face
        max.x, min.y, max.z,
        ((max.x - x1) / w1), ((min.y - y1) / h1), ((max.z - z1) / d1),
        ((max.x - x2) / w2), ((min.y - y2) / h2), ((max.z - z2) / d2),
        min.x, min.y, max.z,
        ((min.x - x1) / w1), ((min.y - y1) / h1), ((max.z - z1) / d1),
        ((min.x - x2) / w2), ((min.y - y2) / h2), ((max.z - z2) / d2),
        min.x, min.y, min.z,
        ((min.x - x1) / w1), ((min.y - y1) / h1), ((min.z - z1) / d1),
        ((min.x - x2) / w2), ((min.y - y2) / h2), ((min.z - z2) / d2),
        min.x, min.y, min.z,
        ((min.x - x1) / w1), ((min.y - y1) / h1), ((min.z - z1) / d1),
        ((min.x - x2) / w2), ((min.y - y2) / h2), ((min.z - z2) / d2),
        max.x, min.y, min.z,
        ((max.x - x1) / w1), ((min.y - y1) / h1), ((min.z - z1) / d1),
        ((max.x - x2) / w2), ((min.y - y2) / h2), ((min.z - z2) / d2),
        max.x, min.y, max.z,
        ((max.x - x1) / w1), ((min.y - y1) / h1), ((max.z - z1) / d1),
        ((max.x - x2) / w2), ((min.y - y2) / h2), ((max.z - z2) / d2)
    };

    // Send to buffer
    const Gloop::BufferTarget arrayBuffer = Gloop::BufferTarget::arrayBuffer();
    arrayBuffer.bind(vbo);
    arrayBuffer.subData(0, sizeof(data), data);
    arrayBuffer.unbind(vbo);
}

void BooleanAndNode::visit(RapidGL::State& state) {

    // Update if dirty
    if (dirty) {
        update();
        dirty = false;
    }

    // Skip if not drawable
    if (!drawable) {
        return;
    }

    // Get the current program
    const Gloop::Program program = Gloop::Program::current();

    // Get the VAO for that program
    const Gloop::VertexArrayObject vao = getVertexArrayObject(program);

    // Draw
    vao.bind();
    glDrawArrays(GL_TRIANGLES, 0, layout.begin()->count());
    vao.unbind();
}
