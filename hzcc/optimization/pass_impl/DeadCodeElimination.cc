#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
//
// Created by chen_ on 2022/4/18.
//

#include "optimization/PassManagerImpl.h"
#include "optimization/pass/FunctionPass.h"
#include "utils/logging.h"
namespace hzcc::Pass {
/**
 * @brief Remove statements following the first top-level return in a function body.
 */
class DeadCodeElimination : public FunctionPass {
  public:
    /**
     * @brief Transform a function body and report whether the pass succeeded and changed it.
     * @param F Owned function to inspect and modify in place.
     * @return {true, found_return}; the second flag is true even when the return is already last.
     */
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
static PassRegister<DeadCodeElimination> DCE("DeadCodeElimination",
                                             "DeadCodeElimination");
}  // namespace hzcc::Pass
#pragma clang diagnostic pop
