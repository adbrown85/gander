#include <algorithm>
#include <stdexcept>
#include "WindowAdapter.h"

WindowAdapter::WindowAdapter(const std::string& title) {

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

    glfwSwapInterval(0);
    glfwSetWindowTitle(title.c_str());
}

WindowAdapter::~WindowAdapter() {
    glfwTerminate();
}

/*
void WindowAdapter::addWindowListener(WindowListener* listener) {
    windowListeners.push_back(listener);
}
*/

void WindowAdapter::close() {
    glfwCloseWindow();
}

void WindowAdapter::open() {

    // Open the window
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
    glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwOpenWindowHint(GLFW_WINDOW_NO_RESIZE, GL_TRUE);
    if (!glfwOpenWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, 0, 0, 0, 0, 24, 0, GLFW_WINDOW)) {
        throw std::runtime_error("Could not open window!");
    }

    // Initialize
    opened();

    // Store initial state
    int lastX = 0;
    int lastY = 0;
    int lastMouseWheelPosition = 0;
    int lastLeftMouseButton = GLFW_RELEASE;
    int lastRightMouseButton = GLFW_RELEASE;

    // Continuously paint
    paint();
    while (glfwGetWindowParam(GLFW_OPENED)) {

        // Get state of mouse
        int x, y;
        glfwGetMousePos(&x, &y);
        const int leftMouseButton = glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT);
        const int rightMouseButton = glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT);
        int mouseWheelPosition = glfwGetMouseWheel();

        // Check for button presses
        if (leftMouseButton != lastLeftMouseButton) {
            if (leftMouseButton == GLFW_PRESS) {
                mousePressed(GLFW_MOUSE_BUTTON_LEFT, x, y);
            }
        } else if (rightMouseButton != lastRightMouseButton) {
            if (rightMouseButton == GLFW_PRESS) {
                mousePressed(GLFW_MOUSE_BUTTON_RIGHT, x, y);
            }
        }

        // Check for dragging
        if (leftMouseButton == GLFW_PRESS) {
            const int dx = x - lastX;
            const int dy = y - lastY;
            if ((dx != 0) || (dy != 0)) {
                mouseDragged(x, y);
            }
        }

        // Check for mouse wheel
        int mouseWheelMovement = mouseWheelPosition - lastMouseWheelPosition;
        if (mouseWheelMovement != 0) {
            mouseWheelMoved(mouseWheelMovement);
        }

        // Paint
        paint();
        glfwSwapBuffers();

        // Store state of mouse for next event
        lastX = x;
        lastY = y;
        lastLeftMouseButton = leftMouseButton;
        lastRightMouseButton = rightMouseButton;
        lastMouseWheelPosition = mouseWheelPosition;
    }
}

/*
void WindowAdapter::removeWindowListener(WindowListener* listener) {
    std::vector<WindowListener*>::iterator it = std::find(windowListeners.begin(), windowListeners.end(), listener);
    if (it != windowListeners.end()) {
        windowListeners.erase(it);
    }
}
*/
