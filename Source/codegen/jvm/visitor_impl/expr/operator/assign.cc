//
// Created by chen_ on 2022/4/10.
//
#include "AST/expr/operator/assign.h"

#include "AST/type/Type.h"
#include "codegen/jvm/JVMGenerator.h"
#include "codegen/jvm/utils/TypeUtils.h"
namespace Mycc::Codegen {
Status JVMGenerator::visit(std::unique_ptr<AST::AssignExpr>& p_expr) {
    // first we acquire RHS
    p_expr->GetRHS()->visit(*this);

    // then we acquire LHS with its type and stack id
    p_expr->GetLHS()->visit(*this);
    auto lhs_id = ConsumeReturnStack();
    auto lhs_type = std::tolower(
        TypeUtils::GetJVMTypename(p_expr->GetLHS()->GetType()->GetName()));

    // perform assignment
    auto& output = GetOstream();
    switch (p_expr->GetAssignType()) {
        case AST::AssignExpr::kAssign:
            // do nothing
            break;
        case AST::AssignExpr::kAddAssign:
            output << GetLineIndent() << lhs_type << "load_" << lhs_id
                   << std::endl;
            output << GetLineIndent() << lhs_type << "add" << std::endl;
            break;
        case AST::AssignExpr::kSubAssign:
            output << GetLineIndent() << lhs_type << "load_" << lhs_id
                   << std::endl;
            output << GetLineIndent() << lhs_type << "sub" << std::endl;
            break;
        case AST::AssignExpr::kMulAssign:
            output << GetLineIndent() << lhs_type << "load_" << lhs_id
                   << std::endl;
            output << GetLineIndent() << lhs_type << "mul" << std::endl;
            break;
        case AST::AssignExpr::kDivAssign:
            output << GetLineIndent() << lhs_type << "load_" << lhs_id
                   << std::endl;
            output << GetLineIndent() << lhs_type << "div" << std::endl;
            break;
        case AST::AssignExpr::kModAssign:
            output << GetLineIndent() << lhs_type << "load_" << lhs_id
                   << std::endl;
            output << GetLineIndent() << lhs_type << "rem" << std::endl;
            break;
        case AST::AssignExpr::kLShiftAssign:
            output << GetLineIndent() << lhs_type << "load_" << lhs_id
                   << std::endl;
            output << GetLineIndent() << lhs_type << "shl" << std::endl;
            break;
        case AST::AssignExpr::kRShiftAssign:
            output << GetLineIndent() << lhs_type << "load_" << lhs_id
                   << std::endl;
            output << GetLineIndent() << lhs_type << "shr" << std::endl;
            break;
        case AST::AssignExpr::kAndAssign:
            output << GetLineIndent() << lhs_type << "load_" << lhs_id
                   << std::endl;
            output << GetLineIndent() << lhs_type << "and" << std::endl;
            break;
        case AST::AssignExpr::kOrAssign:
            output << GetLineIndent() << lhs_type << "load_" << lhs_id
                   << std::endl;
            output << GetLineIndent() << lhs_type << "or" << std::endl;
            break;
        case AST::AssignExpr::kXorAssign:
            output << GetLineIndent() << lhs_type << "load_" << lhs_id
                   << std::endl;
            output << GetLineIndent() << lhs_type << "xor" << std::endl;
            break;
    }

    // there we are saving the result to LHS
    output << GetLineIndent()
           << std::tolower(TypeUtils::GetJVMTypename(
                  p_expr->GetLHS()->GetType()->GetName()))
           << "store_" << ConsumeReturnStack() << std::endl;
}

}  // namespace Mycc::Codegen
