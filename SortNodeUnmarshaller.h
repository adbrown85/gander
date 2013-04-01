#ifndef GANDER_SORT_NODE_UNMARSHALLER_H
#define GANDER_SORT_NODE_UNMARSHALLER_H
#include <map>
#include <string>
#include <RapidGL/Node.h>
#include <RapidGL/Unmarshaller.h>
#include "SortNode.h"


/**
 * Unmarshaller for `SortNode`.
 */
class SortNodeUnmarshaller : public RapidGL::Unmarshaller {
public:
// Methods
    SortNodeUnmarshaller();
    ~SortNodeUnmarshaller();
    virtual RapidGL::Node* unmarshal(const std::map<std::string,std::string>& attributes);
};

#endif
