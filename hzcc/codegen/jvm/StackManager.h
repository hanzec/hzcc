//
// Created by chen_ on 2022/5/6.
//
#include <string>
#include <unordered_map>
#ifndef HZCC_CODEGEN_JVM_STACK_MANAGER_H
#define HZCC_CODEGEN_JVM_STACK_MANAGER_H
namespace hzcc::Codegen::JVM {
using GlobalVarTable = std::unordered_map<std::string, std::string>;

/**
 * @brief Map variable names to JVM local slots, temporary slots and static fields.
 */
class StackManager {
  public:
    /**
     * @brief Initialize StackManager with the supplied backend state.
     * @param class_name Name of the JVM class owning generated fields and methods.
     */
    explicit StackManager(std::string class_name);

    /**
     * @brief Return the JVM local slot assigned to a named local variable.
     * @param name Variable or symbol name to register or look up.
     * @return The local-variable slot index.
     * @pre name is registered in the local-variable table.
     */
    [[nodiscard]] uint32_t GetStackID(const std::string& name) const;

    /**
     * @brief Test whether a name is registered in the local-variable table.
     * @param name Variable or symbol name to register or look up.
     * @return True when the stated condition holds; otherwise false.
     */
    [[nodiscard]] bool IsLocalVar(const std::string& name);

    /**
     * @brief Test whether a name is registered as a static field.
     * @param name Variable or symbol name to register or look up.
     * @return True when the stated condition holds; otherwise false.
     */
    [[nodiscard]] bool IsGlobalVar(const std::string& name);

    /**
     * @brief Return a borrowed view of the global name-to-type table.
     * @return A const reference valid while this manager exists.
     */
    [[nodiscard]] const GlobalVarTable& GetGlobalVars();

    /**
     * @brief Resolve a variable type, preferring locals to global fields.
     * @param name Variable or symbol name to register or look up.
     * @return Stored JVM type string for the variable.
     * @pre name is registered as a local variable or global field.
     */
    [[nodiscard]] std::string GetVarType(const std::string& name);

    /**
     * @brief Generate the JVM store instruction for a local, array element or static field.
     * @param name Variable or symbol name to register or look up.
     * @return JVM store instruction text.
     * @pre name is registered as a local variable or global field.
     */
    [[nodiscard]] std::string SaveToVariable(const std::string& name);

    /**
     * @brief Generate the JVM load instruction for a local, array element or static field.
     * @param name Variable or symbol name to register or look up.
     * @return JVM load instruction text.
     * @pre name is registered as a local variable or global field.
     */
    [[nodiscard]] std::string LoadFromVariable(const std::string& name);

    /**
     * @brief Register a global variable as a static field of the current class.
     * @param name Variable or symbol name to register or look up.
     * @param type JVM type or descriptor used by this operation.
     */
    void AddGlobalVar(const std::string& name, const std::string& type);

    /**
     * @brief Register a named variable at a JVM local slot.
     * @param id JVM local-variable slot index.
     * @param name Variable or symbol name to register or look up.
     * @param type JVM type or descriptor used by this operation.
     */
    void AddLocalVar(uint32_t id, const std::string& name,
                     const std::string& type);

    /**
     * @brief Record a temporary name and its assigned local slot.
     * @param id JVM local-variable slot index.
     * @param name Variable or symbol name to register or look up.
     * @param type JVM type or descriptor used by this operation.
     */
    void AddLocalTmpVar(uint32_t id, const std::string& name,
                        const std::string& type);

    /**
     * @brief Clear local and temporary slot mappings while preserving global fields.
     */
    void ResetLocalVars();

  private:
    std::string _class_name;

    /**
     * Varname: [stackID, RetType]
     */
    GlobalVarTable _global_vars;
    std::unordered_map<std::string, std::pair<int, std::string>> _tmp_vars;
    std::unordered_map<std::string, std::pair<int, std::string>> _local_vars;
};
}  // namespace hzcc::Codegen::JVM
#endif  // HZCC_CODEGEN_JVM_STACK_MANAGER_H
