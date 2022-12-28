//
// Created by chen_ on 2022/5/3.
//
#include <list>
#include <memory>
#include <unordered_map>
#include <vector>

#include "AST/visitor.h"
#ifndef HZCC_CODEGEN_JVM_FUNC_ANALYZER_H
#define HZCC_CODEGEN_JVM_FUNC_ANALYZER_H
namespace Hzcc::Codegen::JVM {

class FuncAnalyzer : public AST::ASTVisitor {
  public:
    using SymbolTable =
        std::vector<std::list<std::tuple<std::string, std::string, uint64_t>>>;

    explicit FuncAnalyzer(
        const std::unordered_map<std::string, std::string>& global_vars);
    /**
     * @brief Get the symbol table of the function.
     *
     * Table Structure:
     * [
     *     ...,
     *     [..., [name, type, line_no], ...],
     *     ...
     * ]
     * The index of the first level of list is the stack ID. Then the second
     * level is the list of variables which shared the same stack ID. Since it's
     * not necessary to give every local temp variables a unique stack ID.
     * Noticed that the stack ID is that only contains one variable is the
     * function's variable.
     *
     *  @return The symbol table of the function.
     */
    [[nodiscard]] SymbolTable GetLocalVariable();

    [[nodiscard]] uint32_t GetMaxStackSize() const;

    /**
     * @brief  Rerturn if current function has return stmt or not
     * @return true if has return stmt, false otherwise
     */
    [[nodiscard]] bool HasValidReturn() const;

    Status visit(Hzcc::AST::CastExpr* p_expr) override;
    Status visit(Hzcc::AST::ArithmeticExpr* p_expr) override;
    Status visit(Hzcc::AST::AssignExpr* p_expr) override;
    Status visit(Hzcc::AST::BitwiseExpr* p_expr) override;
    Status visit(Hzcc::AST::LogicalExpr* p_expr) override;
    Status visit(Hzcc::AST::CommaExpr* p_expr) override;
    Status visit(Hzcc::AST::RelationalExpr* p_expr) override;
    Status visit(Hzcc::AST::AccessExpr* p_expr) override;
    Status visit(Hzcc::AST::ArraySubscriptExpr* p_expr) override;
    Status visit(Hzcc::AST::TernaryExpr* p_expr) override;
    Status visit(Hzcc::AST::DeclRefExpr* p_expr) override;
    Status visit(Hzcc::AST::SizeofExpr* p_expr) override;
    Status visit(Hzcc::AST::UnaryExpr* p_expr) override;
    Status visit(Hzcc::AST::BreakStmt* p_expr) override;
    Status visit(Hzcc::AST::CompoundStmt* p_expr) override;
    Status visit(Hzcc::AST::ContinueStmt* p_expr) override;
    Status visit(Hzcc::AST::DoStatement* p_expr) override;
    Status visit(Hzcc::AST::EmptyStatement* p_expr) override;
    Status visit(Hzcc::AST::ForStmt* p_expr) override;
    Status visit(Hzcc::AST::FuncCallStmt* p_expr) override;
    Status visit(Hzcc::AST::FuncDeclStmt* p_expr) override;
    Status visit(Hzcc::AST::DeclStmt* p_expr) override;
    Status visit(Hzcc::AST::IfStmt* p_expr) override;
    Status visit(Hzcc::AST::ParamVarDecl* p_expr) override;
    Status visit(Hzcc::AST::ReturnStmt* p_expr) override;
    Status visit(Hzcc::AST::VarDecl* p_expr) override;
    Status visit(Hzcc::AST::WhileStmt* p_expr) override;
    Status visit(Hzcc::AST::LiteralExpr* p_expr) override;

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

    [[nodiscard]] int SaveToVariable(const std::string& name);

    [[nodiscard]] int LoadFromVariable(const std::string& name);

    void PushReturnStack(const std::string& stackID);

    std::string ConsumeReturnStack();

    void EnterScope();
    void LeaveScope();

    bool IsLocal(const std::string& p_name);

    bool IsGlobal(const std::string& p_name);

    std::string GetLocalVarType(const std::string& p_name);

    void AddLocalVariable(const std::string& name, const std::string& type,
                          uint32_t line_no);

  private:
    /**
     * [name, type, line_no]
     */
    using LocalSymbol =
        std::tuple<const std::string, const std::string, uint32_t>;

    class ScopedSymbolTable
        : public std::enable_shared_from_this<ScopedSymbolTable> {
      public:
        void AddSymbol(const std::string& name, const std::string& type,
                       uint32_t line_no);
        bool HasSymbol(const std::string& name);
        std::string GetSymbolType(const std::string& name);
        std::list<LocalSymbol> GetSymbols();

      private:
        /**
         * _local_var_map will have a formate like:
         * [variable_name,type, line_number]
         */
        std::list<LocalSymbol> _symbol_table;
    };

    bool _has_return = false;
    bool _generate_load = false;
    bool _request_leave = false;
    int32_t _max_stack_size = 0;
    int32_t _current_stack_size = 0;

    std::list<std::string> _return_stack;

    std::list<std::shared_ptr<ScopedSymbolTable>> _symbol_table_stack;
    std::vector<std::shared_ptr<ScopedSymbolTable>> _saved_symbol_table;

    /**
     * Reference of the Global Variable Table
     * [variable_name,type]
     */
    const std::unordered_map<std::string, std::string>& _global_vars;
};
}  // namespace Hzcc::Codegen::JVM
#endif  // HZCC_CODEGEN_JVM_FUNC_ANALYZER_H
