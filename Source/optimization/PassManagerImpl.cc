//
// Created by chen_ on 2022/4/18.
//

#include "PassManagerImpl.h"
namespace Hzcc::Pass {
bool PassManagerImpl::reg_pass_internal(std::unique_ptr<PassBase>,
                                        const std::string_view& command,
                                        const std::string_view& description) {
    return false;
}
Status PassManagerImpl::RunFunctionPass(
    std::unique_ptr<AST::FunctionDeclNode>& F) {
    return Status(Status::NOT_FOUND);
}
}  // namespace Hzcc::Pass