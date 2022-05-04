//
// Created by chen_ on 2022/5/3.
//

#include "FuncAnalyzer.h"

namespace Hzcc::Codegen {

Status FuncAnalyzer::visit(Hzcc::AST::CastExpr *p_expr) {
    return Status(Status::NOT_FOUND);
}
Status FuncAnalyzer::visit(Hzcc::AST::ArithmeticExpr *p_expr) {
    return Status(Status::NOT_FOUND);
}
Status FuncAnalyzer::visit(Hzcc::AST::AssignExpr *p_expr) {
    return Status(Status::NOT_FOUND);
}
Status FuncAnalyzer::visit(Hzcc::AST::BitwiseExpr *p_expr) {
    return Status(Status::NOT_FOUND);
}
Status FuncAnalyzer::visit(Hzcc::AST::LogicalExpr *p_expr) {
    return Status(Status::NOT_FOUND);
}
Status FuncAnalyzer::visit(Hzcc::AST::CommaExpr *p_expr) {
    return Status(Status::NOT_FOUND);
}
Status FuncAnalyzer::visit(Hzcc::AST::RelationalExpr *p_expr) {
    return Status(Status::NOT_FOUND);
}
Status FuncAnalyzer::visit(Hzcc::AST::AccessExpr *p_expr) {
    return Status(Status::NOT_FOUND);
}
Status FuncAnalyzer::visit(Hzcc::AST::ArraySubscriptExpr *p_expr) {
    return Status(Status::NOT_FOUND);
}
Status FuncAnalyzer::visit(Hzcc::AST::ConditionalExpr *p_expr) {
    return Status(Status::NOT_FOUND);
}
Status FuncAnalyzer::visit(Hzcc::AST::DeclRefExpr *p_expr) {
    return Status(Status::NOT_FOUND);
}
Status FuncAnalyzer::visit(Hzcc::AST::SizeofExpr *p_expr) {
    return Status(Status::NOT_FOUND);
}
Status FuncAnalyzer::visit(Hzcc::AST::UnaryExpr *p_expr) {
    return Status(Status::NOT_FOUND);
}
Status FuncAnalyzer::visit(Hzcc::AST::CompoundStmt *p_expr) {
    return Status(Status::NOT_FOUND);
}
Status FuncAnalyzer::visit(Hzcc::AST::BreakStatement *p_expr) {
    return Status(Status::NOT_FOUND);
}
Status FuncAnalyzer::visit(Hzcc::AST::ContinueStatement *p_expr) {
    return Status(Status::NOT_FOUND);
}
Status FuncAnalyzer::visit(Hzcc::AST::DoStatement *p_expr) {
    return Status(Status::NOT_FOUND);
}
Status FuncAnalyzer::visit(Hzcc::AST::EmptyStatement *p_expr) {
    return Status(Status::NOT_FOUND);
}
Status FuncAnalyzer::visit(Hzcc::AST::ForStatement *p_expr) {
    return Status(Status::NOT_FOUND);
}
Status FuncAnalyzer::visit(Hzcc::AST::FunctionCall *p_expr) {
    return Status(Status::NOT_FOUND);
}
Status FuncAnalyzer::visit(Hzcc::AST::FunctionDeclNode *p_expr) {
    return Status(Status::NOT_FOUND);
}
Status FuncAnalyzer::visit(Hzcc::AST::DeclNode *p_expr) {
    return Status(Status::NOT_FOUND);
}
Status FuncAnalyzer::visit(Hzcc::AST::IfStatement *p_expr) {
    return Status(Status::NOT_FOUND);
}
Status FuncAnalyzer::visit(Hzcc::AST::ParamVarDecl *p_expr) {
    return Status(Status::NOT_FOUND);
}
Status FuncAnalyzer::visit(Hzcc::AST::ReturnNode *p_expr) {
    return Status(Status::NOT_FOUND);
}
Status FuncAnalyzer::visit(Hzcc::AST::WhileStatement *p_expr) {
    return Status(Status::NOT_FOUND);
}
Status FuncAnalyzer::visit(Hzcc::AST::LiteralExpr *p_expr) {
    return Status(Status::NOT_FOUND);
}
}  // namespace Hzcc::Codegen