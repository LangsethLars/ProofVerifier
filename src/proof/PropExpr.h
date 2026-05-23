#pragma once

#include "IntExpr.h"
#include <memory>
#include <string>

// Forward declaration
class PropExpr;
using PropExprPtr = std::shared_ptr<PropExpr>;

// Propositional expression types
enum class PropExprType {
	Var,           // IDENT (proposition variable)
	True,          // Prop.True
	False,         // Prop.False
	Not,           // Prop.Not(p)
	And,           // Prop.And(p q)
	Or,            // Prop.Or(p q)
	Implies,       // Prop.Implies(p q)
	Iff,           // Prop.Iff(p q)
	IntEq          // Prop.Int.Eq(a b)
};

class PropExpr {
public:
	PropExprType type;
	std::string name;                    // For Var
	PropExprPtr arg;                     // For Not
	PropExprPtr left, right;             // For And, Or, Implies, Iff
	IntExprPtr  intLeft, intRight;       // For IntEq
	int lineNumber;

	// Factory methods
	static PropExprPtr makeVar(const std::string& name, int line = -1);
	static PropExprPtr makeTrue(int line = -1);
	static PropExprPtr makeFalse(int line = -1);
	static PropExprPtr makeNot(PropExprPtr arg, int line = -1);
	static PropExprPtr makeAnd(PropExprPtr left, PropExprPtr right, int line = -1);
	static PropExprPtr makeOr(PropExprPtr left, PropExprPtr right, int line = -1);
	static PropExprPtr makeImplies(PropExprPtr left, PropExprPtr right, int line = -1);
	static PropExprPtr makeIff(PropExprPtr left, PropExprPtr right, int line = -1);
	static PropExprPtr makeIntEq(IntExprPtr left, IntExprPtr right, int line = -1);

	// String representation
	std::string toString() const;

	// Return a deep copy with every IntExpr Var(varName) replaced by replacement.
	PropExprPtr substitute(const std::string& varName, IntExprPtr replacement) const;

protected:
	PropExpr(PropExprType t, int line = -1) : type(t), lineNumber(line) {}
};
