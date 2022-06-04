//
// Created by Hanze Chen on 2022/3/29.
//
#include "ASTNode.h"

#include <algorithm>
#include <utility>

#include "AST/DeduceValue.h"
#include "AST/expr/CastExpr.h"
#include "AST/type/ArrayType.h"
#include "AST/utils/macro.h"
#include "utils/message_utils.h"
namespace Hzcc::AST {
ASTNode::ASTNode(std::pair<int, int> loc)
    : _id(_counter_ += 1), _node_location(std::move(loc)) {
    HZCC_RUNTIME_CHECK(_node_location.first >= 0 && _node_location.second >= 0)
        << "location invalid (should be greater than 0)";
}

std::string ASTNode::Dump(const std::string& ident) const {
    auto new_ident = ident;
    std::replace(new_ident.begin(), new_ident.end(), '`', ' ');
    return ident + "-" + NodeName() + " [" + std::to_string(Id()) + "] " +
           "<line:" + std::to_string(_node_location.first) +
           ", col:" + std::to_string(_node_location.second) + ">" + " " +
           PrintDetail(new_ident);
}

const std::pair<int, int>& ASTNode::Location() const { return _node_location; }

uint64_t ASTNode::Id() const { return _id; }

std::string ASTNode::PrintDetail(const std::string& ident) const { return {}; }
ASTNode::~ASTNode() {}
std::optional<DeduceValue> ASTNode::GetDeducedValue() const {
    return std::nullopt;
}
std::shared_ptr<Type> ASTNode::RetType() const {
    DLOG(FATAL) << "RetType() is not implemented for " << NodeName();
    return nullptr;
}
std::unique_ptr<AST::ASTNode> ASTNode::CastTo(
    const std::shared_ptr<Type>& lhs_type, std::unique_ptr<ASTNode> rhs) {
    bool modified = false;
    while (!(*rhs->RetType() == *lhs_type)) {
        modified = false;
        DVLOG(SYNTAX_LOG_LEVEL) << "Tying Cast: " << rhs->RetType()->GetName()
                                << " to " << lhs_type->Dump();

        // special rules for deduce value
        if (rhs->GetDeducedValue().has_value()) {
            // we allowed double to float cast since value type since all
            // deduced value are calculate as double
            if (rhs->GetDeducedValue().value().GetType() ==
                    DeduceValueType::kDeduceValueType_Real_Number &&
                lhs_type->IsFloat()) {
                modified = true;
                auto loc = rhs->Location();
                rhs = std::make_unique<AST::CastExpr>(lhs_type, std::move(rhs),
                                                      loc);
                DVLOG(SYNTAX_LOG_LEVEL)
                    << "Cast DeducedValue:" << rhs->Dump("");
            }

            // [int = char] cast is allowed
            else if (rhs->RetType()->IsChar() && lhs_type->IsInteger()) {
                modified = true;
                auto loc = rhs->Location();
                rhs = std::make_unique<AST::CastExpr>(lhs_type, std::move(rhs),
                                                      loc);
                DVLOG(SYNTAX_LOG_LEVEL)
                    << "Cast DeducedValue:" << rhs->Dump("");
            }
        }

        // variable wider cast
        else if (rhs->RetType()->IsInteger() && !lhs_type->IsArray() &&
                 !rhs->RetType()->IsArray()) {
            if (lhs_type->IsFloat()) {
                modified = true;
                auto loc = rhs->Location();
                rhs = std::make_unique<AST::CastExpr>(
                    Type::GetTypeOf("float", rhs->RetType()->GetAttributes()),
                    std::move(rhs), loc);
                DVLOG(SYNTAX_LOG_LEVEL) << "Using CastExpr:" << rhs->Dump("");
            }
        }

        else if (rhs->RetType()->IsDouble() && !lhs_type->IsArray() &&
                 !rhs->RetType()->IsArray()) {
            if (lhs_type->IsFloat()) {
                if (!rhs->IsLiteral()) {
                    Message::set_current_part("RetType checking");
                    Message::print_message(
                        Message::Level::kWarning,
                        "Narrowing conversion from 'double' to 'float'",
                        rhs->_node_location);
                    return nullptr;
                }
                modified = true;
                auto loc = rhs->Location();
                rhs = std::make_unique<AST::CastExpr>(
                    Type::GetTypeOf("float", rhs->RetType()->GetAttributes()),
                    std::move(rhs), loc);
                DVLOG(SYNTAX_LOG_LEVEL) << "Using CastExpr:" << rhs->Dump("");
            } else {
                return nullptr;
            }
        }

        else if (rhs->RetType()->IsFloat() && !lhs_type->IsArray() &&
                 !rhs->RetType()->IsArray()) {
            if (lhs_type->IsDouble()) {
                modified = true;
                auto loc = rhs->Location();
                rhs = std::make_unique<AST::CastExpr>(
                    Type::GetTypeOf("double", rhs->RetType()->GetAttributes()),
                    std::move(rhs), loc);
                DVLOG(SYNTAX_LOG_LEVEL) << "Using CastExpr:" << rhs->Dump("");
            } else {
                return nullptr;
            }
        }

        else if (rhs->RetType()->IsChar() && !lhs_type->IsArray() &&
                 !rhs->RetType()->IsArray()) {
            if (lhs_type->IsInteger()) {
                modified = true;
                auto loc = rhs->Location();
                rhs = std::make_unique<AST::CastExpr>(
                    Type::GetTypeOf("int", rhs->RetType()->GetAttributes()),
                    std::move(rhs), loc);
                DVLOG(SYNTAX_LOG_LEVEL) << "Using CastExpr:" << rhs->Dump("");
            } else if (lhs_type->IsFloat()) {
                modified = true;
                auto loc = rhs->Location();
                auto rhs_attr = rhs->RetType()->GetAttributes();
                rhs = std::make_unique<AST::CastExpr>(
                    Type::GetTypeOf("int", rhs_attr),
                    std::make_unique<AST::CastExpr>(
                        Type::GetTypeOf("float", rhs_attr), std::move(rhs),
                        loc),
                    loc);
                DVLOG(SYNTAX_LOG_LEVEL) << "Using CastExpr:" << rhs->Dump("");
            }
            break;
        } else {
            return nullptr;
        }

        // prevent infinite loop when no modification is made
        if (!modified) {
            DVLOG(SYNTAX_LOG_LEVEL) << "No modification made, failed to cast";
            return nullptr;
        }
    }

    return std::move(rhs);
}
}  // namespace Hzcc::AST
