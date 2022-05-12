//
// Created by chen_ on 2022/4/18.
//
#include "PassManagerImpl.h"

#include <memory>

#include "AST/statement/compound.h"
#include "AST/statement/function_decl.h"
namespace Hzcc::Pass {
bool PassManagerImpl::reg_pass_internal(std::unique_ptr<PassBase> pass,
                                        const std::string& command,
                                        const std::string& description) {
    if (_registered_passes.find(command) != _registered_passes.end()) {
        return false;
    } else {
        _registered_passes.emplace(command);
        _pass_map.emplace_back(command, std::move(pass));
        return true;
    }
}

Status PassManagerImpl::RunPass(
    std::shared_ptr<AST::CompilationUnit> compile_unit) {
    // Run Function Pass
    for (auto& decl : compile_unit->GetDecls()) {
        DVLOG(OPT_LOG_LEVEL) << "Visit: " << decl.first;
        if (decl.second->IsFuncDecl()) {
            /**
             * Here we will temporarily move the ownership of the
             * function from ASTNode to current function temporary
             */
            auto func_decl = std::unique_ptr<AST::FunctionDeclNode>(
                dynamic_cast<AST::FunctionDeclNode*>(decl.second.release()));
            DLOG_ASSERT(func_decl != nullptr) << "FuncDecl cast failed";
            HZCC_ExceptOK_WITH_RETURN(RunFunctionPass(func_decl));
            decl.second = std::move(func_decl);
        }
    }

    // print final AST
    DVLOG(OPT_LOG_LEVEL) << "Final AST: " << std::endl << compile_unit->Dump();
    return Status::OkStatus();
}

Status PassManagerImpl::RunFunctionPass(
    std::unique_ptr<AST::FunctionDeclNode>& F) {
    DVLOG(OPT_LOG_LEVEL) << "Running Function Pass";
    DVLOG(OPT_LOG_LEVEL) << "Optimizing function: " << F->GetName();

    for (auto& pass : _pass_map) {
        if (pass.second->IsFunctionPass()) {
            auto [status, modified] =
                std::dynamic_pointer_cast<FunctionPass>(pass.second)
                    ->RunOnFunction(F);

            // check PASS result
            if (status) {
                DVLOG(OPT_LOG_LEVEL) << "Pass: " << pass.first << " finished";
            } else {
                DVLOG(OPT_LOG_LEVEL) << "Pass: " << pass.first << " failed";
                return {Status::Code::UNAVAILABLE,
                        "Pass: " + std::string(pass.first) + " failed"};
            }

            // print the AST if the pass is modified AST
            DVLOG_IF(OPT_LOG_LEVEL, modified)
                << "[" << pass.first << "]: After: \n"
                << F->Dump("").c_str();
        }
    }
    return Status::OkStatus();
}
}  // namespace Hzcc::Pass