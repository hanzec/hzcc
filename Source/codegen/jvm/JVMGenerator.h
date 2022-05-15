//
// Created by chen_ on 2022/4/10.
//

#ifndef MYCC_SOURCE_CODEGEN_JVM_JVMGENERATOR_H_
#define MYCC_SOURCE_CODEGEN_JVM_JVMGENERATOR_H_
#include <memory>
#include <string_view>

#include "AST/ASTVisitor.h"
#include "codegen/Generator.h"
#include "codegen/IndentWriter.h"
#include "codegen/LabelManager.h"
#include "codegen/jvm/StackManager.h"
#include "codegen/jvm/utils/macro.h"

namespace Hzcc::Codegen {
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
                     protected IndentWriter,
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
     * @return Status   The status of the generation.
     */
    Status Generate() override;  // NOLINT

    /**######################################################
     * ## AST Visitor                                      ##
     **#######################################################**/
    Status visit(Hzcc::AST::VarDecl* p_expr) override;
    Status visit(Hzcc::AST::CastExpr* p_expr) override;
    Status visit(Hzcc::AST::UnaryExpr* p_expr) override;
    Status visit(Hzcc::AST::ReturnNode* p_expr) override;
    Status visit(Hzcc::AST::AssignExpr* p_expr) override;
    Status visit(Hzcc::AST::LogicalExpr* p_expr) override;
    Status visit(Hzcc::AST::DoStatement* p_expr) override;
    Status visit(Hzcc::AST::IfStatement* p_expr) override;
    Status visit(Hzcc::AST::DeclRefExpr* p_expr) override;
    Status visit(Hzcc::AST::LiteralExpr* p_expr) override;
    Status visit(Hzcc::AST::FunctionCall* p_expr) override;
    Status visit(Hzcc::AST::ForStatement* p_expr) override;
    Status visit(Hzcc::AST::CompoundStmt* p_expr) override;
    Status visit(Hzcc::AST::RelationalExpr* p_expr) override;
    Status visit(Hzcc::AST::EmptyStatement* p_expr) override;
    Status visit(Hzcc::AST::ArithmeticExpr* p_expr) override;
    Status visit(Hzcc::AST::WhileStatement* p_expr) override;
    Status visit(Hzcc::AST::BreakStatement* p_expr) override;
    Status visit(Hzcc::AST::TernaryExpr* p_expr) override;
    Status visit(Hzcc::AST::FunctionDeclNode* p_expr) override;
    Status visit(Hzcc::AST::ContinueStatement* p_expr) override;
    Status visit(Hzcc::AST::ArraySubscriptExpr* p_expr) override;
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

}  // namespace Hzcc::Codegen

#endif  // MYCC_SOURCE_CODEGEN_JVM_JVMGENERATOR_H_
