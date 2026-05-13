# Parameter Manager Documentation

## Overview

The Parameter Manager provides scoped parameter tracking for the theorem verifier. It manages parameter declarations with proper scoping rules, allowing parameters to be overridden in nested scopes while preventing duplicate declarations at the same level.

## Architecture

### Files

- **`ParamManager.h/cpp`** - Core parameter management with scoping
- **`ParamReader.h/cpp`** - Reads parameters from the parse tree

## Classes

### `Param` - Parameter Structure

Represents a single parameter declaration.

```cpp
struct Param {
    std::string name;      // Parameter name (e.g., "p", "x", "myVar")
    ParamType type;        // Bool or Prop
    int lineNumber;        // Source line for error reporting
};
```

### `ParamType` - Parameter Types

```cpp
enum class ParamType {
    Bool,    // Boolean type
    Prop     // Proposition type
};
```

Helper function:
```cpp
std::string paramTypeToString(ParamType type);
// Returns "Bool" or "Prop"
```

### `ParamManager` - Scoped Parameter Management

Manages parameters across multiple scopes with proper shadowing rules.

**Key Methods:**

```cpp
// Scope management
void pushScope();                  // Create new inner scope
void popScope();                   // Remove innermost scope (not global)
int getScopeDepth() const;         // Current nesting level

// Parameter operations
void addParam(const std::string& name, ParamType type, int lineNumber = -1);
// Adds parameter to current scope
// Throws if parameter already exists at current level

ParamPtr lookupParam(const std::string& name) const;
// Search from innermost to outermost scope
// Returns nullptr if not found

bool existsInCurrentScope(const std::string& name) const;
// Check only current scope (not parent scopes)

std::vector<ParamPtr> getCurrentScopeParams() const;
// Get all parameters in current scope

void clear();
// Reset to initial state (global scope only)
```

### `ParamReader` - Parse Tree Reader

Utility class to read parameter declarations from the parse tree.

**Key Methods:**

```cpp
static int readParams(ParseTreeIterator& it, ParamManager& paramMgr);
// Reads parameters from parse tree
// Iterator should be positioned after theorem name (at <ParamList>)
// Returns number of parameters read
// Advances iterator to COLON
```

## Grammar Correspondence

The reader handles this grammar structure:

```
Rule <Theorem> = _Theorem IDENT <ParamList> COLON <PropExpr> <ProofBlock>;
Rule <ParamList> = <Param> <ParamList> | ;
Rule <Param> = OPEN_PAR IDENT COLON <TypeExpr> CLOSE_PAR;
Skip <TypeExpr> = _Bool | _Prop;
```

## Usage Example

```cpp
#include "proof/ParamManager.h"
#include "proof/ParamReader.h"

// Initialize parameter manager
ParamManager paramMgr;

// For each theorem, create a new scope
paramMgr.pushScope();

// Read parameters from parse tree
ParseTreeIterator it = /* positioned after theorem name */;
int paramCount = ParamReader::readParams(it, paramMgr);

// Display parameters
for (const auto& param : paramMgr.getCurrentScopeParams()) {
    std::cout << param->name << " : " << paramTypeToString(param->type) << std::endl;
}

// Look up a parameter
ParamPtr p = paramMgr.lookupParam("x");
if (p) {
    std::cout << "Found: " << p->name << " : " << paramTypeToString(p->type) << std::endl;
} else {
    std::cout << "Parameter 'x' not found" << std::endl;
}

// Done with theorem, pop scope
paramMgr.popScope();
```

## Scoping Rules

### 1. Duplicate Detection

Parameters cannot be declared twice at the same scope level:

```
Theorem test (p : Prop) (p : Prop) : p
                        ^^^^^^^^
                        ERROR: Already declared
```

### 2. Shadowing (Future)

Inner scopes can shadow outer scope parameters:

```
Global: p : Prop

Theorem outer (p : Bool) : ...    ✓ OK: Shadows global p
  Proof
    Cases x                        ✓ OK: New parameter
    ...
  End
```

### 3. Lookup Order

Parameters are searched from innermost to outermost scope:

```
Scope 0 (global): p : Prop, q : Prop
Scope 1 (theorem): p : Bool, r : Prop

Lookup "p" → Bool (from Scope 1, shadows global)
Lookup "q" → Prop (from Scope 0)
Lookup "r" → Prop (from Scope 1)
Lookup "x" → nullptr (not found)
```

## Test Results

From `TestParams.txt`:

```
Theorem test_no_params : Prop.True
→ No parameters

Theorem test_one_param (p : Prop) : p
→ Parameters:
    p : Prop (line 5)

Theorem test_two_params (p : Prop) (q : Prop) : Prop.And(p q)
→ Parameters:
    p : Prop (line 9)
    q : Prop (line 9)

Theorem test_bool_param (b : Bool) : Bool.Eq(b b)
→ Parameters:
    b : Bool (line 13)

Theorem test_mixed (p : Prop) (b : Bool) (q : Prop) : Prop.And(p q)
→ Parameters:
    p : Prop (line 17)
    b : Bool (line 17)
    q : Prop (line 17)
```

## Error Handling

### Duplicate Parameter Error

```
Theorem test_duplicate (p : Prop) (p : Prop) : p
→ ERROR: Parameter 'p' already declared in current scope (line 1)
```

### Undefined Parameter (Future)

When a parameter is referenced but not declared, the verifier should report:
```
ERROR: Undefined parameter 'x'
```

## Implementation Notes

1. **Shared Pointers**: Uses `std::shared_ptr<Param>` for easy sharing
2. **Global Scope**: Always maintains at least one scope (global)
3. **Stack-Based**: Scopes are managed as a stack (vector of maps)
4. **Hash Map Lookup**: O(1) parameter lookup within each scope
5. **Linear Search**: O(n) lookup across all scopes (where n = scope depth)

## Integration with Proof Verification

The ParamManager should be:
- **Created**: At the start of verification
- **Scoped**: Push scope for each theorem, proof block, or local context
- **Queried**: When validating parameter references in expressions
- **Cleaned**: Pop scope when exiting context

## Future Enhancements

1. **Type Checking**: Verify parameter usage matches declared type
2. **Proof Scope**: Add scopes for proof blocks and local assumptions
3. **Global Parameters**: Support file-level parameter declarations
4. **Parameter Substitution**: Replace parameters with concrete values
5. **Well-Formedness**: Check that all referenced parameters are declared

## API Summary

```cpp
// Initialization
ParamManager paramMgr;

// Scope management
paramMgr.pushScope();                                    // Enter new scope
paramMgr.popScope();                                     // Exit scope
int depth = paramMgr.getScopeDepth();                   // Get depth

// Add parameters
paramMgr.addParam("p", ParamType::Prop, lineNo);        // Add to current scope

// Query parameters
ParamPtr p = paramMgr.lookupParam("p");                 // Search all scopes
bool exists = paramMgr.existsInCurrentScope("p");       // Check current only
auto params = paramMgr.getCurrentScopeParams();         // Get all in scope

// Reset
paramMgr.clear();                                        // Clear all scopes

// Read from parse tree
int count = ParamReader::readParams(it, paramMgr);      // Read parameters
```

## Next Steps

The parameter system is ready for:
- **Proof Blocks**: Adding scopes for proof contexts
- **Type Validation**: Checking parameter usage against declarations
- **Substitution**: Replacing parameters with values
- **Error Messages**: Providing detailed parameter-related errors
