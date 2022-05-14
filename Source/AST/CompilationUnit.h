#ifndef MYCC_AST_AST_H
#define MYCC_AST_AST_H
#include <memory>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "AST/ASTNode.h"
#include "AST/expr/decl_ref.h"
#include "AST/statement/function_decl.h"
#include "AST/type/Type.h"
#include "lexical/Token.h"
#include "utils/Status.h"

namespace Hzcc::AST {
class StructType;
class CompilationUnit {
  public:
    int a;
    explicit CompilationUnit(std::string file_name);

    CompilationUnit(CompilationUnit&&) = default;

    ~CompilationUnit();

    [[nodiscard]] std::string Dump() const;

    [[nodiscard]] std::string GetFileName() const;

    /**
     * @brief Add a new Declare Node to the AST
     *
     * @Note: if the Declare Node has a duplicate name, it will be replaced the
     * old one
     * @param node
     * @return
     */
    void addDecl(std::unique_ptr<DeclNode> node);

    std::list<std::pair<std::string, std::unique_ptr<AST::DeclNode>>>&
    GetDecls();

  protected:
    const std::string _file_name;

    std::list<std::pair<std::string, std::unique_ptr<AST::DeclNode>>>
        _global_decl;
};

}  // namespace Hzcc::AST

#endif  // MYCC_AST_AST_H
