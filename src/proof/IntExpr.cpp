#include "IntExpr.h"
#include <sstream>
#include <stdexcept>

// Factory methods

IntExprPtr IntExpr::makeVar(const std::string& name, int line) {
	auto e = std::shared_ptr<IntExpr>(new IntExpr(IntExprType::Var, line));
	e->name = name;
	return e;
}

IntExprPtr IntExpr::makeLit(int64_t value, int line) {
	auto e = std::shared_ptr<IntExpr>(new IntExpr(IntExprType::Lit, line));
	e->value = value;
	return e;
}

IntExprPtr IntExpr::makeNeg(IntExprPtr arg, int line) {
	auto e = std::shared_ptr<IntExpr>(new IntExpr(IntExprType::Neg, line));
	e->arg = arg;
	return e;
}

IntExprPtr IntExpr::makeAdd(IntExprPtr left, IntExprPtr right, int line) {
	auto e = std::shared_ptr<IntExpr>(new IntExpr(IntExprType::Add, line));
	e->left = left; e->right = right;
	return e;
}

IntExprPtr IntExpr::makeSub(IntExprPtr left, IntExprPtr right, int line) {
	auto e = std::shared_ptr<IntExpr>(new IntExpr(IntExprType::Sub, line));
	e->left = left; e->right = right;
	return e;
}

IntExprPtr IntExpr::makeMul(IntExprPtr left, IntExprPtr right, int line) {
	auto e = std::shared_ptr<IntExpr>(new IntExpr(IntExprType::Mul, line));
	e->left = left; e->right = right;
	return e;
}

// evaluate

std::optional<int64_t> IntExpr::evaluate() const {
	switch (type) {
		case IntExprType::Var:
			return std::nullopt; // unsubstituted variable
		case IntExprType::Lit:
			return value;
		case IntExprType::Neg: {
			auto v = arg->evaluate();
			if (!v) return std::nullopt;
			return -(*v);
		}
		case IntExprType::Add: {
			auto l = left->evaluate(), r = right->evaluate();
			if (!l || !r) return std::nullopt;
			return *l + *r;
		}
		case IntExprType::Sub: {
			auto l = left->evaluate(), r = right->evaluate();
			if (!l || !r) return std::nullopt;
			return *l - *r;
		}
		case IntExprType::Mul: {
			auto l = left->evaluate(), r = right->evaluate();
			if (!l || !r) return std::nullopt;
			return *l * *r;
		}
	}
	return std::nullopt;
}

// substitute

IntExprPtr IntExpr::substitute(const std::string& varName, IntExprPtr replacement) const {
	switch (type) {
		case IntExprType::Var:
			return (name == varName) ? replacement : makeVar(name, lineNumber);
		case IntExprType::Lit:
			return makeLit(value, lineNumber);
		case IntExprType::Neg:
			return makeNeg(arg->substitute(varName, replacement), lineNumber);
		case IntExprType::Add:
			return makeAdd(left->substitute(varName, replacement),
						   right->substitute(varName, replacement), lineNumber);
		case IntExprType::Sub:
			return makeSub(left->substitute(varName, replacement),
						   right->substitute(varName, replacement), lineNumber);
		case IntExprType::Mul:
			return makeMul(left->substitute(varName, replacement),
						   right->substitute(varName, replacement), lineNumber);
	}
	return makeLit(0, lineNumber); // unreachable
}

// toString

std::string IntExpr::toString() const {
	std::ostringstream oss;
	switch (type) {
		case IntExprType::Var:
			oss << name;
			break;
		case IntExprType::Lit:
			oss << value;
			break;
		case IntExprType::Neg:
			oss << "Int.Neg(" << arg->toString() << ")";
			break;
		case IntExprType::Add:
			oss << "Int.Add(" << left->toString() << " " << right->toString() << ")";
			break;
		case IntExprType::Sub:
			oss << "Int.Sub(" << left->toString() << " " << right->toString() << ")";
			break;
		case IntExprType::Mul:
			oss << "Int.Mul(" << left->toString() << " " << right->toString() << ")";
			break;
	}
	return oss.str();
}
