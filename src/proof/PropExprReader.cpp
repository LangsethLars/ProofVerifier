#include "PropExprReader.h"
#include <stdexcept>

int PropExprReader::getTokenId(const ParseTreeIterator& it) {
    return it.getTokenIdNoThrow();
}

int PropExprReader::getVariableId(const ParseTreeIterator& it) {
    return it.getVariableIdNoThrow();
}

PropExprPtr PropExprReader::readPropExpr(ParseTreeIterator& it) {
    // Rule <PropExpr>
    //     = IDENT
    //     | _Prop_True
    //     | _Prop_False
    //     | _Prop_Not     OPEN_PAR <PropExpr> CLOSE_PAR
    //     | _Prop_And     OPEN_PAR <PropExpr> <PropExpr> CLOSE_PAR
    //     | _Prop_Or      OPEN_PAR <PropExpr> <PropExpr> CLOSE_PAR
    //     | _Prop_Implies OPEN_PAR <PropExpr> <PropExpr> CLOSE_PAR
    //     | _Prop_Iff     OPEN_PAR <PropExpr> <PropExpr> CLOSE_PAR
    //     | _Bool_Eq      OPEN_PAR <BoolExpr> <BoolExpr> CLOSE_PAR;

    if (getVariableId(it) != static_cast<int>(Proof_Parser::VariableId::_PropExpr_)) {
        throw std::runtime_error("Expected <PropExpr> node");
    }

    // Move to first child to see which production we have
    ParseTreeIterator child = it.firstChild();
    int tokenId = getTokenId(child);
    int line = child.isTerminal() ? child.getToken().lineNumber : -1;

    if (tokenId == static_cast<int>(Proof_Lexer::TokenId::IDENT)) {
        // Prop variable
        std::string name = child.getLexeme();
        return PropExpr::makeVar(name, line);
    }
    else if (tokenId == static_cast<int>(Proof_Lexer::TokenId::_Prop_True)) {
        return PropExpr::makeTrue(line);
    }
    else if (tokenId == static_cast<int>(Proof_Lexer::TokenId::_Prop_False)) {
        return PropExpr::makeFalse(line);
    }
    else if (tokenId == static_cast<int>(Proof_Lexer::TokenId::_Prop_Not)) {
        // _Prop_Not OPEN_PAR <PropExpr> CLOSE_PAR
        child.moveToNextSibling(); // Skip _Prop_Not
        child.moveToNextSibling(); // Skip OPEN_PAR

        PropExprPtr arg = readPropExpr(child);
        return PropExpr::makeNot(arg, line);
    }
    else if (tokenId == static_cast<int>(Proof_Lexer::TokenId::_Prop_And)) {
        // _Prop_And OPEN_PAR <PropExpr> <PropExpr> CLOSE_PAR
        child.moveToNextSibling(); // Skip _Prop_And
        child.moveToNextSibling(); // Skip OPEN_PAR

        PropExprPtr left = readPropExpr(child);
        child.moveToNextSibling();
        PropExprPtr right = readPropExpr(child);
        return PropExpr::makeAnd(left, right, line);
    }
    else if (tokenId == static_cast<int>(Proof_Lexer::TokenId::_Prop_Or)) {
        // _Prop_Or OPEN_PAR <PropExpr> <PropExpr> CLOSE_PAR
        child.moveToNextSibling(); // Skip _Prop_Or
        child.moveToNextSibling(); // Skip OPEN_PAR

        PropExprPtr left = readPropExpr(child);
        child.moveToNextSibling();
        PropExprPtr right = readPropExpr(child);
        return PropExpr::makeOr(left, right, line);
    }
    else if (tokenId == static_cast<int>(Proof_Lexer::TokenId::_Prop_Implies)) {
        // _Prop_Implies OPEN_PAR <PropExpr> <PropExpr> CLOSE_PAR
        child.moveToNextSibling(); // Skip _Prop_Implies
        child.moveToNextSibling(); // Skip OPEN_PAR

        PropExprPtr left = readPropExpr(child);
        child.moveToNextSibling();
        PropExprPtr right = readPropExpr(child);
        return PropExpr::makeImplies(left, right, line);
    }
    else if (tokenId == static_cast<int>(Proof_Lexer::TokenId::_Prop_Iff)) {
        // _Prop_Iff OPEN_PAR <PropExpr> <PropExpr> CLOSE_PAR
        child.moveToNextSibling(); // Skip _Prop_Iff
        child.moveToNextSibling(); // Skip OPEN_PAR

        PropExprPtr left = readPropExpr(child);
        child.moveToNextSibling();
        PropExprPtr right = readPropExpr(child);
        return PropExpr::makeIff(left, right, line);
    }
    else if (tokenId == static_cast<int>(Proof_Lexer::TokenId::_Bool_Eq)) {
        // _Bool_Eq OPEN_PAR <BoolExpr> <BoolExpr> CLOSE_PAR
        child.moveToNextSibling(); // Skip _Bool_Eq
        child.moveToNextSibling(); // Skip OPEN_PAR

        BoolExprPtr left = readBoolExpr(child);
        child.moveToNextSibling();
        BoolExprPtr right = readBoolExpr(child);
        return PropExpr::makeBoolEq(left, right, line);
    }

    throw std::runtime_error("Unknown PropExpr production");
}

BoolExprPtr PropExprReader::readBoolExpr(ParseTreeIterator& it) {
    // Rule <BoolExpr>
    //     = IDENT
    //     | _Bool_True
    //     | _Bool_False
    //     | _Bool_Not   OPEN_PAR <BoolExpr> CLOSE_PAR
    //     | _Bool_And   OPEN_PAR <BoolExpr> <BoolExpr> CLOSE_PAR
    //     | _Bool_Or    OPEN_PAR <BoolExpr> <BoolExpr> CLOSE_PAR;

    if (getVariableId(it) != static_cast<int>(Proof_Parser::VariableId::_BoolExpr_)) {
        throw std::runtime_error("Expected <BoolExpr> node");
    }

    // Move to first child to see which production we have
    ParseTreeIterator child = it.firstChild();
    int tokenId = getTokenId(child);
    int line = child.isTerminal() ? child.getToken().lineNumber : -1;

    if (tokenId == static_cast<int>(Proof_Lexer::TokenId::IDENT)) {
        // Bool variable
        std::string name = child.getLexeme();
        return BoolExpr::makeVar(name, line);
    }
    else if (tokenId == static_cast<int>(Proof_Lexer::TokenId::_Bool_True)) {
        return BoolExpr::makeTrue(line);
    }
    else if (tokenId == static_cast<int>(Proof_Lexer::TokenId::_Bool_False)) {
        return BoolExpr::makeFalse(line);
    }
    else if (tokenId == static_cast<int>(Proof_Lexer::TokenId::_Bool_Not)) {
        // _Bool_Not OPEN_PAR <BoolExpr> CLOSE_PAR
        child.moveToNextSibling(); // Skip _Bool_Not
        child.moveToNextSibling(); // Skip OPEN_PAR

        BoolExprPtr arg = readBoolExpr(child);
        return BoolExpr::makeNot(arg, line);
    }
    else if (tokenId == static_cast<int>(Proof_Lexer::TokenId::_Bool_And)) {
        // _Bool_And OPEN_PAR <BoolExpr> <BoolExpr> CLOSE_PAR
        child.moveToNextSibling(); // Skip _Bool_And
        child.moveToNextSibling(); // Skip OPEN_PAR

        BoolExprPtr left = readBoolExpr(child);
        child.moveToNextSibling();
        BoolExprPtr right = readBoolExpr(child);
        return BoolExpr::makeAnd(left, right, line);
    }
    else if (tokenId == static_cast<int>(Proof_Lexer::TokenId::_Bool_Or)) {
        // _Bool_Or OPEN_PAR <BoolExpr> <BoolExpr> CLOSE_PAR
        child.moveToNextSibling(); // Skip _Bool_Or
        child.moveToNextSibling(); // Skip OPEN_PAR

        BoolExprPtr left = readBoolExpr(child);
        child.moveToNextSibling();
        BoolExprPtr right = readBoolExpr(child);
        return BoolExpr::makeOr(left, right, line);
    }

    throw std::runtime_error("Unknown BoolExpr production");
}
