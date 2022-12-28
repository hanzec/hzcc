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

class JVMGenerator : public Generator,
                     public AST::ASTVisitor,
                     protected LabelManager,
                     protected JVM::StackManager {
  public:
    JVMGenerator(const std::string& output,
                 const std::shared_ptr<AST::CompilationUnit>& unit,
                 bool output_c_ret_code = false);

    /**
     * @brief Generate the JVM Bytecode for the given compilation unit.
     *
     * @note output file name is related to the input file name. For example, if
     *       the input file is "test.c", the output file is "test.class". There
     *       will be no effect setting the output file name.
     *
     * @param input_file  The input file.
     * @param output_file The output file to write the generated code to.
     * @param unit      The compilation unit to generate code for.
     * @return Status   The Status of the generation.
     */
    Status Generate() override;  // NOLINT

    /**######################################################
     * ## ast Visitor                                      ##
     **#######################################################**/
    Status visit(hzcc::AST::VarDecl* p_expr) override;
    Status visit(hzcc::AST::CastExpr* p_expr) override;
    Status visit(hzcc::AST::UnaryOperator* p_expr) override;
    Status visit(hzcc::AST::ReturnStmt* p_expr) override;
    Status visit(hzcc::AST::AssignExpr* p_expr) override;
    Status visit(hzcc::AST::LogicalExpr* p_expr) override;
    Status visit(hzcc::AST::DoStmt* p_expr) override;
    Status visit(hzcc::AST::IfStmt* p_expr) override;
    Status visit(hzcc::AST::DeclRefExpr* p_expr) override;
    Status visit(hzcc::AST::LiteralExpr* p_expr) override;
    Status visit(hzcc::AST::FuncCallStmt* p_expr) override;
    Status visit(hzcc::AST::ForStmt* p_expr) override;
    Status visit(hzcc::AST::CompoundStmt* p_expr) override;
    Status visit(hzcc::AST::RelationalExpr* p_expr) override;
    Status visit(hzcc::AST::EmptyStmt* p_expr) override;
    Status visit(hzcc::AST::ArithmeticExpr* p_expr) override;
    Status visit(hzcc::AST::WhileStmt* p_expr) override;
    Status visit(hzcc::AST::BreakStmt* p_expr) override;
    Status visit(hzcc::AST::TernaryExpr* p_expr) override;
    Status visit(hzcc::AST::FuncDeclStmt* p_expr) override;
    Status visit(hzcc::AST::ContinueStmt* p_expr) override;
    Status visit(hzcc::AST::ArraySubscriptExpr* p_expr) override;
    /**######################################################
     * ## Stack Management                                  ##
     **#######################################################**/

    void PushReturnStack(const std::string& stackID);

    std::string ConsumeReturnStack();

  protected:
    const std::string& GetInputFileName();
    const std::string& GetCurrentClassName();
    inline OpHint GetLastOpHint() {
        auto current_hint = _prevOpType;
        _prevOpType = kOpHint_None;
        return current_hint;
    };

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
