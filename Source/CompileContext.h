//
// Created by chen_ on 2022/4/19.
//

#ifndef MYCC_SOURCE_COMPILECONTEXT_H_
#define MYCC_SOURCE_COMPILECONTEXT_H_
#include <list>

#include "codegen/Generator.h"
#include "optimization/PassManager.h"
#include "utils/Status.h"
namespace Mycc {
namespace AST {
class CompilationUnit;
}
class CompileContext {
  public:
    void AddCompileUnit(AST::CompilationUnit& unit);

    /**
     * @brief Run all available passes on the compilation unit
     *
     * @return Status
     */
    template <class PassMgr, typename std::enable_if<std::is_base_of<
                                 Pass::PassManager, PassMgr>::value>>
    Status Optmize();

    /**
     * @brief Run all available passes on the compilation unit
     * @tparam Generator
     * @tparam PassMgr
     * @param output_file
     * @return
     */
     // TODO: add template base class limitation
    template <typename Generator, typename PassMgr>
    Status Compile(const std::string& output_file);

  private:
    bool _enable_optimization = true;
    std::list<AST::CompilationUnit> _units;
};
}  // namespace Mycc

#endif  // MYCC_SOURCE_COMPILECONTEXT_H_
