//
// Created by chen_ on 2022/3/30.
//

#include "const_add.h"
namespace Mycc::AST {
std::string Mycc::AST::ConstAddCast::GetNodeName() const {
    return "ConstAddCast";
}

Mycc::AST::ConstAddCast::ConstAddCast(std::unique_ptr<ASTNode> expr,
                                      const std::shared_ptr<Type>& node)
    : CastExpr(node, std::move(expr)) {}

}  // namespace Mycc::AST