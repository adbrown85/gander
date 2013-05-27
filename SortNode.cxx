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
