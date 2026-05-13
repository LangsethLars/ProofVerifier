#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <stdexcept>

// Parameter types
enum class ParamType {
    Bool,
    Prop
};

// String representation of parameter types
inline std::string paramTypeToString(ParamType type) {
    switch (type) {
        case ParamType::Bool: return "Bool";
        case ParamType::Prop: return "Prop";
    }
    return "Unknown";
}

// A single parameter
struct Param {
    std::string name;
    ParamType type;
    int lineNumber;

    Param(const std::string& n, ParamType t, int line = -1)
        : name(n), type(t), lineNumber(line) {}
};

using ParamPtr = std::shared_ptr<Param>;

// Parameter manager with scoping support
class ParamManager {
public:
    ParamManager();

    // Scope management
    void pushScope();
    void popScope();
    int getScopeDepth() const;

    // Add a parameter to the current scope
    // Throws if parameter already exists at current level
    void addParam(const std::string& name, ParamType type, int lineNumber = -1);

    // Lookup a parameter (searches from innermost to outermost scope)
    // Returns nullptr if not found
    ParamPtr lookupParam(const std::string& name) const;

    // Check if parameter exists at current level only
    bool existsInCurrentScope(const std::string& name) const;

    // Get all parameters in current scope (for debugging)
    std::vector<ParamPtr> getCurrentScopeParams() const;

    // Clear all scopes (reset to initial state)
    void clear();

private:
    // Each scope is a map of parameter name to parameter info
    using Scope = std::unordered_map<std::string, ParamPtr>;
    std::vector<Scope> m_scopes;
};
