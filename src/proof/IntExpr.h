#pragma once

#include <memory>
#include <string>
#include <optional>
#include <cstdint>

class IntExpr;
using IntExprPtr = std::shared_ptr<IntExpr>;

enum class IntExprType {
	Var,    // IDENT (integer variable)
	Lit,    // NUMBER literal
	Neg,    // Int.Neg(a)
	Add,    // Int.Add(a b)
	Sub,    // Int.Sub(a b)
	Mul,    // Int.Mul(a b)
};

class IntExpr {
public:
	IntExprType type;
	std::string name;       // For Var
	int64_t     value;      // For Lit
	IntExprPtr  arg;        // For Neg
	IntExprPtr  left, right;// For Add, Sub, Mul
	int lineNumber;

	// Factory methods
	static IntExprPtr makeVar(const std::string& name, int line = -1);
	static IntExprPtr makeLit(int64_t value, int line = -1);
	static IntExprPtr makeNeg(IntExprPtr arg, int line = -1);
	static IntExprPtr makeAdd(IntExprPtr left, IntExprPtr right, int line = -1);
	static IntExprPtr makeSub(IntExprPtr left, IntExprPtr right, int line = -1);
	static IntExprPtr makeMul(IntExprPtr left, IntExprPtr right, int line = -1);

	// Evaluate to a concrete integer value.
	// Returns nullopt if any variable has not been substituted yet.
	std::optional<int64_t> evaluate() const;

	// Return a deep copy with every Var(varName) replaced by replacement.
	IntExprPtr substitute(const std::string& varName, IntExprPtr replacement) const;

	std::string toString() const;

protected:
	IntExpr(IntExprType t, int line = -1) : type(t), value(0), lineNumber(line) {}
};
