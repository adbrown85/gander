#include "config.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <GL/glfw.h>
#include <glycerin/Projection.hxx>
#include <m3d/Mat4.h>
#include <m3d/Math.h>
#include <m3d/Quat.h>
#include <m3d/Vec3.h>
#include <m3d/Vec4.h>
#include <RapidGL/Reader.h>
#include <RapidGL/AttachmentNodeUnmarshaller.h>
#include <RapidGL/AttributeNodeUnmarshaller.h>
#include <RapidGL/ClearNodeUnmarshaller.h>
#include <RapidGL/CubeNodeUnmarshaller.h>
#include <RapidGL/CullNodeUnmarshaller.h>
#include <RapidGL/FramebufferNodeUnmarshaller.h>
#include <RapidGL/ProgramNodeUnmarshaller.h>
#include <RapidGL/RotateNodeUnmarshaller.h>
#include <RapidGL/ScaleNodeUnmarshaller.h>
#include <RapidGL/SceneNodeUnmarshaller.h>
#include <RapidGL/ShaderNodeUnmarshaller.h>
#include <RapidGL/SquareNodeUnmarshaller.h>
#include <RapidGL/State.h>
#include <RapidGL/TextureNodeUnmarshaller.h>
#include <RapidGL/TranslateNodeUnmarshaller.h>
#include <RapidGL/Visitor.h>
#include <RapidGL/UniformNodeUnmarshaller.h>
#include <RapidGL/UseNodeUnmarshaller.h>
#include "Sphere.h"
#include "Window.h"



/**
 * Application.
 */
class Gander : public Window {
public:
// Methods
    Gander(const std::string& filename);
    void run();
protected:
// Methods
    virtual void mouseDragged(int x, int y);
    virtual void mousePressed(int button, int x, int y);
    virtual void mouseWheelMoved(int movement);
    virtual void opened();
    virtual void paint();
private:
// Attributes
    RapidGL::Node* root;
    RapidGL::Reader reader;
    const std::string filename;
    double zoom;
    M3d::Quat rotation;
    int previousX;
    int previousY;
// Methods
    static Glycerin::Ray createRay(int x, int y);
    static M3d::Mat4 getProjectionMatrix();
    M3d::Mat4 getViewMatrix() const;
    void leftMousePressed(int x, int y);
    void rightMousePressed(int x, int y);
};

/**
 * Constructs the application.
 *
 * @param filename Path to the file to open
 * @throws std::runtime_error
 */
Gander::Gander(const std::string& filename) : Window(filename),
        filename(filename),
        root(NULL),
        zoom(-5.0),
        rotation(0, 0, 0, 1),
        previousX(0),
        previousY(0) {
    reader.addUnmarshaller("attribute", new RapidGL::AttributeNodeUnmarshaller());
    reader.addUnmarshaller("attachment", new RapidGL::AttachmentNodeUnmarshaller());
    reader.addUnmarshaller("clear", new RapidGL::ClearNodeUnmarshaller());
    reader.addUnmarshaller("cube", new RapidGL::CubeNodeUnmarshaller());
    reader.addUnmarshaller("cull", new RapidGL::CullNodeUnmarshaller());
    reader.addUnmarshaller("framebuffer", new RapidGL::FramebufferNodeUnmarshaller());
    reader.addUnmarshaller("program", new RapidGL::ProgramNodeUnmarshaller());
    reader.addUnmarshaller("rotate", new RapidGL::RotateNodeUnmarshaller());
    reader.addUnmarshaller("scale", new RapidGL::ScaleNodeUnmarshaller());
    reader.addUnmarshaller("scene", new RapidGL::SceneNodeUnmarshaller());
    reader.addUnmarshaller("shader", new RapidGL::ShaderNodeUnmarshaller());
    reader.addUnmarshaller("square", new RapidGL::SquareNodeUnmarshaller());
    reader.addUnmarshaller("texture", new RapidGL::TextureNodeUnmarshaller());
    reader.addUnmarshaller("translate", new RapidGL::TranslateNodeUnmarshaller());
    reader.addUnmarshaller("uniform", new RapidGL::UniformNodeUnmarshaller());
    reader.addUnmarshaller("use", new RapidGL::UseNodeUnmarshaller());
}

Glycerin::Ray Gander::createRay(const int x, const int y) {

    // Compute origin
    M3d::Vec4 o;
    o.x = (2.0 * ((double) x) / 768.0) - 1.0;
    o.y = (2.0 * (768.0 - ((double) y)) / 768.0) - 1.0;
    o.z = 1.0;
    o.w = 1.0;

    // Compute direction
    M3d::Vec4 d;
    d.x = 0;
    d.y = 0;
    d.z = -1;
    d.w = 0;

    // Return ray
    return Glycerin::Ray(o, d);
}

/**
 * Computes the projection matrix.
 */
M3d::Mat4 Gander::getProjectionMatrix() {
    return Glycerin::Projection::perspective(30.0f, 1.0f, 1, 100);
}

/**
 * Computes the view matrix.
 */
M3d::Mat4 Gander::getViewMatrix() const {

    // Compute translation matrix
    M3d::Mat4 translationMatrix(1);
    translationMatrix[3] = M3d::Vec4(0, 0, zoom, 1);

    // Compute rotation matrix
    M3d::Mat4 rotationMatrix = rotation.toMat4();

    // Concatenate
    return translationMatrix * rotationMatrix;
}

void Gander::leftMousePressed(const int x, const int y) {
    previousX = x;
    previousY = y;
}

void Gander::mouseDragged(const int x, const int y) {
    if (picked == NULL) {
        rotate(x, y);
    }
    previousX = x;
    previousY = y;
    paint();
}

void Gander::mousePressed(const int button, const int x, const int y) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        leftMousePressed(x, y);
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        rightMousePressed(x, y);
    }
    paint();
}

void Gander::mouseWheelMoved(int movement) {
    if (movement > 0) {
        zoom++;
    } else if (movement < 0) {
        zoom--;
    }
    paint();
}

void Gander::opened() {

    // Open file
    std::ifstream file(filename.c_str());
    if (!file) {
        throw std::runtime_error("Could not open file!");
    }

    // Read file
    root = reader.read(file);

    // Disable depth
    glDisable(GL_DEPTH_TEST);
}

void Gander::paint() {

    // Set up state
    RapidGL::State state;
    state.setProjectionMatrix(getProjectionMatrix());
    state.setViewMatrix(getViewMatrix());

    // Visit the root node
    RapidGL::Visitor visitor(&state);
    visitor.visit(root);

    // Refresh
    glfwSwapBuffers();
}

void Gander::rightMousePressed(const int x, const int y) {
    // empty
}

void Gander::rotate(const int x, const int y) {

    // Make sphere
    const Sphere sphere(0.95);

    // Compute rays
    const Glycerin::Ray r1 = createRay(previousX, previousY);
    const Glycerin::Ray r2 = createRay(x, y);

    // Compute intersections
    const double t1 = sphere.intersectedByRay(r1);
    const double t2 = sphere.intersectedByRay(r2);

    // No intersection
    if ((t1 != t1) || (t2 != t2)) {
        return;
    }
    if ((t1 < 0) || (t2 < 0)) {
        return;
    }

    // Compute points
    const M3d::Vec4 p1 = r1.origin + r1.direction * t1;
    const M3d::Vec4 p2 = r2.origin + r2.direction * t2;

    // Compute vectors
    const M3d::Vec3 v1 = normalize(p1.toVec3());
    const M3d::Vec3 v2 = normalize(p2.toVec3());

    // Compute rotation
    const M3d::Vec3 axis = cross(v1, v2);
    const double angle = dot(v1, v2);
    const M3d::Quat rot = M3d::Quat::fromAxisAngle(axis, angle);

    // Multiply with existing rotation
    rotation = rot * rotation;
}

/**
 * Runs the application.
 */
void Gander::run() {
    open();
}

int main(int argc, char* argv[]) {

    // Check for arguments
    if (argc != 2) {
        std::cout << "Usage:" << std::endl;
        std::cout << argv[0] << " <file>" << std::endl;
        return 0;
    }

    // Make application
    try {
        Gander gander(argv[1]);
        gander.run();
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    // Exit
    return 0;
}
