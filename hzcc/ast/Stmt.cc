//
// Created by Hanze Chen on 2022/3/29.
//
#include "Stmt.h"

#include <glog/logging.h>
#include <utility>
#include <ostream>

#include "utils/logging.h"

namespace hzcc::ast {
Stmt::Stmt(Position loc, std::string_view name)
    : _id(s_id_counter += 1), _node_location(std::move(loc)), _node_name(name) {
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    INTERNAL_LOG_IF(FATAL,
                    !(_node_location.first >= 0 && _node_location.second >= 0))
        << "location invalid (should be greater than 0)";
    INTERNAL_LOG_IF(FATAL, _node_name.empty()) << "node name empty";
#endif
}

const Position& Stmt::loc() const { return _node_location; }

uint64_t Stmt::Id() const { return _id; }

std::string_view Stmt::NodeName() const { return _node_name; }

std::string Stmt::UniqueName() const {
    return "[" + std::string(NodeName().data()) + ":" + std::to_string(Id()) +
           "]";
}

}  // namespace hzcc::ast
