#include "ParamReader.h"
#include <stdexcept>

int ParamReader::readParams(ParseTreeIterator& it, ParamManager& paramMgr) {
    // Grammar:
    // Rule <Theorem> = _Theorem IDENT <ParamList> COLON <PropExpr> <ProofBlock>;
    // Skip <ParamList> = <Param> <ParamList> | ;
    // Rule <Param> = OPEN_PAR IDENT COLON <TypeExpr> CLOSE_PAR;
    // Skip <TypeExpr> = _Bool | _Prop;

    // The iterator is positioned after IDENT
    // Next should be <ParamList>, which may be empty or contain <Param> children

    int varId = it.getVariableIdNoThrow();

    // Check if we're at <ParamList>
    if (varId != static_cast<int>(Proof_Parser::VariableId::_ParamList_)) {
        throw std::runtime_error("Expected <ParamList> node");
    }

    // If <ParamList> has no children, there are no parameters
    if (!it.hasChildren()) {
        it.moveToNextSibling(); // Move to COLON
        return 0;
    }

    int paramCount = 0;

    // Read <Param> children
    ParseTreeIterator child = it.firstChild();
    while (child.isValid()) {
        int childVarId = child.getVariableIdNoThrow();

        if (childVarId == static_cast<int>(Proof_Parser::VariableId::_Param_)) {
            readSingleParam(child, paramMgr);
            paramCount++;

            // Check if there's a next sibling
            if (child.hasNextSibling()) {
                child.moveToNextSibling();
                // The next sibling should be another <ParamList> (recursive structure)
                // But since <ParamList> is Skip, we'll just see more <Param> nodes or nothing
            } else {
                break;
            }
        } else if (childVarId == static_cast<int>(Proof_Parser::VariableId::_ParamList_)) {
            // Recursive <ParamList> - dive into it
            if (child.hasChildren()) {
                child.moveToFirstChild();
            } else {
                break;
            }
        } else {
            // Unexpected node
            throw std::runtime_error("Expected <Param> or <ParamList> while reading parameters");
        }
    }

    // Move the original iterator to COLON (next sibling of <ParamList>)
    it.moveToNextSibling();

    return paramCount;
}

void ParamReader::readSingleParam(ParseTreeIterator& it, ParamManager& paramMgr) {
    // Rule <Param> = OPEN_PAR IDENT COLON <TypeExpr> CLOSE_PAR;

    if (it.getVariableIdNoThrow() != static_cast<int>(Proof_Parser::VariableId::_Param_)) {
        throw std::runtime_error("Expected <Param> node");
    }

    ParseTreeIterator child = it.firstChild();

    // Skip OPEN_PAR
    if (child.getTokenIdNoThrow() != static_cast<int>(Proof_Lexer::TokenId::OPEN_PAR)) {
        throw std::runtime_error("Expected OPEN_PAR at start of parameter");
    }
    child.moveToNextSibling();

    // Get IDENT
    if (child.getTokenIdNoThrow() != static_cast<int>(Proof_Lexer::TokenId::IDENT)) {
        throw std::runtime_error("Expected IDENT in parameter");
    }
    std::string paramName = child.getLexeme();
    int lineNumber = child.getToken().lineNumber;
    child.moveToNextSibling();

    // Skip COLON
    if (child.getTokenIdNoThrow() != static_cast<int>(Proof_Lexer::TokenId::COLON)) {
        throw std::runtime_error("Expected COLON after parameter name");
    }
    child.moveToNextSibling();

    // Get <TypeExpr> - it's a Skip node, so we get the token directly
    // Skip <TypeExpr> = _Bool | _Prop;
    int typeTokenId = child.getTokenIdNoThrow();
    ParamType paramType = tokenToParamType(typeTokenId);

    // Add parameter to manager
    paramMgr.addParam(paramName, paramType, lineNumber);
}

ParamType ParamReader::tokenToParamType(int tokenId) {
    if (tokenId == static_cast<int>(Proof_Lexer::TokenId::_Bool)) {
        return ParamType::Bool;
    } else if (tokenId == static_cast<int>(Proof_Lexer::TokenId::_Prop)) {
        return ParamType::Prop;
    } else {
        throw std::runtime_error("Invalid parameter type token");
    }
}
