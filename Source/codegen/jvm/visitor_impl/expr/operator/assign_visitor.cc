//
// Created by chen_ on 2022/4/10.
//
#include "AST/expr/operator/assign.h"
#include "AST/type/Type.h"
#include "codegen/jvm/JVMGenerator.h"
#include "codegen/jvm/utils/TypeUtils.h"
namespace Hzcc::Codegen {
constexpr std::array<const char*, AST::kAssignType_Max> kAssignOPASM = {
    "add", "sub", "mul", "div", "shl", "shr", "and", "or", "xor"};

Status JVMGenerator::visit(AST::AssignExpr* p_expr) {
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

    /** #####################################################################
     *  ### Code Generation                                               ###
     *  ##################################################################### */
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
        (!p_expr->GetLHS()->RetType()->IsArray() &&
         !p_expr->GetLHS()->RetType()->IsFuncPtr() &&
         Utils::GetTypeName(p_expr->GetLHS()->RetType()) == "i") &&
        // Rule2
        p_expr->GetLHS()->GetDeducedValue().has_value()) {
        // then we insert the "IINC" instruction
        auto var_name = ConsumeReturnStack();
        AddToCache(
            "iinc " + std::to_string(GetStackID(var_name)) + " " +
            std::to_string(
                (p_expr->GetAssignType() == AST::kAssignType_AddAssign
                     ? p_expr->GetLHS()->GetDeducedValue().value().AsInt()
                     : -p_expr->GetLHS()->GetDeducedValue().value().AsInt())));
    } else {
        // we first evaluate the LHS get stack location
        HZCC_JVM_NOT_GENERATE_LOAD_INSTR(HZCC_JVM_Visit_Node(p_expr->GetLHS()));

        auto var_name = ConsumeReturnStack();

        // then we evaluate the RHS, for expression need to access its original
        // value like "-=", "+=" etc we have to load the original value first
        if (p_expr->GetAssignType() != AST::kAssignType_Assign) {
            // for reference type like array, we will duplicate the value and
            // refer for gain the value and push back to stack
            if (p_expr->GetLHS()->IsDereference()) {
                AddToCache("dup2");
                AddToCache(Utils::GetTypeName(p_expr->GetLHS()->RetType()) +
                           "aload");
            }

            // for primitive type we can directly use the value
            else {
                AddToCache(LoadFromVariable(var_name));
            }
        }

        // we evaluate the RHS
        HZCC_LEAVE_RET_ON_STACK(                                       // NOLINT
            HZCC_JVM_GENERATE_LOAD_INSTR(                              // NOLINT
                HZCC_JVM_Use_Deduced_IF_POSSIBLE(p_expr->GetRHS())));  // NOLINT

        // here we push instructions to generate the assignment
        if (p_expr->GetAssignType() != AST::kAssignType_Assign) {
            // we then do the operation
            AddToCache(Utils::GetTypeName(p_expr->GetLHS()->RetType(), true) +
                       kAssignOPASM[p_expr->GetAssignType() - 1]);
        }

        // if requested, we will duplicate the value and leave result on stack
        if (_request_leave || _under_compare) {
            if (p_expr->GetLHS()->IsDereference()) {
                AddToCache("dup_x2");
            } else {
                AddToCache("dup");
            }
        }

        // we push the value back to the stack
        AddToCache(SaveToVariable(var_name));
    }

    return Status::OkStatus();
}

}  // namespace Hzcc::Codegen
