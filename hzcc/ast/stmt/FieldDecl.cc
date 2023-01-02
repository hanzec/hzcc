//
// Created by chen_ on 2022/12/28.
//
#include <glog/logging.h>
#include <memory>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>

#include "ast/Stmt.h"
#include "macro.h"
#include "utils/logging.h"

namespace hzcc::ast {
FieldDecl::FieldDecl(const Position& loc,         // NOLINT
                     std::string_view name,       // NOLINT
                     std::unique_ptr<TypeProxyExpr> type)  // NOLINT
    : IDeclStmt("FieldDecl",name, loc), _type(std::move(type)) {
    HZCC_RUNTIME_CHECK_BLOCK({
        INTERNAL_LOG_IF(FATAL, _type != nullptr)
            << UniqueName() << "type is nullptr";
    })
}

}  // namespace hzcc::ast