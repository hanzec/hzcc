//
// Created by chen_ on 2022/4/19.
//

#include "CompileContext.h"

#include "AST/CompilationUnit.h"
#include "optimization/PassManager.h"
#include "utils/logging.h"
namespace Mycc {
template <
    typename PassMgr,
    typename std::enable_if<std::is_base_of<Pass::PassManager, PassMgr>::value>>
Status CompileContext::Optmize() {
    // Run Function Pass
    DVLOG(OPT_LOG_LEVEL) << "Running Function Pass";
    for (auto &compile_unit : _units) {
        for (auto &decl : compile_unit.GetDecls()) {
            DVLOG(OPT_LOG_LEVEL) << "Visit: " << decl.first;
            if (decl.second->IsFuncDecl()) {
                /**
                 * Here we will temporarily move the ownership of the function
                 * from ASTNode to current function temporary
                 */
                DVLOG(OPT_LOG_LEVEL) << "Optimizing function: " << decl.first;
                auto func_decl = std::unique_ptr<AST::FunctionDeclNode>(
                    dynamic_cast<AST::FunctionDeclNode *>(
                        decl.second.release()));
                PassMgr::RunFunctionPass(func_decl.get());
                decl.second = std::move(func_decl);
            }
        }
    }
    return Status::OkStatus();
}

void CompileContext::AddCompileUnit(AST::CompilationUnit &unit) {
    _units.push_back(std::move(unit));
}

template <
    typename Gen, typename PassMgr>
Status CompileContext::Compile(const std::string &output_file) {
    static Gen generator;

    if (_enable_optimization) {
        DVLOG(OPT_LOG_LEVEL) << "Running Optimization Pass";
        Optmize<PassMgr>();
    } else {
        DVLOG(OPT_LOG_LEVEL) << "Optimization is disabled";
    }

    // generate code for each compilation unit
    for (auto &compile_unit : _units) {
        DVLOG(OPT_LOG_LEVEL)
            << "Generating code for: " << compile_unit.GetFileName();
        generator.Generate(compile_unit.GetFileName(), output_file,
                           compile_unit);
    }
}

}  // namespace Mycc