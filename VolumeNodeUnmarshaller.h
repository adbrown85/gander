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
#ifndef GANDER_VOLUME_NODE_UNMARSHALLER_H
#define GANDER_VOLUME_NODE_UNMARSHALLER_H
#include <map>
#include <string>
#include <RapidGL/Node.h>
#include <RapidGL/Unmarshaller.h>
#include "VolumeNode.h"
#include "VolumeNodeUnmarshaller.h"


/**
 * Unmarshaller for a `VolumeNode`.
 */
class VolumeNodeUnmarshaller : public RapidGL::Unmarshaller {
public:
// Methods
    VolumeNodeUnmarshaller();
    virtual ~VolumeNodeUnmarshaller();
    virtual RapidGL::Node* unmarshal(const std::map<std::string,std::string>& attributes);
private:
// Methods
    std::string getTexture(const std::map<std::string,std::string>& attributes);
};

#endif
