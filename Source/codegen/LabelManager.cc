//
// Created by chen_ on 2022/5/6.
//

#include "LabelManager.h"

#include "utils/logging.h"
namespace Hzcc::Codegen {
constexpr const static std::array<const char*, kScopedType_ENUM_SIZE>
    kLabelNames = {"if",    "do",       "while",   "for",     "switch",
                   "equal", "notequal", "else_if", "logical", "ternary"};

ScopedType LabelManager::GetCurrentScopeType() {
    return _scope_stack.back().second;
}

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

const std::string& LabelManager::GenerateLabel(const std::string& name) {
    auto new_label =
        ("L" + (_scope_stack.empty() ? "" : _scope_stack.back().first + "_")) +
        (name.empty() ? std::to_string(_label_count++) : name);

    DLOG_ASSERT(_generated_labels.find(new_label) == _generated_labels.end())
        << " Label " << new_label << " already exists";

    return *_generated_labels.insert(new_label).first;
}

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

LabelManager::~LabelManager() {
    DLOG_ASSERT(_scope_stack.empty())
        << " scope stack is not empty when destructing LabelManager, "
        << _scope_stack.size()
        << " scopes left, head: " << _scope_stack.front().first;
}

void LabelManager::ResetLables() {
    _label_count = 0;
    _scope_stack.clear();
    _generated_labels.clear();
    _line_number_table.clear();
}

void LabelManager::SetLineNumberTag(const std::string& tag, int line_number) {
    _line_number_table.emplace_back(tag, line_number < 0 ? 0 : line_number);
}

std::list<std::pair<std::string, uint64_t>>& LabelManager::GetLineNumberTags() {
    return _line_number_table;
}
bool LabelManager::WithinLoop() { return _loop_count != 0; }

const std::string& LabelManager::GenerateLineLabel() {
    auto new_label = ("L_LINE_" + std::to_string(_label_count++));
    DLOG_ASSERT(_generated_labels.find(new_label) == _generated_labels.end())
        << " Label " << new_label << " already exists";

    return *_generated_labels.insert(new_label).first;
}
}  // namespace Hzcc::Codegen
