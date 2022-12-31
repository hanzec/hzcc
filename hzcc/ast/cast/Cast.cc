//
// Created by chen_ on 2022/6/13.
//
#include "Cast.h"

#include "ast/expr/Expr.h"
#include "utils/logging.h"
namespace hzcc::ast {
StatusOr<std::unique_ptr<Expr>> Cast::apply(
    bool require_rvalue,                      // NOLINT
    std::unique_ptr<Expr> rhs,                // NOLINT
    const std::shared_ptr<Type>& lhs_type) {  // NOLINT

    auto ret = std::move(rhs);

    // LvalueToRvalueCast
    if (require_rvalue) {
        VLOG_IF(SYNTAX_LOG_LEVEL, !ret->IsReturnLValue())
            << "No need to apply <LvalueToRvalueCast>cast since the expression "
               "is already an rvalue.";
        if (ret->IsReturnLValue()) {
            VLOG(SYNTAX_LOG_LEVEL) << "Applying <LvalueToRvalueCast>cast";
            ret = std::make_unique<LvalueToRvalueCast>(ret->loc(),
                                                       std::move(ret));
        }
    }

    // apply all rules
    bool changed = false;
    auto& r_ref = _rules;
    while (ret != nullptr && ret->retType() != lhs_type) {
        auto&& status = for_each_product([&](const std::string& cast_id)
                                             -> Status {
            auto rule = r_ref.find(cast_id);

            if (rule == r_ref.end()) {
                r_ref.insert({cast_id, create(cast_id)});
            }

            if (rule->second->CouldApplyTo(ret->retType(), ret)) {
                DVLOG(SYNTAX_LOG_LEVEL) << "Applying <" << cast_id << "> cast";
                auto move_ret =
                    std::move(rule->second->Apply(std::move(ret), lhs_type));

                if (!move_ret.ok()) {
                    return move_ret.status();
                }

                ret = std::move(move_ret.value());
                changed = true;

                return NoError();
            }
            return {StatusCode::kCastStage, "<" + cast_id + "> not applicable"};
        });

        // prevent infinite loop
        if (!changed) {
            DVLOG(HZCC_VLOG_DEBUG_LEVEL)
                << "Could not apply any Cast Rule rhs node "
                << ret->UniqueName() << " because it's not match";
            return Status(StatusCode::kCastStage,
                          "Cannot find any suitable cast rule");
        } else {
            changed = false;

            // print Debug message
            if (VLOG_IS_ON(HZCC_VLOG_DEBUG_LEVEL)) {
                std::stringstream ss;

                ss << "Trying to apply cast rules from ["
                   << ret->retType()->UniqueName() << "] to ["
                   << lhs_type->UniqueName() << "]:" << std::endl;

                for (auto&& statue : status) {
                    if (!statue.ok()) {
                        ss << statue.message() << std::endl;
                    }
                }

                DVLOG(HZCC_VLOG_DEBUG_LEVEL) << ss.str();
            }
        }
    }

    // return the final node
    INTERNAL_LOG_IF(ERROR, ret == nullptr || ret->retType() != lhs_type)
        << "Failed rhs resolve Cast Rule lhs: " << ret->UniqueName() << "("
        << ret->retType()->Name() << ") rhs type: "
        << "(" << lhs_type->Name() << ")";

    if(ret == nullptr || ret->retType() != lhs_type) {
        return Status(StatusCode::kCastStage, "Failed rhs resolve Cast Rule");
    }
    return std::move(ret);
}
}  // namespace hzcc::ast