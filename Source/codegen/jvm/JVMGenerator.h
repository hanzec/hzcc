//
// Created by chen_ on 2022/4/10.
//

#ifndef MYCC_SOURCE_CODEGEN_JVM_JVMGENERATOR_H_
#define MYCC_SOURCE_CODEGEN_JVM_JVMGENERATOR_H_
#include <string_view>

#include "AST/ASTVisitor.h"
#include "codegen/Generator.h"
namespace Mycc::Codegen {
class JVMGenerator : public AST::ASTVisitor, public Generator {
  public:
    explicit JVMGenerator(const std::string &output_name,
                          const std::string &input_name);

    bool InitSource() override;

    bool FinalizedSource() override;

    void visit(Mycc::AST::VarDecl *p_expr) override;
    void visit(Mycc::AST::DeclNode *p_expr) override;
    void visit(Mycc::AST::AssignExpr *p_expr) override;
    void visit(Mycc::AST::CompoundStmt *p_expr) override;
    void visit(Mycc::AST::ArithmeticExpr *p_expr) override;
    void visit(Mycc::AST::FunctionDeclNode *p_expr) override;

    /**######################################################
     * ##Stack Management                                  ##
     **#######################################################**/

    int PushReturnStack();

    int ConsumeReturnStack();

    std::pair<int, char> GetStack(std::string name);

    int BindStack(std::string name, char type, bool is_local);

  protected:
    void IncLindeIndent();

    void DecLindeIndent();

    const std::string &GetLineIndent() const;

  private:
    std::string _indent;
    constexpr static const char *_indent_str = "    ";
};

}  // namespace Mycc::Codegen

#endif  // MYCC_SOURCE_CODEGEN_JVM_JVMGENERATOR_H_
