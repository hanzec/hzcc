#ifndef MYCC_FUNCTION_NODE_H
#define MYCC_FUNCTION_NODE_H
#include <algorithm>
#include <list>
#include <memory>
#include <string_view>
#include <utility>
#include <vector>

#include "AST/ASTNode.h"
#include "AST/type/Type.h"
#include "decl.h"
#include "param_val_decl.h"

namespace Hzcc::AST {
using ArgumentList =
    std::list<std::tuple<std::string, std::shared_ptr<AST::Type>, int>>;

class FunctionDeclNode : public DeclNode {
  public:
    /**
     * @brief Construct a new Function Prototype For Function Node
     *
     * @param name the name of the function
     * @param return_type the return type of the function
     * @param arguments the arguments of the function
     */
    FunctionDeclNode(const Lexical::Token& function_name,    // NOLINT
                     std::shared_ptr<Type> return_type,      // NOLINT
                     std::list<Lexical::Token>& attribute);  // NOLINT

    ~FunctionDeclNode() override = default;

    Status visit(ASTVisitor& visitor) override;

    bool set_body(std::unique_ptr<AST::CompoundStmt> declaration);

    std::unique_ptr<AST::CompoundStmt>& GetBody();

    ArgumentList getArguments();

    [[nodiscard]] bool HasBody() const override;

    [[nodiscard]] std::shared_ptr<Type> GetType() const override;

#ifdef NDEBUG
    [[nodiscard]] std::string Dump(std::string_view ident) const override;
#endif

    [[nodiscard]] bool IsFuncDecl() const override;

    bool AddFunctionArgument(std::unique_ptr<ParamVarDecl> type);

  protected:
    [[nodiscard]] std::string GetNodeName() const override;
    [[nodiscard]] std::string PrintAdditionalInfo(
        std::string_view ident) const override;

  private:
    std::shared_ptr<Type> _return_type;
    std::unique_ptr<CompoundStmt> _function_body;
    std::list<std::unique_ptr<ParamVarDecl>> _function_param;
};
}  // namespace Hzcc::AST
#endif  // MYCC_FUNCTION_NODE_H
