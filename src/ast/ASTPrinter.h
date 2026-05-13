#pragma once

#include "ASTTypes.h"
#include <iostream>
#include <string>

class ASTPrinter {
public:
    ASTPrinter(std::ostream& out = std::cout) : m_out(out), m_indentLevel(0) {}

    void print(const AST& ast);
    void printDecl(const DeclNode& decl);
    void printExpr(const ExprNode& expr);

private:
    void printIdent(const IdentNode& node);
    void printFunExpr(const FunExprNode& node);
    void printPiExpr(const PiExprNode& node);
    void printAppExpr(const AppExprNode& node);
    void printType(const TypeNode& node);

    void indent();
    void incIndent() { m_indentLevel++; }
    void decIndent() { m_indentLevel--; }

    std::ostream& m_out;
    int m_indentLevel;
};
