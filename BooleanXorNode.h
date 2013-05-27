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
#ifndef GANDER_BOOLEAN_XOR_NODE_H
#define GANDER_BOOLEAN_XOR_NODE_H
#include <set>
#include <string>
#include <vector>
#include <gloop/BufferObject.hxx>
#include <gloop/BufferTarget.hxx>
#include <gloop/VertexArrayObject.hxx>
#include <m3d/Vec3.h>
#include <m3d/Vec4.h>
#include <RapidGL/CubeNode.h>
#include <RapidGL/Node.h>
#include <RapidGL/State.h>


/**
 * Node performing a exclusive or operation.
 */
class BooleanXorNode : public RapidGL::Node, public RapidGL::NodeListener {
public:
// Methods
    BooleanXorNode(const std::set<std::string>& ids, const std::string& hide = "", const GLenum filter = GL_ALWAYS);
    virtual ~BooleanXorNode();
    virtual void nodeChanged(RapidGL::Node* node);
    virtual void preVisit(RapidGL::State& state);
    virtual void visit(RapidGL::State& state);
private:
// Types
    struct Extent {
        M3d::Vec3 min;
        M3d::Vec3 max;
    };
    struct Vertex {
        double x, y, z;
        double s, t, p;
    };
    struct Piece {
        Vertex min;
        Vertex max;
        const Extent* of;
    };
    class Filter {
    public:
        virtual bool filter(double x, double y) = 0;
    protected:
        virtual ~Filter();
    };
    class AlwaysFilter : public Filter {
    public:
        static AlwaysFilter INSTANCE;
        virtual bool filter(double x, double y);
    private:
        AlwaysFilter() { }
    };
    class EqualFilter : public Filter {
    public:
        static EqualFilter INSTANCE;
        virtual bool filter(double x, double y);
    private:
        EqualFilter() { }
    };
    class GreaterThanFilter : public Filter {
    public:
        static GreaterThanFilter INSTANCE;
        virtual bool filter(double x, double y);
    private:
        GreaterThanFilter() { }
    };
    class GreaterThanOrEqualFilter : public Filter {
    public:
        static GreaterThanOrEqualFilter INSTANCE;
        virtual bool filter(double x, double y);
    private:
        GreaterThanOrEqualFilter() { }
    };
    class LessThanFilter : public Filter {
    public:
        static LessThanFilter INSTANCE;
        virtual bool filter(double x, double y);
    private:
        LessThanFilter() { }
    };
    class LessThanOrEqualFilter : public Filter {
    public:
        static LessThanOrEqualFilter INSTANCE;
        virtual bool filter(double x, double y);
    private:
        LessThanOrEqualFilter() { }
    };
    class NeverFilter : public Filter {
    public:
        static NeverFilter INSTANCE;
        virtual bool filter(double x, double y);
    private:
        NeverFilter() { }
    };
    class NotEqualFilter : public Filter {
    public:
        static NotEqualFilter INSTANCE;
        virtual bool filter(double x, double y);
    private:
        NotEqualFilter() { }
    };
// Constants
    static const int VERTICES_PER_PIECE = 36;
    static const int MAX_PIECES = 6;
    static const int MAX_VERTICES_PER_BUFFER = MAX_PIECES * VERTICES_PER_PIECE;
    static const std::map<GLenum,Filter*> FILTERS;
// Attributes
    bool ready;
    bool dirty;
    const std::string id1;
    const std::string id2;
    RapidGL::CubeNode* c1;
    RapidGL::CubeNode* c2;
    Extent e1;
    Extent e2;
    Extent intersection;
    std::vector<Piece> pieces;
    const bool hidePiecesOfFirstCube;
    const bool hidePiecesOfSecondCube;
    Filter* const filter;
    std::map<Gloop::Program,Gloop::VertexArrayObject> vaos;
    const Gloop::BufferObject vbo;
    const Glycerin::BufferLayout layout;
    const Gloop::BufferTarget arrayBuffer;
// Methods
    static void clipLeft(const Piece& piece, const Extent& area, std::vector<Piece>& pieces);
    static void clipRight(const Piece& piece, const Extent& area, std::vector<Piece>& pieces);
    static void clipTop(const Piece& piece, const Extent& area, std::vector<Piece>& pieces);
    static void clipBottom(const Piece& piece, const Extent& area, std::vector<Piece>& pieces);
    static void clipFront(const Piece& piece, const Extent& area, std::vector<Piece>& pieces);
    static void clipBack(const Piece& piece, const Extent& area, std::vector<Piece>& pieces);
    Gloop::VertexArrayObject createVertexArrayObject(const Gloop::Program& program);
    static void explode(const Piece& piece, const Extent& area, std::vector<Piece>& pieces);
    static Glycerin::BufferLayout createBufferLayout();
    static std::map<GLenum,Filter*> createFilters();
    static Piece createPiece(const Extent& extent);
    static Extent findExtent(RapidGL::CubeNode* cubeNode);
    static Extent findIntersection(const Extent& e1, const Extent& e2);
    static M3d::Vec4 getCenter(const Extent& extent);
    static M3d::Vec4 getCenter(const Piece& piece);
    static double getDepth(const Extent& extent);
    static Filter* getFilter(GLenum depthFunction);
    template<typename T> static T getFirst(const std::set<T>& set);
    static double getHeight(const Extent& extent);
    template<typename T> static T getSecond(const std::set<T>& set);
    Gloop::VertexArrayObject getVertexArrayObject(const Gloop::Program& program);
    static double getWidth(const Extent& extent);
    static bool isDepthFunction(GLenum enumeration);
    static bool isTangible(const Extent& extent);
    void loadPiece(const Piece& piece, GLsizei offset);
    void update(RapidGL::State& state);
};


/**
 * Returns the first element in a set.
 *
 * @param set Set to get first element of
 * @return First element in set
 * @throws std::invalid_argument if set does not have at least one element
 */
template<typename T>
T BooleanXorNode::getFirst(const std::set<T>& set) {
    if (set.empty()) {
        throw std::invalid_argument("Set does not have at least one element!");
    }
    typename std::set<T>::iterator it = set.begin();
    return *it;
}


/**
 * Returns the second element in a set.
 *
 * @param set Set to get second element of
 * @return Second element in set
 * @throws std::invalid_argument if set does not have at least two elements
 */
template<typename T>
T BooleanXorNode::getSecond(const std::set<T>& set) {
    if (set.size() < 2) {
        throw std::invalid_argument("Set does not have at least two elements!");
    }
    typename std::set<T>::iterator it = set.begin();
    ++it;
    return *it;
}

#endif
