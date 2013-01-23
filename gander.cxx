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
    RapidGL::Reader reader;
    const std::string filename;
// Methods
    static M3d::Mat4 getProjectionMatrix();
    static M3d::Mat4 getViewMatrix();
};

/**
 * Constructs the application.
 *
 * @param filename Path to the file to open
 * @throws std::runtime_error
 */
Gander::Gander(const std::string& filename) : filename(filename) {

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
M3d::Mat4 Gander::getViewMatrix() {
    M3d::Mat4 translationMatrix(1);
    translationMatrix[3] = M3d::Vec4(0, 0, -3, 1);
    return translationMatrix;
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
    RapidGL::Node* root = reader.read(file);

    // Disable depth
    glDisable(GL_DEPTH_TEST);

    // Make state and visitor
    RapidGL::State state;
    state.setViewMatrix(getViewMatrix());
    state.setProjectionMatrix(getProjectionMatrix());
    RapidGL::Visitor visitor(&state);

    // Render
    while (glfwGetWindowParam(GLFW_OPENED)) {
        visitor.visit(root);
        glfwSwapBuffers();
        glfwWaitEvents();
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
