//
// Created by chen_ on 2022/4/18.
//
#include "AST/statement/compound.h"
#include "AST/statement/function_decl.h"
#include "optimization/PassManagerImpl.h"
#include "optimization/pass/FunctionPass.h"
#include "utils/logging.h"
namespace Mycc::Pass {
class DeadCodeElimination : public FunctionPass {
  public:
    bool runOnFunction(std::unique_ptr<AST::FunctionDeclNode>& F) override {
        /**
         * Case: delete all code after return statement
         */
        auto& body = F->GetBody();
        auto& stmts = body->GetBodyStatements();
        for (auto it = stmts.begin(); it != stmts.begin(); ++it) {
            if ((*it)->GetNodeName() == "ReturnNode") {
                stmts.erase(it, stmts.end());
                break;
            }
        }
        return true;
    }
};

// Register pass
static RegisterPass<DeadCodeElimination> DCE("DeadCodeElimination",
                                             "DeadCodeElimination");
}  // namespace Mycc::Pass