//
// Created by Hanze Chen on 2022/4/10.
//
#include "utils/Status.h"
#include "utils/logging.h"
#include "utils/type_name_utils.h"

#ifndef HZCC_AST_AST_VISITOR_H
#define HZCC_AST_AST_VISITOR_H
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
class BreakStmt;
class CompoundStmt;
class ContinueStmt;
class DeclStmt;
class DoStatement;
class EmptyStatement;
class ForStmt;
class FuncCallStmt;
class FuncDeclStmt;
class IfStmt;
class ParamVarDecl;
class ReturnStmt;
class StructDeclStmt;
class VarDecl;
class WhileStmt;

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
    virtual Status visit(Hzcc::AST::BreakStmt* p_expr) {
        DLOG(WARNING) << "AST Visitor for Node " << __FUNCTION__
                      << " Hzcc::AST::BreakStmt"
                      << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::CompoundStmt* p_expr) {
        DLOG(WARNING) << "AST Visitor for Node " << __FUNCTION__
                      << " Hzcc::AST::CompoundStmt"
                      << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::ContinueStmt* p_expr) {
        DLOG(WARNING) << "AST Visitor for Node " << __FUNCTION__
                      << " Hzcc::AST::ContinueStmt"
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
    virtual Status visit(Hzcc::AST::ForStmt* p_expr) {
        DLOG(WARNING) << "AST Visitor for Node " << __FUNCTION__
                      << " Hzcc::AST::ForStmt"
                      << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::FuncCallStmt* p_expr) {
        DLOG(WARNING) << "AST Visitor for Node " << __FUNCTION__
                      << " Hzcc::AST::FuncCallStmt"
                      << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::FuncDeclStmt* p_expr) {
        DLOG(WARNING) << "AST Visitor for Node " << __FUNCTION__
                      << " Hzcc::AST::FuncDeclStmt"
                      << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::DeclStmt* p_expr) {
        DLOG(WARNING) << "AST Visitor for Node " << __FUNCTION__
                      << " Hzcc::AST::DeclStmt"
                      << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::IfStmt* p_expr) {
        DLOG(WARNING) << "AST Visitor for Node " << __FUNCTION__
                      << " Hzcc::AST::IfStmt"
                      << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::ParamVarDecl* p_expr) {
        DLOG(WARNING) << "AST Visitor for Node " << __FUNCTION__
                      << " Hzcc::AST::ParamVarDecl"
                      << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::ReturnStmt* p_expr) {
        DLOG(WARNING) << "AST Visitor for Node " << __FUNCTION__
                      << " Hzcc::AST::ReturnStmt"
                      << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::VarDecl* p_expr) {
        DLOG(WARNING) << "AST Visitor for Node " << __FUNCTION__
                      << " Hzcc::AST::VarDecl"
                      << " not implemented!";
        return Status::OkStatus();
    };
    virtual Status visit(Hzcc::AST::WhileStmt* p_expr) {
        DLOG(WARNING) << "AST Visitor for Node " << __FUNCTION__
                      << " Hzcc::AST::WhileStmt"
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

#endif  // HZCC_AST_AST_VISITOR_H
