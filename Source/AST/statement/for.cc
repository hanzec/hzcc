//
// Created by chen_ on 2022/3/29.
//
#include "for.h"
namespace Mycc::AST{
std::string ForStatement::GetNodeName() const { return "ForStatement"; }


#ifdef NDEBUG
std::string ForStatement::Dump(std::string_view ident) const {
    std::string ret;

    // print parameter
    ret += _body->Dump(ident) + "\n";
    return ret;
}
#endif
}
