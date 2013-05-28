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
#ifndef GANDER_SLICING_VOLUME_RENDERER_NODE_UNMARSHALLER_H
#define GANDER_SLICING_VOLUME_RENDERER_NODE_UNMARSHALLER_H
#include <map>
#include <string>
#include <RapidGL/Node.h>
#include <RapidGL/Unmarshaller.h>
#include "SlicingVolumeRendererNode.h"


/**
 * Unmarshaller for a `SlicingVolumeRendererNode`.
 */
class SlicingVolumeRendererNodeUnmarshaller : public RapidGL::Unmarshaller {
public:
// Methods
    SlicingVolumeRendererNodeUnmarshaller();
    virtual ~SlicingVolumeRendererNodeUnmarshaller();
    virtual RapidGL::Node* unmarshal(const std::map<std::string,std::string>& attributes);
private:
// Constants
    static const GLint DEFAULT_SLICES = 100;
// Methods
    static GLint getSlices(const std::map<std::string,std::string>& attributes);
    static std::string getStrategy(const std::map<std::string,std::string>& attributes);
    static std::string getUniform(const std::map<std::string,std::string>& attributes);
};

#endif
