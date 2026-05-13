#include "ASTBuilder.h"
#include <cassert>

ASTBuilder::ASTBuilder(const Proof_Parser& parser)
    : m_parser(parser),
      m_parseTree(parser.m_ParseTree),
      m_tokenSequence(parser.m_Proof_Lexer.m_TokenSequence),
      m_rawText(parser.m_Proof_Lexer.m_RawText)
{
}

std::unique_ptr<AST> ASTBuilder::buildAST() {
    if (m_parseTree.empty()) {
        printf("Parse tree is empty\n");
        return nullptr;
    }

    auto ast = std::make_unique<AST>();

    // Root node is at index 0, which represents <>
    // Due to skip nodes, <Decl> nodes are direct children of <>
    int child = getFirstChild(0);

    while (child >= 0) {
        int symbolId = getSymbolId(child);

        // symbolId is stored as -(varId+1) for variables
        // <Decl> has varId = 3, so symbolId = -4
        int declSymbolId = 0; // -(static_cast<int>(Proof_Parser::VariableId::_Decl_) + 1);

        if (symbolId == declSymbolId) {
            auto decl = buildDecl(child);
            if (decl) {
                ast->declarations.push_back(std::move(decl));
            }
        }

        child = getNextSibling(child);
    }

    return ast;
}

DeclNodePtr ASTBuilder::buildDecl(int nodeIndex) {
    // <Decl> -> <Expr>
    int exprNode = getFirstChild(nodeIndex);
    if (exprNode < 0) {
        return nullptr;
    }

    auto expr = buildExpr(exprNode);
    if (!expr) {
        return nullptr;
    }

    int line = expr->lineNumber;
    return std::make_unique<DeclNode>(std::move(expr), line);
}

ExprNodePtr ASTBuilder::buildExpr(int nodeIndex) {
    // <Expr> -> <FunExpr> | <PiExpr> | <AppExpr>
    int child = getFirstChild(nodeIndex);
    if (child < 0) {
        return nullptr;
    }

    int symbolId = getSymbolId(child);

    // Convert VariableId enum to parse tree symbolId format: -(varId+1)
    int funExprSymbolId = 0; // -(static_cast<int>(Proof_Parser::VariableId::_FunExpr_) + 1);
    int piExprSymbolId = 0; // -(static_cast<int>(Proof_Parser::VariableId::_PiExpr_) + 1);
    int appExprSymbolId = 0; // -(static_cast<int>(Proof_Parser::VariableId::_AppExpr_) + 1);

    if (symbolId == funExprSymbolId) {
        return buildFunExpr(child);
    } else if (symbolId == piExprSymbolId) {
        return buildPiExpr(child);
    } else if (symbolId == appExprSymbolId) {
        return buildAppExpr(child);
    }

    return nullptr;
}

ExprNodePtr ASTBuilder::buildFunExpr(int nodeIndex) {
    // <FunExpr> -> _fun IDENT COLON <Expr> FAT_ARROW <Expr>

    int child = getFirstChild(nodeIndex);
    if (child < 0) return nullptr;

    // Skip _fun token
    child = getNextSibling(child);
    if (child < 0) return nullptr;

    // Get IDENT - child is a parse tree node, extract token sequence number
    int tokenSeqNo = getSymbolId(child); // For token nodes, this is the token sequence number
    std::string paramName = getTokenText(tokenSeqNo);
    int line = getTokenLine(tokenSeqNo);

    // Skip COLON
    child = getNextSibling(child);
    if (child < 0) return nullptr;
    child = getNextSibling(child);
    if (child < 0) return nullptr;

    // Get param type <Expr>
    auto paramType = buildExpr(child);
    if (!paramType) return nullptr;

    // Skip FAT_ARROW
    child = getNextSibling(child);
    if (child < 0) return nullptr;
    child = getNextSibling(child);
    if (child < 0) return nullptr;

    // Get body <Expr>
    auto body = buildExpr(child);
    if (!body) return nullptr;

    return std::make_unique<FunExprNode>(paramName, std::move(paramType), std::move(body), line);
}

ExprNodePtr ASTBuilder::buildPiExpr(int nodeIndex) {
    // <PiExpr> -> _Pi IDENT COLON <Expr> COMMA <Expr>

    int child = getFirstChild(nodeIndex);
    if (child < 0) return nullptr;

    // Skip _Pi token
    child = getNextSibling(child);
    if (child < 0) return nullptr;

    // Get IDENT - child is a parse tree node, extract token sequence number
    int tokenSeqNo = getSymbolId(child); // For token nodes, this is the token sequence number
    std::string paramName = getTokenText(tokenSeqNo);
    int line = getTokenLine(tokenSeqNo);

    // Skip COLON
    child = getNextSibling(child);
    if (child < 0) return nullptr;
    child = getNextSibling(child);
    if (child < 0) return nullptr;

    // Get param type <Expr>
    auto paramType = buildExpr(child);
    if (!paramType) return nullptr;

    // Skip COMMA
    child = getNextSibling(child);
    if (child < 0) return nullptr;
    child = getNextSibling(child);
    if (child < 0) return nullptr;

    // Get result type <Expr>
    auto resultType = buildExpr(child);
    if (!resultType) return nullptr;

    return std::make_unique<PiExprNode>(paramName, std::move(paramType), std::move(resultType), line);
}

ExprNodePtr ASTBuilder::buildAppExpr(int nodeIndex) {
    // <AppExpr> -> <Atom> <AppLoop>
    // But due to skip nodes, children are flattened
    // Structure: direct token children (IDENT, _Type, OPEN_PAR, etc.) or <Expr>

    int child = getFirstChild(nodeIndex);
    if (child < 0) return nullptr;

    int exprSymbolId = 0; // -(static_cast<int>(Proof_Parser::VariableId::_Expr_) + 1);

    // Collect all atoms/expressions at this level
    std::vector<ExprNodePtr> atoms;

    while (child >= 0) {
        int symbolId = getSymbolId(child);

        // Negative symbolId means it's a variable (non-terminal)
        if (symbolId < 0) {
            // It's a variable, should be <Expr>
            if (symbolId == exprSymbolId) {
                auto expr = buildExpr(child);
                if (expr) {
                    atoms.push_back(std::move(expr));
                }
            }
        } else {
            // Positive symbolId means it's a token sequence number
            int tokenSeqNo = symbolId;
            if (tokenSeqNo >= 0 && tokenSeqNo < static_cast<int>(m_tokenSequence.size())) {
                int tokenClassId = m_tokenSequence[tokenSeqNo].tokenClassId;

                if (tokenClassId == static_cast<int>(Proof_Lexer::TokenId::IDENT)) {
                    // Simple identifier
                    std::string name = getTokenText(tokenSeqNo);
                    int line = getTokenLine(tokenSeqNo);
                    atoms.push_back(std::make_unique<IdentNode>(name, line));
                } 
                else if (tokenClassId == static_cast<int>(Proof_Lexer::TokenId::_Type)) {
                    // Type keyword
                    int line = getTokenLine(tokenSeqNo);
                    atoms.push_back(std::make_unique<TypeNode>(line));
                }
                // Skip OPEN_PAR and CLOSE_PAR - the <Expr> between them will be handled above
            }
        }

        child = getNextSibling(child);
    }

    if (atoms.empty()) {
        return nullptr;
    }

    // If only one atom, return it directly (not an application)
    if (atoms.size() == 1) {
        return std::move(atoms[0]);
    }

    // Multiple atoms: first is function, rest are arguments
    auto function = std::move(atoms[0]);
    std::vector<ExprNodePtr> args;
    for (size_t i = 1; i < atoms.size(); ++i) {
        args.push_back(std::move(atoms[i]));
    }

    int line = function->lineNumber;
    return std::make_unique<AppExprNode>(std::move(function), std::move(args), line);
}

// Helper functions

std::string ASTBuilder::getTokenText(int tokenIndex) const {
    if (tokenIndex < 0 || tokenIndex >= static_cast<int>(m_tokenSequence.size())) {
        return "";
    }

    const Token& token = m_tokenSequence[tokenIndex];
    std::string text;

    for (size_t i = 0; i < token.lexemeLength; ++i) {
        text += static_cast<char>(m_rawText[token.lexemeStart + i]);
    }

    return text;
}

int ASTBuilder::getTokenLine(int tokenIndex) const {
    if (tokenIndex < 0 || tokenIndex >= static_cast<int>(m_tokenSequence.size())) {
        return -1;
    }

    return m_tokenSequence[tokenIndex].lineNumber;
}

int ASTBuilder::getFirstChild(int nodeIndex) const {
    if (nodeIndex < 0 || nodeIndex >= static_cast<int>(m_parseTree.size())) {
        return -1;
    }

    return m_parseTree[nodeIndex].firstChild;
}

int ASTBuilder::getNextSibling(int nodeIndex) const {
    if (nodeIndex < 0 || nodeIndex >= static_cast<int>(m_parseTree.size())) {
        return -1;
    }

    return m_parseTree[nodeIndex].next;
}

int ASTBuilder::getSymbolId(int nodeIndex) const {
    if (nodeIndex < 0 || nodeIndex >= static_cast<int>(m_parseTree.size())) {
        return 0;
    }

    return m_parseTree[nodeIndex].symbolIdOrTokenSequenceNo;
}

bool ASTBuilder::isVariable(int nodeIndex) const {
    return getSymbolId(nodeIndex) < 0;
}

bool ASTBuilder::isToken(int nodeIndex) const {
    return getSymbolId(nodeIndex) >= 0;
}
