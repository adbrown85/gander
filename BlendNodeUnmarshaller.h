#ifndef BLEND_NODE_UNMARSHALLER
#define BLEND_NODE_UNMARSHALLER
#include <map>
#include <string>
#include <RapidGL/Node.h>
#include <RapidGL/Unmarshaller.h>
#include "BlendNode.h"


/**
 * Unmarshaller for `BlendNode`.
 */
class BlendNodeUnmarshaller : public RapidGL::Unmarshaller {
public:
// Methods
    BlendNodeUnmarshaller();
    virtual ~BlendNodeUnmarshaller();
    virtual RapidGL::Node* unmarshal(const std::map<std::string,std::string>& attributes);
private:
// Methods
    static bool getEnabled(const std::map<std::string,std::string>& attributes);
};

#endif
