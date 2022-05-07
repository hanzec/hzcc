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
class JVMGenerator : public Generator,
                     public AST::ASTVisitor,
                     protected LabelManager,
                     protected IndentWriter,
                     protected JVM::StackManager {
  public:
    JVMGenerator(const std::string& output, AST::CompilationUnit& unit);

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
    Status visit(Hzcc::AST::DeclRefExpr* p_expr) override;
    Status visit(Hzcc::AST::LiteralExpr* p_expr) override;
    Status visit(Hzcc::AST::FunctionCall* p_expr) override;
    Status visit(Hzcc::AST::ForStatement* p_expr) override;
    Status visit(Hzcc::AST::CompoundStmt* p_expr) override;
    Status visit(Hzcc::AST::EmptyStatement* p_expr) override;
    Status visit(Hzcc::AST::ArithmeticExpr* p_expr) override;
    Status visit(Hzcc::AST::FunctionDeclNode* p_expr) override;
    Status visit(Hzcc::AST::ArraySubscriptExpr* p_expr) override;
    /**######################################################
     * ## Stack Management                                  ##
     **#######################################################**/

    void PushReturnStack(const std::string& stackID);

    std::string ConsumeReturnStack();

  protected:
    const std::string& GetInputFileName();
    const std::string& GetCurrentClassName();

  private:
    std::string _input_file_name;
    std::string _current_class_name;

    bool _generate_load = false;
    bool _request_leave = false;

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
