#pragma once

#include "ParamManager.h"
#include "../parser/ParseTreeIterator.h"
#include "../parser/Proof_Parser.h"

class ParamReader {
public:
    // Read parameters from parse tree and add them to the parameter manager
    // The iterator should be positioned after the theorem name (IDENT)
    // Returns the number of parameters read
    static int readParams(ParseTreeIterator& it, ParamManager& paramMgr);

private:
    // Read a single parameter
    // Returns the iterator positioned after the parameter
    static void readSingleParam(ParseTreeIterator& it, ParamManager& paramMgr);

    // Convert TypeExpr token to ParamType
    static ParamType tokenToParamType(int tokenId);
};
