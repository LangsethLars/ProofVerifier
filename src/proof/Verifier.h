#pragma once

#include "../parser/Proof_Parser.h"
#include "../parser/ParseTreeIterator.h"
#include "ParamManager.h"
#include "PropExpr.h"

class Verifier {
public:
    // Top-level entry point. Returns true if all theorems verify successfully.
    bool verify(Proof_Parser& parser);

private:
    ParamManager m_paramMgr;

    // Verify a single <Theorem> node.
    // itTheorem is positioned at the <Theorem> variable node.
    void verifyTheorem(ParseTreeIterator& itTheorem);

    // Verify a <ProofStepList> node sequence.
    // itProofBlock is positioned at the first child after _Proof (or after _Case <BoolValue> =>).
    // propExpr is the current goal proposition.
    // Stops when it meets _End or _Case.
    void verifyProofStepList(ParseTreeIterator& itProofBlock, const PropExprPtr& propExpr);

    // Individual <ProofStep> handlers.
    // Each receives an iterator positioned at the step's variable node.
    void verifyAssumeStep(ParseTreeIterator& it);
    void verifyHaveStep(ParseTreeIterator& it);
    void verifyExactStep(ParseTreeIterator& it);
    void verifyShowStep(ParseTreeIterator& it);
    void verifyCasesStep(ParseTreeIterator& it, const PropExprPtr& propExpr);
    void verifyReflStep(ParseTreeIterator& it, const PropExprPtr& propExpr);
    void verifyTrivialStep(ParseTreeIterator& it, const PropExprPtr& propExpr);
};
