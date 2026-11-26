//
// Created by chen_ on 2022/4/18.
//
#include "PassManagerImpl.h"

#include "optimization/pass/FunctionPass.h"

namespace hzcc::Pass {
Status PassManagerImpl::RunPass(
    std::shared_ptr<ast::CompilationUnit> compile_unit) {
    if (!compile_unit) return InvalidArgumentError("Missing compilation unit");
    const auto results = for_each_product([&](const std::string& name) {
        if (_pass_map.find(name) == _pass_map.end()) {
            auto pass = create(name);
            if (!pass) return InternalError("Cannot create registered pass: " + name);
            _pass_map.emplace(name, std::move(pass));
        }
        return NoError();
    });
    for (const auto& result : results) {
        if (!result.ok()) return result;
    }

    // Run Function Pass
    for (auto& entry : compile_unit->_func_tbl) {
        HZCC_CHECK_OK_OR_RETURN(RunFunctionPass(entry.second));
    }
    return NoError();
}

Status PassManagerImpl::RunFunctionPass(std::unique_ptr<ast::FuncDeclStmt>& F) {
    for (const auto& [name, pass] : _pass_map) {
        if (pass->IsFunctionPass()) {
            auto* function_pass = dynamic_cast<FunctionPass*>(pass.get());
            if (!function_pass) return InternalError("Invalid function pass: " + name);
            const auto [success, modified] = function_pass->RunOnFunction(F);

            // check PASS result
            if (!success) return UnavailableError("Pass failed: " + name);

            VLOG_IF(1, modified) << "Pass " << name << " modified " << F->name();
        }
    }
    return NoError();
}
}  // namespace hzcc::Pass
