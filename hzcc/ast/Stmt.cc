//
// Created by Hanze Chen on 2022/3/29.
//
#include "Stmt.h"

#include <algorithm>
#include <utility>

#include "ast/DeduceValue.h"
#include "ast/expr/cast/CastExpr.h"
#include "utils/status/statusor.h"
namespace hzcc::ast {
Stmt::Stmt(Position loc, const std::string& name)
    : _id(s_id_counter += 1), _node_location(std::move(loc)), _node_name(name) {
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    INTERNAL_LOG_IF(FATAL,
                    !(_node_location.first >= 0 && _node_location.second >= 0))
        << "location invalid (should be greater than 0)";
    INTERNAL_LOG_IF(FATAL, _node_name.empty()) << "node name empty";
#endif
}

void Stmt::Dump(std::ostream& out, const std::string& ident) const {
    auto new_ident = ident;
    std::replace(new_ident.begin(), new_ident.end(), '`', ' ');

    // print node info
    out << std::endl
        << ident << '-' << NodeName() << " [" << std::to_string(Id()) << "] "
        << "<line:" << std::to_string(_node_location.first)
        << ", col:" << std::to_string(_node_location.second) << "> ";

    // print additional info
    PrintDetail(out, new_ident);
}

const Position& Stmt::Location() const { return _node_location; }

uint64_t Stmt::Id() const { return _id; }

std::ostream& Stmt::operator<<(std::ostream& out) const {
    Dump(out, "");
    return out;
}
const std::string_view& Stmt::NodeName() const { return _node_name; }

std::string Stmt::UniqueName() const {
    return "[" + std::string(NodeName().data()) + ":" + std::to_string(Id()) +
           "]";
}

}  // namespace hzcc::ast
