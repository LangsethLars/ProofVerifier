#include "PropExpr.h"
#include <sstream>

// PropExpr factory methods

PropExprPtr PropExpr::makeVar(const std::string& name, int line) {
    auto expr = std::shared_ptr<PropExpr>(new PropExpr(PropExprType::Var, line));
    expr->name = name;
    return expr;
}

PropExprPtr PropExpr::makeTrue(int line) {
    return std::shared_ptr<PropExpr>(new PropExpr(PropExprType::True, line));
}

PropExprPtr PropExpr::makeFalse(int line) {
    return std::shared_ptr<PropExpr>(new PropExpr(PropExprType::False, line));
}

PropExprPtr PropExpr::makeNot(PropExprPtr arg, int line) {
    auto expr = std::shared_ptr<PropExpr>(new PropExpr(PropExprType::Not, line));
    expr->arg = arg;
    return expr;
}

PropExprPtr PropExpr::makeAnd(PropExprPtr left, PropExprPtr right, int line) {
    auto expr = std::shared_ptr<PropExpr>(new PropExpr(PropExprType::And, line));
    expr->left = left;
    expr->right = right;
    return expr;
}

PropExprPtr PropExpr::makeOr(PropExprPtr left, PropExprPtr right, int line) {
    auto expr = std::shared_ptr<PropExpr>(new PropExpr(PropExprType::Or, line));
    expr->left = left;
    expr->right = right;
    return expr;
}

PropExprPtr PropExpr::makeImplies(PropExprPtr left, PropExprPtr right, int line) {
    auto expr = std::shared_ptr<PropExpr>(new PropExpr(PropExprType::Implies, line));
    expr->left = left;
    expr->right = right;
    return expr;
}

PropExprPtr PropExpr::makeIff(PropExprPtr left, PropExprPtr right, int line) {
    auto expr = std::shared_ptr<PropExpr>(new PropExpr(PropExprType::Iff, line));
    expr->left = left;
    expr->right = right;
    return expr;
}

PropExprPtr PropExpr::makeIntEq(IntExprPtr left, IntExprPtr right, int line) {
    auto expr = std::shared_ptr<PropExpr>(new PropExpr(PropExprType::IntEq, line));
    expr->intLeft = left;
    expr->intRight = right;
    return expr;
}

std::string PropExpr::toString() const {
    std::ostringstream oss;
    switch (type) {
        case PropExprType::Var:
            oss << name;
            break;
        case PropExprType::True:
            oss << "Prop.True";
            break;
        case PropExprType::False:
            oss << "Prop.False";
            break;
        case PropExprType::Not:
            oss << "Prop.Not(" << (arg ? arg->toString() : "?") << ")";
            break;
        case PropExprType::And:
            oss << "Prop.And(" << (left ? left->toString() : "?") << " "
                << (right ? right->toString() : "?") << ")";
            break;
        case PropExprType::Or:
            oss << "Prop.Or(" << (left ? left->toString() : "?") << " "
                << (right ? right->toString() : "?") << ")";
            break;
        case PropExprType::Implies:
            oss << "Prop.Implies(" << (left ? left->toString() : "?") << " "
                << (right ? right->toString() : "?") << ")";
            break;
        case PropExprType::Iff:
            oss << "Prop.Iff(" << (left ? left->toString() : "?") << " "
                << (right ? right->toString() : "?") << ")";
            break;
        case PropExprType::IntEq:
            oss << "Prop.Int.Eq(" << (intLeft ? intLeft->toString() : "?") << " "
                << (intRight ? intRight->toString() : "?") << ")";
            break;
    }
    return oss.str();
}

PropExprPtr PropExpr::substitute(const std::string& varName, IntExprPtr replacement) const {
    switch (type) {
        case PropExprType::Var:
            return makeVar(name, lineNumber);
        case PropExprType::True:
            return makeTrue(lineNumber);
        case PropExprType::False:
            return makeFalse(lineNumber);
        case PropExprType::Not:
            return makeNot(arg->substitute(varName, replacement), lineNumber);
        case PropExprType::And:
            return makeAnd(left->substitute(varName, replacement),
                           right->substitute(varName, replacement), lineNumber);
        case PropExprType::Or:
            return makeOr(left->substitute(varName, replacement),
                          right->substitute(varName, replacement), lineNumber);
        case PropExprType::Implies:
            return makeImplies(left->substitute(varName, replacement),
                               right->substitute(varName, replacement), lineNumber);
        case PropExprType::Iff:
            return makeIff(left->substitute(varName, replacement),
                           right->substitute(varName, replacement), lineNumber);
        case PropExprType::IntEq:
            return makeIntEq(intLeft->substitute(varName, replacement),intRight->substitute(varName, replacement), lineNumber);
    }
    return nullptr;
}
