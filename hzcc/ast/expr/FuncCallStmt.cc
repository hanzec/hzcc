//
// Created by chen_ on 2022/3/29.
//
#include <glog/logging.h>
#include <stdint.h>
#include <list>
#include <memory>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>

#include "Expr.h"
#include "ast/Stmt.h"
#include "ast/visitor.h"
#include "macro.h"
#include "utils/logging.h"
#include "utils/status/status.h"

namespace hzcc {
namespace ast {
class Type;
}  // namespace ast
}  // namespace hzcc

namespace hzcc::ast {
FuncCallStmt::FuncCallStmt(const Position& loc, std::string_view name,
                           std::list<std::unique_ptr<Expr>> args)  // NO_LINT
    : Expr(loc, "FuncCallStmt"),
      _func_args(std::move(args)),
      _name(std::string(name)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    INTERNAL_LOG_IF(FATAL, _name.empty()) << UniqueName() << "name is empty";
    INTERNAL_LOG_IF(FATAL, _ret_type != nullptr)
        << UniqueName() << "return type is nullptr";
    {
        uint_fast32_t args_count = 0;
        for (auto& arg : _func_args) {
            INTERNAL_LOG_IF(FATAL, arg != nullptr)
                << UniqueName()
                << "argument " + std::to_string(args_count) + " is nullptr";
        }
    }
#endif
}

std::shared_ptr<Type> FuncCallStmt::retType() const { return _ret_type; }

Status FuncCallStmt::visit(Visitor& visitor) { return visitor.visit(this); }
 std::list<std::unique_ptr<Expr>>& FuncCallStmt::args() {
    return _func_args;
}
 std::string_view FuncCallStmt::name() const { return _name; };

}  // namespace hzcc::ast
