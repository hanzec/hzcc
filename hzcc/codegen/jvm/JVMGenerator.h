//
// Created by chen_ on 2022/4/10.
//
#include <memory>
#include <string_view>

#include "AST/visitor.h"
#include "codegen/Generator.h"
#include "codegen/IndentWriter.h"
#include "codegen/LabelManager.h"
#include "codegen/jvm/StackManager.h"
#include "codegen/jvm/utils/macro.h"

#ifndef HZCC_CODEGEN_JVM_JVM_GENERATOR_H
#define HZCC_CODEGEN_JVM_JVM_GENERATOR_H
namespace hzcc::Codegen {
enum PACKED OpHint {
    kOpHint_Less = 0,
    kOpHint_Equal = 1,
    kOpHint_NEqual = 2,
    kOpHint_Greater = 3,
    kOpHint_LessEqual = 4,
    kOpHint_GreaterEqual = 5,
    kOpHint_Logical_Less = 6,
    kOpHint_Logical_Equal = 7,
    kOpHint_Logical_NEqual = 8,
    kOpHint_Logical_Greater = 9,
    kOpHint_Logical_LessEqual = 10,
    kOpHint_Logical_GreaterEqual = 11,
    kOpHint_None = 12,
    kOpHint_ENUM_SIZE = 13,  // keep last
};

/**
 * @brief Experimental AST visitor emitting textual JVM assembly using the legacy AST interface.
 */
class JVMGenerator : public Generator,
                     public AST::ASTVisitor,
                     protected LabelManager,
                     protected JVM::StackManager {
  public:
    /**
     * @brief Initialize JVMGenerator with the supplied backend state.
     * @param output Requested output path.
     * @param unit Non-null compilation unit retained through shared ownership.
     * @param output_c_ret_code Select the legacy entry-wrapper return-code behavior.
     */
    JVMGenerator(const std::string& output,
                 const std::shared_ptr<AST::CompilationUnit>& unit,
                 bool output_c_ret_code = false);

    /**
     * @brief Generate output for the compilation unit retained by this generator.
     * @return OK on success, or the first backend/pass error.
     */
    Status Generate() override;  // NOLINT

    /**
     * @brief Emit JVM assembly for a VarDecl node in the legacy backend.
     * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
     * @return OK on success, or the first backend/pass error.
     */
    Status visit(hzcc::AST::VarDecl* p_expr) override;
    /**
     * @brief Emit JVM assembly for a CastExpr node in the legacy backend.
     * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
     * @return OK on success, or the first backend/pass error.
     */
    Status visit(hzcc::AST::CastExpr* p_expr) override;
    /**
     * @brief Emit JVM assembly for a UnaryOperator node in the legacy backend.
     * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
     * @return OK on success, or the first backend/pass error.
     */
    Status visit(hzcc::AST::UnaryOperator* p_expr) override;
    /**
     * @brief Emit JVM assembly for a ReturnStmt node in the legacy backend.
     * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
     * @return OK on success, or the first backend/pass error.
     */
    Status visit(hzcc::AST::ReturnStmt* p_expr) override;
    /**
     * @brief Emit JVM assembly for a AssignExpr node in the legacy backend.
     * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
     * @return OK on success, or the first backend/pass error.
     */
    Status visit(hzcc::AST::AssignExpr* p_expr) override;
    /**
     * @brief Emit JVM assembly for a LogicalExpr node in the legacy backend.
     * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
     * @return OK on success, or the first backend/pass error.
     */
    Status visit(hzcc::AST::LogicalExpr* p_expr) override;
    /**
     * @brief Emit JVM assembly for a DoStmt node in the legacy backend.
     * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
     * @return OK on success, or the first backend/pass error.
     */
    Status visit(hzcc::AST::DoStmt* p_expr) override;
    /**
     * @brief Emit JVM assembly for a IfStmt node in the legacy backend.
     * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
     * @return OK on success, or the first backend/pass error.
     */
    Status visit(hzcc::AST::IfStmt* p_expr) override;
    /**
     * @brief Emit JVM assembly for a DeclRefExpr node in the legacy backend.
     * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
     * @return OK on success, or the first backend/pass error.
     */
    Status visit(hzcc::AST::DeclRefExpr* p_expr) override;
    /**
     * @brief Emit JVM assembly for a LiteralExpr node in the legacy backend.
     * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
     * @return OK on success, or the first backend/pass error.
     */
    Status visit(hzcc::AST::LiteralExpr* p_expr) override;
    /**
     * @brief Emit JVM assembly for a FuncCallStmt node in the legacy backend.
     * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
     * @return OK on success, or the first backend/pass error.
     */
    Status visit(hzcc::AST::FuncCallStmt* p_expr) override;
    /**
     * @brief Emit JVM assembly for a ForStmt node in the legacy backend.
     * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
     * @return OK on success, or the first backend/pass error.
     */
    Status visit(hzcc::AST::ForStmt* p_expr) override;
    /**
     * @brief Emit JVM assembly for a CompoundStmt node in the legacy backend.
     * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
     * @return OK on success, or the first backend/pass error.
     */
    Status visit(hzcc::AST::CompoundStmt* p_expr) override;
    /**
     * @brief Emit JVM assembly for a RelationalExpr node in the legacy backend.
     * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
     * @return OK on success, or the first backend/pass error.
     */
    Status visit(hzcc::AST::RelationalExpr* p_expr) override;
    /**
     * @brief Emit JVM assembly for a EmptyStmt node in the legacy backend.
     * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
     * @return OK on success, or the first backend/pass error.
     */
    Status visit(hzcc::AST::EmptyStmt* p_expr) override;
    /**
     * @brief Emit JVM assembly for a ArithmeticExpr node in the legacy backend.
     * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
     * @return OK on success, or the first backend/pass error.
     */
    Status visit(hzcc::AST::ArithmeticExpr* p_expr) override;
    /**
     * @brief Emit JVM assembly for a WhileStmt node in the legacy backend.
     * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
     * @return OK on success, or the first backend/pass error.
     */
    Status visit(hzcc::AST::WhileStmt* p_expr) override;
    /**
     * @brief Emit JVM assembly for a BreakStmt node in the legacy backend.
     * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
     * @return OK on success, or the first backend/pass error.
     */
    Status visit(hzcc::AST::BreakStmt* p_expr) override;
    /**
     * @brief Emit JVM assembly for a TernaryExpr node in the legacy backend.
     * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
     * @return OK on success, or the first backend/pass error.
     */
    Status visit(hzcc::AST::TernaryExpr* p_expr) override;
    /**
     * @brief Emit JVM assembly for a FuncDeclStmt node in the legacy backend.
     * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
     * @return OK on success, or the first backend/pass error.
     */
    Status visit(hzcc::AST::FuncDeclStmt* p_expr) override;
    /**
     * @brief Emit JVM assembly for a ContinueStmt node in the legacy backend.
     * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
     * @return OK on success, or the first backend/pass error.
     */
    Status visit(hzcc::AST::ContinueStmt* p_expr) override;
    /**
     * @brief Emit JVM assembly for a ArraySubscriptExpr node in the legacy backend.
     * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
     * @return OK on success, or the first backend/pass error.
     */
    Status visit(hzcc::AST::ArraySubscriptExpr* p_expr) override;
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

  protected:
    /**
     * @brief Return the input source filename; this legacy accessor is only declared.
     * @return The generator-owned input filename.
     */
    const std::string& GetInputFileName();
    /**
     * @brief Return the class name derived from the input filename.
     * @return The generator-owned JVM class name.
     */
    const std::string& GetCurrentClassName();
    /**
     * @brief Consume the previous comparison hint and reset it to kOpHint_None.
     * @return Previous hint before it was reset.
     */
    inline OpHint GetLastOpHint() {
        auto current_hint = _prevOpType;
        _prevOpType = kOpHint_None;
        return current_hint;
    };

    /**
     * @brief Record the comparison hint for the next emitted operation.
     * @param opType Comparison hint for the next operation.
     */
    inline void SetLastOpHint(OpHint opType) { _prevOpType = opType; };

  private:
    OpHint _prevOpType = kOpHint_None;
    std::string _input_file_name;
    std::string _current_class_name;

    bool _generate_load = false;
    bool _request_leave = false;
    bool _under_compare = false;
    bool _generate_c_ret_code = false;

    std::list<std::string> _return_stack;

    /**
     * [name, [class_name,function_signature]]
     * We have following predefined functions:
     *      -  public static int getchar() from class [libc]
     *      -  public static int putchar(int c) from class [libc]
     */
    inline static std::unordered_map<std::string,
                                     std::pair<std::string, std::string>>
        _function_table{{"putchar", {"libc", "(I)I"}},
                        {"getchar", {"libc", "()I"}}};
};

}  // namespace hzcc::Codegen

#endif  // HZCC_CODEGEN_JVM_JVM_GENERATOR_H
