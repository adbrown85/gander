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
