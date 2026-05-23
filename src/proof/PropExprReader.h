#pragma once

#include "PropExpr.h"
#include "IntExpr.h"
#include "../parser/ParseTreeIterator.h"
#include "../parser/Proof_Parser.h"

class PropExprReader {
public:
    // Read a <PropExpr> node. Iterator must be positioned at the <PropExpr> node.
    static PropExprPtr readPropExpr(ParseTreeIterator& it);

    // Read an <IntExpr> node. Iterator must be positioned at the <IntExpr> node.
    static IntExprPtr readIntExpr(ParseTreeIterator& it);

private:
    static int getTokenId(const ParseTreeIterator& it);
    static int getVariableId(const ParseTreeIterator& it);
};
