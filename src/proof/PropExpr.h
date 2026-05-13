#pragma once

#include <memory>
#include <string>
#include <vector>

// Forward declarations
class PropExpr;
class BoolExpr;

using PropExprPtr = std::shared_ptr<PropExpr>;
using BoolExprPtr = std::shared_ptr<BoolExpr>;

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
    BoolEq         // Bool.Eq(a b)
};

// Boolean expression types
enum class BoolExprType {
    Var,           // IDENT (boolean variable)
    True,          // Bool.True
    False,         // Bool.False
    Not,           // Bool.Not(b)
    And,           // Bool.And(b1 b2)
    Or             // Bool.Or(b1 b2)
};

// Boolean expression evaluation result
enum class BoolExprEval {
    True,          // Bool.True
    False,         // Bool.False
	Error		   // Evaluation error (e.g., due to undefined variable)
};

// Boolean expression class
class BoolExpr {
public:
    BoolExprType type;
    std::string name;                    // For Var
    BoolExprPtr arg;                     // For Not
    BoolExprPtr left, right;             // For And, Or
    int lineNumber;

    // Constructors
    static BoolExprPtr makeVar(const std::string& name, int line = -1);
    static BoolExprPtr makeTrue(int line = -1);
    static BoolExprPtr makeFalse(int line = -1);
    static BoolExprPtr makeNot(BoolExprPtr arg, int line = -1);
    static BoolExprPtr makeAnd(BoolExprPtr left, BoolExprPtr right, int line = -1);
    static BoolExprPtr makeOr(BoolExprPtr left, BoolExprPtr right, int line = -1);

    // String representation
    std::string toString() const;

    BoolExprEval evaluate() const;

protected:
    BoolExpr(BoolExprType t, int line = -1) : type(t), lineNumber(line) {}
};

// Propositional expression class
class PropExpr {
public:
    PropExprType type;
    std::string name;                    // For Var
    PropExprPtr arg;                     // For Not
    PropExprPtr left, right;             // For And, Or, Implies, Iff
    BoolExprPtr boolLeft, boolRight;     // For BoolEq
    int lineNumber;

    // Constructors
    static PropExprPtr makeVar(const std::string& name, int line = -1);
    static PropExprPtr makeTrue(int line = -1);
    static PropExprPtr makeFalse(int line = -1);
    static PropExprPtr makeNot(PropExprPtr arg, int line = -1);
    static PropExprPtr makeAnd(PropExprPtr left, PropExprPtr right, int line = -1);
    static PropExprPtr makeOr(PropExprPtr left, PropExprPtr right, int line = -1);
    static PropExprPtr makeImplies(PropExprPtr left, PropExprPtr right, int line = -1);
    static PropExprPtr makeIff(PropExprPtr left, PropExprPtr right, int line = -1);
    static PropExprPtr makeBoolEq(BoolExprPtr left, BoolExprPtr right, int line = -1);

    // String representation
    std::string toString() const;

protected:
    PropExpr(PropExprType t, int line = -1) : type(t), lineNumber(line) {}
};
