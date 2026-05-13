# PropExpr Documentation

## Overview

The `PropExpr` class represents propositional logic expressions in the theorem verifier. It provides a type-safe representation of propositions and boolean expressions, along with utilities to read them from the parse tree.

## Classes

### `PropExpr` - Propositional Expressions

Represents propositions that can be proven true or false.

**Types:**
- `Var` - Proposition variable (e.g., `p`, `q`, `myProp`)
- `True` - Always true proposition (`Prop.True`)
- `False` - Always false proposition (`Prop.False`)  
- `Not` - Negation (`Prop.Not(p)`)
- `And` - Conjunction (`Prop.And(p q)`)
- `Or` - Disjunction (`Prop.Or(p q)`)
- `Implies` - Implication (`Prop.Implies(p q)`)
- `Iff` - Bi-implication (`Prop.Iff(p q)`)
- `BoolEq` - Boolean equality (`Bool.Eq(a b)`)

**Factory Methods:**
```cpp
PropExprPtr PropExpr::makeVar(const std::string& name, int line = -1);
PropExprPtr PropExpr::makeTrue(int line = -1);
PropExprPtr PropExpr::makeFalse(int line = -1);
PropExprPtr PropExpr::makeNot(PropExprPtr arg, int line = -1);
PropExprPtr PropExpr::makeAnd(PropExprPtr left, PropExprPtr right, int line = -1);
PropExprPtr PropExpr::makeOr(PropExprPtr left, PropExprPtr right, int line = -1);
PropExprPtr PropExpr::makeImplies(PropExprPtr left, PropExprPtr right, int line = -1);
PropExprPtr PropExpr::makeIff(PropExprPtr left, PropExprPtr right, int line = -1);
PropExprPtr PropExpr::makeBoolEq(BoolExprPtr left, BoolExprPtr right, int line = -1);
```

### `BoolExpr` - Boolean Expressions

Represents boolean values and operations.

**Types:**
- `Var` - Boolean variable (e.g., `x`, `b`)
- `True` - Boolean true (`Bool.True`)
- `False` - Boolean false (`Bool.False`)
- `Not` - Boolean negation (`Bool.Not(b)`)
- `And` - Boolean AND (`Bool.And(b1 b2)`)
- `Or` - Boolean OR (`Bool.Or(b1 b2)`)

**Factory Methods:**
```cpp
BoolExprPtr BoolExpr::makeVar(const std::string& name, int line = -1);
BoolExprPtr BoolExpr::makeTrue(int line = -1);
BoolExprPtr BoolExpr::makeFalse(int line = -1);
BoolExprPtr BoolExpr::makeNot(BoolExprPtr arg, int line = -1);
BoolExprPtr BoolExpr::makeAnd(BoolExprPtr left, BoolExprPtr right, int line = -1);
BoolExprPtr BoolExpr::makeOr(BoolExprPtr left, BoolExprPtr right, int line = -1);
```

### `PropExprReader` - Parse Tree Reader

Utility class to construct `PropExpr` and `BoolExpr` objects from the parse tree.

**Methods:**
```cpp
// Read a PropExpr from a ParseTreeIterator positioned at <PropExpr>
static PropExprPtr readPropExpr(ParseTreeIterator& it);

// Read a BoolExpr from a ParseTreeIterator positioned at <BoolExpr>
static BoolExprPtr readBoolExpr(ParseTreeIterator& it);
```

## Usage Example

```cpp
#include "proof/PropExprReader.h"
#include "parser/ParseTreeIterator.h"

// Assuming we have a parse tree iterator positioned at <PropExpr>
ParseTreeIterator it(parser.m_ParseTree, parser.m_TokenSequence, parser.m_RawText);

// ... navigate to <PropExpr> node ...

// Read the propositional expression
PropExprPtr expr = PropExprReader::readPropExpr(it);

// Display it
std::cout << "Statement: " << expr->toString() << std::endl;

// Access the structure
if (expr->type == PropExprType::And) {
    std::cout << "Left: " << expr->left->toString() << std::endl;
    std::cout << "Right: " << expr->right->toString() << std::endl;
}
```

## Test Examples

From `TestProp.txt`:

```
Theorem test1 : Prop.True
→ Prop.True

Theorem test2 : Prop.Not(Prop.False)
→ Prop.Not(Prop.False)

Theorem test3 : Prop.And(p q)
→ Prop.And(p q)

Theorem test4 : Prop.Implies(Prop.And(p q) p)
→ Prop.Implies(Prop.And(p q) p)

Theorem test5 : Bool.Eq(Bool.True Bool.False)
→ Bool.Eq(Bool.True Bool.False)
```

## Key Features

1. **Type Safety**: Uses enums to distinguish expression types
2. **Shared Ownership**: Uses `std::shared_ptr` for easy sharing and copying
3. **Line Tracking**: Preserves source line numbers for error reporting
4. **String Representation**: `toString()` methods for debugging and display
5. **Immutable**: Factory methods create immutable expression trees

## Grammar Correspondence

The reader directly corresponds to the grammar rules:

```
Rule <PropExpr>
    = IDENT
    | _Prop_True
    | _Prop_False
    | _Prop_Not     OPEN_PAR <PropExpr> CLOSE_PAR
    | _Prop_And     OPEN_PAR <PropExpr> <PropExpr> CLOSE_PAR
    | _Prop_Or      OPEN_PAR <PropExpr> <PropExpr> CLOSE_PAR
    | _Prop_Implies OPEN_PAR <PropExpr> <PropExpr> CLOSE_PAR
    | _Prop_Iff     OPEN_PAR <PropExpr> <PropExpr> CLOSE_PAR
    | _Bool_Eq      OPEN_PAR <BoolExpr> <BoolExpr> CLOSE_PAR;

Rule <BoolExpr>
    = IDENT
    | _Bool_True
    | _Bool_False
    | _Bool_Not   OPEN_PAR <BoolExpr> CLOSE_PAR
    | _Bool_And   OPEN_PAR <BoolExpr> <BoolExpr> CLOSE_PAR
    | _Bool_Or    OPEN_PAR <BoolExpr> <BoolExpr> CLOSE_PAR;
```

## Next Steps

The PropExpr class is ready for:
- **Proof Verification**: Checking that proofs match their stated propositions
- **Type Checking**: Verifying propositions are well-formed  
- **Normalization**: Simplifying expressions
- **Substitution**: Replacing variables with values
- **Evaluation**: Computing boolean values when possible
