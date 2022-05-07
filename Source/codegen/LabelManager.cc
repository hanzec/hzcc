//
// Created by chen_ on 2022/5/6.
//

#include "LabelManager.h"

#include "utils/logging.h"
namespace Hzcc::Codegen {

ScopedType LabelManager::GetCurrentScopeType() {
    return _scope_stack.back().second;
}

std::string LabelManager::GetScopeExitLabel() {
    DLOG_ASSERT(!_scope_stack.empty()) << " scope stack is empty";
    return _scope_stack.back().first + "_exit";
}

std::string LabelManager::GenerateLabel(const std::string& name) {
    auto new_label =
        (_scope_stack.empty() ? "L" : _scope_stack.back().first + "_") +
        (name.empty() ? std::to_string(_label_count++) : name);

    DLOG_ASSERT(_generated_labels.find(new_label) == _generated_labels.end())
        << " Label " << new_label << " already exists";

    return *_generated_labels.insert(new_label).first;
}

void LabelManager::LeaveScope() {
    DLOG_ASSERT(!_scope_stack.empty())
        << " LabelManager::LeaveScope() called without entering scope";
    _scope_stack.pop_back();
}

void LabelManager::EnterScope(std::pair<int, int> loop_loc, ScopedType type) {
    auto scope_id =
        std::to_string(loop_loc.first) + std::to_string(loop_loc.second);

    // push scope id to stack
    _scope_stack.emplace_back(scope_id, type);

    // push the current exit label to the stack
    _generated_labels.emplace(scope_id + "_exit");
}
LabelManager::~LabelManager() {
    DLOG_ASSERT(_scope_stack.empty())
        << " scope stack is not empty when destructing LabelManager";
}
}  // namespace Hzcc::Codegen