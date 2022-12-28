//
// Created by chen_ on 2022/5/6.
//

#include "StackManager.h"

#include <algorithm>
#include <utility>

#include "utils/logging.h"
namespace Hzcc::Codegen::JVM {
StackManager::StackManager(std::string class_name)
    : _class_name(std::move(class_name)) {}

bool StackManager::IsLocalVar(const std::string& name) {
    return _local_vars.find(name) != _local_vars.end();
}

std::string StackManager::GetVarType(const std::string& name) {
    if (_local_vars.find(name) != _local_vars.end()) {
        return _local_vars[name].second;
    } else if (_global_vars.find(name) != _global_vars.end()) {
        return _global_vars[name];
    } else {
        LOG(FATAL) << "Variable " << name << " not found";
    }
}

std::string StackManager::SaveToVariable(const std::string& name) {
    if (_local_vars.find(name) != _local_vars.end()) {
        auto [stack_id, var_type] = _local_vars[name];
        if (var_type.find('[') != std::string::npos) {
            var_type.erase(var_type.find('['), 1);
            return var_type + "astore";
        } else {
            // convert char to int if necessary
            if (var_type == "c") {
                var_type = "i";
            }
            return var_type + "store " + std::to_string(stack_id);
        }
    } else if (_global_vars.find(name) != _global_vars.end()) {
        auto var_type = _global_vars[name];
        if (var_type.find('[') != std::string::npos) {
            var_type.erase(var_type.find('['), 1);
            return var_type + "astore";
        } else {
            // to upper case
            std::transform(var_type.begin(), var_type.end(), var_type.begin(),
                           ::toupper);
            return "putstatic Field " + _class_name + " " + name + " " +
                   var_type;
        }
    } else {
        LOG(FATAL) << "Variable " << name << " not found";
    }
}

std::string StackManager::LoadFromVariable(const std::string& name) {
    if (_local_vars.find(name) != _local_vars.end()) {
        auto [stack_id, var_type] = _local_vars[name];
        if (var_type.find('[') != std::string::npos) {
            var_type.erase(var_type.find('['), 1);
            return var_type + "aload " + std::to_string(stack_id);
        } else {
            // convert char to int if necessary
            if (var_type == "c") {
                var_type = "i";
            }
            return var_type + "load " + std::to_string(stack_id);
        }
    } else if (_global_vars.find(name) != _global_vars.end()) {
        auto var_type = _global_vars[name];
        std::transform(var_type.begin(), var_type.end(), var_type.begin(),
                       ::toupper);
        return "getstatic Field " + _class_name + " " + name + " " + var_type;
    } else {
        LOG(FATAL) << "Variable " << name << " not found";
    }
}

uint32_t StackManager::GetStackID(const std::string& name) const {
    if (_local_vars.find(name) != _local_vars.end()) {
        return _local_vars.at(name).first;
    } else {
        LOG(FATAL) << "Variable " << name << " not found";
    }
}

bool StackManager::IsGlobalVar(const std::string& name) {
    return _global_vars.find(name) != _global_vars.end();
}
const GlobalVarTable& StackManager::GetGlobalVars() { return _global_vars; }
void StackManager::ResetLocalVars() {
    _local_vars.clear();
    _tmp_vars.clear();
}
void StackManager::AddLocalVar(uint32_t id, const std::string& name,
                               const std::string& type) {
    DLOG_ASSERT(_local_vars.find(name) == _local_vars.end())
        << " Variable " << name << " already exists";
    _local_vars[name] = std::make_pair(id, type);
}
void StackManager::AddGlobalVar(const std::string& name,
                                const std::string& type) {
    DLOG_ASSERT(_global_vars.find(name) == _global_vars.end())
        << " Variable " << name << " already exists";
    _global_vars[name] = type;
}
void StackManager::AddLocalTmpVar(uint32_t id, const std::string& name,
                                  const std::string& type) {
    DLOG_ASSERT(_tmp_vars.find(name) == _tmp_vars.end())
        << " Variable " << name << " already exists";
    _tmp_vars[name] = std::make_pair(id, type);
}
}  // namespace Hzcc::Codegen::JVM
