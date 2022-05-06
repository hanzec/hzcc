//
// Created by chen_ on 2022/4/10.
//
#include "AST/statement/compound.h"
#include "AST/statement/function_decl.h"
#include "codegen/jvm/FuncAnalyzer.h"
#include "codegen/jvm/JVMGenerator.h"
#include "codegen/jvm/utils/TypeUtils.h"
#include "utils/message_utils.h"
namespace Hzcc::Codegen {
Status JVMGenerator::visit(AST::FunctionDeclNode* p_expr) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    DLOG_ASSERT(p_expr != nullptr) << "p_expr is nullptr";
    DLOG_ASSERT(!p_expr->GetName().empty())
        << "Function declaration does not have name";

    /** #####################################################################
     *  ### Code Generation                                               ###
     *  ##################################################################### */

    /**
     * We need to analyze the function to get the max stack size and max locals
     */
    FuncAnalyzer funcAnalyzer(_global_vars);

    if (p_expr->HasBody()) {
        /** #################################################################
         *  ## Generate Function Info #######################################
         *  ################################################################# */
        for (auto& param : p_expr->GetParams()) {
            param->visit(funcAnalyzer);
        }
        p_expr->GetBody()->visit(funcAnalyzer);

        /** #################################################################
         *  ## Function Checking ############################################
         *  ################################################################# */
        if (p_expr->GetType()->GetName() != "void" &&
            !funcAnalyzer.HasValidReturn()) {
            Message::print_message(
                Message::kFatalError,
                "Missing return in non-void function " + p_expr->GetName(),
                p_expr->Location());
            return {Status::Code::INVALID_ARGUMENT,
                    "Missing return in non-void function " + p_expr->GetName()};
        }

        /** #################################################################
         *  ## Function Header ##############################################
         *  ################################################################# */
        // we first generate the function body in order to get the stack depth
        // and variable count
        std::stringstream ss;
        ss << ".method public static " << p_expr->GetName() << " : "
           << _function_table[p_expr->GetName()].second;

        // write function header to the class file
        AddToCache(ss.str());

        /** #################################################################
         *  ## Function body ################################################
         *  ################################################################# */
        IncLindeIndent();
        auto locals = funcAnalyzer.GetLocalVariable();
        AddToCache(".code stack " +
                   std::to_string(funcAnalyzer.GetMaxStackSize()) + " locals " +
                   std::to_string(locals.size()));

        // write comment for local variables declaration
        IncLindeIndent();
        int index = 0;
        _local_vars.clear();  // reset the stack table
        for (auto& local : locals) {
            auto [variable_name, variable_type, line_number] = local;
            AddToCache(";; local " + std::to_string(index) + " " +
                       std::string(variable_name) + " " + GetInputFileName() +
                       " " + std::to_string(line_number));

            // insert to local variable declaration
            _local_vars.insert(
                {variable_name,
                 std::make_pair(_local_vars.size(), variable_type)});
            index++;
        }
        DecLindeIndent();

        // visit the function body
        HZCC_JVM_Visit_Node(p_expr->GetBody());
        AddToCache(".end code");
        DecLindeIndent();

        // write function end
        AddToCache(".end method");
    } else {
        DVLOG(CODE_GEN_LEVEL)
            << "Function " << p_expr->GetName() << " has no body";
    }

    return Status::OkStatus();
}

}  // namespace Hzcc::Codegen
