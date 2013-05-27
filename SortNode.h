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
#ifndef GANDER_SORT_NODE_H
#define GANDER_SORT_NODE_H
#include <map>
#include <set>
#include <RapidGL/Node.h>
#include <RapidGL/State.h>
#include <RapidGL/TranslateNode.h>


/**
 * Node that depth-sorts its immediate children.
 */
class SortNode : public RapidGL::Node {
public:
// Methods
    SortNode();
    virtual ~SortNode();
    virtual void preVisit(RapidGL::State& state);
    virtual void visit(RapidGL::State& state);
private:
// Types
    class Depth {
    public:
    // Attributes
        double value;
        RapidGL::Node* node;
    // Methods
        Depth();
        Depth(double value, RapidGL::Node* node);
        bool operator<(const Depth& that) const;
    };
// Attributes
    bool ready;
    std::set<RapidGL::Node*> children;
    std::map<RapidGL::Node*,RapidGL::TranslateNode*> translateNodes;
// Methods
    RapidGL::TranslateNode* getTranslateNode(RapidGL::Node* node);
};


template<typename T>
T* findNode(RapidGL::Node* root) {

    if (root == NULL) {
        throw std::invalid_argument("Node is NULL!");
    }

    // Initialize queue
    std::queue<RapidGL::Node*> q;
    q.push(root);

    // Search for type
    while (!q.empty()) {
        RapidGL::Node* const node = q.front();
        T* const t = dynamic_cast<T*>(node);
        if (t != NULL) {
            return t;
        }
        RapidGL::addToQueue(q, node->getChildren());
        q.pop();
    }

    // Not found
    return NULL;
}

#endif
