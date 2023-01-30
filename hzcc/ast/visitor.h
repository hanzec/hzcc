//
// Created by Hanze Chen on 2022/4/10.
//
#include "utils/logging.h"
#include "utils/status/status.h"
#ifndef HZCC_AST_AST_VISITOR_H
#define HZCC_AST_AST_VISITOR_H
namespace hzcc::ast {
class Stmt;
class ArithmeticExpr;
class AssignExpr;
class BitwiseExpr;
class CommaExpr;
class LogicalExpr;
class RelationalExpr;
class MemberExpr;
class ArraySubscriptExpr;
class TernaryExpr;
class DeclRefExpr;
class LiteralExpr;
class SizeofExpr;
class UnaryOperator;
class TypeProxyExpr;
class BreakStmt;
class CompoundStmt;
class ContinueStmt;
class IDeclStmt;
class DeclStmt;
class DoStmt;
class EmptyStmt;
class ForStmt;
class FuncCallStmt;
class FuncDeclStmt;
class IfStmt;
class ParamVarDecl;
class ReturnStmt;
class RecordDecl;
class WhileStmt;
class VarDecl;
class ExplicitCastExpr;
class ImplicitCastExpr;
class CompilationUnit;

class Visitor {
  public:
    virtual ~Visitor() = default;
    virtual Status visit(hzcc::ast::ArithmeticExpr* p_expr) {
        DLOG(WARNING) << "ast Visitor for Node " << __FUNCTION__
                      << " hzcc::ast::ArithmeticExpr"
                      << " not implemented!";
        return NoError();
    };
    virtual Status visit(hzcc::ast::AssignExpr* p_expr) {
        DLOG(WARNING) << "ast Visitor for Node " << __FUNCTION__
                      << " hzcc::ast::AssignExpr"
                      << " not implemented!";
        return NoError();
    };
    virtual Status visit(hzcc::ast::BitwiseExpr* p_expr) {
        DLOG(WARNING) << "ast Visitor for Node " << __FUNCTION__
                      << " hzcc::ast::BitwiseExpr"
                      << " not implemented!";
        return NoError();
    };
    virtual Status visit(hzcc::ast::LogicalExpr* p_expr) {
        DLOG(WARNING) << "ast Visitor for Node " << __FUNCTION__
                      << " hzcc::ast::LogicalExpr"
                      << " not implemented!";
        return NoError();
    };
    virtual Status visit(hzcc::ast::CommaExpr* p_expr) {
        DLOG(WARNING) << "ast Visitor for Node " << __FUNCTION__
                      << " hzcc::ast::CommaExpr"
                      << " not implemented!";
        return NoError();
    };
    virtual Status visit(hzcc::ast::RelationalExpr* p_expr) {
        DLOG(WARNING) << "ast Visitor for Node " << __FUNCTION__
                      << " hzcc::ast::RelationalExpr"
                      << " not implemented!";
        return NoError();
    };
    virtual Status visit(hzcc::ast::MemberExpr* p_expr) {
        DLOG(WARNING) << "ast Visitor for Node " << __FUNCTION__
                      << " hzcc::ast::AccessExpr"
                      << " not implemented!";
        return NoError();
    };
    virtual Status visit(hzcc::ast::ArraySubscriptExpr* p_expr) {
        DLOG(WARNING) << "ast Visitor for Node " << __FUNCTION__
                      << " hzcc::ast::ArraySubscriptExpr"
                      << " not implemented!";
        return NoError();
    };
    virtual Status visit(hzcc::ast::TernaryExpr* p_expr) {
        DLOG(WARNING) << "ast Visitor for Node " << __FUNCTION__
                      << " hzcc::ast::TernaryExpr"
                      << " not implemented!";
        return NoError();
    };
    virtual Status visit(hzcc::ast::DeclRefExpr* p_expr) {
        DLOG(WARNING) << "ast Visitor for Node " << __FUNCTION__
                      << " hzcc::ast::DeclRefExpr"
                      << " not implemented!";
        return NoError();
    };
    virtual Status visit(hzcc::ast::SizeofExpr* p_expr) {
        DLOG(WARNING) << "ast Visitor for Node " << __FUNCTION__
                      << " hzcc::ast::SizeofExpr"
                      << " not implemented!";
        return NoError();
    };
    virtual Status visit(hzcc::ast::UnaryOperator* p_expr) {
        DLOG(WARNING) << "ast Visitor for Node " << __FUNCTION__
                      << " hzcc::ast::UnaryOperator"
                      << " not implemented!";
        return NoError();
    };
    virtual Status visit(hzcc::ast::BreakStmt* p_expr) {
        DLOG(WARNING) << "ast Visitor for Node " << __FUNCTION__
                      << " hzcc::ast::BreakStmt"
                      << " not implemented!";
        return NoError();
    };
    virtual Status visit(hzcc::ast::CompoundStmt* p_expr) {
        DLOG(WARNING) << "ast Visitor for Node " << __FUNCTION__
                      << " hzcc::ast::CompoundStmt"
                      << " not implemented!";
        return NoError();
    };
    virtual Status visit(hzcc::ast::ContinueStmt* p_expr) {
        DLOG(WARNING) << "ast Visitor for Node " << __FUNCTION__
                      << " hzcc::ast::ContinueStmt"
                      << " not implemented!";
        return NoError();
    };
    virtual Status visit(hzcc::ast::DoStmt* p_expr) {
        DLOG(WARNING) << "ast Visitor for Node " << __FUNCTION__
                      << " hzcc::ast::DoStmt"
                      << " not implemented!";
        return NoError();
    };
    virtual Status visit(hzcc::ast::EmptyStmt* p_expr) {
        DLOG(WARNING) << "ast Visitor for Node " << __FUNCTION__
                      << " hzcc::ast::EmptyStmt"
                      << " not implemented!";
        return NoError();
    };
    virtual Status visit(hzcc::ast::ForStmt* p_expr) {
        DLOG(WARNING) << "ast Visitor for Node " << __FUNCTION__
                      << " hzcc::ast::ForStmt"
                      << " not implemented!";
        return NoError();
    };
    virtual Status visit(hzcc::ast::FuncCallStmt* p_expr) {
        DLOG(WARNING) << "ast Visitor for Node " << __FUNCTION__
                      << " hzcc::ast::FuncCallStmt"
                      << " not implemented!";
        return NoError();
    };
    virtual Status visit(hzcc::ast::FuncDeclStmt* p_expr) {
        DLOG(WARNING) << "ast Visitor for Node " << __FUNCTION__
                      << " hzcc::ast::FuncDeclStmt"
                      << " not implemented!";
        return NoError();
    };
    virtual Status visit(hzcc::ast::IDeclStmt* p_expr) {
        DLOG(WARNING) << "ast Visitor for Node " << __FUNCTION__
                      << " hzcc::ast::DeclStmt"
                      << " not implemented!";
        return NoError();
    };
    virtual Status visit(hzcc::ast::IfStmt* p_expr) {
        DLOG(WARNING) << "ast Visitor for Node " << __FUNCTION__
                      << " hzcc::ast::IfStmt"
                      << " not implemented!";
        return NoError();
    };
    virtual Status visit(hzcc::ast::VarDecl* p_expr) {
        DLOG(WARNING) << "ast Visitor for Node " << __FUNCTION__
                      << " hzcc::ast::ParamVarDecl"
                      << " not implemented!";
        return NoError();
    };

    virtual Status visit(hzcc::ast::ParamVarDecl* p_expr) {
        DLOG(WARNING) << "ast Visitor for Node " << __FUNCTION__
                      << " hzcc::ast::ParamVarDecl"
                      << " not implemented!";
        return NoError();
    };
    virtual Status visit(hzcc::ast::ReturnStmt* p_expr) {
        DLOG(WARNING) << "ast Visitor for Node " << __FUNCTION__
                      << " hzcc::ast::ReturnStmt"
                      << " not implemented!";
        return NoError();
    };
    virtual Status visit(hzcc::ast::WhileStmt* p_expr) {
        DLOG(WARNING) << "ast Visitor for Node " << __FUNCTION__
                      << " hzcc::ast::WhileStmt"
                      << " not implemented!";
        return NoError();
    };
    virtual Status visit(hzcc::ast::LiteralExpr* p_expr) {
        DLOG(WARNING) << "ast Visitor for Node " << __FUNCTION__
                      << " hzcc::ast::LiteralExpr"
                      << " not implemented!";
        return NoError();
    };

    virtual Status visit(hzcc::ast::TypeProxyExpr* p_expr) {
        DLOG(WARNING) << "ast Visitor for Node " << __FUNCTION__
                      << " hzcc::ast::LiteralExpr"
                      << " not implemented!";
        return NoError();
    };

    virtual Status visit(hzcc::ast::DeclStmt* p_expr) {
        DLOG(WARNING) << "ast Visitor for Node " << __FUNCTION__
                      << " hzcc::ast::LiteralExpr"
                      << " not implemented!";
        return NoError();
    };

    virtual Status visit(hzcc::ast::ExplicitCastExpr* p_expr) {
        DLOG(WARNING) << "ast Visitor for Node " << __FUNCTION__
                      << " hzcc::ast::ExplicitCastExpr"
                      << " not implemented!";
        return NoError();
    };

    virtual Status visit(hzcc::ast::ImplicitCastExpr* p_expr) {
        DLOG(WARNING) << "ast Visitor for Node " << __FUNCTION__
                      << " hzcc::ast::ExplicitCastExpr"
                      << " not implemented!";
        return NoError();
    };

    virtual Status visit(hzcc::ast::CompilationUnit* p_expr) {
        DLOG(WARNING) << "ast Visitor for Node " << __FUNCTION__
                      << " hzcc::ast::ExplicitCastExpr"
                      << " not implemented!";
        return NoError();
    };
};
}  // namespace hzcc::ast

#endif  // HZCC_AST_AST_VISITOR_H
