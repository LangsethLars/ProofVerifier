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

    // Build the initial proof context: goal is the theorem statement, no assumptions yet.
    ProofContext ctx;
    ctx.goal = propExpr;

    verifyProofStepList(itProofBlock, ctx);

    if (itProofBlock.getTokenIdNoThrow() != int(Proof_Lexer::TokenId::_End)) {
        throw std::runtime_error("Expected _End at end of proof block.");
    }
    if (itProofBlock.hasNextSibling()) {
        throw std::runtime_error("Expected no siblings after _End in proof block.");
    }

    m_paramMgr.popScope();
}

void Verifier::verifyProofStepList(ParseTreeIterator& it, ProofContext& ctx) {
    // Skip <ProofStepList> = <ProofStep> <ProofStepList> | ;
    // Skip <ProofStep> = <AssumeStep> | <HaveStep> | <ExactStep> | <ShowStep>
    //                  | <CasesStep>  | <ReflStep>  | <TrivialStep>;
    // Stops when it meets _End or _Case (not a <ProofStep> start).
    while (it.getTokenIdNoThrow() != int(Proof_Lexer::TokenId::_End)) {
        switch (it.getVariableIdNoThrow()) {
            case int(Proof_Parser::VariableId::_AssumeStep_):
                verifyAssumeStep(it, ctx);
                break;
            case int(Proof_Parser::VariableId::_HaveStep_):
                verifyHaveStep(it, ctx);
                break;
            case int(Proof_Parser::VariableId::_ExactStep_):
                verifyExactStep(it, ctx);
                break;
            case int(Proof_Parser::VariableId::_ShowStep_):
                verifyShowStep(it, ctx);
                break;
            case int(Proof_Parser::VariableId::_ReflStep_):
                verifyReflStep(it, ctx);
                break;
            case int(Proof_Parser::VariableId::_TrivialStep_):
                verifyTrivialStep(it, ctx);
                break;
            default:
                throw std::runtime_error("Unknown proof step type.");
        }
        it.moveToNextSibling();
    }
}

void Verifier::verifyAssumeStep(ParseTreeIterator& it, ProofContext& ctx) {
    // Rule <AssumeStep> = _Assume IDENT COLON <PropExpr>;
    // Adds a proof assumption h : P to the context.
    ParseTreeIterator itStep = it.firstChild();

    if (itStep.getTokenIdNoThrow() != int(Proof_Lexer::TokenId::_Assume)) {
        throw std::runtime_error("Expected _Assume at start of assume step.");
    }
    itStep.moveToNextSibling();

    if (itStep.getTokenIdNoThrow() != int(Proof_Lexer::TokenId::IDENT)) {
        throw std::runtime_error("Expected IDENT for hypothesis name in assume step.");
    }
    std::string hypName = itStep.getLexeme();
    itStep.moveToNextSibling();

    if (itStep.getTokenIdNoThrow() != int(Proof_Lexer::TokenId::COLON)) {
        throw std::runtime_error("Expected COLON in assume step.");
    }
    itStep.moveToNextSibling();

    if (itStep.getVariableIdNoThrow() != int(Proof_Parser::VariableId::_PropExpr_)) {
        throw std::runtime_error("Expected <PropExpr> for hypothesis type in assume step.");
    }
    PropExprPtr hypType = PropExprReader::readPropExpr(itStep);

    ctx.addAssumption(hypName, hypType);
    std::cout << "    Assume " << hypName << " : " << hypType->toString() << std::endl;
}

void Verifier::verifyHaveStep(ParseTreeIterator& it, ProofContext& ctx) {
    // Rule <HaveStep> = _Have IDENT COLON <PropExpr> COLONEQUAL <ProofExpr>;
    std::cout << "    Have step" << std::endl;
}

void Verifier::verifyExactStep(ParseTreeIterator& it, ProofContext& ctx) {
    // Rule <ExactStep> = _Exact <ProofExpr>;
    std::cout << "    Exact step" << std::endl;
}

void Verifier::verifyShowStep(ParseTreeIterator& it, ProofContext& ctx) {
    // Rule <ShowStep> = _Show <PropExpr>;
    std::cout << "    Show step" << std::endl;
}

void Verifier::verifyReflStep(ParseTreeIterator& it, ProofContext& ctx) {
    // Rule <ReflStep> = _Refl;
    // Closes goal Prop.Int.Eq(a b) if evaluate(a) == evaluate(b).
    const PropExprPtr& goal = ctx.goal;
    if (goal->type == PropExprType::IntEq) {
        auto evalLeft  = goal->intLeft->evaluate();
        auto evalRight = goal->intRight->evaluate();
        if (!evalLeft || !evalRight) {
            std::cout << "    Refl step invalid (cannot evaluate — unsubstituted variables remain)" << std::endl;
        } else if (*evalLeft == *evalRight) {
            std::cout << "    Refl step valid" << std::endl;
        } else {
            std::cout << "    Refl step invalid (" << *evalLeft << " != " << *evalRight << ")" << std::endl;
        }
    } else {
        std::cout << "    Refl step invalid (goal is not Prop.Int.Eq)" << std::endl;
    }
}

void Verifier::verifyTrivialStep(ParseTreeIterator& it, ProofContext& ctx) {
    // Rule <TrivialStep> = _Trivial;
    // Closes goal Prop.True.
    if (ctx.goal->type == PropExprType::True) {
        std::cout << "    Trivial step valid" << std::endl;
    } else {
        std::cout << "    Trivial step invalid (goal is not Prop.True)" << std::endl;
    }
}
