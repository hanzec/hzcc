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
    JVM::FuncAnalyzer funcAnalyzer(GetGlobalVars());

    /**
     * Pre steps:
     */
    ResetLables();

    if (p_expr->HasBody()) {
        /** #################################################################
         *  ## Generate Function Info #######################################
         *  ################################################################# */
        // visiting function parameters
        std::stringstream func_signature;
        func_signature << '(';
        for (auto& param : p_expr->GetParams()) {
            param->visit(funcAnalyzer);
            func_signature << Utils::GetTypeName(param->GetType());
        }
        func_signature << ')' << Utils::GetTypeName(p_expr->GetType());

        // visiting function body
        p_expr->Body()->visit(funcAnalyzer);

        // add current function to function table
        auto final_func_signature = func_signature.str();
        std::transform(final_func_signature.begin(), final_func_signature.end(),
                       final_func_signature.begin(), ::toupper);
        _function_table[p_expr->GetName()] = {GetCurrentClassName(),
                                              final_func_signature};

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
        ResetLocalVars();  // reset the stack table
        int index = 0;
        for (auto& local : locals) {
            // if variable is function argument or local variable
            if (local.size() == 1) {
                auto [name, type, line_no] = local.front();
                // insert to local variable declaration
                AddLocalVar(index, name, type);

                // write debug comment
                AddToCache(";; local " + std::to_string(GetStackID(name)) +
                           " " + std::string(name) + " " + GetInputFileName() +
                           " " + std::to_string(line_no));
            }
            // if variable is local temporary variable
            else {
                for (auto& temp : local) {
                    auto [name, type, line_no] = temp;
                    auto new_name = name + "_" + std::to_string(line_no);
                    // insert to local tmp variable declaration
                    AddLocalTmpVar(index, new_name, type);

                    // write debug comment
                    AddToCache(";; local(tmp) " +
                               std::to_string(GetStackID(new_name)) + " " +
                               std::string(name) + " " + GetInputFileName() +
                               " " + std::to_string(line_no));
                }
            }
            index += 1;
        }

        // visit the function body
        HZCC_JVM_Visit_Node(p_expr->Body());

        // generate return statement if last statement is not return
        auto& last_stmt = p_expr->Body()->GetLastStatement();
        if (!last_stmt->IsReturn()) {
            AddToCache("return");
        }

        /** #################################################################
         *  ## Line Number Table ############################################
         *  ################################################################# */
        AddToCache(".linenumbertable");
        IncLindeIndent();
        for (auto& tag_pair : GetLineNumberTags()) {
            AddToCache(tag_pair.first + " " + std::to_string(tag_pair.second));
        }
        DecLindeIndent();
        AddToCache(".end linenumbertable");

        // write function end
        DecLindeIndent();
        AddToCache(".end code");

        DecLindeIndent();
        AddToCache(".end method");
    } else {
        DVLOG(CODE_GEN_LEVEL)
            << "Function " << p_expr->GetName() << " has no body";
    }

    return Status::OkStatus();
}

}  // namespace Hzcc::Codegen
