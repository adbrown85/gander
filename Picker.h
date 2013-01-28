#ifndef GANDER_PICKER_H
#define GANDER_PICKER_H
#include <glycerin/Ray.hxx>
#include <RapidGL/Node.h>
#include <RapidGL/State.h>


struct Pick {
    RapidGL::Node* node;
    double depth;
};


class Picker {
public:
// Methods
    Picker(RapidGL::State* state);
    Pick pick(RapidGL::Node* root, int x, int y);
private:
// Attributes
    RapidGL::State* const state;
    RapidGL::Node* picked;
    double time;
// Methods
    Glycerin::Ray createRay(int x, int y);
    void pick(RapidGL::Node* node, const Glycerin::Ray& ray);
    static Glycerin::Ray transform(const M3d::Mat4& mat, const Glycerin::Ray& ray);
};

#endif
