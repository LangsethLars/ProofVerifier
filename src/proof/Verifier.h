#pragma once

#include "../parser/Proof_Parser.h"
#include "../parser/ParseTreeIterator.h"
#include "ParamManager.h"
#include "ProofContext.h"

class Verifier {
public:
    // Top-level entry point. Returns true if all theorems verify successfully.
    bool verify(Proof_Parser& parser);

private:
    ParamManager m_paramMgr;

    // Verify a single <Theorem> node.
    // itNode is positioned at the <Theorem> variable node.
    void verifyTheorem(ParseTreeIterator& itNode);

    // Verify a <ProofStepList>.
    // it is positioned at the first step node (or _End/_Case if the list is empty).
    // ctx holds the current goal and assumptions; steps may mutate it.
    void verifyProofStepList(ParseTreeIterator& it, ProofContext& ctx);

    // Individual <ProofStep> handlers.
    // Each receives an iterator positioned at the step's variable node and the current context.
    void verifyAssumeStep(ParseTreeIterator& it, ProofContext& ctx);
    void verifyHaveStep(ParseTreeIterator& it, ProofContext& ctx);
    void verifyExactStep(ParseTreeIterator& it, ProofContext& ctx);
    void verifyShowStep(ParseTreeIterator& it, ProofContext& ctx);
    void verifyReflStep(ParseTreeIterator& it, ProofContext& ctx);
    void verifyTrivialStep(ParseTreeIterator& it, ProofContext& ctx);
};
