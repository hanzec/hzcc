//
// Created by chen_ on 2022/4/18.
//
#include "PassManagerImpl.h"

#include <memory>

#include "AST/statement/compound.h"
#include "AST/statement/function_decl.h"
namespace Hzcc::Pass {
bool PassManagerImpl::reg_pass_internal(std::unique_ptr<PassBase>,
                                        const std::string_view& command,
                                        const std::string_view& description) {
    return false;
}

Status PassManagerImpl::RunPass(AST::CompilationUnit& compile_unit) {
    // Run Function Pass
    for (auto& decl : compile_unit.GetDecls()) {
        DVLOG(OPT_LOG_LEVEL) << "Visit: " << decl.first;
        if (decl.second->IsFuncDecl()) {
            /**
             * Here we will temporarily move the ownership of the
             * function from ASTNode to current function temporary
             */
            auto func_decl = std::unique_ptr<AST::FunctionDeclNode>(
                dynamic_cast<AST::FunctionDeclNode*>(decl.second.release()));
            HZCC_ExceptOK_WITH_RETURN(RunFunctionPass(func_decl));
            decl.second = std::move(func_decl);
        }
    }
    return Status::OkStatus();
}

Status PassManagerImpl::RunFunctionPass(
    std::unique_ptr<AST::FunctionDeclNode>& F) {
    DVLOG(OPT_LOG_LEVEL) << "Running Function Pass";
    DVLOG(OPT_LOG_LEVEL) << "Optimizing function: " << F->GetName();

    for (auto& pass : _pass_map) {
        if (pass.second->IsFunctionPass()) {
            DVLOG(OPT_LOG_LEVEL) << "Running pass: " << pass.first;
            if (std::dynamic_pointer_cast<FunctionPass>(pass.second)
                    ->RunOnFunction(F)) {
                DVLOG(OPT_LOG_LEVEL) << "Pass: " << pass.first << " finished";
            } else {
                DVLOG(OPT_LOG_LEVEL) << "Pass: " << pass.first << " failed";
                return {Status::Code::UNAVAILABLE,
                        "Pass: " + std::string(pass.first) + " failed"};
            }
        }
    }
    return Status::OkStatus();
}
}  // namespace Hzcc::Pass