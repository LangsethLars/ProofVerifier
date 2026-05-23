#include "PropExprReader.h"
#include <stdexcept>
#include <cstdlib>

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
    //     | _Prop_Int_Eq  OPEN_PAR <IntExpr> <IntExpr> CLOSE_PAR;

    if (getVariableId(it) != static_cast<int>(Proof_Parser::VariableId::_PropExpr_)) {
        throw std::runtime_error("Expected <PropExpr> node");
    }

    ParseTreeIterator child = it.firstChild();
    int tokenId = getTokenId(child);
    int line = child.isTerminal() ? child.getToken().lineNumber : -1;

    if (tokenId == static_cast<int>(Proof_Lexer::TokenId::IDENT)) {
        return PropExpr::makeVar(child.getLexeme(), line);
    }
    else if (tokenId == static_cast<int>(Proof_Lexer::TokenId::_Prop_True)) {
        return PropExpr::makeTrue(line);
    }
    else if (tokenId == static_cast<int>(Proof_Lexer::TokenId::_Prop_False)) {
        return PropExpr::makeFalse(line);
    }
    else if (tokenId == static_cast<int>(Proof_Lexer::TokenId::_Prop_Not)) {
        child.moveToNextSibling(); // skip _Prop_Not
        child.moveToNextSibling(); // skip OPEN_PAR
        return PropExpr::makeNot(readPropExpr(child), line);
    }
    else if (tokenId == static_cast<int>(Proof_Lexer::TokenId::_Prop_And)) {
        child.moveToNextSibling(); child.moveToNextSibling();
        PropExprPtr left = readPropExpr(child);
        child.moveToNextSibling();
        return PropExpr::makeAnd(left, readPropExpr(child), line);
    }
    else if (tokenId == static_cast<int>(Proof_Lexer::TokenId::_Prop_Or)) {
        child.moveToNextSibling(); child.moveToNextSibling();
        PropExprPtr left = readPropExpr(child);
        child.moveToNextSibling();
        return PropExpr::makeOr(left, readPropExpr(child), line);
    }
    else if (tokenId == static_cast<int>(Proof_Lexer::TokenId::_Prop_Implies)) {
        child.moveToNextSibling(); child.moveToNextSibling();
        PropExprPtr left = readPropExpr(child);
        child.moveToNextSibling();
        return PropExpr::makeImplies(left, readPropExpr(child), line);
    }
    else if (tokenId == static_cast<int>(Proof_Lexer::TokenId::_Prop_Iff)) {
        child.moveToNextSibling(); child.moveToNextSibling();
        PropExprPtr left = readPropExpr(child);
        child.moveToNextSibling();
        return PropExpr::makeIff(left, readPropExpr(child), line);
    }
    else if (tokenId == static_cast<int>(Proof_Lexer::TokenId::_Prop_Int_Eq)) {
        // _Prop_Int_Eq OPEN_PAR <IntExpr> <IntExpr> CLOSE_PAR
        child.moveToNextSibling(); child.moveToNextSibling();
        IntExprPtr left = readIntExpr(child);
        child.moveToNextSibling();
        return PropExpr::makeIntEq(left, readIntExpr(child), line);
    }

    throw std::runtime_error("Unknown PropExpr production");
}

IntExprPtr PropExprReader::readIntExpr(ParseTreeIterator& it) {
    // Rule <IntExpr>
    //     = IDENT
    //     | NUMBER
    //     | _Int_Neg  OPEN_PAR <IntExpr>            CLOSE_PAR
    //     | _Int_Add  OPEN_PAR <IntExpr> <IntExpr>  CLOSE_PAR
    //     | _Int_Sub  OPEN_PAR <IntExpr> <IntExpr>  CLOSE_PAR
    //     | _Int_Mul  OPEN_PAR <IntExpr> <IntExpr>  CLOSE_PAR;

    if (getVariableId(it) != static_cast<int>(Proof_Parser::VariableId::_IntExpr_)) {
        throw std::runtime_error("Expected <IntExpr> node");
    }

    ParseTreeIterator child = it.firstChild();
    int tokenId = getTokenId(child);
    int line = child.isTerminal() ? child.getToken().lineNumber : -1;

    if (tokenId == static_cast<int>(Proof_Lexer::TokenId::IDENT)) {
        return IntExpr::makeVar(child.getLexeme(), line);
    }
    else if (tokenId == static_cast<int>(Proof_Lexer::TokenId::NUMBER)) {
        int64_t value = std::stoll(child.getLexeme());
        return IntExpr::makeLit(value, line);
    }
    else if (tokenId == static_cast<int>(Proof_Lexer::TokenId::_Int_Neg)) {
        child.moveToNextSibling(); // skip _Int_Neg
        child.moveToNextSibling(); // skip OPEN_PAR
        return IntExpr::makeNeg(readIntExpr(child), line);
    }
    else if (tokenId == static_cast<int>(Proof_Lexer::TokenId::_Int_Add)) {
        child.moveToNextSibling(); child.moveToNextSibling();
        IntExprPtr left = readIntExpr(child);
        child.moveToNextSibling();
        return IntExpr::makeAdd(left, readIntExpr(child), line);
    }
    else if (tokenId == static_cast<int>(Proof_Lexer::TokenId::_Int_Sub)) {
        child.moveToNextSibling(); child.moveToNextSibling();
        IntExprPtr left = readIntExpr(child);
        child.moveToNextSibling();
        return IntExpr::makeSub(left, readIntExpr(child), line);
    }
    else if (tokenId == static_cast<int>(Proof_Lexer::TokenId::_Int_Mul)) {
        child.moveToNextSibling(); child.moveToNextSibling();
        IntExprPtr left = readIntExpr(child);
        child.moveToNextSibling();
        return IntExpr::makeMul(left, readIntExpr(child), line);
    }

    throw std::runtime_error("Unknown IntExpr production");
}

