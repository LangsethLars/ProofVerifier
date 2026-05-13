#include "Verifier.h"
#include "PropExprReader.h"
#include "ParamReader.h"
#include <iostream>
#include <stdexcept>

bool Verifier::verify(Proof_Parser& parser) {
    ParseTreeIterator it(parser.m_ParseTree, parser.m_TokenSequence, parser.m_RawText);

    try {
        // Rule <> = <Start> END_OF_FILE;
        if (it.getVariableIdNoThrow() != int(Proof_Parser::VariableId::__)) {
            throw std::runtime_error("Root node of parse tree is not <>.");
        }
        it.moveToFirstChild();

        // Skip <Start> = <Item>;
        // Skip <Item> = <Theorem> <Item> | ;
        // Siblings are either <Theorem> or END_OF_FILE (last sibling).
        for (; it.hasNextSibling(); it.moveToNextSibling()) {
            if (it.getVariableIdNoThrow() != int(Proof_Parser::VariableId::_Theorem_)) {
                throw std::runtime_error("Expected <Theorem>.");
            }
            verifyTheorem(it);
        }

        if (it.getTokenIdNoThrow() != int(Proof_Lexer::TokenId::END_OF_FILE)) {
            throw std::runtime_error("Expected END_OF_FILE at end of parser tree.");
        }
    }
    catch (const std::exception& ex) {
        it.debug(ex.what());
        return false;
    }

    return true;
}

void Verifier::verifyTheorem(ParseTreeIterator& itNode) {
    // Rule <Theorem> = _Theorem IDENT <ParamList> COLON <PropExpr> <ProofBlock>;
    ParseTreeIterator it = itNode.firstChild();

    if (it.getTokenIdNoThrow() != int(Proof_Lexer::TokenId::_Theorem)) {
        throw std::runtime_error("Expected _Theorem at start of theorem declaration.");
    }
    it.moveToNextSibling();

    if (it.getTokenIdNoThrow() != int(Proof_Lexer::TokenId::IDENT)) {
        throw std::runtime_error("Expected IDENT for theorem name.");
    }
    std::string theoremName = it.getLexeme();
    std::cout << std::endl << "Verifying theorem \"" << theoremName << "\" on line " << it.getToken().lineNumber << std::endl;
    it.moveToNextSibling();

    m_paramMgr.pushScope();

    int paramCount = ParamReader::readParams(it, m_paramMgr);
    if (paramCount > 0) {
        std::cout << "  Parameters:" << std::endl;
        for (const auto& param : m_paramMgr.getCurrentScopeParams()) {
            std::cout << "    " << param->name << " : " << paramTypeToString(param->type)
                      << " (line " << param->lineNumber << ")" << std::endl;
        }
    }

    // it is now positioned at COLON
    if (it.getTokenIdNoThrow() != int(Proof_Lexer::TokenId::COLON)) {
        throw std::runtime_error("Expected COLON after theorem name (and optional parameters).");
    }
    it.moveToNextSibling();

    if (it.getVariableIdNoThrow() != int(Proof_Parser::VariableId::_PropExpr_)) {
        throw std::runtime_error("Expected <PropExpr> for theorem statement.");
    }
    PropExprPtr propExpr = PropExprReader::readPropExpr(it);
    std::cout << "  Statement: " << propExpr->toString() << std::endl;
    it.moveToNextSibling();

    // Rule <ProofBlock> = _Proof <ProofStepList> _End;
    if (it.getVariableIdNoThrow() != int(Proof_Parser::VariableId::_ProofBlock_)) {
        throw std::runtime_error("Expected <ProofBlock> for theorem proof.");
    }

    ParseTreeIterator itProofBlock = it.firstChild();
    if (itProofBlock.getTokenIdNoThrow() != int(Proof_Lexer::TokenId::_Proof)) {
        throw std::runtime_error("Expected _Proof at start of proof block.");
    }
    itProofBlock.moveToNextSibling();

    verifyProofStepList(itProofBlock, propExpr);

    if (itProofBlock.getTokenIdNoThrow() != int(Proof_Lexer::TokenId::_End)) {
        throw std::runtime_error("Expected _End at end of proof block.");
    }
    if (itProofBlock.hasNextSibling()) {
        throw std::runtime_error("Expected no siblings after _End in proof block.");
    }

    m_paramMgr.popScope();
}

void Verifier::verifyProofStepList(ParseTreeIterator& it, const PropExprPtr& propExpr) {
    // Skip <ProofStepList> = <ProofStep> <ProofStepList> | ;
    // Skip <ProofStep> = <AssumeStep> | <HaveStep> | <ExactStep> | <ShowStep>
    //                  | <CasesStep>  | <ReflStep>  | <TrivialStep>;
    // Stops when it meets _End or _Case (not a <ProofStep> start).
    while (it.getTokenIdNoThrow() != int(Proof_Lexer::TokenId::_End) &&
           it.getTokenIdNoThrow() != int(Proof_Lexer::TokenId::_Case)) {
        switch (it.getVariableIdNoThrow()) {
            case int(Proof_Parser::VariableId::_AssumeStep_):
                verifyAssumeStep(it);
                break;
            case int(Proof_Parser::VariableId::_HaveStep_):
                verifyHaveStep(it);
                break;
            case int(Proof_Parser::VariableId::_ExactStep_):
                verifyExactStep(it);
                break;
            case int(Proof_Parser::VariableId::_ShowStep_):
                verifyShowStep(it);
                break;
            case int(Proof_Parser::VariableId::_CasesStep_):
                verifyCasesStep(it, propExpr);
                break;
            case int(Proof_Parser::VariableId::_ReflStep_):
                verifyReflStep(it, propExpr);
                break;
            case int(Proof_Parser::VariableId::_TrivialStep_):
                verifyTrivialStep(it, propExpr);
                break;
            default:
                throw std::runtime_error("Unknown proof step type.");
        }
        it.moveToNextSibling();
    }
}

void Verifier::verifyAssumeStep(ParseTreeIterator& it) {
    // Rule <AssumeStep> = _Assume IDENT COLON <PropExpr>;
    std::cout << "    Assume step" << std::endl;
}

void Verifier::verifyHaveStep(ParseTreeIterator& it) {
    // Rule <HaveStep> = _Have IDENT COLON <PropExpr> COLONEQUAL <ProofExpr>;
    std::cout << "    Have step" << std::endl;
}

void Verifier::verifyExactStep(ParseTreeIterator& it) {
    // Rule <ExactStep> = _Exact <ProofExpr>;
    std::cout << "    Exact step" << std::endl;
}

void Verifier::verifyShowStep(ParseTreeIterator& it) {
    // Rule <ShowStep> = _Show <PropExpr>;
    std::cout << "    Show step" << std::endl;
}

void Verifier::verifyCasesStep(ParseTreeIterator& it, const PropExprPtr& propExpr) {
    // Rule <CasesStep>    = _Cases IDENT <BoolCaseList> _End;
    // Rule <BoolCaseList> = <BoolCase> <BoolCase>;
    // Rule <BoolCase>     = _Case <BoolValue> FAT_ARROW <ProofStepList>;
    // Rule <BoolValue>    = _Bool_False | _Bool_True;
    std::cout << "    Cases step" << std::endl;

    ParseTreeIterator itCases = it.firstChild();

    if (itCases.getTokenIdNoThrow() != int(Proof_Lexer::TokenId::_Cases)) {
        throw std::runtime_error("Expected _Cases at start of cases step.");
    }
    itCases.moveToNextSibling();

    if (itCases.getTokenIdNoThrow() != int(Proof_Lexer::TokenId::IDENT)) {
        throw std::runtime_error("Expected IDENT (variable name) after _Cases.");
    }
    std::string casesVar = itCases.getLexeme();
    std::cout << "      Cases on variable: " << casesVar << std::endl;
    itCases.moveToNextSibling();

    // <BoolCaseList> = <BoolCase> <BoolCase>
    if (itCases.getVariableIdNoThrow() != int(Proof_Parser::VariableId::_BoolCaseList_)) {
        throw std::runtime_error("Expected <BoolCaseList> in cases step.");
    }

    ParseTreeIterator itCaseList = itCases.firstChild();

    // Process each <BoolCase>
    for (int caseIndex = 0; caseIndex < 2; ++caseIndex) {
        if (itCaseList.getVariableIdNoThrow() != int(Proof_Parser::VariableId::_BoolCase_)) {
            throw std::runtime_error("Expected <BoolCase> in case list.");
        }

        ParseTreeIterator itCase = itCaseList.firstChild();

        if (itCase.getTokenIdNoThrow() != int(Proof_Lexer::TokenId::_Case)) {
            throw std::runtime_error("Expected _Case at start of bool case.");
        }
        itCase.moveToNextSibling();

        // <BoolValue> = _Bool_False | _Bool_True
        if (itCase.getVariableIdNoThrow() != int(Proof_Parser::VariableId::_BoolValue_)) {
            throw std::runtime_error("Expected <BoolValue> after _Case.");
        }
        ParseTreeIterator itBoolValue = itCase.firstChild();
        int boolValueToken = itBoolValue.getTokenIdNoThrow();
        std::string boolValueStr = (boolValueToken == int(Proof_Lexer::TokenId::_Bool_True)) ? "Bool.True" : "Bool.False";
        std::cout << "      Case " << boolValueStr << ":" << std::endl;
        itCase.moveToNextSibling();

        if (itCase.getTokenIdNoThrow() != int(Proof_Lexer::TokenId::FAT_ARROW)) {
            throw std::runtime_error("Expected FAT_ARROW (=>) after <BoolValue> in case.");
        }
        itCase.moveToNextSibling();

        // Recursively verify the nested <ProofStepList>
        verifyProofStepList(itCase, propExpr);

        if (caseIndex == 0) {
            itCaseList.moveToNextSibling();
        }
    }

    itCases.moveToNextSibling();

    if (itCases.getTokenIdNoThrow() != int(Proof_Lexer::TokenId::_End)) {
        throw std::runtime_error("Expected _End at end of cases step.");
    }
}

void Verifier::verifyReflStep(ParseTreeIterator& it, const PropExprPtr& propExpr) {
    // Rule <ReflStep> = _Refl;
    if (propExpr->type == PropExprType::BoolEq) {
        BoolExprPtr boolLeft = propExpr->boolLeft, boolRight = propExpr->boolRight;
        BoolExprEval evalLeft = boolLeft->evaluate();
        BoolExprEval evalRight = boolRight->evaluate();
        if (evalLeft == BoolExprEval::Error || evalRight == BoolExprEval::Error) {
            std::cout << "    Refl step invalid (error evaluating boolean expressions)" << std::endl;
        } else if (evalLeft == evalRight) {
            std::cout << "    Refl step valid" << std::endl;
        } else {
            std::cout << "    Refl step invalid (boolean expressions do not evaluate to the same value)" << std::endl;
        }
    } else {
        std::cout << "    Refl step invalid (statement is not BoolEq)" << std::endl;
    }
}

void Verifier::verifyTrivialStep(ParseTreeIterator& it, const PropExprPtr& propExpr) {
    // Rule <TrivialStep> = _Trivial;
    if (propExpr->type == PropExprType::True) {
        std::cout << "    Trivial step valid" << std::endl;
    } else {
        std::cout << "    Trivial step invalid (statement is not True)" << std::endl;
    }
}
