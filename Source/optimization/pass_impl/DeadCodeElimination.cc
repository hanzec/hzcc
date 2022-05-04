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
class DeadCodeElimination : public FunctionPass {
  public:
    bool RunOnFunction(std::unique_ptr<AST::FunctionDeclNode>& F) override {
        /**
         * Case: delete all code after return statement
         */
        DVLOG(OPT_LOG_LEVEL) << "[DeadCodeElimination] cast 1: Before: \n"
                             << F->Dump("").c_str();
        auto& body = F->GetBody();
        auto& stmts = body->GetBodyStatements();
        for (auto it = stmts.begin(); it != stmts.begin(); ++it) {
            if (typeid((*it)->GetNodeName()) == typeid(AST::ReturnNode)) {
                stmts.erase(it, stmts.end());
                break;
            }
        }
        DVLOG(OPT_LOG_LEVEL) << "[DeadCodeElimination] cast 1: After: \n"
                             << F->Dump("").c_str();
        return true;
    }
};

// Register pass
static RegisterPass<DeadCodeElimination> DCE("DeadCodeElimination",
                                             "DeadCodeElimination");
}  // namespace Hzcc::Pass
#pragma clang diagnostic pop