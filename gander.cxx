#include "config.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <GL/glfw.h>
#include <glycerin/Projection.hxx>
#include <m3d/Mat4.hxx>
#include <m3d/Math.hxx>
#include <m3d/Quat.hxx>
#include <m3d/Vec3.hxx>
#include <m3d/Vec4.hxx>
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


/**
 * Application.
 */
class Gander {
public:
// Methods
    Gander(const std::string& filename);
    ~Gander();
    void run();
private:
// Attributes
    RapidGL::Node* root;
    RapidGL::Reader reader;
    const std::string filename;
    double zoom;
    M3d::Quat rotation;
// Methods
    static M3d::Mat4 getProjectionMatrix();
    M3d::Mat4 getViewMatrix() const;
    void mouseMoved(int dx, int dy);
    void mouseWheelMoved(int movement);
    void render();
};

/**
 * Constructs the application.
 *
 * @param filename Path to the file to open
 * @throws std::runtime_error
 */
Gander::Gander(const std::string& filename) :
        filename(filename),
        root(NULL),
        zoom(-5.0),
        rotation(0, 0, 0, 1) {

    // Capture working directory before GLFW changes it
#ifdef __APPLE__
    char cwd[PATH_MAX];
    getcwd(cwd, PATH_MAX);
#endif

    // Initialize GLFW
    if (!glfwInit()) {
        throw std::runtime_error("Could not initialize GLFW!");
    }

    // Reset working directory
#ifdef __APPLE__
    chdir(cwd);
#endif

    // Add unmarshallers
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

/**
 * Destructs the application.
 */
Gander::~Gander() {
    glfwTerminate();
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

void Gander::mouseMoved(const int dx, const int dy) {

    // Compute rotation around X axis
    M3d::Vec3 xAxis = M3d::Vec3(1, 0, 0);
    const double xAngle = M3d::toRadians(dy);
    M3d::Quat xRotation = M3d::Quat::fromAxisAngle(xAxis, xAngle);

    // Compute rotation around Y axis
    M3d::Vec3 yAxis = M3d::Vec3(0, 1, 0);
    const double yAngle = M3d::toRadians(dx);
    M3d::Quat yRotation = M3d::Quat::fromAxisAngle(yAxis, yAngle);

    // Concatenate
    rotation = xRotation * yRotation * rotation;
}

void Gander::mouseWheelMoved(int movement) {
    if (movement > 0) {
        zoom++;
    } else if (movement < 0) {
        zoom--;
    }
}

void Gander::render() {

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

/**
 * Runs the application.
 */
void Gander::run() {

    // Open file
    std::ifstream file(filename.c_str());
    if (!file) {
        throw std::runtime_error("Could not open file!");
    }

    // Open window
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
    glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);
    if (!glfwOpenWindow(768, 768, 0, 0, 0, 0, 0, 0, GLFW_WINDOW)) {
        throw std::runtime_error("Could not open window!");
    }

    // Set window title
    glfwSetWindowTitle(filename.c_str());

    // Read file
    root = reader.read(file);

    // Disable depth
    glDisable(GL_DEPTH_TEST);

    // Set up variables to hold event state
    int lastX = 0;
    int lastY = 0;
    int lastMouseWheelPosition = 0;

    // Render
    render();
    while (glfwGetWindowParam(GLFW_OPENED)) {

        // Wait for new events
        glfwWaitEvents();
        bool changed = false;

        // Get mouse position
        int x, y;
        glfwGetMousePos(&x, &y);

        // Check for dragging
        const int leftMouseButton = glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT);
        if (leftMouseButton == GLFW_PRESS) {
            const int dx = x - lastX;
            const int dy = y - lastY;
            if ((dx != 0) || (dy != 0)) {
                changed = true;
                mouseMoved(dx, dy);
            }
        }

        // Store mouse position for next event
        lastX = x;
        lastY = y;

        // Check for mouse wheel
        int mouseWheelPosition = glfwGetMouseWheel();
        int mouseWheelMovement = mouseWheelPosition - lastMouseWheelPosition;
        lastMouseWheelPosition = mouseWheelPosition;
        if (mouseWheelMovement != 0) {
            changed = true;
            mouseWheelMoved(mouseWheelMovement);
        }

        // Render if anything changed
        if (changed) {
            render();
        }
    }
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
