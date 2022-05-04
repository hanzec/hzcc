//
// Created by chen_ on 2022/4/10.
//
#include "AST/statement/value_decl.h"

#include "codegen/jvm/JVMGenerator.h"
#include "codegen/jvm/utils/TypeUtils.h"
namespace Hzcc::Codegen {
Status JVMGenerator::visit(std::unique_ptr<Hzcc::AST::VarDecl> &p_expr) {}

}  // namespace Hzcc::Codegen
