//
// Created by chen_ on 2022/4/10.
//
#include "AST/expr/operator/arithmetic.h"

#include "AST/ASTNode.h"
#include "codegen/jvm/JVMGenerator.h"
#include "codegen/jvm/utils/TypeUtils.h"

namespace Hzcc::Codegen {
constexpr static std::array<const char*, AST::kArithmeticType_Max>
    ArithmeticASM = {"add", "sub", "mul", "div", "rem"};

Status JVMGenerator::visit(Hzcc::AST::ArithmeticExpr* p_expr) {
    // STATIC ASSERT
    DLOG_ASSERT(p_expr != nullptr) << "expression is nullptr";
    DLOG_ASSERT(p_expr->GetLHS() != nullptr)
        << "lhs of expr " << p_expr->GetLHS()->GetNodeName()
        << "(line:" << p_expr->GetLHS()->GetLine() << ") is nullptr";
    DLOG_ASSERT(p_expr->GetRHS() != nullptr)
        << "rhs of expr " << p_expr->GetRHS()->GetNodeName()
        << "(line:" << p_expr->GetRHS()->GetLine() << ") is nullptr";
    DLOG_ASSERT(p_expr->GetRHS()->GetDeducedValue() != std::nullopt &&
                p_expr->GetLHS()->GetDeducedValue() != std::nullopt)
        << "Current expr could deduce value, but use code gen";


    // visit LHS and RHS
    EnableGenerateLoad();
    HZCC_JVM_Use_Deduced_IF_POSSIBLE(p_expr->GetLHS());
    HZCC_JVM_Use_Deduced_IF_POSSIBLE(p_expr->GetLHS());
    DisableGenerateLoad();

    // write operation
    AddToCache(Utils::GetJVMTypename(p_expr->GetType()->GetName()) +
               ArithmeticASM[p_expr->GetOpType()]);

    return Status::OkStatus();
}
}  // namespace Hzcc::Codegen
