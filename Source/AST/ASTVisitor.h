//
// Created by chen_ on 2022/4/10.
//

#ifndef MYCC_SOURCE_AST_ASTVISITOR_H_
#define MYCC_SOURCE_AST_ASTVISITOR_H_
#include "utils/logging.h"

namespace Mycc::AST {
class ASTNode;
class CastExpr;
class ArithmeticExpr;
class AssignExpr;
class BitwiseExpr;
class CommaExpr;
class LogicalExpr;
class RelationalExpr;
class AccessExpr;
class ArraySubscriptExpr;
class ConditionalExpr;
class DeclRefExpr;
class LiteralExpr;
class SizeofExpr;
class UnaryExpr;
class BreakStatement;
class CompoundStmt;
class ContinueStatement;
class DeclNode;
class DoStatement;
class EmptyStatement;
class ForStatement;
class FunctionCall;
class FunctionDeclNode;
class IfStatement;
class ParamVarDecl;
class ReturnNode;
class StructDeclareNode;
class VarDecl;
class WhileStatement;

class ASTVisitor {
  public:
    virtual ~ASTVisitor() = default;
    virtual void visit(Mycc::AST::CastExpr* p_expr) = 0;
    virtual void visit(Mycc::AST::ArithmeticExpr* p_expr) = 0;
    virtual void visit(Mycc::AST::AssignExpr* p_expr) = 0;
    virtual void visit(Mycc::AST::BitwiseExpr* p_expr) = 0;
    virtual void visit(Mycc::AST::LogicalExpr* p_expr) = 0;
    virtual void visit(Mycc::AST::CommaExpr* p_expr) = 0;
    virtual void visit(Mycc::AST::RelationalExpr* p_expr) = 0;
    virtual void visit(Mycc::AST::AccessExpr* p_expr) = 0;
    virtual void visit(Mycc::AST::ArraySubscriptExpr* p_expr) = 0;
    virtual void visit(Mycc::AST::ConditionalExpr* p_expr) = 0;
    virtual void visit(Mycc::AST::DeclRefExpr* p_expr) = 0;
    virtual void visit(Mycc::AST::SizeofExpr* p_expr) = 0;
    virtual void visit(Mycc::AST::UnaryExpr* p_expr) = 0;
    virtual void visit(Mycc::AST::BreakStatement* p_expr) = 0;
    virtual void visit(Mycc::AST::CompoundStmt* p_expr) = 0;
    virtual void visit(Mycc::AST::ContinueStatement* p_expr) = 0;
    virtual void visit(Mycc::AST::DoStatement* p_expr) = 0;
    virtual void visit(Mycc::AST::EmptyStatement* p_expr) = 0;
    virtual void visit(Mycc::AST::ForStatement* p_expr) = 0;
    virtual void visit(Mycc::AST::FunctionCall* p_expr) = 0;
    virtual void visit(Mycc::AST::FunctionDeclNode* p_expr) = 0;
    virtual void visit(Mycc::AST::DeclNode* p_expr) = 0;
    virtual void visit(Mycc::AST::IfStatement* p_expr) = 0;
    virtual void visit(Mycc::AST::ParamVarDecl* p_expr) = 0;
    virtual void visit(Mycc::AST::ReturnNode* p_expr) = 0;
    virtual void visit(Mycc::AST::VarDecl* p_expr) = 0;
    virtual void visit(Mycc::AST::WhileStatement* p_expr) = 0;
    virtual void visit(Mycc::AST::LiteralExpr* p_expr) = 0;
};
}  // namespace Mycc::AST

#endif  // MYCC_SOURCE_AST_ASTVISITOR_H_
