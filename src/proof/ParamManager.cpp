#include "ParamManager.h"
#include <sstream>

ParamManager::ParamManager() {
    // Start with global scope
    m_scopes.emplace_back();
}

void ParamManager::pushScope() {
    m_scopes.emplace_back();
}

void ParamManager::popScope() {
    if (m_scopes.size() <= 1) {
        throw std::runtime_error("ParamManager: Cannot pop global scope");
    }
    m_scopes.pop_back();
}

int ParamManager::getScopeDepth() const {
    return static_cast<int>(m_scopes.size());
}

void ParamManager::addParam(const std::string& name, ParamType type, int lineNumber) {
    if (m_scopes.empty()) {
        throw std::runtime_error("ParamManager: No scope available");
    }

    // Check if parameter already exists at current level
    Scope& currentScope = m_scopes.back();
    if (currentScope.find(name) != currentScope.end()) {
        std::ostringstream oss;
        oss << "Parameter '" << name << "' already declared in current scope";
        if (lineNumber >= 0) {
            oss << " (line " << lineNumber << ")";
        }
        throw std::runtime_error(oss.str());
    }

    // Add parameter to current scope
    auto param = std::make_shared<Param>(name, type, lineNumber);
    currentScope[name] = param;
}

ParamPtr ParamManager::lookupParam(const std::string& name) const {
    // Search from innermost to outermost scope
    for (auto it = m_scopes.rbegin(); it != m_scopes.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) {
            return found->second;
        }
    }
    return nullptr;
}

bool ParamManager::existsInCurrentScope(const std::string& name) const {
    if (m_scopes.empty()) {
        return false;
    }
    const Scope& currentScope = m_scopes.back();
    return currentScope.find(name) != currentScope.end();
}

std::vector<ParamPtr> ParamManager::getCurrentScopeParams() const {
    std::vector<ParamPtr> params;
    if (!m_scopes.empty()) {
        const Scope& currentScope = m_scopes.back();
        for (const auto& pair : currentScope) {
            params.push_back(pair.second);
        }
    }
    return params;
}

void ParamManager::clear() {
    m_scopes.clear();
    // Re-create global scope
    m_scopes.emplace_back();
}
