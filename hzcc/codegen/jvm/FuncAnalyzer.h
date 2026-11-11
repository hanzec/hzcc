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

/**
 * @brief Legacy AST visitor estimating operand-stack usage and local-variable slot assignments.
 */
class FuncAnalyzer : public AST::ASTVisitor {
  public:
    using SymbolTable =
        std::vector<std::list<std::tuple<std::string, std::string, uint64_t>>>;

    /**
     * @brief Initialize FuncAnalyzer with the supplied backend state.
     * @param global_vars Borrowed global name-to-type table; it must outlive this analyzer.
     */
    explicit FuncAnalyzer(
        const std::unordered_map<std::string, std::string>& global_vars);
    /**
     * @brief Build slot-indexed lists of variable names, types and declaration lines.
     * @return Slot-indexed lists of (name, type, declaration line) tuples.
     */
    [[nodiscard]] SymbolTable GetLocalVariable();

    /**
     * @brief Return the observed maximum operand-stack size plus the legacy safety margin of 20.
     * @return Maximum observed slot count plus 20.
     */
    [[nodiscard]] uint32_t GetMaxStackSize() const;

    /**
     * @brief Report whether a return statement has been visited.
     * @return True when the stated condition holds; otherwise false.
     */
    [[nodiscard]] bool HasValidReturn() const;

    /**
     * @brief Analyze stack and symbol effects for a CastExpr node in the legacy backend.
     * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
     * @return OK on success, or the first backend/pass error.
     */
    Status visit(Hzcc::AST::CastExpr* p_expr) override;
    /**
     * @brief Analyze stack and symbol effects for a ArithmeticExpr node in the legacy backend.
     * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
     * @return OK on success, or the first backend/pass error.
     */
    Status visit(Hzcc::AST::ArithmeticExpr* p_expr) override;
    /**
     * @brief Analyze stack and symbol effects for a AssignExpr node in the legacy backend.
     * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
     * @return OK on success, or the first backend/pass error.
     */
    Status visit(Hzcc::AST::AssignExpr* p_expr) override;
    /**
     * @brief Analyze stack and symbol effects for a BitwiseExpr node in the legacy backend.
     * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
     * @return OK on success, or the first backend/pass error.
     */
    Status visit(Hzcc::AST::BitwiseExpr* p_expr) override;
    /**
     * @brief Analyze stack and symbol effects for a LogicalExpr node in the legacy backend.
     * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
     * @return OK on success, or the first backend/pass error.
     */
    Status visit(Hzcc::AST::LogicalExpr* p_expr) override;
    /**
     * @brief Analyze stack and symbol effects for a CommaExpr node in the legacy backend.
     * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
     * @return OK on success, or the first backend/pass error.
     */
    Status visit(Hzcc::AST::CommaExpr* p_expr) override;
    /**
     * @brief Analyze stack and symbol effects for a RelationalExpr node in the legacy backend.
     * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
     * @return OK on success, or the first backend/pass error.
     */
    Status visit(Hzcc::AST::RelationalExpr* p_expr) override;
    /**
     * @brief Accept a AccessExpr node without analyzing it in this unfinished visitor.
     * @param p_expr Borrowed node pointer; this handler does not inspect or traverse it.
     * @return Success without changing stack or symbol state.
     */
    Status visit(Hzcc::AST::AccessExpr* p_expr) override;
    /**
     * @brief Analyze stack and symbol effects for a ArraySubscriptExpr node in the legacy backend.
     * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
     * @return OK on success, or the first backend/pass error.
     */
    Status visit(Hzcc::AST::ArraySubscriptExpr* p_expr) override;
    /**
     * @brief Accept a TernaryExpr node without analyzing it in this unfinished visitor.
     * @param p_expr Borrowed node pointer; this handler does not inspect or traverse it.
     * @return Success without changing stack or symbol state.
     */
    Status visit(Hzcc::AST::TernaryExpr* p_expr) override;
    /**
     * @brief Analyze stack and symbol effects for a DeclRefExpr node in the legacy backend.
     * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
     * @return OK on success, or the first backend/pass error.
     */
    Status visit(Hzcc::AST::DeclRefExpr* p_expr) override;
    /**
     * @brief Analyze stack and symbol effects for a SizeofExpr node in the legacy backend.
     * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
     * @return OK on success, or the first backend/pass error.
     */
    Status visit(Hzcc::AST::SizeofExpr* p_expr) override;
    /**
     * @brief Analyze stack and symbol effects for a UnaryExpr node in the legacy backend.
     * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
     * @return OK on success, or the first backend/pass error.
     */
    Status visit(Hzcc::AST::UnaryExpr* p_expr) override;
    /**
     * @brief Accept a BreakStmt node without analyzing it in this unfinished visitor.
     * @param p_expr Borrowed node pointer; this handler does not inspect or traverse it.
     * @return Success without changing stack or symbol state.
     */
    Status visit(Hzcc::AST::BreakStmt* p_expr) override;
    /**
     * @brief Analyze stack and symbol effects for a CompoundStmt node in the legacy backend.
     * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
     * @return OK on success, or the first backend/pass error.
     */
    Status visit(Hzcc::AST::CompoundStmt* p_expr) override;
    /**
     * @brief Accept a ContinueStmt node without analyzing it in this unfinished visitor.
     * @param p_expr Borrowed node pointer; this handler does not inspect or traverse it.
     * @return Success without changing stack or symbol state.
     */
    Status visit(Hzcc::AST::ContinueStmt* p_expr) override;
    /**
     * @brief Accept a DoStatement node without analyzing it in this unfinished visitor.
     * @param p_expr Borrowed node pointer; this handler does not inspect or traverse it.
     * @return Success without changing stack or symbol state.
     */
    Status visit(Hzcc::AST::DoStatement* p_expr) override;
    /**
     * @brief Accept a EmptyStatement node without analyzing it in this unfinished visitor.
     * @param p_expr Borrowed node pointer; this handler does not inspect or traverse it.
     * @return Success without changing stack or symbol state.
     */
    Status visit(Hzcc::AST::EmptyStatement* p_expr) override;
    /**
     * @brief Accept a ForStmt node without analyzing it in this unfinished visitor.
     * @param p_expr Borrowed node pointer; this handler does not inspect or traverse it.
     * @return Success without changing stack or symbol state.
     */
    Status visit(Hzcc::AST::ForStmt* p_expr) override;
    /**
     * @brief Analyze stack and symbol effects for a FuncCallStmt node in the legacy backend.
     * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
     * @return OK on success, or the first backend/pass error.
     */
    Status visit(Hzcc::AST::FuncCallStmt* p_expr) override;
    /**
     * @brief Accept a FuncDeclStmt node without analyzing it in this unfinished visitor.
     * @param p_expr Borrowed node pointer; this handler does not inspect or traverse it.
     * @return Success without changing stack or symbol state.
     */
    Status visit(Hzcc::AST::FuncDeclStmt* p_expr) override;
    /**
     * @brief Accept a DeclStmt node without analyzing it in this unfinished visitor.
     * @param p_expr Borrowed node pointer; this handler does not inspect or traverse it.
     * @return Success without changing stack or symbol state.
     */
    Status visit(Hzcc::AST::DeclStmt* p_expr) override;
    /**
     * @brief Accept a IfStmt node without analyzing it in this unfinished visitor.
     * @param p_expr Borrowed node pointer; this handler does not inspect or traverse it.
     * @return Success without changing stack or symbol state.
     */
    Status visit(Hzcc::AST::IfStmt* p_expr) override;
    /**
     * @brief Analyze stack and symbol effects for a ParamVarDecl node in the legacy backend.
     * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
     * @return OK on success, or the first backend/pass error.
     */
    Status visit(Hzcc::AST::ParamVarDecl* p_expr) override;
    /**
     * @brief Analyze stack and symbol effects for a ReturnStmt node in the legacy backend.
     * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
     * @return OK on success, or the first backend/pass error.
     */
    Status visit(Hzcc::AST::ReturnStmt* p_expr) override;
    /**
     * @brief Analyze stack and symbol effects for a VarDecl node in the legacy backend.
     * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
     * @return OK on success, or the first backend/pass error.
     */
    Status visit(Hzcc::AST::VarDecl* p_expr) override;
    /**
     * @brief Accept a WhileStmt node without analyzing it in this unfinished visitor.
     * @param p_expr Borrowed node pointer; this handler does not inspect or traverse it.
     * @return Success without changing stack or symbol state.
     */
    Status visit(Hzcc::AST::WhileStmt* p_expr) override;
    /**
     * @brief Analyze stack and symbol effects for a LiteralExpr node in the legacy backend.
     * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
     * @return OK on success, or the first backend/pass error.
     */
    Status visit(Hzcc::AST::LiteralExpr* p_expr) override;

  protected:
    /**
     * @brief Apply a signed operand-stack adjustment and update the observed maximum.
     * @param p_size Number of slots by which the operand-stack estimate is adjusted.
     */
    void ModifyStackSize(int p_size);

    /**
     * @brief Subtract slots from the current operand-stack estimate.
     * @param p_size Number of slots by which the operand-stack estimate is adjusted.
     */
    void DecreaseCurrentStack(uint8_t p_size);

    /**
     * @brief Compute the operand-stack change caused by storing a variable.
     * @param name Variable or symbol name to register or look up.
     * @return Signed stack-size adjustment.
     */
    [[nodiscard]] int SaveToVariable(const std::string& name);

    /**
     * @brief Compute the operand-stack change caused by loading a variable.
     * @param name Variable or symbol name to register or look up.
     * @return Signed stack-size adjustment.
     */
    [[nodiscard]] int LoadFromVariable(const std::string& name);

    /**
     * @brief Push an intermediate result identifier onto the return-value stack.
     * @param stackID Identifier of the intermediate result to retain.
     */
    void PushReturnStack(const std::string& stackID);

    /**
     * @brief Pop and return the most recent intermediate result identifier.
     * @return Most recently pushed result identifier.
     * @pre The corresponding stack or scope list is nonempty.
     */
    std::string ConsumeReturnStack();

    /**
     * @brief Push a new lexical scope for subsequent declarations or labels.
     */
    void EnterScope();
    /**
     * @brief Pop the innermost scope after its contents have been processed.
     * @pre The corresponding stack or scope list is nonempty.
     */
    void LeaveScope();

    /**
     * @brief Look up a variable in active lexical scopes, searching from the innermost scope.
     * @param p_name Variable name to look up.
     * @return True when the stated condition holds; otherwise false.
     */
    bool IsLocal(const std::string& p_name);

    /**
     * @brief Test whether a variable is present in the global variable table.
     * @param p_name Variable name to look up.
     * @return True when the stated condition holds; otherwise false.
     */
    bool IsGlobal(const std::string& p_name);

    /**
     * @brief Return the type of the nearest visible local declaration.
     * @param p_name Variable name to look up.
     * @return Type descriptor of the nearest visible local variable.
     */
    std::string GetLocalVarType(const std::string& p_name);

    /**
     * @brief Append a local declaration to the active scope.
     * @param name Variable or symbol name to register or look up.
     * @param type JVM type or descriptor used by this operation.
     * @param line_no Source line of the variable declaration.
     */
    void AddLocalVariable(const std::string& name, const std::string& type,
                          uint32_t line_no);

  private:
    /**
     * [name, type, line_no]
     */
    using LocalSymbol =
        std::tuple<const std::string, const std::string, uint32_t>;

    /**
     * @brief Store variable names, JVM types and declaration lines for one lexical scope.
     */
    class ScopedSymbolTable
        : public std::enable_shared_from_this<ScopedSymbolTable> {
      public:
        /**
         * @brief Append a symbol name, type and declaration line to this scope.
         * @param name Variable or symbol name to register or look up.
         * @param type JVM type or descriptor used by this operation.
         * @param line_no Source line of the variable declaration.
         */
        void AddSymbol(const std::string& name, const std::string& type,
                       uint32_t line_no);
        /**
         * @brief Test whether this scope contains the given symbol name.
         * @param name Variable or symbol name to register or look up.
         * @return True when the stated condition holds; otherwise false.
         */
        bool HasSymbol(const std::string& name);
        /**
         * @brief Return the type recorded for a symbol in this scope.
         * @param name Variable or symbol name to register or look up.
         * @return Type string stored for the named declaration.
         */
        std::string GetSymbolType(const std::string& name);
        /**
         * @brief Return a copy of the declarations recorded in this scope.
         * @return A copy of this scope's declarations.
         */
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
