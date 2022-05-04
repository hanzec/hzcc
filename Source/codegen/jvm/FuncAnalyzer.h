//
// Created by chen_ on 2022/5/3.
//
#include <unordered_map>

#include "AST/ASTVisitor.h"
#ifndef MYCC_SOURCE_CODEGEN_JVM_FUNCANALYZER_H_
#define MYCC_SOURCE_CODEGEN_JVM_FUNCANALYZER_H_
namespace Hzcc::Codegen {

class FuncAnalyzer : public AST::ASTVisitor {
  public:
    /**######################################################
     * ## AST Visitor                                      ##
     **#######################################################**/

    Status visit(Hzcc::AST::CastExpr* p_expr) override;
    Status visit(Hzcc::AST::ArithmeticExpr* p_expr) override;
    Status visit(Hzcc::AST::AssignExpr* p_expr) override;
    Status visit(Hzcc::AST::BitwiseExpr* p_expr) override;
    Status visit(Hzcc::AST::LogicalExpr* p_expr) override;
    Status visit(Hzcc::AST::CommaExpr* p_expr) override;
    Status visit(Hzcc::AST::RelationalExpr* p_expr) override;
    Status visit(Hzcc::AST::AccessExpr* p_expr) override;
    Status visit(Hzcc::AST::ArraySubscriptExpr* p_expr) override;
    Status visit(Hzcc::AST::ConditionalExpr* p_expr) override;
    Status visit(Hzcc::AST::DeclRefExpr* p_expr) override;
    Status visit(Hzcc::AST::SizeofExpr* p_expr) override;
    Status visit(Hzcc::AST::UnaryExpr* p_expr) override;
    Status visit(Hzcc::AST::BreakStatement* p_expr) override;
    Status visit(Hzcc::AST::CompoundStmt* p_expr) override;
    Status visit(Hzcc::AST::ContinueStatement* p_expr) override;
    Status visit(Hzcc::AST::DoStatement* p_expr) override;
    Status visit(Hzcc::AST::EmptyStatement* p_expr) override;
    Status visit(Hzcc::AST::ForStatement* p_expr) override;
    Status visit(Hzcc::AST::FunctionCall* p_expr) override;
    Status visit(Hzcc::AST::FunctionDeclNode* p_expr) override;
    Status visit(Hzcc::AST::DeclNode* p_expr) override;
    Status visit(Hzcc::AST::IfStatement* p_expr) override;
    Status visit(Hzcc::AST::ParamVarDecl* p_expr) override;
    Status visit(Hzcc::AST::ReturnNode* p_expr) override;
    Status visit(Hzcc::AST::VarDecl* p_expr) override;
    Status visit(Hzcc::AST::WhileStatement* p_expr) override;
    Status visit(Hzcc::AST::LiteralExpr* p_expr) override;

    /**######################################################
     * ## Information Collected                            ##
     **#######################################################**/
  private:
    uint32_t _max_stack_size = 0;
    std::unordered_map<std::string, std::pair<uint32_t, const std::string_view>>
        _local_var_map;
};
}  // namespace Hzcc::Codegen
#endif  // MYCC_SOURCE_CODEGEN_JVM_FUNCANALYZER_H_
