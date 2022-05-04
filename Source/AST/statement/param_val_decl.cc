//
// Created by chen_ on 2022/3/29.
//

#include "param_val_decl.h"
namespace Hzcc::AST {
std::string ParamVarDecl::GetNodeName() const { return "ParamVarDecl"; }
Status ParamVarDecl::visit(ASTVisitor& visitor) { return visitor.visit(this); }
}  // namespace Hzcc::AST
