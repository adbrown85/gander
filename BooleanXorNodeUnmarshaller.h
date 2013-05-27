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
#ifndef GANDER_BOOLEAN_XOR_NODE_UNMARSHALLER_H
#define GANDER_BOOLEAN_XOR_NODE_UNMARSHALLER_H
#include <map>
#include <set>
#include <string>
#include <vector>
#include <RapidGL/Node.h>
#include <RapidGL/Unmarshaller.h>
#include "BooleanXorNode.h"


/**
 * Unmarshaller for `BooleanXorNode`.
 */
class BooleanXorNodeUnmarshaller : public RapidGL::Unmarshaller {
public:
// Methods
    BooleanXorNodeUnmarshaller();
    virtual ~BooleanXorNodeUnmarshaller();
    virtual RapidGL::Node* unmarshal(const std::map<std::string,std::string>& attributes);
private:
// Constants
    static const std::map<std::string,GLenum> DEPTH_FUNCTIONS;
// Methods
    static std::map<std::string,GLenum> createDepthFunctions();
    static GLenum getFilter(const std::map<std::string,std::string>& attributes);
    static std::string getHide(const std::map<std::string,std::string>& attributes);
    static std::set<std::string> getOf(const std::map<std::string,std::string>& attributes);
    static GLenum parseDepthFunction(const std::string& str);
    static std::set<std::string> toSet(const std::vector<std::string>& vec);
};

#endif
