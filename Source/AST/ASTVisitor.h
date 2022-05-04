//
// Created by chen_ on 2022/4/10.
//

#ifndef MYCC_SOURCE_AST_ASTVISITOR_H_
#define MYCC_SOURCE_AST_ASTVISITOR_H_
#include "utils/Status.h"
#include "utils/logging.h"
namespace Hzcc::AST {
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
    virtual Status visit(Hzcc::AST::CastExpr* p_expr) {
        DVLOG(MESSAGE_ERROR_TRACING)
            << "AST Visitor for Node" << __FUNCTION__ << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::ArithmeticExpr* p_expr) {
        DVLOG(MESSAGE_ERROR_TRACING)
            << "AST Visitor for Node" << __FUNCTION__ << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::AssignExpr* p_expr) {
        DVLOG(MESSAGE_ERROR_TRACING)
            << "AST Visitor for Node" << __FUNCTION__ << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::BitwiseExpr* p_expr) {
        DVLOG(MESSAGE_ERROR_TRACING)
            << "AST Visitor for Node" << __FUNCTION__ << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::LogicalExpr* p_expr) {
        DVLOG(MESSAGE_ERROR_TRACING)
            << "AST Visitor for Node" << __FUNCTION__ << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::CommaExpr* p_expr) {
        DVLOG(MESSAGE_ERROR_TRACING)
            << "AST Visitor for Node" << __FUNCTION__ << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::RelationalExpr* p_expr) {
        DVLOG(MESSAGE_ERROR_TRACING)
            << "AST Visitor for Node" << __FUNCTION__ << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::AccessExpr* p_expr) {
        DVLOG(MESSAGE_ERROR_TRACING)
            << "AST Visitor for Node" << __FUNCTION__ << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::ArraySubscriptExpr* p_expr) {
        DVLOG(MESSAGE_ERROR_TRACING)
            << "AST Visitor for Node" << __FUNCTION__ << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::ConditionalExpr* p_expr) {
        DVLOG(MESSAGE_ERROR_TRACING)
            << "AST Visitor for Node" << __FUNCTION__ << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::DeclRefExpr* p_expr) {
        DVLOG(MESSAGE_ERROR_TRACING)
            << "AST Visitor for Node" << __FUNCTION__ << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::SizeofExpr* p_expr) {
        DVLOG(MESSAGE_ERROR_TRACING)
            << "AST Visitor for Node" << __FUNCTION__ << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::UnaryExpr* p_expr) {
        DVLOG(MESSAGE_ERROR_TRACING)
            << "AST Visitor for Node" << __FUNCTION__ << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::BreakStatement* p_expr) {
        DVLOG(MESSAGE_ERROR_TRACING)
            << "AST Visitor for Node" << __FUNCTION__ << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::CompoundStmt* p_expr) {
        DVLOG(MESSAGE_ERROR_TRACING)
            << "AST Visitor for Node" << __FUNCTION__ << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::ContinueStatement* p_expr) {
        DVLOG(MESSAGE_ERROR_TRACING)
            << "AST Visitor for Node" << __FUNCTION__ << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::DoStatement* p_expr) {
        DVLOG(MESSAGE_ERROR_TRACING)
            << "AST Visitor for Node" << __FUNCTION__ << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::EmptyStatement* p_expr) {
        DVLOG(MESSAGE_ERROR_TRACING)
            << "AST Visitor for Node" << __FUNCTION__ << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::ForStatement* p_expr) {
        DVLOG(MESSAGE_ERROR_TRACING)
            << "AST Visitor for Node" << __FUNCTION__ << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::FunctionCall* p_expr) {
        DVLOG(MESSAGE_ERROR_TRACING)
            << "AST Visitor for Node" << __FUNCTION__ << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::FunctionDeclNode* p_expr) {
        DVLOG(MESSAGE_ERROR_TRACING)
            << "AST Visitor for Node" << __FUNCTION__ << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::DeclNode* p_expr) {
        DVLOG(MESSAGE_ERROR_TRACING)
            << "AST Visitor for Node" << __FUNCTION__ << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::IfStatement* p_expr) {
        DVLOG(MESSAGE_ERROR_TRACING)
            << "AST Visitor for Node" << __FUNCTION__ << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::ParamVarDecl* p_expr) {
        DVLOG(MESSAGE_ERROR_TRACING)
            << "AST Visitor for Node" << __FUNCTION__ << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::ReturnNode* p_expr) {
        DVLOG(MESSAGE_ERROR_TRACING)
            << "AST Visitor for Node" << __FUNCTION__ << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::VarDecl* p_expr) {
        DVLOG(MESSAGE_ERROR_TRACING)
            << "AST Visitor for Node" << __FUNCTION__ << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::WhileStatement* p_expr) {
        DVLOG(MESSAGE_ERROR_TRACING)
            << "AST Visitor for Node" << __FUNCTION__ << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::LiteralExpr* p_expr) {
        DVLOG(MESSAGE_ERROR_TRACING)
            << "AST Visitor for Node" << __FUNCTION__ << " not implemented!";
        return Status::OkStatus();
    };
};
}  // namespace Hzcc::AST

#endif  // MYCC_SOURCE_AST_ASTVISITOR_H_
