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
#include <Poco/String.h>
#include "SlicingVolumeRendererNodeUnmarshaller.h"

/**
 * Constructs a `SlicingVolumeRendererNodeUnmarshaller`.
 */
SlicingVolumeRendererNodeUnmarshaller::SlicingVolumeRendererNodeUnmarshaller() {
    // empty
}

/**
 * Destructs a `SlicingVolumeRendererNodeUnmarshaller`.
 */
SlicingVolumeRendererNodeUnmarshaller::~SlicingVolumeRendererNodeUnmarshaller() {
    // empty
}

/**
 * Determines the value of the _slices_ attribute in a map.
 *
 * @param map Map of XML attributes
 * @return Value of the attribute in the map
 */
GLint SlicingVolumeRendererNodeUnmarshaller::getSlices(const std::map<std::string,std::string>& map) {

    // Get value
    const std::string value = findValue(map, "slices");

    // If nothing specified, just return the default
    if (value.empty()) {
        return DEFAULT_SLICES;
    }

    // Parse value
    const GLint slices = parseInt(value);
    if (slices < 0) {
        throw std::runtime_error("Slices cannot be negative!");
    }

    // Return it
    return slices;
}

/**
 * Determines the value of the _strategy_ attribute in a map.
 *
 * @param map Map of XML attributes
 * @return Value of the attribute in the map
 */
std::string SlicingVolumeRendererNodeUnmarshaller::getStrategy(const std::map<std::string,std::string>& map) {
    const std::string value = findValue(map, "strategy");
    const std::string valueAsLower = Poco::toLower(value);
    if (valueAsLower != "attribute" && valueAsLower != "uniform") {
    }
    return valueAsLower;
}

/**
 * Determines the value of the _uniform_ attribute in a map.
 *
 * @param map Map of XML attributes
 * @return Value of the attribute in the map
 */
std::string SlicingVolumeRendererNodeUnmarshaller::getUniform(const std::map<std::string,std::string>& map) {
    const std::string value = findValue(map, "uniform");
    return value;
}

/**
 * Creates a `SlicingVolumeRendererNode` from a map of XML attributes.
 *
 * @param attributes Map of XML attributes to create node from
 * @return Pointer to the new node
 */
RapidGL::Node* SlicingVolumeRendererNodeUnmarshaller::unmarshal(const std::map<std::string,std::string>& attributes) {

    // Get uniform
    const std::string uniform = getUniform(attributes);

    // Get number of slices
    const GLint slices = getSlices(attributes);

    // Get strategy
    const std::string strategyAsString = getStrategy(attributes);
    SlicingVolumeRendererNode::Strategy* strategy;
    if (strategyAsString == "attribute") {
        strategy = new SlicingVolumeRendererNode::AttributeStrategy();
    } else if (strategyAsString == "uniform") {
        if (uniform.empty()) {
            throw std::runtime_error("[SlicingVolumeRendererNodeUnmarshaller] Value of uniform is unspecified!");
        }
        strategy = new SlicingVolumeRendererNode::UniformStrategy(uniform);
    } else {
        throw std::runtime_error("Unrecognized strategy!");
    }

    // Create node
    return new SlicingVolumeRendererNode(strategy, slices);
}
