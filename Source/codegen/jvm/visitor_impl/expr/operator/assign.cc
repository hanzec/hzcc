//
// Created by chen_ on 2022/4/10.
//
#include "AST/expr/operator/assign.h"

#include "AST/type/Type.h"
#include "codegen/jvm/JVMGenerator.h"
#include "codegen/jvm/utils/TypeUtils.h"
namespace Hzcc::Codegen {
constexpr std::array<const char*, AST::kAssignType_Max> kAssignOPASM = {
    "add", "sub", "mul", "div", "rem", "and",
    "or",  "xor", "shl", "shr", "ushr"};

Status JVMGenerator::visit(AST::AssignExpr* p_expr) {
    // STATIC ASSERT
    DLOG_ASSERT(p_expr != nullptr) << "expression is nullptr";
    DLOG_ASSERT(p_expr->GetLHS() != nullptr)
        << "lhs of expr " << p_expr->GetLHS()->GetNodeName()
        << "(line:" << p_expr->GetLHS()->GetLine() << ") is nullptr";
    DLOG_ASSERT(p_expr->GetRHS() != nullptr)
        << "rhs of expr " << p_expr->GetRHS()->GetNodeName()
        << "(line:" << p_expr->GetRHS()->GetLine() << ") is nullptr";

    // we first evaluate the LHS get stack location
    HZCC_JVM_Visit_Node(p_expr->GetLHS());

    /**
     * if the LHS's type is int and rhs could be deduced we can use iinc
     * instruction. More detail, Expression has to meed to following conditions:
     *      1. LHS is a valid variable with a type of int
     *      2. RHS could be deduced or literal with type of int
     *      3. Assign type only be "+=" or "-="
     * TODO: ptr support here could have problem
     */
    if (  // Rule3
        (p_expr->GetAssignType() == AST::kAssignType_AddAssign ||
         p_expr->GetAssignType() == AST::kAssignType_SubAssign) &&
        // Rule1
        (!p_expr->GetLHS()->GetType()->IsArray() &&
         !p_expr->GetLHS()->GetType()->IsFuncPtr() &&
         p_expr->GetLHS()->GetType()->GetName().find("int") !=
             std::string::npos) &&
        // Rule2
        p_expr->GetLHS()->GetDeducedValue().has_value()) {
        // then we insert the "IINC" instruction
        AddToCache(
            "IINC " + std::to_string(ConsumeReturnStack()) + " " +
            std::to_string(
                (p_expr->GetAssignType() == AST::kAssignType_AddAssign
                     ? p_expr->GetLHS()->GetDeducedValue().value().AsInt()
                     : -p_expr->GetLHS()->GetDeducedValue().value().AsInt())));
    } else {
        // for expression need to access its original value like "-=", "+=" etc.
        auto var_stack_id = ConsumeReturnStack();
        auto var_lhs_type = p_expr->GetLHS()->GetType();

        // then we evaluate the RHS
        if (var_lhs_type->IsArray()) {
            AddToCache("DUP2");
            AddToCache("IALOAD");
        } else {
            AddToCache(HZCC_JVM_GET_EXPR_TYPE(p_expr->GetLHS()) + "LOAD " +
                       std::to_string(var_stack_id));
        }

        // we evaluate the RHS
        EnableGenerateLoad();
        HZCC_JVM_Use_Deduced_IF_POSSIBLE(p_expr->GetRHS());
        DisableGenerateLoad();

        // here we get the original value of the LHS if needed
        if (p_expr->GetAssignType() != AST::kAssignType_Assign) {
            // we then do the operation
            AddToCache(HZCC_JVM_GET_EXPR_TYPE(p_expr->GetLHS()) +
                       kAssignOPASM[p_expr->GetAssignType()]);
        }

        // we push the value back to the stack
        if (var_lhs_type->IsArray()) {
            /**
             * Here we have a stack layout like:
             *  [... , array_ptr, index, added_value]
             */
            AddToCache(HZCC_JVM_GET_EXPR_TYPE(p_expr->GetLHS()) + "ASTORE");
        } else {
            AddToCache(HZCC_JVM_GET_EXPR_TYPE(p_expr->GetLHS()) + "STORE " +
                       std::to_string(var_stack_id));
        }
    }

    return Status::OkStatus();
}

}  // namespace Hzcc::Codegen
