//
// Created by chen_ on 2022/4/10.
//
#include "AST/statement/compound.h"
#include "AST/statement/function_decl.h"
#include "codegen/jvm/FuncAnalyzer.h"
#include "codegen/jvm/JVMGenerator.h"
#include "codegen/jvm/utils/TypeUtils.h"
namespace Hzcc::Codegen {
Status JVMGenerator::visit(AST::FunctionDeclNode* p_expr) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */

    /** #####################################################################
     *  ### Code Generation                                               ###
     *  ##################################################################### */

    /**
     * We need to analyze the function to get the max stack size and max locals
     */
    FuncAnalyzer funcAnalyzer;

    /** #####################################################################
     *  ## Function Header ##################################################
     *  ##################################################################### */
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
        ss << Utils::GetTypeName(std::get<1>(param));
    }

    // write function return type
    ss << ")" << Utils::GetTypeName(p_expr->GetType());

    // write function header to the class file
    AddToCache(ss.str());

    /** #####################################################################
     *  ## Function body ####################################################
     *  ##################################################################### */
    IncLindeIndent();
    auto locals = funcAnalyzer.GetLocalVariable();
    AddToCache(".code stack " + std::to_string(funcAnalyzer.GetMaxStackSize()) +
               " locals " + std::to_string(locals.size()));

    // write comment for local variables declaration
    IncLindeIndent();
    int index = 0;
    _local_vars.clear();  // reset the stack table
    for (auto& local : locals) {
        auto [variable_name, variable_type, line_number] = local;
        AddToCache(";; local " + std::to_string(index) + " " +
                   std::string(variable_name) + " " + GetInputFileName() + " " +
                   std::to_string(line_number));

        // insert to local variable declaration
        _local_vars.insert(
            {variable_name, std::make_pair(_local_vars.size(), variable_type)});
        index++;
    }
    DecLindeIndent();

    // visit the function body
    HZCC_JVM_Visit_Node(p_expr->GetBody());
    AddToCache(".end code");
    DecLindeIndent();

    // write function end
    AddToCache(".end method");

    return Status::OkStatus();
}

}  // namespace Hzcc::Codegen
