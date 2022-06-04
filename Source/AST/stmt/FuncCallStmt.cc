//
// Created by chen_ on 2022/3/29.
//
#include "FuncCallStmt.h"

#include "AST/utils/macro.h"
namespace Hzcc::AST {
FuncCallStmt::FuncCallStmt(std::shared_ptr<Type> type,
                           const std::string_view& name,
                           const std::pair<int, int>& location,
                           std::list<std::unique_ptr<ASTNode>> args)  // NO_LINT
    : ASTNode(location),
      _args(std::move(args)),
      _name(std::string(name)),
      _return_type(std::move(type)) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    HZCC_RUNTIME_CHECK(_name.empty())
        << HZCC_AST_PRINT_CHECK_ERROR_INFO("name is empty", this);
    HZCC_RUNTIME_CHECK(_return_type != nullptr)
        << HZCC_AST_PRINT_CHECK_ERROR_INFO("return type is nullptr", this);

    {
        uint_fast32_t args_count = 0;
        for (auto& arg : _args) {
            HZCC_RUNTIME_CHECK(arg != nullptr)
                << HZCC_AST_PRINT_CHECK_ERROR_INFO(
                       "argument " + std::to_string(args_count) + " is nullptr",
                       this);
        }
    }
}

const char* FuncCallStmt::NodeName() const { return "FuncCallStmt"; }

std::shared_ptr<Type> FuncCallStmt::RetType() const { return _return_type; }

std::string FuncCallStmt::PrintDetail(const std::string& ident) const {
    std::stringstream info;
    info << _name;

    if (!_args.empty()) {
        for (auto& arg : _args) {
            info << "\n" +
                        arg->Dump(ident + (arg == _args.back() ? " `" : " |"));
        }
    }
    return info.str();
}
Status FuncCallStmt::visit(ASTVisitor& visitor) { return visitor.visit(this); }
const std::list<std::unique_ptr<ASTNode>>& FuncCallStmt::GetArgsNode() {
    return _args;
}
const std::string& FuncCallStmt::FuncName() const { return _name; };

}  // namespace Hzcc::AST