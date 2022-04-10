//
// Created by chen_ on 2022/3/29.
//

#include "param_val_decl.h"
namespace Mycc::AST {
std::string ParamVarDecl::GetNodeName() const { return "ParamVarDecl"; }
void ParamVarDecl::visit(ASTVisitor& visitor) {
    DVLOG(CODE_GEN_LEVEL) << "OP " << GetNodeName() << "Not implemented";
    visitor.visit(this);
}
}  // namespace Mycc::AST
