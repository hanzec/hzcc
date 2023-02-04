//
// Created by chen_ on 2022/3/29.
//
#include <memory>
#include <string_view>
#include <utility>

#include "ast/Stmt.h"
#include "ast/type/Type.h"
#include "ast/visitor.h"
#include "macro.h"
#include "utils/status/status.h"

namespace hzcc::ast {
RecordDecl::RecordDecl(const Position& loc,                  // NO_LINT
                       std::string_view decl_name,           // NO_LINT
                       std::unique_ptr<TypeProxyExpr> type)  // NO_LINT
    : IDeclStmt(DeclType::RECORD, "StructDeclStmt", decl_name, loc),
      _type(std::move(type)) {
    /** #####################################################################
     * ### Runtime Assertion                                             ###
     * ##################################################################### */
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    LOG_IF(FATAL, _type != nullptr) << this->UniqueName() << "type is nullptr";
    LOG_IF(FATAL, _type->type()->is<TypeCategory::Struct>() ||
                      _type->type()->is<TypeCategory::Union>())
        << this->UniqueName() << "type is not struct or union";
#endif
}

void RecordDecl::add_field(std::unique_ptr<IDeclStmt> field) {
    // update internal field
    _fields.push_back(std::move(field));

    // update its corresponding type
#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-static-cast-downcast"
    _type->type()->as<IRecordType>()->add_record(_fields.back()->name(),
                                                 _fields.back()->type());
#pragma clang diagnostic pop
}

Status RecordDecl::visit(Visitor& visitor) { return visitor.visit(this); }
}  // namespace hzcc::ast
