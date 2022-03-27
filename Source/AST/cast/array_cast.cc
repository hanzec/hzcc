//
// Created by chen_ on 2022/3/30.
//

#include "array_cast.h"
namespace Mycc::AST {
std::string Mycc::AST::ArrayUnsizeCast::GetNodeName() const {
    return "ArrayUnsizeCast";
}

Mycc::AST::ArrayUnsizeCast::ArrayUnsizeCast(std::unique_ptr<ASTNode> expr,
                                            const std::shared_ptr<Type>& node)
    : CastExpr(node, std::move(expr)) {}

}  // namespace Mycc::AST