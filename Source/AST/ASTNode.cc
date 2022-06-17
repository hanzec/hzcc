//
// Created by Hanze Chen on 2022/3/29.
//
#include "ASTNode.h"

#include <algorithm>
#include <utility>

#include "AST/DeduceValue.h"
#include "AST/cast/expr/CastExpr.h"
#include "AST/type/ArrayType.h"
#include "AST/type/BuiltinType.h"
#include "AST/utils/macro.h"
#include "utils/message_utils.h"
namespace Hzcc::AST {
ASTNode::ASTNode(Position loc) : _node_location(std::move(loc)) {
    HZCC_RUNTIME_CHECK(_node_location.first >= 0 && _node_location.second >= 0)
        << "location invalid (should be greater than 0)";
}

void ASTNode::Dump(std::ostream& out, const std::string& ident) const {
    auto new_ident = ident;
    std::replace(new_ident.begin(), new_ident.end(), '`', ' ');

    // print node info
    if (Options::Global_disable_color) {
        out << std::endl
            << ident << '-' << NodeName() << " [" << std::hex << this << "] "
            << "<line:" << std::to_string(_node_location.first)
            << ", col:" << std::to_string(_node_location.second) << "> ";
    } else {
        // ident
        out << std::endl << KEnableBlue << ident << '-' << KDisableColor;

        // name
        if (IsDeclNode()) {
            out << KEnableGreen << NodeName() << KDisableColor;
        } else {
            out << KEnableMagenta << NodeName() << KDisableColor;
        }

        // line info
        out << KEnableYellow << " [" << std::hex << this << "] "
            << "<line:" << std::to_string(_node_location.first)
            << ", col:" << std::to_string(_node_location.second) << "> "
            << KDisableColor;
    }

    // print additional info
    PrintDetail(out, new_ident);
}

const Position& ASTNode::Location() const { return _node_location; }

void ASTNode::PrintDetail(std::ostream& out, const std::string& ident) const {}
std::optional<DeduceValue> ASTNode::GetDeducedValue() const {
    return std::nullopt;
}

std::ostream& ASTNode::operator<<(std::ostream& out) const {
    Dump(out, "");
    return out;
}
uintptr_t ASTNode::Id() const { return reinterpret_cast<uintptr_t>(this); }

std::shared_ptr<Type> ASTNode::GetCharType() {
    const static std::shared_ptr<Type> char_type =
        std::make_shared<BuiltinType>(kPrimitiveType_char);
    return char_type;
}
std::shared_ptr<Type> ASTNode::GetIntType() {
    const static std::shared_ptr<Type> int_type =
        std::make_shared<BuiltinType>(kPrimitiveType_int);
    return int_type;
}
std::shared_ptr<Type> ASTNode::GetFloatType() {
    const static std::shared_ptr<Type> float_type =
        std::make_shared<BuiltinType>(kPrimitiveType_float);
    return float_type;
}
std::shared_ptr<Type> ASTNode::GetVoidType() {
    const static std::shared_ptr<Type> void_type =
        std::make_shared<BuiltinType>(kPrimitiveType_void);
    return void_type;
}
}  // namespace Hzcc::AST
