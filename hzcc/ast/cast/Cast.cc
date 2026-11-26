//
// Created by chen_ on 2022/6/13.
//
#include "Cast.h"

#include <absl/strings/str_cat.h>

#include <vector>

namespace hzcc::ast {
StatusOr<std::unique_ptr<Expr>> Cast::apply(
    bool require_rvalue,        // NOLINT
    ExprPtr rhs,                // NOLINT
    QualTypePtr lhs_type) {     // NOLINT
    HZCC_CHECK_OK_OR_RETURN(apply_to(require_rvalue, rhs, lhs_type));
    return std::move(rhs);
}

Status Cast::apply_to(bool require_rvalue, ExprPtr& source, const QualTypePtr& target) {
    if (!source) {
        return {StatusCode::kCastStage,
                absl::StrCat("Cannot implicitly convert a missing expression to '",
                             target ? target->to_str() : "<unresolved>", "'")};
    }
    const auto from = source->type();
    const auto node_name = source->NodeName();
    const auto location = source->loc();
    auto failure = [&](std::string_view reason,
                       StatusCode code = StatusCode::kCastStage) {
        return Status(code, absl::StrCat("Cannot implicitly convert '",
                                         from ? from->to_str() : "<unresolved>", "' to '",
                                         target ? target->to_str() : "<unresolved>",
                                         "' (", node_name, " at ", location.first, ":",
                                         location.second, "): ", reason));
    };
    if (!from || !target) {
        return failure(!from ? "source type is unresolved"
                             : "destination type is unresolved");
    }
    // LvalueToRvalueCast
    if (*from == *target) {
        if (require_rvalue && source->IsReturnLValue()) {
            source = std::make_unique<LvalueToRvalueCast>(location, std::move(source));
        }
        return NoError();
    }

    static const auto rules = [] {
        std::vector<std::pair<std::string, std::unique_ptr<ICastRule>>> registered;
        for_each_product([&](const std::string& id) {
            registered.emplace_back(id, create(id));
            return NoError();
        });
        return registered;
    }();
    // Try each registered rule against the current owned expression.
    for (const auto& [id, rule] : rules) {
        if (rule && rule->CouldApplyTo(source, target)) {
            auto converted = rule->Apply(source, target, require_rvalue);
            if (!converted.ok()) {
                return failure(absl::StrCat("rule '", id,
                                            "' failed: ", converted.status().message()),
                               converted.status().code());
            }
            if (!converted.value())
                return failure(absl::StrCat("rule '", id, "' returned an empty cast"));
            source = std::move(converted.value());
            return NoError();
        }
    }
    std::string tried;
    for (const auto& entry : rules) {
        if (!tried.empty()) tried += ", ";
        tried += entry.first;
    }
    return failure(tried.empty() ? "no cast rules are registered"
                                 : "no suitable cast rule among " + tried);
}
}  // namespace hzcc::ast