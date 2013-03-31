#include "config.h"
#include "BlendNode.h"

/**
 * Constructs a blend node.
 *
 * @param enabled Whether blending should be enabled
 */
BlendNode::BlendNode(const bool enabled) : enabled(enabled) {
    // empty
}

/**
 * Destructs a blend node.
 */
BlendNode::~BlendNode() {
    // empty
}

void BlendNode::visit(RapidGL::State& state) {
    if (enabled) {
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_BLEND);
    } else {
        glDisable(GL_BLEND);
    }
}
