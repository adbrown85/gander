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
#include "config.h"
#include "VolumeNodeUnmarshaller.h"

/**
 * Constructs a `VolumeNodeUnmarshaller`.
 */
VolumeNodeUnmarshaller::VolumeNodeUnmarshaller() {
    // empty
}

/**
 * Destructs a `VolumeNodeUnmarshaler`.
 */
VolumeNodeUnmarshaller::~VolumeNodeUnmarshaller() {
    // empty
}

/**
 * Determines the value of the _texture_ attribute in a map of XML attributes.
 *
 * @param attributes Map of XML attributes to look in
 * @return Value of attributes
 * @throws std::runtime_error if texture is unspecified or empty
 */
std::string VolumeNodeUnmarshaller::getTexture(const std::map<std::string,std::string>& attributes) {
    const std::string value = findValue(attributes, "texture");
    if (value.empty()) {
        throw std::runtime_error("[VolumeNodeUnmarshaller] Texture is unspecified!");
    }
    return value;
}

/**
 * Creates a `VolumeNode` from a map of XML attributes.
 *
 * @param attributes Map of XML attribute to create node from
 * @return Pointer to the new `VolumeNode`
 * @throws std::runtime_error if any required attribute is missing, or if an attribute is invalid
 */
RapidGL::Node* VolumeNodeUnmarshaller::unmarshal(const std::map<std::string,std::string>& attributes) {
    const std::string texture = getTexture(attributes);
    return new VolumeNode(texture);
}
