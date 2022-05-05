//
// Created by chen_ on 2022/4/10.
//

#ifndef MYCC_SOURCE_CODEGEN_JVM_JVMGENERATOR_H_
#define MYCC_SOURCE_CODEGEN_JVM_JVMGENERATOR_H_
#include <memory>
#include <string_view>

#include "AST/ASTVisitor.h"
#include "codegen/Generator.h"
#include "codegen/jvm/utils/macro.h"

namespace Hzcc::Codegen {
class JVMGenerator : public AST::ASTVisitor, public Generator {
  public:
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
    Status Generate(const std::string& output,
                    AST::CompilationUnit& unit) override;  // NOLINT

    /**######################################################
     * ## AST Visitor                                      ##
     **#######################################################**/
    Status visit(Hzcc::AST::VarDecl* p_expr) override;
    Status visit(Hzcc::AST::UnaryExpr* p_expr) override;
    Status visit(Hzcc::AST::ReturnNode* p_expr) override;
    Status visit(Hzcc::AST::AssignExpr* p_expr) override;
    Status visit(Hzcc::AST::DeclRefExpr* p_expr) override;
    Status visit(Hzcc::AST::LiteralExpr* p_expr) override;
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
    void IncLindeIndent();
    void DecLindeIndent();

    void AddToCache(const std::string& output);
    std::string GetAllCachedLine();
    const std::string& GetInputFileName();
    const std::string& GetCurrentClassName();

    [[nodiscard]] uint32_t GetStackID(std::string& name);

    [[nodiscard]] bool IsGlobalVar(const std::string& name);

    [[nodiscard]] std::string GetVarType(const std::string& name);

    [[nodiscard]] std::string SaveToVariable(const std::string& name);

    [[nodiscard]] std::string LoadFromVariable(const std::string& name);

  private:
    std::string _indent;
    std::stringstream _output;
    std::string _intput_file_name;
    std::string _current_class_name;

    bool _generate_load = false;
    bool _request_leave = false;
    constexpr static const char* _indent_str = "    ";

    std::list<std::string> _return_stack;

    /**
     * Varname ： [stackID, Type]
     */
    std::unordered_map<std::string, std::string> _global_vars;
    std::unordered_map<std::string, std::pair<int, std::string>> _local_vars;
};

}  // namespace Hzcc::Codegen

#endif  // MYCC_SOURCE_CODEGEN_JVM_JVMGENERATOR_H_
