//
// Created by chen_ on 2022/4/10.
//
#include "AST/ASTNode.h"
#include "AST/expr/op/ArithmeticExpr.h"
#include "codegen/jvm/JVMGenerator.h"
#include "codegen/jvm/utils/TypeUtils.h"

namespace Hzcc::Codegen {
constexpr static std::array<const char*, AST::kArithmeticType_ENUM_SIZE>
    ArithmeticASM = {"add", "sub", "mul", "div", "rem"};

Status JVMGenerator::visit(Hzcc::AST::ArithmeticExpr* p_expr) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    DLOG_ASSERT(p_expr != nullptr) << "expression is nullptr";
    DLOG_ASSERT(p_expr->GetLHS() != nullptr)
        << "lhs of expr " << p_expr->GetLHS()->NodeName()
        << "(line:" << p_expr->GetLHS()->GetLine() << ") is nullptr";
    DLOG_ASSERT(p_expr->GetRHS() != nullptr)
        << "rhs of expr " << p_expr->GetRHS()->NodeName()
        << "(line:" << p_expr->GetRHS()->GetLine() << ") is nullptr";
    DLOG_ASSERT(!(p_expr->GetRHS()->GetDeducedValue() != std::nullopt &&
                  p_expr->GetLHS()->GetDeducedValue() != std::nullopt))
        << "Current expr could deduce value, but use code gen";

    /** #####################################################################
     *  ### Code Generation                                               ###
     *  ##################################################################### */
    if (_request_leave || _under_compare) {
        HZCC_LEAVE_RET_ON_STACK(HZCC_JVM_GENERATE_LOAD_INSTR({
            HZCC_JVM_Use_Deduced_IF_POSSIBLE(p_expr->GetLHS());
            HZCC_JVM_Use_Deduced_IF_POSSIBLE(p_expr->GetRHS());
        }))

        // write operation
        AddToCache(Utils::GetTypeName(p_expr->RetType(), true) +
                   ArithmeticASM[p_expr->GetOpType()]);
    } else {
        DVLOG(MESSAGE_ERROR_TRACING)
            << "skip code gen for node " << p_expr->NodeName()
            << "(line:" << p_expr->GetLine() << " id: " << p_expr->Id() << ")";
    }

    return Status::OkStatus();
}
}  // namespace Hzcc::Codegen
