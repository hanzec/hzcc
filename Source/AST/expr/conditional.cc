//
// Created by chen_ on 2022/3/29.
//
#include "conditional.h"

namespace Mycc::AST {

std::string AST::ConditionalExpr::GetNodeName() const { return "ConditionalExpr"; }
std::shared_ptr<Type> ConditionalExpr::GetType() const {
    return _true_expr->GetType();
}
}