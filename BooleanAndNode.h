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
