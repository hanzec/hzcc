//
// Created by chen_ on 2022/5/8.
//
#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
//
// Created by chen_ on 2022/4/18.
//
#include "AST/statement/compound.h"
#include "AST/statement/function_decl.h"
#include "AST/statement/return.h"
#include "optimization/PassManagerImpl.h"
#include "optimization/pass/FunctionPass.h"
#include "utils/logging.h"
namespace Hzcc::Pass {
class EmptyStatementRemoval : public FunctionPass {
  public:
    std::pair<bool, bool> RunOnFunction(
        std::unique_ptr<AST::FunctionDeclNode>& F) override {
        if (!F->HasBody()) {
            VLOG(OPT_LOG_LEVEL)
                << "Function " << F->GetName() << " has no body";
            return {true, false};
        }

        bool modified = false;
        /**
         * Case: delete all code after return statement
         */
        auto& body = F->Body();
        auto& stmts = body->GetBodyStatements();
        auto iter = stmts.begin();
        while (iter != body->GetBodyStatements().end()) {
            if ((*iter)->IsEmptyStmt()) {
                modified = true;
                stmts.erase(iter++);
            } else {
                ++iter;
            }
        }
        return {true, modified};
    }
};

// Register pass
static RegisterPass<EmptyStatementRemoval> DCE("EmptyStatementRemoval",
                                               "Delete all empty statements");
}  // namespace Hzcc::Pass
#pragma clang diagnostic pop
