//
// Created by chen_ on 2022/5/6.
//

#include "LabelManager.h"

#include "utils/logging.h"
namespace Hzcc::Codegen {
constexpr const static std::array<const char*, kScopedType_ENUM_SIZE>
    kLabelNames = {"if",    "do",       "while",   "for",     "switch",
                   "equal", "notequal", "else_if", "logical", "ternary"};

/**
 * @brief Return the control-flow kind of the innermost active scope.
 * @return Kind of the innermost active control-flow scope.
 * @pre The corresponding stack or scope list is nonempty.
 */
ScopedType LabelManager::GetCurrentScopeType() {
    return _scope_stack.back().second;
}

/**
 * @brief Return the exit label of the nearest enclosing loop.
 * @return Label used by break in the nearest enclosing loop.
 */
std::string LabelManager::GetLoopExitLabel() {
    DLOG_ASSERT(_loop_count != 0) << " No loop scope available";

    for (auto iter = _scope_stack.rbegin(); iter != _scope_stack.rend();
         ++iter) {
        auto type = iter->second;
        if (type == kScopedType_While || type == kScopedType_For ||
            type == kScopedType_Do) {
            return "L" + iter->first + "_exit";
        }
    }
    return "";
}

/**
 * @brief Return the entry label of the nearest enclosing loop.
 * @return Label used by continue in the nearest enclosing loop.
 */
std::string LabelManager::GetLoopEntryLabel() {
    DLOG_ASSERT(_loop_count != 0) << " No loop scope available";

    for (auto iter = _scope_stack.rbegin(); iter != _scope_stack.rend();
         ++iter) {
        auto type = iter->second;
        if (type == kScopedType_While || type == kScopedType_For ||
            type == kScopedType_Do) {
            return "L" + iter->first + "_entry";
        }
    }
    return "";
}

/**
 * @brief Intern and return the exit label of the current scope.
 * @return Interned label reference, invalidated by ResetLables().
 */
const std::string& LabelManager::GetScopeExitLabel() {
    static const std::string empty_string;
    DLOG_ASSERT(!_scope_stack.empty()) << " scope stack is empty";

    if (_scope_stack.empty()) {
        return empty_string;
    }

    auto new_label =
        _generated_labels.emplace("L" + _scope_stack.back().first + "_exit");
    return *new_label.first;
}

/**
 * @brief Intern and return the entry label of the current scope.
 * @return Interned label reference, invalidated by ResetLables().
 */
const std::string& LabelManager::GetScopeEntryLabel() {
    static const std::string empty_string;
    DLOG_ASSERT(!_scope_stack.empty()) << " scope stack is empty";

    if (_scope_stack.empty()) {
        return empty_string;
    }

    auto new_label =
        _generated_labels.emplace("L" + _scope_stack.back().first + "_entry");

    return *new_label.first;
}

/**
 * @brief Intern a scope-qualified label, generating a suffix when none is supplied.
 * @param name Label suffix; empty requests an automatically numbered label.
 * @return Interned label reference, invalidated by ResetLables().
 */
const std::string& LabelManager::GenerateLabel(const std::string& name) {
    auto new_label =
        ("L" + (_scope_stack.empty() ? "" : _scope_stack.back().first + "_")) +
        (name.empty() ? std::to_string(_label_count++) : name);

    DLOG_ASSERT(_generated_labels.find(new_label) == _generated_labels.end())
        << " Label " << new_label << " already exists";

    return *_generated_labels.insert(new_label).first;
}

/**
 * @brief Pop the innermost scope after its contents have been processed.
 * @pre The corresponding stack or scope list is nonempty.
 */
void LabelManager::LeaveScope() {
    DLOG_ASSERT(!_scope_stack.empty())
        << " LabelManager::LeaveScope() called without entering scope";

    // count loop depth
    auto type = _scope_stack.back().second;
    if (type == kScopedType_While || type == kScopedType_For ||
        type == kScopedType_Do) {
        _loop_count--;
    }

    _scope_stack.pop_back();
}

/**
 * @brief Push a new lexical scope for subsequent declarations or labels.
 * @param loop_loc Source line and column identifying the control-flow scope.
 * @param type Control-flow construct represented by the new scope.
 */
void LabelManager::EnterScope(std::pair<int, int> loop_loc, ScopedType type) {
    auto scope_id = std::to_string(loop_loc.first) + "_" +
                    std::to_string(loop_loc.second) + "_" + kLabelNames[type];

    // count loop depth
    if (type == kScopedType_While || type == kScopedType_For ||
        type == kScopedType_Do) {
        _loop_count++;
    }

    // push scope id to stack
    _scope_stack.emplace_back(scope_id, type);
}

/**
 * @brief Release LabelManager state.
 * @pre All entered scopes have been left.
 */
LabelManager::~LabelManager() {
    DLOG_ASSERT(_scope_stack.empty())
        << " scope stack is not empty when destructing LabelManager, "
        << _scope_stack.size()
        << " scopes left, head: " << _scope_stack.front().first;
}

/**
 * @brief Clear labels, scope records and line tags, then reset the shared label counter.
 * @note The loop-depth counter is not reset; leave entered scopes before calling this.
 */
void LabelManager::ResetLables() {
    _label_count = 0;
    _scope_stack.clear();
    _generated_labels.clear();
    _line_number_table.clear();
}

/**
 * @brief Associate an assembly label with a source line, clamping negative lines to zero.
 * @param tag Assembly label associated with the source line.
 * @param line_number Source line associated with the label.
 */
void LabelManager::SetLineNumberTag(const std::string& tag, int line_number) {
    _line_number_table.emplace_back(tag, line_number < 0 ? 0 : line_number);
}

/**
 * @brief Return the mutable source-line mapping table owned by this manager.
 * @return Reference to the manager-owned label/line table.
 */
std::list<std::pair<std::string, uint64_t>>& LabelManager::GetLineNumberTags() {
    return _line_number_table;
}
/**
 * @brief Test whether the retained loop-depth counter is nonzero.
 * @return True when the stated condition holds; otherwise false.
 */
bool LabelManager::WithinLoop() { return _loop_count != 0; }

/**
 * @brief Intern a fresh source-line label using the shared label counter.
 * @return Interned label reference, invalidated by ResetLables().
 */
const std::string& LabelManager::GenerateLineLabel() {
    auto new_label = ("L_LINE_" + std::to_string(_label_count++));
    DLOG_ASSERT(_generated_labels.find(new_label) == _generated_labels.end())
        << " Label " << new_label << " already exists";

    return *_generated_labels.insert(new_label).first;
}
}  // namespace Hzcc::Codegen
