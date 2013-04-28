#include "config.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <gloop/Program.hxx>
#include <gloop/VertexAttribPointer.hxx>
#include <glycerin/BufferLayout.hxx>
#include <glycerin/BufferLayoutBuilder.hxx>
#include <glycerin/BufferRegion.hxx>
#include <m3d/Vec4.h>
#include <RapidGL/AttributeNode.h>
#include <RapidGL/ProgramNode.h>
#include <RapidGL/TransformNode.h>
#include <RapidGL/UseNode.h>
#include "BooleanXorNode.h"

// Singleton instance of `AlwaysFilter`
BooleanXorNode::AlwaysFilter BooleanXorNode::AlwaysFilter::INSTANCE;

// Singleton instance of `EqualFilter`
BooleanXorNode::EqualFilter BooleanXorNode::EqualFilter::INSTANCE;

// Singleton instance of `GreaterThanFilter`
BooleanXorNode::GreaterThanFilter BooleanXorNode::GreaterThanFilter::INSTANCE;

// Singleton instance of `GreaterThanOrEqualFilter`
BooleanXorNode::GreaterThanOrEqualFilter BooleanXorNode::GreaterThanOrEqualFilter::INSTANCE;

// Singleton instance of `LessThanFilter`
BooleanXorNode::LessThanFilter BooleanXorNode::LessThanFilter::INSTANCE;

// Singleton instance of `LessThanOrEqualFilter`
BooleanXorNode::LessThanOrEqualFilter BooleanXorNode::LessThanOrEqualFilter::INSTANCE;

// Singleton instance of `NeverFilter`
BooleanXorNode::NeverFilter BooleanXorNode::NeverFilter::INSTANCE;

// Singleton instance of `NotEqualFilter`
BooleanXorNode::NotEqualFilter BooleanXorNode::NotEqualFilter::INSTANCE;

// Map of filters indexed by depth function
const std::map<GLenum,BooleanXorNode::Filter*> BooleanXorNode::FILTERS = BooleanXorNode::createFilters();

/**
 * Constructs a `BooleanXorNode`.
 *
 * @param ids Identifiers of nodes to include in operation
 * @param hide Identifier of node to hide pieces for, which may be empty
 * @param filter Depth function used to filter pieces, e.g. `GL_LESS` or `GL_ALWAYS`
 * @throws std::invalid_argument
 *         if size of set is not two, or
 *         if either identifier in set is empty, or
 *         if identifier of node to hide pieces for is invalid, or
 *         if filter is not a valid depth function
 */
BooleanXorNode::BooleanXorNode(const std::set<std::string>& ids, const std::string& hide, const GLenum filter) :
        ready(false), dirty(true), id1(getFirst(ids)), id2(getSecond(ids)), c1(NULL), c2(NULL),
        hidePiecesOfFirstCube(id1 == hide), hidePiecesOfSecondCube(id2 == hide),
        filter(getFilter(filter)),
        vbo(Gloop::BufferObject::generate()),
        layout(createBufferLayout()),
        arrayBuffer(Gloop::BufferTarget::arrayBuffer()) {
    if (ids.size() != 2) {
        throw std::invalid_argument("[BooleanXorNode] Size of set is not two!");
    } else if (id1.empty()) {
        throw std::invalid_argument("[BooleanXorNode] First identifier is empty!");
    } else if (id2.empty()) {
        throw std::invalid_argument("[BooleanXorNode] Second identifier is empty!");
    } else if (!hide.empty() && (ids.count(hide) == 0)) {
        throw std::invalid_argument("[BooleanXorNode] Identifier to hide is not in set!");
    } else if (!isDepthFunction(filter)) {
        throw std::invalid_argument("[BooleanXorNode] Filter is not a valid depth function!");
    }
}

/**
 * Destructs this `BooleanXorNode`.
 */
BooleanXorNode::~BooleanXorNode() {
    vbo.dispose();
    for (std::map<Gloop::Program,Gloop::VertexArrayObject>::iterator it = vaos.begin(); it != vaos.end(); ++it) {
        it->second.dispose();
    }
}

bool BooleanXorNode::AlwaysFilter::filter(const double x, const double y) {
    return true;
}

void BooleanXorNode::clipBack(const Piece& piece, const Extent& area, std::vector<Piece>& pieces) {
    const double back = area.min.z;
    if (piece.max.z <= back) {
        pieces.push_back(piece);
    } else if (piece.min.z >= back) {
        // discard piece
    } else {

        // Calculate texture coordinate
        const Extent* of = piece.of;
        const double crd = (back - of->min.z) / getDepth(*of);

        // Store the first piece
        Piece p1 = piece;
        p1.max.z = back;
        p1.max.p = crd;
        pieces.push_back(p1);
    }
}

void BooleanXorNode::clipBottom(const Piece& piece, const Extent& area, std::vector<Piece>& pieces) {
    const double bottom = area.min.y;
    if (piece.max.y <= bottom) {
        pieces.push_back(piece);
    } else if (piece.min.y >= bottom) {
        clipFront(piece, area, pieces);
    } else {

        // Calculate texture coordinate
        const Extent* of = piece.of;
        const double crd = (bottom - of->min.y) / getHeight(*of);

        // Store the first piece
        Piece p1 = piece;
        p1.max.y = bottom;
        p1.max.t = crd;
        pieces.push_back(p1);

        // Pass the second piece on
        Piece p2 = piece;
        p2.min.y = bottom;
        p2.min.t = crd;
        clipFront(p2, area, pieces);
    }
}

void BooleanXorNode::clipFront(const Piece& piece, const Extent& area, std::vector<Piece>& pieces) {
    const double front = area.max.z;
    if (piece.min.z >= front) {
        pieces.push_back(piece);
    } else if (piece.max.z <= front) {
        clipBack(piece, area, pieces);
    } else {

        // Calculate texture coordinate
        const Extent* of = piece.of;
        const double crd = (front - of->min.z) / getDepth(*of);

        // Store the first piece
        Piece p1 = piece;
        p1.min.z = front;
        p1.min.p = crd;
        pieces.push_back(p1);

        // Pass the second piece on
        Piece p2 = piece;
        p2.max.z = front;
        p2.max.p = crd;
        clipBack(p2, area, pieces);
    }
}

void BooleanXorNode::clipLeft(const Piece& piece, const Extent& area, std::vector<Piece>& pieces) {
    const double left = area.min.x;
    if (piece.max.x <= left) {
        pieces.push_back(piece);
    } else if (piece.min.x >= left) {
        clipRight(piece, area, pieces);
    } else {

        // Calculate texture coordinate
        const Extent* of = piece.of;
        const double crd = (left - of->min.x) / getWidth(*of);

        // Store the first piece
        Piece p1 = piece;
        p1.max.x = left;
        p1.max.s = crd;
        pieces.push_back(p1);

        // Pass the second piece on
        Piece p2 = piece;
        p2.min.x = left;
        p2.min.s = crd;
        clipRight(p2, area, pieces);
    }
}

void BooleanXorNode::clipRight(const Piece& piece, const Extent& area, std::vector<Piece>& pieces) {
    const double right = area.max.x;
    if (piece.min.x >= right) {
        pieces.push_back(piece);
    } else if (piece.max.x <= right) {
        clipTop(piece, area, pieces);
    } else {

        // Calculate texture coordinate
        const Extent* of = piece.of;
        const double crd = (right - of->min.x) / getWidth(*of);

        // Store the first piece
        Piece p1 = piece;
        p1.min.x = right;
        p1.min.s = crd;
        pieces.push_back(p1);

        // Pass the second piece on
        Piece p2 = piece;
        p2.max.x = right;
        p2.max.s = crd;
        clipTop(piece, area, pieces);
    }
}

void BooleanXorNode::clipTop(const Piece& piece, const Extent& area, std::vector<Piece>& pieces) {
    const double top = area.max.y;
    if (piece.min.y >= top) {
        pieces.push_back(piece);
    } else if (piece.max.y <= top) {
        clipBottom(piece, area, pieces);
    } else {

        // Calculate texture coordinate
        const Extent* of = piece.of;
        const double crd = (top - of->min.y) / getHeight(*of);

        // Store the first piece
        Piece p1 = piece;
        p1.min.y = top;
        p1.min.t = crd;
        pieces.push_back(p1);

        // Pass the second piece on
        Piece p2 = piece;
        p2.max.y = top;
        p2.max.t = crd;
        clipBottom(piece, area, pieces);
    }
}

/**
 * Makes the buffer layout.
 */
Glycerin::BufferLayout BooleanXorNode::createBufferLayout() {
    return Glycerin::BufferLayoutBuilder()
            .count(MAX_VERTICES_PER_BUFFER)
            .interleaved(true)
            .components(3)
            .region("POSITION")
            .region("TEXCOORD0")
            .build();
}

/**
 * Makes the map of filters indexed by depth function.
 *
 * @return Map of filters indexed by depth function
 */
std::map<GLenum,BooleanXorNode::Filter*> BooleanXorNode::createFilters() {
    std::map<GLenum,Filter*> filters;
    filters[GL_ALWAYS] = &AlwaysFilter::INSTANCE;
    filters[GL_EQUAL] = &EqualFilter::INSTANCE;
    filters[GL_LESS] = &LessThanFilter::INSTANCE;
    filters[GL_LEQUAL] = &LessThanOrEqualFilter::INSTANCE;
    filters[GL_GREATER] = &GreaterThanFilter::INSTANCE;
    filters[GL_GEQUAL] = &GreaterThanOrEqualFilter::INSTANCE;
    filters[GL_NEVER] = &NeverFilter::INSTANCE;
    filters[GL_NOTEQUAL] = &NotEqualFilter::INSTANCE;
    return filters;
}

/**
 * Creates a piece from an extent.
 *
 * @param extent Extent to use for piece's position
 * @return Piece made from extent
 */
BooleanXorNode::Piece BooleanXorNode::createPiece(const Extent& extent) {

    // Make piece
    Piece piece;

    // Set minimum
    piece.min.x = extent.min.x;
    piece.min.y = extent.min.y;
    piece.min.z = extent.min.z;
    piece.min.s = 0.0;
    piece.min.t = 0.0;
    piece.min.p = 0.0;

    // Set maximum
    piece.max.x = extent.max.x;
    piece.max.y = extent.max.y;
    piece.max.z = extent.max.z;
    piece.max.s = 1.0;
    piece.max.t = 1.0;
    piece.max.p = 1.0;

    // Store pointer to extent
    piece.of = &extent;

    // Return piece
    return piece;
}

Gloop::VertexArrayObject BooleanXorNode::createVertexArrayObject(const Gloop::Program& program) {

    // Generate new VAO
    const Gloop::VertexArrayObject vao = Gloop::VertexArrayObject::generate();

    // Find root
    RapidGL::Node* root = RapidGL::findRoot(this);

    // Find program node
    const RapidGL::ProgramNode* programNode = RapidGL::findProgramNode(root, program);
    if (programNode == NULL) {
        throw std::runtime_error("[BooleanXorNode] Could not find node with program!");
    }

    // Collect usages
    std::map<RapidGL::AttributeNode::Usage,GLint> locationsByUsage;
    const RapidGL::Node::node_range_t programChildren = programNode->getChildren();
    for (RapidGL::Node::node_iterator_t it = programChildren.begin; it != programChildren.end; ++it) {
        const RapidGL::AttributeNode* attributeNode = dynamic_cast<RapidGL::AttributeNode*>(*it);
        if (attributeNode != NULL) {
            const GLint location = attributeNode->getLocation();
            if (location >= 0) {
                locationsByUsage[attributeNode->getUsage()] = location;
            }
        }
    }

    // Bind VAO and VBO
    vao.bind();
    arrayBuffer.bind(vbo);

    // Enable attributes
    for (Glycerin::BufferLayout::const_iterator it = layout.begin(); it != layout.end(); ++it) {
        const RapidGL::AttributeNode::Usage usage = RapidGL::AttributeNode::parseUsage(it->name());
        if (locationsByUsage.count(usage) > 0) {
            const GLint location = locationsByUsage[usage];
            vao.enableVertexAttribArray(location);
            vao.vertexAttribPointer(Gloop::VertexAttribPointer()
                    .index(location)
                    .size(it->components())
                    .type(it->type())
                    .stride(it->stride())
                    .offset(it->offset()));
        }
    }

    // Unbind VAO and VBO
    arrayBuffer.unbind(vbo);
    vao.unbind();

    // Return VAO
    return vao;
}

/**
 * Destructs a `Filter`.
 */
BooleanXorNode::Filter::~Filter() {
    // empty
}

bool BooleanXorNode::EqualFilter::filter(const double x, const double y) {
    return x == y;
}

void BooleanXorNode::explode(const Piece& piece, const Extent& area, std::vector<Piece>& pieces) {
    clipLeft(piece, area, pieces);
}

BooleanXorNode::Extent BooleanXorNode::findExtent(RapidGL::CubeNode* cubeNode) {

    // Find all the ancestor nodes
    std::vector<RapidGL::Node*> ancestors;
    RapidGL::Node* parent = cubeNode->getParent();
    while (parent != NULL) {
        ancestors.push_back(parent);
        parent = parent->getParent();
    }

    // Reverse the nodes
    std::reverse(ancestors.begin(), ancestors.end());

    // Apply all their transformations
    RapidGL::State state;
    for (std::vector<RapidGL::Node*>::iterator it = ancestors.begin(); it != ancestors.end(); ++it) {
        RapidGL::TransformNode* transformNode = dynamic_cast<RapidGL::TransformNode*>(*it);
        if (transformNode != NULL) {
            transformNode->visit(state);
        }
    }

    // Get the model matrix
    const M3d::Mat4 modelMatrix = state.getModelMatrix();

    // Compute minimum and maximum
    Extent extent;
    extent.min = (modelMatrix * M3d::Vec4(-0.5, -0.5, -0.5, 1.0)).toVec3();
    extent.max = (modelMatrix * M3d::Vec4(+0.5, +0.5, +0.5, 1.0)).toVec3();
    return extent;
}

BooleanXorNode::Extent BooleanXorNode::findIntersection(const Extent& e1, const Extent& e2) {
    Extent intersection;
    intersection.min.x = std::max(e1.min.x, e2.min.x);
    intersection.min.y = std::max(e1.min.y, e2.min.y);
    intersection.min.z = std::max(e1.min.z, e2.min.z);
    intersection.max.x = std::min(e1.max.x, e2.max.x);
    intersection.max.y = std::min(e1.max.y, e2.max.y);
    intersection.max.z = std::min(e1.max.z, e2.max.z);
    return intersection;
}

/**
 * Calculates the center of an extent.
 *
 * @param extent Extent to calculate center of
 * @return Center of the extent
 */
M3d::Vec4 BooleanXorNode::getCenter(const Extent& extent) {
    const GLdouble x = (extent.min.x + extent.max.x) / 2;
    const GLdouble y = (extent.min.y + extent.max.y) / 2;
    const GLdouble z = (extent.min.z + extent.max.z) / 2;
    return M3d::Vec4(x, y, z, 1.0);
}

/**
 * Calculates the center of a piece.
 *
 * @param piece Piece to calculate center of
 * @return Center of the piece
 */
M3d::Vec4 BooleanXorNode::getCenter(const Piece& piece) {
    const GLdouble x = (piece.min.x + piece.max.x) / 2;
    const GLdouble y = (piece.min.y + piece.max.y) / 2;
    const GLdouble z = (piece.min.z + piece.max.z) / 2;
    return M3d::Vec4(x, y, z, 1.0);
}

/**
 * Calculates the depth of an extent.
 *
 * @param extent Extent to calculate depth of
 * @return Depth of the extent
 */
double BooleanXorNode::getDepth(const Extent& extent) {
    return extent.max.z - extent.min.z;
}

BooleanXorNode::Filter* BooleanXorNode::getFilter(const GLenum depthFunction) {
    std::map<GLenum,Filter*>::const_iterator it = FILTERS.find(depthFunction);
    if (it == FILTERS.end()) {
        throw std::invalid_argument("[BooleanXorNode] Depth function does not have a filter!");
    } else {
        return it->second;
    }
}

/**
 * Calculates the height of an extent.
 *
 * @param extent Extent to calculate height of
 * @return Height of the extent
 */
double BooleanXorNode::getHeight(const Extent& extent) {
    return extent.max.y - extent.min.y;
}

Gloop::VertexArrayObject BooleanXorNode::getVertexArrayObject(const Gloop::Program& program) {
    std::map<Gloop::Program,Gloop::VertexArrayObject>::iterator it = vaos.find(program);
    if (it == vaos.end()) {
        const Gloop::VertexArrayObject vao = createVertexArrayObject(program);
        vaos.insert(std::pair<Gloop::Program,Gloop::VertexArrayObject>(program, vao));
        return vao;
    } else {
        return it->second;
    }
}

/**
 * Calculates the width of an extent.
 *
 * @param extent Extent to calculate width of
 * @return Width of the extent
 */
double BooleanXorNode::getWidth(const Extent& extent) {
    return extent.max.x - extent.min.x;
}

bool BooleanXorNode::GreaterThanFilter::filter(const double x, const double y) {
    return x > y;
}

bool BooleanXorNode::GreaterThanOrEqualFilter::filter(const double x, const double y) {
    return x >= y;
}

/**
 * Checks whether an OpenGL enumeration is a valid depth function.
 *
 * @param enumeration Enumeration to check
 * @return `true` if enumeration is a valid depth function
 */
bool BooleanXorNode::isDepthFunction(const GLenum enumeration) {
    switch (enumeration) {
    case GL_NEVER:
    case GL_LESS:
    case GL_EQUAL:
    case GL_LEQUAL:
    case GL_GREATER:
    case GL_NOTEQUAL:
    case GL_GEQUAL:
    case GL_ALWAYS:
        return true;
    default:
        return false;
    }
}

/**
 * Checks if an extent has a real width, height, and depth.
 *
 * @param extent Extent to check
 * @return `true` if the extent has a real width, height, and depth
 */
bool BooleanXorNode::isTangible(const Extent& extent) {
    const double width = getWidth(extent);
    const double height = getHeight(extent);
    const double depth = getDepth(extent);
    return (width > 0) && (height > 0) && (depth > 0);
}

bool BooleanXorNode::LessThanFilter::filter(const double x, const double y) {
    return x < y;
}

bool BooleanXorNode::LessThanOrEqualFilter::filter(const double x, const double y) {
    return x <= y;
}

void BooleanXorNode::loadPiece(const Piece& piece, const GLsizei offset) {

    // Create array
    const GLfloat data[] = {

        // Front face
        piece.max.x, piece.max.y, piece.max.z,
        piece.max.s, piece.max.t, piece.max.p,
        piece.min.x, piece.max.y, piece.max.z,
        piece.min.s, piece.max.t, piece.max.p,
        piece.min.x, piece.min.y, piece.max.z,
        piece.min.s, piece.min.t, piece.max.p,
        piece.min.x, piece.min.y, piece.max.z,
        piece.min.s, piece.min.t, piece.max.p,
        piece.max.x, piece.min.y, piece.max.z,
        piece.max.s, piece.min.t, piece.max.p,
        piece.max.x, piece.max.y, piece.max.z,
        piece.max.s, piece.max.t, piece.max.p,

        // Back face
        piece.min.x, piece.max.y, piece.min.z,
        piece.min.s, piece.max.t, piece.min.p,
        piece.max.x, piece.max.y, piece.min.z,
        piece.max.s, piece.max.t, piece.min.p,
        piece.max.x, piece.min.y, piece.min.z,
        piece.max.s, piece.min.t, piece.min.p,
        piece.max.x, piece.min.y, piece.min.z,
        piece.max.s, piece.min.t, piece.min.p,
        piece.min.x, piece.min.y, piece.min.z,
        piece.min.s, piece.min.t, piece.min.p,
        piece.min.x, piece.max.y, piece.min.z,
        piece.min.s, piece.max.t, piece.min.p,

        // Left face
        piece.min.x, piece.max.y, piece.max.z,
        piece.min.s, piece.max.t, piece.max.p,
        piece.min.x, piece.max.y, piece.min.z,
        piece.min.s, piece.max.t, piece.min.p,
        piece.min.x, piece.min.y, piece.min.z,
        piece.min.s, piece.min.t, piece.min.p,
        piece.min.x, piece.min.y, piece.min.z,
        piece.min.s, piece.min.t, piece.min.p,
        piece.min.x, piece.min.y, piece.max.z,
        piece.min.s, piece.min.t, piece.max.p,
        piece.min.x, piece.max.y, piece.max.z,
        piece.min.s, piece.max.t, piece.max.p,

        // Right face
        piece.max.x, piece.max.y, piece.min.z,
        piece.max.s, piece.max.t, piece.min.p,
        piece.max.x, piece.max.y, piece.max.z,
        piece.max.s, piece.max.t, piece.max.p,
        piece.max.x, piece.min.y, piece.max.z,
        piece.max.s, piece.min.t, piece.max.p,
        piece.max.x, piece.min.y, piece.max.z,
        piece.max.s, piece.min.t, piece.max.p,
        piece.max.x, piece.min.y, piece.min.z,
        piece.max.s, piece.min.t, piece.min.p,
        piece.max.x, piece.max.y, piece.min.z,
        piece.max.s, piece.max.t, piece.min.p,

        // Top face
        piece.max.x, piece.max.y, piece.min.z,
        piece.max.s, piece.max.t, piece.min.p,
        piece.min.x, piece.max.y, piece.min.z,
        piece.min.s, piece.max.t, piece.min.p,
        piece.min.x, piece.max.y, piece.max.z,
        piece.min.s, piece.max.t, piece.max.p,
        piece.min.x, piece.max.y, piece.max.z,
        piece.min.s, piece.max.t, piece.max.p,
        piece.max.x, piece.max.y, piece.max.z,
        piece.max.s, piece.max.t, piece.max.p,
        piece.max.x, piece.max.y, piece.min.z,
        piece.max.s, piece.max.t, piece.min.p,

        // Bottom face
        piece.max.x, piece.min.y, piece.max.z,
        piece.max.s, piece.min.t, piece.max.p,
        piece.min.x, piece.min.y, piece.max.z,
        piece.min.s, piece.min.t, piece.max.p,
        piece.min.x, piece.min.y, piece.min.z,
        piece.min.s, piece.min.t, piece.min.p,
        piece.min.x, piece.min.y, piece.min.z,
        piece.min.s, piece.min.t, piece.min.p,
        piece.max.x, piece.min.y, piece.min.z,
        piece.max.s, piece.min.t, piece.min.p,
        piece.max.x, piece.min.y, piece.max.z,
        piece.max.s, piece.min.t, piece.max.p
    };

    // Load the data
    arrayBuffer.subData(offset, sizeof(data), data);
}

bool BooleanXorNode::NeverFilter::filter(const double x, const double y) {
    return false;
}

void BooleanXorNode::nodeChanged(RapidGL::Node* node) {
    dirty = true;
}

bool BooleanXorNode::NotEqualFilter::filter(const double x, const double y) {
    return x != y;
}

/**
 * Prepares this node.
 */
void BooleanXorNode::preVisit(RapidGL::State& state) {

    // Already ready
    if (ready) {
        return;
    }

    // Find root node
    const RapidGL::Node* root = RapidGL::findRoot(this);
    if (root == NULL) {
        throw std::runtime_error("[BooleanXorNode] Could not find root node!");
    }

    // Find first node
    RapidGL::Node* const n1 = RapidGL::findDescendant(root, id1);
    if (n1 == NULL) {
        throw std::runtime_error("[BooleanXorNode] Could not find first node!");
    }

    // Convert to cube node
    c1 = dynamic_cast<RapidGL::CubeNode*>(n1);
    if (c1 == NULL) {
        throw std::runtime_error("[BooleanXorNode] First node is not a cube node!");
    }

    // Find second node
    RapidGL::Node* const n2 = RapidGL::findDescendant(root, id2);
    if (n2 == NULL) {
        throw std::runtime_error("[BooleanXorNode] Could not find second node!");
    }

    // Convert to cube node
    c2 = dynamic_cast<RapidGL::CubeNode*>(n2);
    if (c2 == NULL) {
        throw std::runtime_error("[BooleanXorNode] Second node is not a cube node!");
    }

    // Find use node
    const RapidGL::UseNode* useNode = RapidGL::findAncestor<RapidGL::UseNode>(this);
    if (useNode == NULL) {
        throw std::runtime_error("[BooleanXorNode] Could not find use node!");
    }

    // Allocate buffer
    arrayBuffer.bind(vbo);
    arrayBuffer.data(layout.sizeInBytes(), NULL, GL_DYNAMIC_DRAW);
    arrayBuffer.unbind(vbo);

    // Add listeners to transform nodes above each cube
    RapidGL::CubeNode* cubes[] = { c1, c2 };
    for (int i = 0; i < 2; ++i) {
        RapidGL::Node* node = cubes[i]->getParent();
        while (node != NULL) {
            RapidGL::TransformNode* transformNode = dynamic_cast<RapidGL::TransformNode*>(node);
            if (transformNode != NULL) {
                transformNode->addNodeListener(this);
            }
            node = node->getParent();
        }
    }

    // Now ready
    ready = true;
}

void BooleanXorNode::update(RapidGL::State& state) {

    // Get the extents of the cubes
    e1 = findExtent(c1);
    e2 = findExtent(c2);

    // Compute the intersection
    intersection = findIntersection(e1, e2);
    const bool intersecting = isTangible(intersection);

    // Create list of pieces
    pieces.clear();

    // Break up the first cube into pieces
    if (!hidePiecesOfFirstCube) {
        const Piece p1 = createPiece(e1);
        if (intersecting) {
            explode(p1, intersection, pieces);
        } else {
            pieces.push_back(p1);
        }
    }

    // Break up the second cube into pieces
    if (!hidePiecesOfSecondCube) {
        const Piece p2 = createPiece(e2);
        if (intersecting) {
            explode(p2, intersection, pieces);
        } else {
            pieces.push_back(p2);
        }
    }

    // Load pieces into the buffer
    arrayBuffer.bind(vbo);
    const GLsizei sizeOfPiece = layout.sizeInBytes() / MAX_PIECES;
    GLsizei offset = 0;
    for (int i = 0; i < pieces.size(); ++i) {
        loadPiece(pieces[i], offset);
        offset += sizeOfPiece;
    }
    arrayBuffer.unbind(vbo);

    // Successfully updated
    dirty = false;
}

/**
 * Draws this node.
 */
void BooleanXorNode::visit(RapidGL::State& state) {

    // Update if dirty
    if (dirty) {
        update(state);
    }

    // Get current program
    const Gloop::Program program = Gloop::Program::current();

    // Get VAO for program
    const Gloop::VertexArrayObject vao = getVertexArrayObject(program);

    // Bind VAO
    vao.bind();

    // Draw pieces if they pass the filter
    const M3d::Mat4 viewMatrix = state.getViewMatrix();
    const M3d::Vec4 intersectionCenter = getCenter(intersection);
    const double intersectionDistance = fabs((viewMatrix * intersectionCenter).z);
    for (int i = 0; i < pieces.size(); ++i) {
        const M3d::Vec4 pieceCenter = getCenter(pieces[i]);
        const double pieceDistance = fabs((viewMatrix * pieceCenter).z);
        if (filter->filter(pieceDistance, intersectionDistance)) {
            glDrawArrays(GL_TRIANGLES, i * VERTICES_PER_PIECE, VERTICES_PER_PIECE);
        }
    }

    // Unbind VAO
    vao.unbind();
}
