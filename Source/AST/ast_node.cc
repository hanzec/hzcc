//
// Created by chen_ on 2022/3/29.
//
#include "ast_node.h"

#include <utility>

#include "AST/cast/array_cast.h"
#include "AST/cast/const_add.h"
#include "AST/cast/const_remove.h"
#include "AST/expr/cast.h"
#include "AST/type/array_type.h"
#include "utils/message_utils.h"
namespace Mycc::AST {

ASTNode::ASTNode() : ASTNode({-1, -1}) {}

ASTNode::ASTNode(std::pair<int, int> loc)
    : _id(_counter_ += 1), _node_location(std::move(loc)) {}

std::string ASTNode::Dump(const std::string_view ident) const {
#ifndef NDEBUG
    return std::string(ident) + "-" + GetNodeName() + " [" +
           std::to_string(GetNodeId()) + "] " + GetLocation() + " " +
           PrintAdditionalInfo(ident);
#else
    return std::string(ident) + "Line " + std::to_string(_node_location.first) +
           ": " +
           (IsDeclNode() ? "local " + GetType()->GetName() + " " +
                               PrintAdditionalInfo(ident)
                         : "expression has a type " + GetType()->GetName());
#endif
}

std::string ASTNode::GetLocation() const {
    return "<line:" + std::to_string(_node_location.first) +
           ", col:" + std::to_string(_node_location.second) + ">";
}
uint64_t ASTNode::GetNodeId() const { return _id; }

std::string ASTNode::PrintAdditionalInfo(const std::string_view ident) const {
    return {};
}
ASTNode::~ASTNode() {}
std::optional<DeduceValue> ASTNode::GetDeducedValue() const {
    return std::nullopt;
}
std::shared_ptr<Type> ASTNode::GetType() const {
    return std::make_shared<Type>("NOT_IMPL");
}
std::unique_ptr<AST::ASTNode> ASTNode::CastTo(
    const std::shared_ptr<Type>& lhs_type, std::unique_ptr<ASTNode> rhs) {
    while (!rhs->GetType()->IsSame(lhs_type)) {
        // const remover
        if (rhs->GetType()->IsConst() && !lhs_type->IsConst()) {
            auto getType =
                std::make_shared<Type>(rhs->GetType()->GetName(true));
            rhs =
                std::make_unique<AST::ConstRemoveCast>(std::move(rhs), getType);
        }

        // const adder
        else if (!rhs->GetType()->IsConst() && lhs_type->IsConst()) {
            static const std::list<std::string> const_attr = {"const"};
            auto getType = std::make_shared<Type>(rhs->GetType()->GetName(true),
                                                  const_attr);
            rhs = std::make_unique<AST::ConstAddCast>(std::move(rhs), getType);

        }

        // variable wider cast
        // array type cast
        else if (rhs->GetType()->IsArray() && lhs_type->IsArray()) {
            auto lhs_array_type =
                std::dynamic_pointer_cast<ArrayType>(lhs_type);
            auto rhs_array_type =
                std::dynamic_pointer_cast<ArrayType>(rhs->GetType());

            // if lhs is a pointer we cast array to pointer
            if (!lhs_array_type->HasSize()) {
                auto newType = std::make_shared<ArrayType>(
                    std::make_shared<Type>(lhs_array_type->GetName(true)));

                rhs = std::make_unique<AST::ArrayUnsizeCast>(std::move(rhs),
                                                             newType);
            }

            break;
        }

        else if (rhs->GetType()->GetName().find("int") != std::string::npos &&
                 !lhs_type->IsArray() && !rhs->GetType()->IsArray()) {
            if (lhs_type->GetName().find("float") != std::string::npos) {
                rhs = std::make_unique<AST::CastExpr>(
                    std::make_shared<Type>("float"), std::move(rhs));
            }
        }

        else if (rhs->GetType()->GetName().find("double") !=
                     std::string::npos &&
                 !lhs_type->IsArray() && !rhs->GetType()->IsArray()) {
            if (lhs_type->GetName().find("float") != std::string::npos) {
                if (!rhs->IsLiteral()) {
                    Message::set_current_part("Type checking");
                    Message::print_message(
                        Message::Level::kWarning,
                        "Narrowing conversion from 'double' to 'float'",
                        rhs->_node_location);
                }
                rhs = std::make_unique<AST::CastExpr>(
                    std::make_shared<Type>("float"), std::move(rhs));
            } else {
                return nullptr;
            }
        }

        else if (rhs->GetType()->GetName().find("float") != std::string::npos &&
                 !lhs_type->IsArray() && !rhs->GetType()->IsArray()) {
            if (lhs_type->GetName().find("double") != std::string::npos) {
                rhs = std::make_unique<AST::CastExpr>(
                    std::make_shared<Type>("double"), std::move(rhs));
            } else {
                return nullptr;
            }
        }

        else if (rhs->GetType()->GetName().find("char") != std::string::npos &&
                 !lhs_type->IsArray() && !rhs->GetType()->IsArray()) {
            if (lhs_type->GetName().find("int") != std::string::npos) {
                rhs = std::make_unique<AST::CastExpr>(
                    std::make_shared<Type>("int"), std::move(rhs));
            } else if (lhs_type->GetName().find("float") != std::string::npos) {
                rhs = std::make_unique<AST::CastExpr>(
                    std::make_shared<Type>("int"),
                    std::make_unique<AST::CastExpr>(
                        std::make_shared<Type>("float"), std::move(rhs)));
            }

            break;
        } else {
            return nullptr;
        }
    }

    return std::move(rhs);
}

}  // namespace Mycc::AST