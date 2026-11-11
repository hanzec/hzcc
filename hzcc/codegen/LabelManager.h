//
// Created by chen_ on 2022/5/6.
//
#include <list>
#include <string>
#include <unordered_set>

#include "macro.h"
#ifndef HZCC_SOURCE_CODEGEN_JVM_LABEL_MANAGER_H
#define HZCC_SOURCE_CODEGEN_JVM_LABEL_MANAGER_H
namespace hzcc::Codegen {
enum PACKED ScopedType {
    kScopedType_If = 0,
    kScopedType_Do = 1,
    kScopedType_While = 2,
    kScopedType_For = 3,
    kScopedType_Switch = 4,
    kScopedType_Equal = 5,
    kScopedType_NotEqual = 6,
    kScopedType_Else_If = 7,
    kScopedType_Logical = 8,
    kScopedType_Ternary = 9,
    kScopedType_ENUM_SIZE = 10  // must be last
};

/**
 * @brief Track nested control-flow scopes, unique assembly labels and source-line tags.
 */
class LabelManager {
  public:
    /**
     * @brief Initialize an empty LabelManager.
     */
    LabelManager() = default;

    /**
     * @brief Release LabelManager state.
     * @pre All entered scopes have been left.
     */
    ~LabelManager();

    /**
     * @brief Push a new lexical scope for subsequent declarations or labels.
     * @param loop_loc Source line and column identifying the control-flow scope.
     * @param type Control-flow construct represented by the new scope.
     */
    void EnterScope(std::pair<int, int> loop_loc, ScopedType type);

    /**
     * @brief Pop the innermost scope after its contents have been processed.
     * @pre The corresponding stack or scope list is nonempty.
     */
    void LeaveScope();

    /**
     * @brief Return the control-flow kind of the innermost active scope.
     * @return Kind of the innermost active control-flow scope.
     * @pre The corresponding stack or scope list is nonempty.
     */
    ScopedType GetCurrentScopeType();

    /**
     * @brief Test whether the retained loop-depth counter is nonzero.
     * @return True when the stated condition holds; otherwise false.
     */
    bool WithinLoop();

    /**
     * @brief Return the exit label of the nearest enclosing loop.
     * @return Label used by break in the nearest enclosing loop.
     */
    std::string GetLoopExitLabel();

    /**
     * @brief Return the entry label of the nearest enclosing loop.
     * @return Label used by continue in the nearest enclosing loop.
     */
    std::string GetLoopEntryLabel();
    /**
     * @brief Intern and return the exit label of the current scope.
     * @return Interned label reference, invalidated by ResetLables().
     */
    const std::string& GetScopeExitLabel();

    /**
     * @brief Intern and return the entry label of the current scope.
     * @return Interned label reference, invalidated by ResetLables().
     */
    const std::string& GetScopeEntryLabel();

    /**
     * @brief Intern a fresh source-line label using the shared label counter.
     * @return Interned label reference, invalidated by ResetLables().
     */
    const std::string& GenerateLineLabel();

    /**
     * @brief Intern a scope-qualified label, generating a suffix when none is supplied.
     * @param name Label suffix; empty requests an automatically numbered label.
     * @return Interned label reference, invalidated by ResetLables().
     */
    const std::string& GenerateLabel(const std::string& name = "");

    /**
     * @brief Clear labels, scope records and line tags, then reset the shared label counter.
     * @note The loop-depth counter is not reset; leave entered scopes before calling this.
     */
    void ResetLables();

    /**
     * @brief Associate an assembly label with a source line, clamping negative lines to zero.
     * @param tag Assembly label associated with the source line.
     * @param line_number Source line associated with the label.
     */
    void SetLineNumberTag(const std::string& tag, int line_number);

    /**
     * @brief Return the mutable source-line mapping table owned by this manager.
     * @return Reference to the manager-owned label/line table.
     */
    std::list<std::pair<std::string, uint64_t>>& GetLineNumberTags();

  private:
    uint64_t _loop_count = 0;

    inline static uint64_t _label_count = 0;

    std::unordered_set<std::string> _generated_labels;

    std::list<std::pair<std::string, ScopedType>> _scope_stack;

    std::list<std::pair<std::string, uint64_t>> _line_number_table;
};
}  // namespace hzcc::Codegen
#endif  // HZCC_SOURCE_CODEGEN_JVM_LABEL_MANAGER_H
