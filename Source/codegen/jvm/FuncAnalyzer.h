//
// Created by chen_ on 2022/5/3.
//
#include <list>
#include <unordered_map>

#include "AST/ASTVisitor.h"
#ifndef MYCC_SOURCE_CODEGEN_JVM_FUNCANALYZER_H_
#define MYCC_SOURCE_CODEGEN_JVM_FUNCANALYZER_H_
namespace Hzcc::Codegen {

class FuncAnalyzer : public AST::ASTVisitor {
  public:
    FuncAnalyzer(
        const std::unordered_map<std::string, std::string>& global_vars);

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
    const std::list<std::tuple<const std::string, const std::string, uint32_t>>&
    GetLocalVariable();

    [[nodiscard]] uint32_t GetMaxStackSize() const;

    /**
     * @brief  Rerturn if current function has return statement or not
     * @return true if has return statement, false otherwise
     */
    bool HasValidReturn();

  protected:
    /**
     * @brief Add given size to current function's stack size, and update the
     * max stack size if necessary
     * @param p_size The increased size of the stack
     */
    void ModifyStackSize(int p_size);

    /**
     * @brief Remove  given size current function's stack size
     * @param p_size The increased size of the stack
     */
    void DecreaseCurrentStack(uint8_t p_size);

    void AddNewLocalVariable(const std::string_view& p_name, uint32_t line_no,
                             std::string_view type);

    [[nodiscard]] int SaveToVariable(const std::string& name);

    [[nodiscard]] int LoadFromVariable(const std::string& name);

    void PushReturnStack(const std::string& stackID);

    std::string ConsumeReturnStack();

  private:
    bool _has_return = false;
    bool _generate_load = false;
    bool _request_leave = false;
    uint32_t _max_stack_size = 0;
    uint32_t _current_stack_size = 0;

    std::list<std::string> _return_stack;


    /**
     * Reference of the Global Variable Table
     * [variable_name,type]
     */
    const std::unordered_map<std::string, std::string>& _global_vars;

    /**
     * _local_var_map will have a formate like:
     * [variable_name,type, line_number]
     */
    std::list<std::tuple<const std::string, const std::string, uint32_t>>
        _local_var_map;
};
}  // namespace Hzcc::Codegen
#endif  // MYCC_SOURCE_CODEGEN_JVM_FUNCANALYZER_H_
