//
// Created by chen_ on 2022/4/10.
//

#ifndef MYCC_SOURCE_AST_ASTVISITOR_H_
#define MYCC_SOURCE_AST_ASTVISITOR_H_
#include "utils/Status.h"
#include "utils/logging.h"
#include "utils/type_name_utils.h"

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
class TernaryExpr;
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
        DLOG(WARNING) << "AST Visitor for Node " << __FUNCTION__
                      << " Hzcc::AST::CastExpr"
                      << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::ArithmeticExpr* p_expr) {
        DLOG(WARNING) << "AST Visitor for Node " << __FUNCTION__
                      << " Hzcc::AST::ArithmeticExpr"
                      << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::AssignExpr* p_expr) {
        DLOG(WARNING) << "AST Visitor for Node " << __FUNCTION__
                      << " Hzcc::AST::AssignExpr"
                      << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::BitwiseExpr* p_expr) {
        DLOG(WARNING) << "AST Visitor for Node " << __FUNCTION__
                      << " Hzcc::AST::BitwiseExpr"
                      << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::LogicalExpr* p_expr) {
        DLOG(WARNING) << "AST Visitor for Node " << __FUNCTION__
                      << " Hzcc::AST::LogicalExpr"
                      << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::CommaExpr* p_expr) {
        DLOG(WARNING) << "AST Visitor for Node " << __FUNCTION__
                      << " Hzcc::AST::CommaExpr"
                      << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::RelationalExpr* p_expr) {
        DLOG(WARNING) << "AST Visitor for Node " << __FUNCTION__
                      << " Hzcc::AST::RelationalExpr"
                      << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::AccessExpr* p_expr) {
        DLOG(WARNING) << "AST Visitor for Node " << __FUNCTION__
                      << " Hzcc::AST::AccessExpr"
                      << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::ArraySubscriptExpr* p_expr) {
        DLOG(WARNING) << "AST Visitor for Node " << __FUNCTION__
                      << " Hzcc::AST::ArraySubscriptExpr"
                      << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::TernaryExpr* p_expr) {
        DLOG(WARNING) << "AST Visitor for Node " << __FUNCTION__
                      << " Hzcc::AST::ConditionalExpr"
                      << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::DeclRefExpr* p_expr) {
        DLOG(WARNING) << "AST Visitor for Node " << __FUNCTION__
                      << " Hzcc::AST::DeclRefExpr"
                      << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::SizeofExpr* p_expr) {
        DLOG(WARNING) << "AST Visitor for Node " << __FUNCTION__
                      << " Hzcc::AST::SizeofExpr"
                      << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::UnaryExpr* p_expr) {
        DLOG(WARNING) << "AST Visitor for Node " << __FUNCTION__
                      << " Hzcc::AST::UnaryExpr"
                      << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::BreakStatement* p_expr) {
        DLOG(WARNING) << "AST Visitor for Node " << __FUNCTION__
                      << " Hzcc::AST::BreakStatement"
                      << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::CompoundStmt* p_expr) {
        DLOG(WARNING) << "AST Visitor for Node " << __FUNCTION__
                      << " Hzcc::AST::CompoundStmt"
                      << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::ContinueStatement* p_expr) {
        DLOG(WARNING) << "AST Visitor for Node " << __FUNCTION__
                      << " Hzcc::AST::ContinueStatement"
                      << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::DoStatement* p_expr) {
        DLOG(WARNING) << "AST Visitor for Node " << __FUNCTION__
                      << " Hzcc::AST::DoStatement"
                      << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::EmptyStatement* p_expr) {
        DLOG(WARNING) << "AST Visitor for Node " << __FUNCTION__
                      << " Hzcc::AST::EmptyStatement"
                      << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::ForStatement* p_expr) {
        DLOG(WARNING) << "AST Visitor for Node " << __FUNCTION__
                      << " Hzcc::AST::ForStatement"
                      << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::FunctionCall* p_expr) {
        DLOG(WARNING) << "AST Visitor for Node " << __FUNCTION__
                      << " Hzcc::AST::FunctionCall"
                      << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::FunctionDeclNode* p_expr) {
        DLOG(WARNING) << "AST Visitor for Node " << __FUNCTION__
                      << " Hzcc::AST::FunctionDeclNode"
                      << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::DeclNode* p_expr) {
        DLOG(WARNING) << "AST Visitor for Node " << __FUNCTION__
                      << " Hzcc::AST::DeclNode"
                      << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::IfStatement* p_expr) {
        DLOG(WARNING) << "AST Visitor for Node " << __FUNCTION__
                      << " Hzcc::AST::IfStatement"
                      << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::ParamVarDecl* p_expr) {
        DLOG(WARNING) << "AST Visitor for Node " << __FUNCTION__
                      << " Hzcc::AST::ParamVarDecl"
                      << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::ReturnNode* p_expr) {
        DLOG(WARNING) << "AST Visitor for Node " << __FUNCTION__
                      << " Hzcc::AST::ReturnNode"
                      << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::VarDecl* p_expr) {
        DLOG(WARNING) << "AST Visitor for Node " << __FUNCTION__
                      << " Hzcc::AST::VarDecl"
                      << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::WhileStatement* p_expr) {
        DLOG(WARNING) << "AST Visitor for Node " << __FUNCTION__
                      << " Hzcc::AST::WhileStatement"
                      << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::LiteralExpr* p_expr) {
        DLOG(WARNING) << "AST Visitor for Node " << __FUNCTION__
                      << " Hzcc::AST::LiteralExpr"
                      << " not implemented!";
        return Status::OkStatus();
    };
};
}  // namespace Hzcc::AST

#endif  // MYCC_SOURCE_AST_ASTVISITOR_H_
