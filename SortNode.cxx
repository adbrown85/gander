#include "config.h"
#include <algorithm>
#include <vector>
#include "SortNode.h"

/**
 * Constructs a `SortNode`.
 */
SortNode::SortNode() : ready(false) {
    // empty
}

/**
 * Destructs a `SortNode`.
 */
SortNode::~SortNode() {
    // empty
}

RapidGL::TranslateNode* SortNode::getTranslateNode(RapidGL::Node* node) {
    std::map<RapidGL::Node*,RapidGL::TranslateNode*>::iterator it = translateNodes.find(node);
    if (it == translateNodes.end()) {
        throw std::runtime_error("[SortNode] Could not find translate node!");
    } else {
        return it->second;
    }
}

SortNode::Depth::Depth(double value, RapidGL::Node* node) : value(value), node(node) {
    // empty
}

bool SortNode::Depth::operator<(const SortNode::Depth& that) const {
    return this->value < that.value;
}

void SortNode::preVisit(RapidGL::State& state) {

    // Skip if already ready
    if (ready) {
        return;
    }

    // Get this node's children
    const RapidGL::Node::node_range_t children = getChildren();

    // Store all the children
    for (RapidGL::Node::node_iterator_t it = children.begin; it != children.end; ++it) {
        this->children.insert(*it);
    }

    // Find translate nodes
    for (RapidGL::Node::node_iterator_t it = children.begin; it != children.end; ++it) {
        RapidGL::Node* const node = *it;
        RapidGL::TranslateNode* const translateNode = findNode<RapidGL::TranslateNode>(node);
        translateNodes[node] = translateNode;
    }

    // Now ready
    ready = true;
}

void SortNode::visit(RapidGL::State& state) {

    // Remove children temporarily
    for (std::set<RapidGL::Node*>::iterator it = children.begin(); it != children.end(); ++it) {
        removeChild(*it);
    }

    // Get view matrix
    const M3d::Mat4 viewMatrix = state.getViewMatrix();

    // Compute depths
    std::vector<Depth> depths;
    for (std::set<RapidGL::Node*>::iterator it = children.begin(); it != children.end(); ++it) {
        RapidGL::Node* const node = *it;
        RapidGL::TranslateNode* const translateNode = getTranslateNode(node);
        const M3d::Vec4 p1 = M3d::Vec4(translateNode->getTranslation(), 1);
        const M3d::Vec4 p2 = viewMatrix * p1;
        depths.push_back(Depth(p2.z, node));
    }

    // Sort depths
    std::sort(depths.begin(), depths.end());

    // Add children back in back-to-front order
    for (std::vector<Depth>::iterator it = depths.begin(); it != depths.end(); ++it) {
        addChild(it->node);
    }
}
