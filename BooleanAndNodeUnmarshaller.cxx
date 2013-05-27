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
#include <stdexcept>
#include <vector>
#include "BooleanAndNodeUnmarshaller.h"

BooleanAndNodeUnmarshaller::BooleanAndNodeUnmarshaller() {
    // empty
}

BooleanAndNodeUnmarshaller::~BooleanAndNodeUnmarshaller() {
    // empty
}

RapidGL::Node* BooleanAndNodeUnmarshaller::unmarshal(const std::map<std::string,std::string>& attributes) {

    // Find value
    const std::string value = findValue(attributes, "of");
    if (value.empty()) {
        throw std::runtime_error("[BooleanAndNodeUnmarshaller] Of is unspecified!");
    }

    // Tokenize
    std::vector<std::string> tokens = tokenize(value);
    if (tokens.size() != 2) {
        throw std::runtime_error("[BooleanAndNodeUnmarshaller] Of should take two IDs!");
    }

    // Return node
    return new BooleanAndNode(tokens[0], tokens[1]);
}
