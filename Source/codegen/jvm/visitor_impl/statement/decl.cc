//
// Created by chen_ on 2022/4/10.
//
#include "AST/statement/decl.h"

#include "AST/type/Type.h"
#include "codegen/jvm/JVMGenerator.h"
#include "codegen/jvm/utils/TypeUtils.h"

namespace Hzcc::Codegen {
Status JVMGenerator::visit(std::unique_ptr<Hzcc::AST::DeclNode>&p_expr) {
    auto var_name = p_expr->GetName();
    BindStack(var_name, TypeUtils::GetJVMTypename(p_expr->GetType()->GetName()),
              true);
}
}  // namespace Hzcc::Codegen
