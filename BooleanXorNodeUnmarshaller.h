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
