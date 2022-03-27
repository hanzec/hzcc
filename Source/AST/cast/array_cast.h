//
// Created by chen_ on 2022/3/30.
//
#include "AST/expr/cast.h"
#ifndef MYCC_SOURCE_AST_CAST_ARRAY_CAST_H_
#define MYCC_SOURCE_AST_CAST_ARRAY_CAST_H_
namespace Mycc::AST {
class ArrayUnsizeCast : public CastExpr {
  public:
    explicit ArrayUnsizeCast(std::unique_ptr<ASTNode> expr,
                             const std::shared_ptr<Type>& node);

  protected:
    [[nodiscard]] std::string GetNodeName() const override;

  private:
    std::string _field;
    bool _ptr_access = false;
    std::unique_ptr<ASTNode> _varaible;
};
}  // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_CAST_ARRAY_CAST_H_
