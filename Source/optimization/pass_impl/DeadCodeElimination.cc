#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
//
// Created by chen_ on 2022/4/18.
//
#include "AST/statement/CompoundStmt.h"
#include "AST/statement/FuncDeclStmt.h"
#include "AST/statement/ReturnStmt.h"
#include "optimization/PassManagerImpl.h"
#include "optimization/pass/FunctionPass.h"
#include "utils/logging.h"
namespace Hzcc::Pass {
class DeadCodeElimination : public FunctionPass {
  public:
    std::pair<bool, bool> RunOnFunction(
        std::unique_ptr<AST::FuncDeclStmt>& F) override {
        if (!F->HasBody()) {
            VLOG(OPT_LOG_LEVEL)
                << "Function " << F->GetName() << " has no body";
            return {true, false};
        }

        bool modified = false;
        /**
         * Case: delete all code after return stmt
         */
        auto& body = F->Body();
        auto& stmts = body->GetBodyStatements();
        for (auto it = stmts.begin(); it != stmts.end(); ++it) {
            if ((*it)->IsReturn()) {
                modified = true;
                stmts.erase(++it, stmts.end());
                break;
            }
        }
        return {true, modified};
    }
};

// Register pass
static RegisterPass<DeadCodeElimination> DCE("DeadCodeElimination",
                                             "DeadCodeElimination");
}  // namespace Hzcc::Pass
#pragma clang diagnostic pop
