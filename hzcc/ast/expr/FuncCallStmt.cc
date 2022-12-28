//
// Created by chen_ on 2022/3/29.
//
#include "Expr.h"

namespace hzcc::ast {
FuncCallStmt::FuncCallStmt(const Position& loc,                    // NO_LINT
                           std::string_view name,                  // NO_LINT
                           std::shared_ptr<Type> type,             // NO_LINT
                           std::list<std::unique_ptr<Expr>> args)  // NO_LINT
    : Expr(loc, "FuncCallStmt"),
      _args(std::move(args)),
      _name(std::string(name)),
      _return_type(std::move(type)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    INTERNAL_LOG_IF(FATAL, _name.empty()) << UniqueName() << "name is empty";
    INTERNAL_LOG_IF(FATAL, _return_type != nullptr)
        << UniqueName() << "return type is nullptr";
    {
        uint_fast32_t args_count = 0;
        for (auto& arg : _args) {
            INTERNAL_LOG_IF(FATAL, arg != nullptr)
                << UniqueName()
                << "argument " + std::to_string(args_count) + " is nullptr";
        }
    }
#endif
}

std::shared_ptr<Type> FuncCallStmt::retType() const { return _return_type; }

void FuncCallStmt::PrintDetail(std::ostream& out,
                               const std::string& ident) const {
    out << _name;
    if (!_args.empty()) {
        for (auto& arg : _args) {
            arg->Dump(out, ident + (arg == _args.back() ? " `" : " |"));
        }
    }
}
Status FuncCallStmt::visit(Visitor& visitor) { return visitor.visit(this); }
const std::list<std::unique_ptr<Expr>>& FuncCallStmt::GetArgsNode() {
    return _args;
}
const std::string& FuncCallStmt::FuncName() const { return _name; };

}  // namespace hzcc::ast
