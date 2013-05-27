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
#include <stdexcept>
#include "BlendNodeUnmarshaller.h"

/**
 * Constructs a `BlendNodeUnmarshaller`.
 */
BlendNodeUnmarshaller::BlendNodeUnmarshaller() {
    // empty
}

/**
 * Destructs a `BlendNodeUnmarshaller`.
 */
BlendNodeUnmarshaller::~BlendNodeUnmarshaller() {
    // empty
}

bool BlendNodeUnmarshaller::getEnabled(const std::map<std::string,std::string>& attributes) {

    // Find value
    const std::string value = findValue(attributes, "enabled");
    if (value.empty()) {
        throw std::runtime_error("[BlendNodeUnmarshaller] Enabled is unspecified!");
    }

    // Parse value
    if (value == "true") {
        return true;
    } else if (value == "false") {
        return false;
    } else {
        throw std::runtime_error("[BlendNodeUnmarshaller] Enabled is invalid!");
    }
}

RapidGL::Node* BlendNodeUnmarshaller::unmarshal(const std::map<std::string,std::string>& attributes) {
    const bool enabled = getEnabled(attributes);
    return new BlendNode(enabled);
}
