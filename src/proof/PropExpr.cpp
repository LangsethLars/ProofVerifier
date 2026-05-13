#include "PropExpr.h"
#include <sstream>

// BoolExpr factory methods
BoolExprPtr BoolExpr::makeVar(const std::string& name, int line) {
    auto expr = std::shared_ptr<BoolExpr>(new BoolExpr(BoolExprType::Var, line));
    expr->name = name;
    return expr;
}

BoolExprPtr BoolExpr::makeTrue(int line) {
    return std::shared_ptr<BoolExpr>(new BoolExpr(BoolExprType::True, line));
}

BoolExprPtr BoolExpr::makeFalse(int line) {
    return std::shared_ptr<BoolExpr>(new BoolExpr(BoolExprType::False, line));
}

BoolExprPtr BoolExpr::makeNot(BoolExprPtr arg, int line) {
    auto expr = std::shared_ptr<BoolExpr>(new BoolExpr(BoolExprType::Not, line));
    expr->arg = arg;
    return expr;
}

BoolExprPtr BoolExpr::makeAnd(BoolExprPtr left, BoolExprPtr right, int line) {
    auto expr = std::shared_ptr<BoolExpr>(new BoolExpr(BoolExprType::And, line));
    expr->left = left;
    expr->right = right;
    return expr;
}

BoolExprPtr BoolExpr::makeOr(BoolExprPtr left, BoolExprPtr right, int line) {
    auto expr = std::shared_ptr<BoolExpr>(new BoolExpr(BoolExprType::Or, line));
    expr->left = left;
    expr->right = right;
    return expr;
}

std::string BoolExpr::toString() const {
    std::ostringstream oss;
    switch (type) {
        case BoolExprType::Var:
            oss << name;
            break;
        case BoolExprType::True:
            oss << "Bool.True";
            break;
        case BoolExprType::False:
            oss << "Bool.False";
            break;
        case BoolExprType::Not:
            oss << "Bool.Not(" << (arg ? arg->toString() : "?") << ")";
            break;
        case BoolExprType::And:
            oss << "Bool.And(" << (left ? left->toString() : "?") << " " 
                << (right ? right->toString() : "?") << ")";
            break;
        case BoolExprType::Or:
            oss << "Bool.Or(" << (left ? left->toString() : "?") << " " 
                << (right ? right->toString() : "?") << ")";
            break;
    }
    return oss.str();
}

BoolExprEval BoolExpr::evaluate() const
{
    switch (type) {
        case BoolExprType::Var:
            return BoolExprEval::Error; // Cannot evaluate variable
        case BoolExprType::True:
            return BoolExprEval::True;
        case BoolExprType::False:
            return BoolExprEval::False;
        case BoolExprType::Not: {
            if (!arg) return BoolExprEval::Error;
            BoolExprEval argVal = arg->evaluate();
            if (argVal == BoolExprEval::Error) return BoolExprEval::Error;
            return (argVal == BoolExprEval::True) ? BoolExprEval::False : BoolExprEval::True;
        }
        case BoolExprType::And: {
            if (!left || !right) return BoolExprEval::Error;
            BoolExprEval leftVal = left->evaluate();
            if (leftVal == BoolExprEval::Error) return BoolExprEval::Error;
            if (leftVal == BoolExprEval::False) return BoolExprEval::False; // Short-circuit
            BoolExprEval rightVal = right->evaluate();
            if (rightVal == BoolExprEval::Error) return BoolExprEval::Error;
            return (rightVal == BoolExprEval::True) ? BoolExprEval::True : BoolExprEval::False;
        }
        case BoolExprType::Or: {
            if (!left || !right) return BoolExprEval::Error;
            BoolExprEval leftVal = left->evaluate();
            if (leftVal == BoolExprEval::Error) return BoolExprEval::Error;
            if (leftVal == BoolExprEval::True) return BoolExprEval::True; // Short-circuit
            BoolExprEval rightVal = right->evaluate();
            if (rightVal == BoolExprEval::Error) return BoolExprEval::Error;
            return (rightVal == BoolExprEval::True) ? BoolExprEval::True : BoolExprEval::False;
        }
    }
	return BoolExprEval::Error; // Should not reach here
}

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

PropExprPtr PropExpr::makeBoolEq(BoolExprPtr left, BoolExprPtr right, int line) {
    auto expr = std::shared_ptr<PropExpr>(new PropExpr(PropExprType::BoolEq, line));
    expr->boolLeft = left;
    expr->boolRight = right;
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
        case PropExprType::BoolEq:
            oss << "Bool.Eq(" << (boolLeft ? boolLeft->toString() : "?") << " " 
                << (boolRight ? boolRight->toString() : "?") << ")";
            break;
    }
    return oss.str();
}
