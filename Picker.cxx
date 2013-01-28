#include "Picker.h"
#include <glycerin/Projection.hxx>
#include <glycerin/Viewport.hxx>
#include <RapidGL/FramebufferNode.h>
#include <RapidGL/Intersectable.h>
#include <RapidGL/TransformNode.h>

Picker::Picker(RapidGL::State* state) : state(state), picked(NULL), time(INFINITY) {
    // empty
}

Glycerin::Ray Picker::createRay(const int x, const int y) {

    // Get inverse of view projection matrix
    const M3d::Mat4 mat = inverse(state->getProjectionMatrix());

    // Get viewport
    const Glycerin::Viewport viewport = Glycerin::Viewport::getViewport();
    const int maxY = viewport.height() - 1;

    // Get first point
    const M3d::Vec3 near(x, maxY - y, 0.0);
    M3d::Vec4 p1 = Glycerin::Projection::unProject(near, mat, viewport);

    // Get second point
    const M3d::Vec3 far(x, maxY - y, 1.0);
    M3d::Vec4 p2 = Glycerin::Projection::unProject(far, mat, viewport);

    // Compute
    const M3d::Vec4 o(0, 0, 0, 1);
    const M3d::Vec4 d = M3d::normalize(p2 - p1);
    return Glycerin::Ray(o, d);
}

Pick Picker::pick(RapidGL::Node* root, const int x, const int y) {
    const Glycerin::Ray ray = createRay(x, y);
    pick(root, ray);
    Pick p;
    p.node = picked;
    p.depth = time;
    return p;
}

void Picker::pick(RapidGL::Node* const node, const Glycerin::Ray& ray) {

    // Check if intersectable
    RapidGL::Intersectable* intersectable = dynamic_cast<RapidGL::Intersectable*>(node);
    if (intersectable != NULL) {
        const M3d::Mat4 mat = inverse(state->getModelViewMatrix());
        const Glycerin::Ray r = transform(mat, ray);
        const double t = intersectable->intersect(r);
        if ((t > 0) && (t < time)) {
            time = t;
            picked = node;
        }
    }

    // Check if framebuffer node
    RapidGL::FramebufferNode* framebufferNode = dynamic_cast<RapidGL::FramebufferNode*>(node);
    if (framebufferNode != NULL) {
        return;
    }

    // Check if transform node
    RapidGL::TransformNode* transformNode = dynamic_cast<RapidGL::TransformNode*>(node);

    // Visit transform nodes
    if (transformNode != NULL) {
        transformNode->preVisit(*state);
        transformNode->visit(*state);
    }

    // Traverse
    RapidGL::Node::node_range_t children = node->getChildren();
    for (RapidGL::Node::node_iterator_t it = children.begin; it != children.end; ++it) {
        pick((*it), ray);
    }

    // Post visit transform nodes
    if (transformNode != NULL) {
        transformNode->postVisit(*state);
    }
}

Glycerin::Ray Picker::transform(const M3d::Mat4& mat, const Glycerin::Ray& ray) {
    const M3d::Vec4 o = mat * ray.origin;
    const M3d::Vec4 d = mat * ray.direction;
    return Glycerin::Ray(o, d);
}
