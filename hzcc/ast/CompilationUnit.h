#include <memory>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "ast/Stmt.h"
#include "ast/expr/Expr.h"
#include "ast/type/Type.h"
#include "utils/status/status.h"

#ifndef HZCC_AST_AST_H
#define HZCC_AST_AST_H
namespace hzcc::ast {
class StructType;
class CompilationUnit {
  public:
    explicit CompilationUnit(std::string file_name);

    CompilationUnit(CompilationUnit&&) = default;

    ~CompilationUnit();

    void Dump(std::ostream& out) const;

    [[nodiscard]] std::string GetFileName() const;

    /**
     * @brief Add a new Declare Node to the ast
     *
     * @Note: if the Declare Node has a duplicate name, it will be replaced the
     * old one
     * @param node
     * @return
     */
    void addDecl(std::unique_ptr<IDeclStmt> node);

    std::list<std::pair<std::string, std::unique_ptr<ast::IDeclStmt>>>&
    GetDecls();

  protected:
    const std::string _file_name;

    std::list<std::pair<std::string, std::unique_ptr<ast::IDeclStmt>>>
        _global_decl;
};

}  // namespace hzcc::ast

#endif  // HZCC_AST_AST_H
