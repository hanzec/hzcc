//
// Created by chen_ on 2022/4/10.
//
#include "AST/statement/function_decl.h"

#include "codegen/jvm/JVMGenerator.h"
#include "codegen/jvm/utils/TypeUtils.h"

namespace Mycc::Codegen {
void JVMGenerator::visit(Mycc::AST::FunctionDeclNode *p_expr) {
    auto &file_handler = GetOstream();

    // we first generate the function body in order to get the stack depth and
    // variable count
    std::stringstream ss;
    RedirectOutputStream(ss);
//    p_expr->GetBody()->visit(*this);

    // write function header
    file_handler << ".method public static " << p_expr->GetName() << " : (";

    // write function parameters
    for (auto &param : p_expr->getArguments()) {
        file_handler << TypeUtils::GetJVMTypename(
            std::get<1>(param)->GetName());
    }

    // write function return type
    file_handler << ")"
                 << TypeUtils::GetJVMTypename(p_expr->GetType()->GetName())
                 << "\n";
}

}  // namespace Mycc::Codegen
