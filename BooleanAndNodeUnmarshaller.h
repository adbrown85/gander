#ifndef BOOLEAN_AND_NODE_UNMARSHALLER_H
#define BOOLEAN_AND_NODE_UNMARSHALLER_H
#include <map>
#include <string>
#include <RapidGL/Node.h>
#include <RapidGL/Unmarshaller.h>
#include "BooleanAndNode.h"


/**
 * Unmarshaller for `BooleanAndNode`.
 */
class BooleanAndNodeUnmarshaller : public RapidGL::Unmarshaller {
public:
// Methods
    BooleanAndNodeUnmarshaller();
    virtual ~BooleanAndNodeUnmarshaller();
    virtual RapidGL::Node* unmarshal(const std::map<std::string,std::string>& attributes);
};

#endif
