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
    Status Generate(
        const std::string &intput,                                    // NOLINT
        const std::string &output,                                    // NOLINT
        const std::unique_ptr<AST::CompilationUnit> &unit) override;  // NOLINT

    bool InitSource();

    bool FinalizedSource();

    /**######################################################
     * ## AST Visitor                                      ##
     **#######################################################**/
    Status VisitAllAST(const std::unique_ptr<Hzcc::AST::CompilationUnit> &p_expr);

    Status visit(Hzcc::AST::VarDecl* p_expr) override;
    Status visit(Hzcc::AST::DeclNode* p_expr) override;
    Status visit(Hzcc::AST::AssignExpr* p_expr) override;
    Status visit(Hzcc::AST::CompoundStmt* p_expr) override;
    Status visit(Hzcc::AST::ArithmeticExpr* p_expr) override;
    Status visit(Hzcc::AST::FunctionDeclNode* p_expr) override;

    /**######################################################
     * ## Stack Management                                  ##
     **#######################################################**/

    int PushReturnStack();

    int ConsumeReturnStack();

    std::pair<int, char> GetStack(std::string name);

    int BindStack(std::string name, char type, bool is_local);

  protected:
    void IncLindeIndent();

    void DecLindeIndent();

    void AddToCache(const std::string &output);
    void EnterScope();
    void ExitScope();

    std::string GetAllCachedLines();

    [[nodiscard]] const std::string &GetLineIndent() const;

  private:
    std::string _indent;
    std::stringstream _output;
    constexpr static const char *_indent_str = "    ";
};

}  // namespace Hzcc::Codegen

#endif  // MYCC_SOURCE_CODEGEN_JVM_JVMGENERATOR_H_
