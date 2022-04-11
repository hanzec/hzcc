//
// Created by chen_ on 2022/4/10.
//
#include "AST/statement/value_decl.h"

#include "codegen/jvm/JVMGenerator.h"
#include "codegen/jvm/utils/TypeUtils.h"
namespace Mycc::Codegen {
void JVMGenerator::visit(Mycc::AST::VarDecl *p_expr) {
    auto var_name = p_expr->GetName();
    auto stack_id = BindStack(
        var_name, TypeUtils::GetJVMTypename(p_expr->GetType()->GetName()),
        true);
}
int JVMGenerator::PushReturnStack() { return 0; }
int JVMGenerator::ConsumeReturnStack() { return 0; }
std::pair<int, char> JVMGenerator::GetStack(std::string name) {
    return std::pair<int, char>();
}
int JVMGenerator::BindStack(std::string name, char type, bool is_local) {
    return 0;
}
}  // namespace Mycc::Codegen
