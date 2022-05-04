//
// Created by chen_ on 2022/4/10.
//
#include "AST/statement/value_decl.h"

#include "codegen/jvm/JVMGenerator.h"
#include "codegen/jvm/utils/TypeUtils.h"
namespace Hzcc::Codegen {
Status JVMGenerator::visit(Hzcc::AST::VarDecl* p_expr) {
    DVLOG(CODE_GEN_LEVEL) << "JVMGenerator::visit(VarDecl) do nothing";
    return Status::OkStatus();
}
}  // namespace Hzcc::Codegen
