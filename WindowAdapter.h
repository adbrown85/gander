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
