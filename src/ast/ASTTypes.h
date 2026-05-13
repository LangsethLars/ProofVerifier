#pragma once

#include <string>
#include <vector>
#include <memory>

// Forward declarations
struct ASTNode;
struct DeclNode;
struct ExprNode;
struct IdentNode;
struct FunExprNode;
struct PiExprNode;
struct AppExprNode;
struct TypeNode;

using ASTNodePtr = std::unique_ptr<ASTNode>;
using DeclNodePtr = std::unique_ptr<DeclNode>;
using ExprNodePtr = std::unique_ptr<ExprNode>;

enum class ASTNodeType {
    // Declarations
    Decl,

    // Expressions
    Ident,
    FunExpr,
    PiExpr,
    AppExpr,
    TypeExpr
};

// Base AST Node
struct ASTNode {
    ASTNodeType nodeType;
    int lineNumber;  // For error reporting

    ASTNode(ASTNodeType type, int line = -1) 
        : nodeType(type), lineNumber(line) {}

    virtual ~ASTNode() = default;
};

// Declaration node (top-level expression)
struct DeclNode : public ASTNode {
    ExprNodePtr expr;

    DeclNode(ExprNodePtr e, int line = -1)
        : ASTNode(ASTNodeType::Decl, line), expr(std::move(e)) {}
};

// Base expression node
struct ExprNode : public ASTNode {
    ExprNode(ASTNodeType type, int line = -1)
        : ASTNode(type, line) {}
};

// Identifier
struct IdentNode : public ExprNode {
    std::string name;

    IdentNode(const std::string& n, int line = -1)
        : ExprNode(ASTNodeType::Ident, line), name(n) {}
};

// Function expression: fun x : A => B
struct FunExprNode : public ExprNode {
    std::string paramName;
    ExprNodePtr paramType;
    ExprNodePtr body;

    FunExprNode(const std::string& param, ExprNodePtr type, ExprNodePtr b, int line = -1)
        : ExprNode(ASTNodeType::FunExpr, line),
          paramName(param),
          paramType(std::move(type)),
          body(std::move(b)) {}
};

// Pi expression (dependent function type): Pi x : A, B
struct PiExprNode : public ExprNode {
    std::string paramName;
    ExprNodePtr paramType;
    ExprNodePtr resultType;

    PiExprNode(const std::string& param, ExprNodePtr type, ExprNodePtr result, int line = -1)
        : ExprNode(ASTNodeType::PiExpr, line),
          paramName(param),
          paramType(std::move(type)),
          resultType(std::move(result)) {}
};

// Application expression: f a b c
struct AppExprNode : public ExprNode {
    ExprNodePtr function;
    std::vector<ExprNodePtr> arguments;

    AppExprNode(ExprNodePtr func, std::vector<ExprNodePtr> args, int line = -1)
        : ExprNode(ASTNodeType::AppExpr, line),
          function(std::move(func)),
          arguments(std::move(args)) {}
};

// Type keyword
struct TypeNode : public ExprNode {
    TypeNode(int line = -1)
        : ExprNode(ASTNodeType::TypeExpr, line) {}
};

// Root of the AST - contains all declarations
struct AST {
    std::vector<DeclNodePtr> declarations;

    AST() = default;
};
