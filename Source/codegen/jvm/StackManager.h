//
// Created by chen_ on 2022/5/6.
//
#include <string>
#include <unordered_map>
#ifndef MYCC_SOURCE_CODEGEN_JVM_STACKMANAGER_H_
#define MYCC_SOURCE_CODEGEN_JVM_STACKMANAGER_H_
namespace Hzcc::Codegen::JVM {
using GlobalVarTable = std::unordered_map<std::string, std::string>;

class StackManager {
  public:
    explicit StackManager(std::string class_name);

    [[nodiscard]] uint32_t GetStackID(const std::string& name) const;

    [[nodiscard]] bool IsLocalVar(const std::string& name);

    [[nodiscard]] bool IsGlobalVar(const std::string& name);

    [[nodiscard]] const GlobalVarTable& GetGlobalVars();

    [[nodiscard]] std::string GetVarType(const std::string& name);

    [[nodiscard]] std::string SaveToVariable(const std::string& name);

    [[nodiscard]] std::string LoadFromVariable(const std::string& name);

    void AddGlobalVar(const std::string& name, const std::string& type);

    void AddLocalVar(uint32_t id, const std::string& name,
                     const std::string& type);

    void AddLocalTmpVar(uint32_t id, const std::string& name,
                        const std::string& type);

    void ResetLocalVars();

  private:
    std::string _class_name;

    /**
     * Varname ： [stackID, Type]
     */
    GlobalVarTable _global_vars;
    std::unordered_map<std::string, std::pair<int, std::string>> _tmp_vars;
    std::unordered_map<std::string, std::pair<int, std::string>> _local_vars;
};
}  // namespace Hzcc::Codegen::JVM
#endif  // MYCC_SOURCE_CODEGEN_JVM_STACKMANAGER_H_
