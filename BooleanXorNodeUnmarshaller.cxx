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
#include <set>
#include <string>
#include <Poco/String.h>
#include "BooleanXorNodeUnmarshaller.h"

// Map of depth functions by name
const std::map<std::string,GLenum> BooleanXorNodeUnmarshaller::DEPTH_FUNCTIONS =
        BooleanXorNodeUnmarshaller::createDepthFunctions();

/**
 * Constructs a `BooleanXorNodeUnmarshaller`.
 */
BooleanXorNodeUnmarshaller::BooleanXorNodeUnmarshaller() {
    // empty
}

/**
 * Destructs this `BooleanXorNodeUnmarshaller`.
 */
BooleanXorNodeUnmarshaller::~BooleanXorNodeUnmarshaller() {
    // empty
}

/**
 * Creates the map of depth functions by name.
 *
 * @return Map of depth functions by name
 */
std::map<std::string,GLenum> BooleanXorNodeUnmarshaller::createDepthFunctions() {
    std::map<std::string,GLenum> depthFunctions;
    depthFunctions["NEVER"] = GL_NEVER;
    depthFunctions["LESS"] = GL_LESS;
    depthFunctions["EQUAL"] = GL_EQUAL;
    depthFunctions["LEQUAL"] = GL_LEQUAL;
    depthFunctions["GREATER"] = GL_GREATER;
    depthFunctions["NOTEQUAL"] = GL_NOTEQUAL;
    depthFunctions["GEQUAL"] = GL_GEQUAL;
    depthFunctions["ALWAYS"] = GL_ALWAYS;
    return depthFunctions;
}

/**
 * Returns the value of the _filter_ attribute in a map.
 *
 * @param attributes Map of attributes to look in
 * @retrun Value of attribute in the map
 */
GLenum BooleanXorNodeUnmarshaller::getFilter(const std::map<std::string,std::string>& attributes) {

    // Find value
    const std::string value = findValue(attributes, "filter");
    if (value.empty()) {
        return GL_ALWAYS;
    }

    // Find
    try {
        return parseDepthFunction(Poco::toUpper(value));
    } catch (std::invalid_argument& e) {
        throw std::runtime_error("[BooleanXorNodeUnmarshaller] Value for 'filter' is not a valid depth function!");
    }
}

/**
 * Returns the value of the _hide_ attribute in a map.
 *
 * @param attributes Map of attributes to look in
 * @return Value of attribute in the map
 */
std::string BooleanXorNodeUnmarshaller::getHide(const std::map<std::string,std::string>& attributes) {
    return findValue(attributes, "hide");
}

/**
 * Returns the value of the _of_ attribute in a map.
 *
 * @param attributes Map of attributes to look in
 * @return Value of attribute in the map
 * @throws std::runtime_error if could not find value or if value is invalid
 */
std::set<std::string> BooleanXorNodeUnmarshaller::getOf(const std::map<std::string,std::string>& attributes) {

    // Find value
    const std::string value = findValue(attributes, "of");
    if (value.empty()) {
        throw std::runtime_error("[BooleanXorNodeUnmarshaller] Could not find value for 'of'!");
    }

    // Tokenize the value
    const std::vector<std::string> tokens = tokenize(value);
    if (tokens.size() != 2) {
        throw std::runtime_error("[BooleanXorNodeUnmarshaller] Requires two nodes!");
    }

    // Create set of IDs
    return toSet(tokens);
}

/**
 * Parses a depth function from a string.
 *
 * @param str String to parse, e.g. 'LESS'
 * @return Corresponding depth function
 * @throws std::invalid_argument if string is not a valid depth function
 */
GLenum BooleanXorNodeUnmarshaller::parseDepthFunction(const std::string& str) {
    std::map<std::string,GLenum>::const_iterator it = DEPTH_FUNCTIONS.find(str);
    if (it == DEPTH_FUNCTIONS.end()) {
        throw std::invalid_argument("[BooleanXorNodeUnmarshaller] String is not a valid depth function!");
    } else {
        return it->second;
    }
}

RapidGL::Node* BooleanXorNodeUnmarshaller::unmarshal(const std::map<std::string,std::string>& attributes) {

    // Find IDs
    const std::set<std::string> ids = getOf(attributes);

    // Get ID of cube to hide pieces for
    const std::string hide = getHide(attributes);
    if (!hide.empty() && (ids.count(hide) == 0)) {
        throw std::runtime_error("[BooleanXorNodeUnmarshaller] Value of 'hide' is not an identifier in 'of'!");
    }

    // Find filter
    const GLenum filter = getFilter(attributes);

    // Create the node
    return new BooleanXorNode(ids, hide, filter);
}

/**
 * Converts a `vector` to a `set`.
 *
 * @param vector Vector to convert
 * @return Set with elements from vector, excluding any duplicates
 */
std::set<std::string> BooleanXorNodeUnmarshaller::toSet(const std::vector<std::string>& vector) {
    std::set<std::string> set;
    for (std::vector<std::string>::const_iterator it = vector.begin(); it != vector.end(); ++it) {
        set.insert(*it);
    }
    return set;
}
