#include <stdexcept>
#include "BlendNodeUnmarshaller.h"

/**
 * Constructs a `BlendNodeUnmarshaller`.
 */
BlendNodeUnmarshaller::BlendNodeUnmarshaller() {
    // empty
}

/**
 * Destructs a `BlendNodeUnmarshaller`.
 */
BlendNodeUnmarshaller::~BlendNodeUnmarshaller() {
    // empty
}

bool BlendNodeUnmarshaller::getEnabled(const std::map<std::string,std::string>& attributes) {

    // Find value
    const std::string value = findValue(attributes, "enabled");
    if (value.empty()) {
        throw std::runtime_error("[BlendNodeUnmarshaller] Enabled is unspecified!");
    }

    // Parse value
    if (value == "true") {
        return true;
    } else if (value == "false") {
        return false;
    } else {
        throw std::runtime_error("[BlendNodeUnmarshaller] Enabled is invalid!");
    }
}

RapidGL::Node* BlendNodeUnmarshaller::unmarshal(const std::map<std::string,std::string>& attributes) {
    const bool enabled = getEnabled(attributes);
    return new BlendNode(enabled);
}
