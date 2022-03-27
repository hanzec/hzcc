//
// Created by chen_ on 2022/3/29.
//
#include "sizeof.h"

namespace Mycc::AST {

std::string AST::SizeofExpr::GetNodeName() const { return "SizeofExpr"; }
std::shared_ptr<Type> SizeofExpr::GetType() const {
    return std::make_shared<Type>("int");
}
}  // namespace Mycc::AST