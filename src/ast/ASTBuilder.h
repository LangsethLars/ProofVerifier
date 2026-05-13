#pragma once

#include "ASTTypes.h"
#include "../parser/ParserTypes.h"
#include "../parser/Proof_Parser.h"

#include <memory>

class ASTBuilder {
public:
    ASTBuilder(const Proof_Parser& parser);

    // Build the AST from the parse tree
    std::unique_ptr<AST> buildAST();

private:
    const Proof_Parser& m_parser;
    const ParseTree& m_parseTree;
    const TokenSequence& m_tokenSequence;
    const RawText& m_rawText;

    // Helper functions to build different node types
    DeclNodePtr buildDecl(int nodeIndex);
    ExprNodePtr buildExpr(int nodeIndex);
    ExprNodePtr buildFunExpr(int nodeIndex);
    ExprNodePtr buildPiExpr(int nodeIndex);
    ExprNodePtr buildAppExpr(int nodeIndex);

    // Helper to get token text
    std::string getTokenText(int tokenIndex) const;
    int getTokenLine(int tokenIndex) const;

    // Navigate parse tree
    int getFirstChild(int nodeIndex) const;
    int getNextSibling(int nodeIndex) const;
    int getSymbolId(int nodeIndex) const;
    bool isVariable(int nodeIndex) const;
    bool isToken(int nodeIndex) const;
};
