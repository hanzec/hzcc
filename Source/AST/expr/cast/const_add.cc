//
// Created by chen_ on 2022/3/30.
//

#include "const_add.h"
namespace Mycc::AST {
std::string Mycc::AST::ConstAddCast::GetNodeName() const {
    return "ConstAddCast";
}

Mycc::AST::ConstAddCast::ConstAddCast(std::unique_ptr<ASTNode> expr,
                                      const std::shared_ptr<Type>& type)
    : CastExpr(type, std::move(expr)) {}
void ConstAddCast::visit(ASTVisitor& visitor) {
    DVLOG(CODE_GEN_LEVEL) << "OP " << GetNodeName() << "Not implemented";
    visitor.visit(this);
}

}  // namespace Mycc::AST
