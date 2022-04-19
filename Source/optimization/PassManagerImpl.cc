//
// Created by chen_ on 2022/4/18.
//

#include "PassManagerImpl.h"
namespace Mycc::Pass {
Status PassManagerImpl::RunFunctionPass() { return Status(Status::NOT_FOUND); }

bool PassManagerImpl::reg_pass_internal(std::unique_ptr<PassBase>,
                                        const std::string_view& command,
                                        const std::string_view& description) {
    return false;
}
}  // namespace Mycc::Pass