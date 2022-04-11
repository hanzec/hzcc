//
// Created by chen_ on 2022/3/29.
//
#include "ASTNode.h"

#include <utility>

#include "AST/DeduceValue.h"
#include "AST/expr/cast/cast.h"
#include "AST/expr/cast/const_add.h"
#include "AST/expr/cast/const_remove.h"
#include "AST/type/ArrayType.h"
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
    std::stringstream ret;

    if (IsDeclNode()) {
        auto type_name = GetType()->GetName();
        if (type_name.find('[') != std::string::npos) {
            ret << "\tLine " << std::setw(3) << std::to_string(GetLine() + 1)
                << std::setw(0)
                << ": local " +
                       GetType()->GetName().substr(
                           0, type_name.find_first_of('[')) +
                       " " + PrintAdditionalInfo("") +
                       GetType()->GetName().substr(type_name.find_first_of('['),
                                                   type_name.find_last_of(']'));
        } else {
            ret << "\tLine " << std::setw(3) << std::to_string(GetLine() + 1)
                << std::setw(0)
                << ": local " + GetType()->GetName() + " " +
                       PrintAdditionalInfo("");
        }
    } else {
        ret << std::string(ident) + "Line " << std::setw(3)
            << std::to_string(GetLine() + 1) << std::setw(0)
            << ": expression has type " + GetType()->GetName();
    }

    return ret.str();
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
std::shared_ptr<Type> ASTNode::GetType() const { return nullptr; }
std::unique_ptr<AST::ASTNode> ASTNode::CastTo(
    const std::shared_ptr<Type>& lhs_type, std::unique_ptr<ASTNode> rhs) {
    bool modified = false;
    while (!rhs->GetType()->IsSame(lhs_type)) {
        modified = false;
        DVLOG(SYNTAX_LOG_LEVEL) << "Tying Cast: " << rhs->GetType()->GetName()
                                << " to " << lhs_type->Dump();
        // const remover
        if (rhs->GetType()->IsConst() && !lhs_type->IsConst()) {
            modified = true;
            auto final_type = rhs->GetType()->GetUnConstType();
            rhs = std::make_unique<AST::ConstRemoveCast>(std::move(rhs),
                                                         final_type);
            DVLOG(SYNTAX_LOG_LEVEL)
                << "Using ConstRemoveCast: " << rhs->Dump("");
        }

        // const adder
        else if (!rhs->GetType()->IsConst() && lhs_type->IsConst()) {
            modified = true;
            auto final_type = rhs->GetType()->GetConstType();
            rhs =
                std::make_unique<AST::ConstAddCast>(std::move(rhs), final_type);
            DVLOG(SYNTAX_LOG_LEVEL)
                << "Using ConstAddCast:" << rhs->GetType()->Dump();
        }

        // variable wider cast
        else if (rhs->GetType()->GetName().find("int") != std::string::npos &&
                 !lhs_type->IsArray() && !rhs->GetType()->IsArray()) {
            if (lhs_type->GetName().find("float") != std::string::npos) {
                modified = true;
                rhs = std::make_unique<AST::CastExpr>(
                    Type::GetBasicType("float",
                                       rhs->GetType()->GetAttributes()),
                    std::move(rhs));
                DVLOG(SYNTAX_LOG_LEVEL) << "Using CastExpr:" << rhs->Dump("");
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
                    return nullptr;
                }
                modified = true;
                rhs = std::make_unique<AST::CastExpr>(
                    Type::GetBasicType("float",
                                       rhs->GetType()->GetAttributes()),
                    std::move(rhs));
                DVLOG(SYNTAX_LOG_LEVEL) << "Using CastExpr:" << rhs->Dump("");
            } else {
                return nullptr;
            }
        }

        else if (rhs->GetType()->GetName().find("float") != std::string::npos &&
                 !lhs_type->IsArray() && !rhs->GetType()->IsArray()) {
            if (lhs_type->GetName().find("double") != std::string::npos) {
                modified = true;
                rhs = std::make_unique<AST::CastExpr>(
                    Type::GetBasicType("double",
                                       rhs->GetType()->GetAttributes()),
                    std::move(rhs));
                DVLOG(SYNTAX_LOG_LEVEL) << "Using CastExpr:" << rhs->Dump("");
            } else {
                return nullptr;
            }
        }

        else if (rhs->GetType()->GetName().find("char") != std::string::npos &&
                 !lhs_type->IsArray() && !rhs->GetType()->IsArray()) {
            if (lhs_type->GetName().find("int") != std::string::npos) {
                modified = true;
                rhs = std::make_unique<AST::CastExpr>(
                    Type::GetBasicType("int", rhs->GetType()->GetAttributes()),
                    std::move(rhs));
                DVLOG(SYNTAX_LOG_LEVEL) << "Using CastExpr:" << rhs->Dump("");
            } else if (lhs_type->GetName().find("float") != std::string::npos) {
                modified = true;
                auto rhs_attr = rhs->GetType()->GetAttributes();
                rhs = std::make_unique<AST::CastExpr>(
                    Type::GetBasicType("int", rhs_attr),
                    std::make_unique<AST::CastExpr>(
                        Type::GetBasicType("float", rhs_attr), std::move(rhs)));
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
int ASTNode::GetLineNumber() const { return _node_location.first; }
}  // namespace Mycc::AST