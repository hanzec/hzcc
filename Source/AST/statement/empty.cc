//
// Created by chen_ on 2022/3/29.
//
#include "empty.h"
#include "AST/type/type.h"

namespace Mycc::AST {

std::string AST::EmptyStatement::GetNodeName() const { return "EmptyStatement"; }
std::shared_ptr<Type> EmptyStatement::GetType() const {
    return std::make_shared<Type>("void");
}
}