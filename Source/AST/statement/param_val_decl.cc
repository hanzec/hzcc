//
// Created by chen_ on 2022/3/29.
//

#include "param_val_decl.h"
namespace Hzcc::AST {
const char* ParamVarDecl::GetNodeName() const { return "ParamVarDecl"; }
Status ParamVarDecl::visit(ASTVisitor& visitor) { return visitor.visit(this); }
}  // namespace Hzcc::AST
