#include <memory>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include "AST/ASTNode.h"
#include "AST/expr/DeclRefExpr.h"
#include "AST/stmt/FuncDeclStmt.h"
#include "AST/type/Type.h"
#include "lexical/Token.h"
#include "utils/Status.h"

#ifndef HZCC_AST_AST_H
#define HZCC_AST_AST_H
namespace Hzcc::AST {
class StructType;
class CompilationUnit {
  public:
    explicit CompilationUnit(std::string file_name);

    CompilationUnit(CompilationUnit&&) = default;

    ~CompilationUnit();

    void Dump(std::ostream& out) const;

    [[nodiscard]] std::string GetFileName() const;

    /**
     * @brief Add a new Declare Node to the AST
     *
     * @Note: if the Declare Node has a duplicate name, it will be replaced the
     * old one
     * @param node
     * @return
     */
    void addDecl(std::unique_ptr<DeclStmt> node);

    std::list<std::pair<std::string, std::unique_ptr<AST::DeclStmt>>>&
    GetDecls();

  protected:
    const std::string _file_name;

    std::list<std::pair<std::string, std::unique_ptr<AST::DeclStmt>>>
        _global_decl;
};

}  // namespace Hzcc::AST

#endif  // HZCC_AST_AST_H
