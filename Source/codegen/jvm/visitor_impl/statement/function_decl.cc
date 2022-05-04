//
// Created by chen_ on 2022/4/10.
//
#include "AST/statement/function_decl.h"

#include "AST/statement/compound.h"
#include "codegen/jvm/FuncAnalyzer.h"
#include "codegen/jvm/JVMGenerator.h"
#include "codegen/jvm/utils/TypeUtils.h"
namespace Hzcc::Codegen {
Status JVMGenerator::visit(AST::FunctionDeclNode* p_expr) {
    /**
     * We need to analyze the function to get the max stack size and max locals
     */
    FuncAnalyzer funcAnalyzer;

    // for function params
    for (auto& param : p_expr->GetParams()) {
        param->visit(funcAnalyzer);
    }

    // for function body
    p_expr->GetBody()->visit(funcAnalyzer);

    // we first generate the function body in order to get the stack depth
    // and variable count
    std::stringstream ss;
    ss << ".method public static " << p_expr->GetName() << " : (";

    // write function parameters
    for (auto& param : p_expr->getArguments()) {
        ss << Utils::GetJVMTypename(std::get<1>(param)->GetName());
    }

    // write function return type
    ss << ")" << Utils::GetJVMTypename(p_expr->GetType()->GetName());

    // write function header to the class file
    AddToCache(ss.str());

    // write function body
    auto locals = funcAnalyzer.GetLocalVariable();
    AddToCache(".code stack " + std::to_string(funcAnalyzer.GetMaxStackSize()) +
               " locals " + std::to_string(locals.size()));

    // write comment for local variables declaration
    int index = 0;
    for (auto& local : locals) {
        auto [variable_name, variable_type, line_number] = local;
        AddToCache(";; local " + std::to_string(index) + " " +
                   std::string(variable_name) + " " + GetInputFileName() + " " +
                   std::to_string(line_number));
    }

    return p_expr->GetBody()->visit(*this);
}

}  // namespace Hzcc::Codegen
