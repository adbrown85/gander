#include "config.h"
#include "SortNodeUnmarshaller.h"

SortNodeUnmarshaller::SortNodeUnmarshaller() {
    // empty
}

SortNodeUnmarshaller::~SortNodeUnmarshaller() {
    // empty
}

RapidGL::Node* SortNodeUnmarshaller::unmarshal(const std::map<std::string,std::string>& attributes) {
    return new SortNode();
}
