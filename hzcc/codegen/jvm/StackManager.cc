//
// Created by chen_ on 2022/5/6.
//

#include "StackManager.h"

#include <algorithm>
#include <utility>

#include "utils/logging.h"
namespace Hzcc::Codegen::JVM {
/**
 * @brief Initialize StackManager with the supplied backend state.
 * @param class_name Name of the JVM class owning generated fields and methods.
 */
StackManager::StackManager(std::string class_name)
    : _class_name(std::move(class_name)) {}

/**
 * @brief Test whether a name is registered in the local-variable table.
 * @param name Variable or symbol name to register or look up.
 * @return True when the stated condition holds; otherwise false.
 */
bool StackManager::IsLocalVar(const std::string& name) {
    return _local_vars.find(name) != _local_vars.end();
}

/**
 * @brief Resolve a variable type, preferring locals to global fields.
 * @param name Variable or symbol name to register or look up.
 * @return Stored JVM type string for the variable.
 * @pre name is registered as a local variable or global field.
 */
std::string StackManager::GetVarType(const std::string& name) {
    if (_local_vars.find(name) != _local_vars.end()) {
        return _local_vars[name].second;
    } else if (_global_vars.find(name) != _global_vars.end()) {
        return _global_vars[name];
    } else {
        LOG(FATAL) << "Variable " << name << " not found";
    }
}

/**
 * @brief Generate the JVM store instruction for a local, array element or static field.
 * @param name Variable or symbol name to register or look up.
 * @return JVM store instruction text.
 * @pre name is registered as a local variable or global field.
 */
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

/**
 * @brief Generate the JVM load instruction for a local, array element or static field.
 * @param name Variable or symbol name to register or look up.
 * @return JVM load instruction text.
 * @pre name is registered as a local variable or global field.
 */
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

/**
 * @brief Return the JVM local slot assigned to a named local variable.
 * @param name Variable or symbol name to register or look up.
 * @return The local-variable slot index.
 * @pre name is registered in the local-variable table.
 */
uint32_t StackManager::GetStackID(const std::string& name) const {
    if (_local_vars.find(name) != _local_vars.end()) {
        return _local_vars.at(name).first;
    } else {
        LOG(FATAL) << "Variable " << name << " not found";
    }
}

/**
 * @brief Test whether a name is registered as a static field.
 * @param name Variable or symbol name to register or look up.
 * @return True when the stated condition holds; otherwise false.
 */
bool StackManager::IsGlobalVar(const std::string& name) {
    return _global_vars.find(name) != _global_vars.end();
}
/**
 * @brief Return a borrowed view of the global name-to-type table.
 * @return A const reference valid while this manager exists.
 */
const GlobalVarTable& StackManager::GetGlobalVars() { return _global_vars; }
/**
 * @brief Clear local and temporary slot mappings while preserving global fields.
 */
void StackManager::ResetLocalVars() {
    _local_vars.clear();
    _tmp_vars.clear();
}
/**
 * @brief Register a named variable at a JVM local slot.
 * @param id JVM local-variable slot index.
 * @param name Variable or symbol name to register or look up.
 * @param type JVM type or descriptor used by this operation.
 */
void StackManager::AddLocalVar(uint32_t id, const std::string& name,
                               const std::string& type) {
    DLOG_ASSERT(_local_vars.find(name) == _local_vars.end())
        << " Variable " << name << " already exists";
    _local_vars[name] = std::make_pair(id, type);
}
/**
 * @brief Register a global variable as a static field of the current class.
 * @param name Variable or symbol name to register or look up.
 * @param type JVM type or descriptor used by this operation.
 */
void StackManager::AddGlobalVar(const std::string& name,
                                const std::string& type) {
    DLOG_ASSERT(_global_vars.find(name) == _global_vars.end())
        << " Variable " << name << " already exists";
    _global_vars[name] = type;
}
/**
 * @brief Record a temporary name and its assigned local slot.
 * @param id JVM local-variable slot index.
 * @param name Variable or symbol name to register or look up.
 * @param type JVM type or descriptor used by this operation.
 */
void StackManager::AddLocalTmpVar(uint32_t id, const std::string& name,
                                  const std::string& type) {
    DLOG_ASSERT(_tmp_vars.find(name) == _tmp_vars.end())
        << " Variable " << name << " already exists";
    _tmp_vars[name] = std::make_pair(id, type);
}
}  // namespace Hzcc::Codegen::JVM
