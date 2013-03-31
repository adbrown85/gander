#ifndef GANDER_BLEND_NODE_H
#define GANDER_BLEND_NODE_H
#include <RapidGL/Node.h>


/**
 * Node that enables or disables blending.
 */
class BlendNode : public RapidGL::Node {
public:
// Methods
    BlendNode(bool enabled);
    virtual ~BlendNode();
    virtual void visit(RapidGL::State& state);
private:
// Attributes
    const bool enabled;
};

#endif
