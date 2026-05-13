#include "ASTPrinter.h"

void ASTPrinter::print(const AST& ast) {
    m_out << "AST:\n";
    incIndent();

    for (const auto& decl : ast.declarations) {
        if (decl) {
            printDecl(*decl);
        }
    }

    decIndent();
}

void ASTPrinter::printDecl(const DeclNode& decl) {
    indent();
    m_out << "Declaration (line " << decl.lineNumber << "):\n";
    incIndent();

    if (decl.expr) {
        printExpr(*decl.expr);
    }

    decIndent();
}

void ASTPrinter::printExpr(const ExprNode& expr) {
    switch (expr.nodeType) {
        case ASTNodeType::Ident:
            printIdent(static_cast<const IdentNode&>(expr));
            break;
        case ASTNodeType::FunExpr:
            printFunExpr(static_cast<const FunExprNode&>(expr));
            break;
        case ASTNodeType::PiExpr:
            printPiExpr(static_cast<const PiExprNode&>(expr));
            break;
        case ASTNodeType::AppExpr:
            printAppExpr(static_cast<const AppExprNode&>(expr));
            break;
        case ASTNodeType::TypeExpr:
            printType(static_cast<const TypeNode&>(expr));
            break;
        default:
            indent();
            m_out << "Unknown expression\n";
            break;
    }
}

void ASTPrinter::printIdent(const IdentNode& node) {
    indent();
    m_out << "Identifier: " << node.name << " (line " << node.lineNumber << ")\n";
}

void ASTPrinter::printFunExpr(const FunExprNode& node) {
    indent();
    m_out << "Function (line " << node.lineNumber << "):\n";
    incIndent();

    indent();
    m_out << "Parameter: " << node.paramName << "\n";

    indent();
    m_out << "Parameter Type:\n";
    incIndent();
    if (node.paramType) {
        printExpr(*node.paramType);
    }
    decIndent();

    indent();
    m_out << "Body:\n";
    incIndent();
    if (node.body) {
        printExpr(*node.body);
    }
    decIndent();

    decIndent();
}

void ASTPrinter::printPiExpr(const PiExprNode& node) {
    indent();
    m_out << "Pi Type (line " << node.lineNumber << "):\n";
    incIndent();

    indent();
    m_out << "Parameter: " << node.paramName << "\n";

    indent();
    m_out << "Parameter Type:\n";
    incIndent();
    if (node.paramType) {
        printExpr(*node.paramType);
    }
    decIndent();

    indent();
    m_out << "Result Type:\n";
    incIndent();
    if (node.resultType) {
        printExpr(*node.resultType);
    }
    decIndent();

    decIndent();
}

void ASTPrinter::printAppExpr(const AppExprNode& node) {
    indent();
    m_out << "Application (line " << node.lineNumber << "):\n";
    incIndent();

    indent();
    m_out << "Function:\n";
    incIndent();
    if (node.function) {
        printExpr(*node.function);
    }
    decIndent();

    indent();
    m_out << "Arguments (" << node.arguments.size() << "):\n";
    incIndent();
    for (const auto& arg : node.arguments) {
        if (arg) {
            printExpr(*arg);
        }
    }
    decIndent();

    decIndent();
}

void ASTPrinter::printType(const TypeNode& node) {
    indent();
    m_out << "Type (line " << node.lineNumber << ")\n";
}

void ASTPrinter::indent() {
    for (int i = 0; i < m_indentLevel; ++i) {
        m_out << "  ";
    }
}
