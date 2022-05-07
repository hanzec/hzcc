//
// Created by chen_ on 2022/5/6.
//
#include <list>
#include <string>
#include <unordered_set>

#include "macro.h"
#ifndef MYCC_SOURCE_CODEGEN_JVM_LABELMANAGER_H_
#define MYCC_SOURCE_CODEGEN_JVM_LABELMANAGER_H_
namespace Hzcc::Codegen {
enum PACKED ScopedType {
    kScopedType_If = 0,
    kScopedType_While = 1,
    kScopedType_For = 2,
    kScopedType_Switch = 3,
    kScopedType_Equal = 4,
    kScopedType_NotEqual = 5,
    kScopedType_ENUM_SIZE = 6  // must be last
};

class LabelManager {
  public:
    LabelManager() = default;

    ~LabelManager();

    /**
     * @brief Enter a new loop
     * @param loop_loc the location of the scope statement.
     */
    void EnterScope(std::pair<int, int> loop_loc, ScopedType type);

    /**
     * @brief Leave the current scope
     */
    void LeaveScope();

    /**
     * @brief Get current scope Type
     */
    ScopedType GetCurrentScopeType();

    /**
     * @brief Get the Exit label name of the current scope.
     * @return the Exit label name of the current scope.
     */
    std::string GetScopeExitLabel();

    /**
     * @brief Get the Unique label name of the current loop.
     * @param name the name of the label, if empty, will be generated
     *        automatically.
     * @return the Unique label name to the source.
     */
    std::string GenerateLabel(const std::string& name = "");

  private:
    inline static uint64_t _label_count = 0;

    std::unordered_set<std::string> _generated_labels;

    std::list<std::pair<std::string, ScopedType>> _scope_stack;
};
}  // namespace Hzcc::Codegen
#endif  // MYCC_SOURCE_CODEGEN_JVM_LABELMANAGER_H_
