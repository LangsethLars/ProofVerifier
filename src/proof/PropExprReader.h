#pragma once

#include "PropExpr.h"
#include "../parser/ParseTreeIterator.h"
#include "../parser/Proof_Parser.h"

class PropExprReader {
public:
    // Read a PropExpr from the parse tree iterator
    // The iterator should be positioned at a <PropExpr> node
    static PropExprPtr readPropExpr(ParseTreeIterator& it);

    // Read a BoolExpr from the parse tree iterator
    // The iterator should be positioned at a <BoolExpr> node
    static BoolExprPtr readBoolExpr(ParseTreeIterator& it);

private:
    // Helper to get the token at the current position
    static int getTokenId(const ParseTreeIterator& it);
    static int getVariableId(const ParseTreeIterator& it);
};
