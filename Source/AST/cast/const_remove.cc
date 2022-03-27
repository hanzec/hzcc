//
// Created by chen_ on 2022/3/29.
//

#include "const_remove.h"
namespace Mycc::AST {
std::string Mycc::AST::ConstRemoveCast::GetNodeName() const {
    return "ConstRemoveCast";
}

Mycc::AST::ConstRemoveCast::ConstRemoveCast(std::unique_ptr<ASTNode> expr,
                                            const std::shared_ptr<Type>& node)
    : CastExpr(node, std::move(expr)) {}

}  // namespace Mycc::AST
