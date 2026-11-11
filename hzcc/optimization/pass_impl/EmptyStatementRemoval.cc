//
// Created by chen_ on 2022/5/8.
//
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
/**
 * @brief Remove empty statements from the top-level body of a function.
 */
class EmptyStatementRemoval : public FunctionPass {
  public:
    /**
     * @brief Transform a function body and report whether the pass succeeded and changed it.
     * @param F Owned function to inspect and modify in place.
     * @return Pair of pass success and whether the function was modified.
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

/** @brief Register empty-statement removal with the legacy pass manager. */
static ::hzcc::utils::internal::Register<EmptyStatementRemoval,
                                         hzcc::Pass::PassManagerImpl>
    aRegiasdasd ("EmptyStatementRemoval",
                                "EmptyStatementRemoval");

}  // namespace Hzcc::Pass
#pragma clang diagnostic pop
