#ifndef GANDER_WINDOW_ADAPTER_H
#define GANDER_WINDOW_ADAPTER_H
#include <vector>
#include <GL/glfw.h>


/**
 * Basic OpenGL window.
 */
class WindowAdapter {
public:
// Methods
    WindowAdapter(const std::string& title);
    virtual ~WindowAdapter();
    void close();
    void open();
protected:
// Methods
    virtual void mouseDragged(int x, int y) = 0;
    virtual void mousePressed(int button, int x, int y) = 0;
    virtual void mouseWheelMoved(int movement) = 0;
    virtual void opened() = 0;
    virtual void paint() = 0;
private:
// Constants
    static const int DEFAULT_WIDTH = 768;
    static const int DEFAULT_HEIGHT = 768;
};

#endif
