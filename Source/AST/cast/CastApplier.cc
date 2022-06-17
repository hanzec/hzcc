//
// Created by chen_ on 2022/6/13.
//
#include "CastApplier.h"

#include "AST/ASTNode.h"
#include "AST/cast/expr/CastExpr.h"
#include "AST/cast/expr/ImplicitCastExpr.h"
#include "AST/cast/expr/implicit_cast/LvalueToRvalueCast.h"
#include "AST/utils/macro.h"
#include "utils/logging.h"
#include "utils/message_utils.h"
namespace Hzcc::AST {
std::unique_ptr<ASTNode> CastApplier::ApplyRules(
    bool require_rvalue,                      // NOLINT
    std::unique_ptr<ASTNode> rhs,             // NOLINT
    const std::shared_ptr<Type>& lhs_type) {  // NOLINT

    auto final_node = std::move(rhs);

    // LvalueToRvalueCast
    if (require_rvalue) {
        if (final_node->IsReturnLValue()) {
            DVLOG(SYNTAX_LOG_LEVEL) << "Applying <LvalueToRvalueCast >cast";
            final_node = std::make_unique<LvalueToRvalueCast>(
                final_node->Location(), std::move(final_node));
        } else {
            HZCC_RUNTIME_CHECK(false)
                << "Not applying <LvalueToRvalueCast> cast rhs"
                << HZCC_NODE_DEBUG_PRINT(final_node)
                << "because it's not return a lvalue";
            return nullptr;
        }
    }

    // apply all rules
    bool is_changed = false;
    while (final_node != nullptr && final_node->RetType() != lhs_type) {
        for (auto& [name, rule] : _rules_base) {
            if (rule->CouldApplyTo(lhs_type, final_node)) {
                DVLOG(SYNTAX_LOG_LEVEL)
                    << "Applying Cast Rule [" << name << "] rhs node "
                    << HZCC_NODE_DEBUG_PRINT(final_node);

                // Apply the rule
                is_changed = true;
                final_node = std::move(rule->Apply(std::move(final_node),
                                                   std::shared_ptr<Type>()));
                break;
            } else {
                DVLOG(MESSAGE_ERROR_TRACING)
                    << "Not applying Cast Rule [" << name << "] rhs node "
                    << HZCC_NODE_DEBUG_PRINT(final_node);
            }
        }

        // prevent infinite loop
        if (!is_changed) {
            DVLOG(MESSAGE_ERROR_TRACING)
                << "Could not apply any Cast Rule rhs node "
                << HZCC_NODE_DEBUG_PRINT(final_node);
            return nullptr;
        } else {
            is_changed = false;
        }
    }

    // return the final node
    HZCC_RUNTIME_CHECK(final_node == nullptr ||
                       final_node->RetType() != lhs_type)
        << "Failed rhs resolve Cast Rule lhs: "
        << HZCC_NODE_DEBUG_PRINT(final_node) << "("
        << final_node->RetType()->Name() << ") rhs type: "
        << "(" << lhs_type->Name() << ")";
    return std::move(final_node);
}

bool CastApplier::reg_pass_internal(const std::string& command,
                                    const std::string& description,
                                    std::unique_ptr<ICastRule> rule) {
    if (_registered_rules.find(command) != _registered_rules.end()) {
        return false;
    } else {
        _registered_rules.emplace(command);
        _rules_base.emplace_back(command, std::move(rule));
        return true;
    }
}
}  // namespace Hzcc::AST