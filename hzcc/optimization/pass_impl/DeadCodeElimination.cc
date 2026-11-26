#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-err58-cpp"
//
// Created by chen_ on 2022/4/18.
//

#include <iterator>

#include "optimization/PassManagerImpl.h"
#include "optimization/pass/FunctionPass.h"
namespace hzcc::Pass {
/**
 * @brief Remove statements following the first top-level return in a function body.
 */
class DeadCodeElimination : public FunctionPass {
  public:
    /**
     * @brief Transform a function body and report whether the pass succeeded and changed it.
     * @param F Owned function to inspect and modify in place.
     * @return Pass success and whether any statements were removed.
     */
    std::pair<bool, bool> RunOnFunction(
        std::unique_ptr<ast::FuncDeclStmt>& F) override {
        if (F->has_body()) {
            /**
             * Case: delete all code after return stmt
             */
            auto& body = *F->body_stmt();
            const auto& stmts = body.body_stmts();
            for (auto it = stmts.begin(); it != stmts.end(); ++it) {
                if ((*it)->stmt_type() == StmtType::RETURN) {
                    return {true, body.erase_suffix(std::next(it))};
                }
            }
        }
        return {true, false};
    }
};

// Register pass
static PassRegister<DeadCodeElimination> DCE("DeadCodeElimination",
                                             "Remove statements after return");
}  // namespace hzcc::Pass
#pragma clang diagnostic pop
