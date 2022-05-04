//
// Created by chen_ on 2022/5/3.
//

#include "FuncAnalyzer.h"

#include "AST/expr/cast.h"
#include "AST/expr/operator/arithmetic.h"
#include "AST/expr/operator/assign.h"
#include "AST/statement/compound.h"
#include "codegen/jvm/utils/macro.h"

namespace Hzcc::Codegen {

Status FuncAnalyzer::visit(Hzcc::AST::CastExpr *p_expr) {
    /**
     * Cast Expression will consume one stack slot and produce one stack slot.
     * Which means the max stack size wil be not changed.
     */
    return p_expr->GetCastExpr()->visit(*this);
}

Status FuncAnalyzer::visit(Hzcc::AST::ArithmeticExpr *p_expr) {
    /**
     * Arithmetic Expression will consume two stack slots and produce one stack
     * slot. Which means decrease the current stack by 1.
     */

    // First we evaluate the left expression.
    HZCC_JVM_Visit_Node(p_expr->GetLHS());

    // Then we evaluate the right expression.
    HZCC_JVM_Visit_Node(p_expr->GetRHS());

    // Then we consume the two stack slots and produce one stack slot.
    DecreaseCurrentStack(1);

    return Status::OkStatus();
}

Status FuncAnalyzer::visit(Hzcc::AST::AssignExpr *p_expr) {
    /**
     * Stack change for Assign Expression will be determined by the RHS
     * expression and the LHS expression. Also depends on the type of the LHS
     */

    // we first evaluate the LHS get stack location
    HZCC_JVM_NOT_GENERATE_LOAD_INSTR(HZCC_JVM_Visit_Node(p_expr->GetLHS()));

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
        // If we use INNC instr here we will not consume the stack.
    } else {
        auto var_lhs_type = p_expr->GetLHS()->GetType();

        // then we evaluate the RHS
        if (var_lhs_type->IsArray()) {
            // Duplicate the array reference and its index
            IncreaseCurrentStack(2);

            // load the array reference and index into the stack
            DecreaseCurrentStack(1);
        } else {
            // Load from the local variable to the stack
            IncreaseCurrentStack(1);
        }

        // we evaluate the RHS
        HZCC_JVM_GENERATE_LOAD_INSTR({
            const auto &deduced_lhs = p_expr->GetRHS()->GetDeducedValue();
            if (deduced_lhs.has_value()) {
                // if we rhs could be deduced, we only need to push the deduced
                // value to the stack
                IncreaseCurrentStack(1);
            } else {
                HZCC_JVM_Visit_Node(p_expr->GetRHS());
            }
        });

        // here we get the original value of the LHS if needed
        if (p_expr->GetAssignType() != AST::kAssignType_Assign) {
            // we consume two stack slots and produce one stack slot for result
            DecreaseCurrentStack(1);
        }

        // we push the value back to the stack
        if (var_lhs_type->IsArray()) {
            // *ASTORE will consume three stack slots
            DecreaseCurrentStack(3);
        } else {
            // *STORE will consume two stack slots
            DecreaseCurrentStack(1);
        }
    }

    return Status::OkStatus();
}

Status FuncAnalyzer::visit(Hzcc::AST::BitwiseExpr *p_expr) {
    return Status::OkStatus();
}

Status FuncAnalyzer::visit(Hzcc::AST::LogicalExpr *p_expr) {
    return Status::OkStatus();
}

Status FuncAnalyzer::visit(Hzcc::AST::CommaExpr *p_expr) {
    return Status::OkStatus();
}

Status FuncAnalyzer::visit(Hzcc::AST::RelationalExpr *p_expr) {
    return Status::OkStatus();
}
Status FuncAnalyzer::visit(Hzcc::AST::AccessExpr *p_expr) {
    return Status::OkStatus();
}
Status FuncAnalyzer::visit(Hzcc::AST::ArraySubscriptExpr *p_expr) {
    return Status::OkStatus();
}
Status FuncAnalyzer::visit(Hzcc::AST::ConditionalExpr *p_expr) {
    return Status::OkStatus();
}
Status FuncAnalyzer::visit(Hzcc::AST::DeclRefExpr *p_expr) {
    return Status::OkStatus();
}
Status FuncAnalyzer::visit(Hzcc::AST::SizeofExpr *p_expr) {
    return Status::OkStatus();
}
Status FuncAnalyzer::visit(Hzcc::AST::UnaryExpr *p_expr) {
    return Status::OkStatus();
}
Status FuncAnalyzer::visit(Hzcc::AST::CompoundStmt *p_expr) {
    /**
     * CompoundStmt is a list of statements. We will visit each statement
     * in the list in order to get the stack size and variables;
     */

    for (auto &stmt : p_expr->GetBodyStatements()) {
        HZCC_JVM_Visit_Node(stmt);
    }
    return Status::OkStatus();
}

Status FuncAnalyzer::visit(Hzcc::AST::BreakStatement *p_expr) {
    return Status::OkStatus();
}
Status FuncAnalyzer::visit(Hzcc::AST::ContinueStatement *p_expr) {
    return Status::OkStatus();
}
Status FuncAnalyzer::visit(Hzcc::AST::DoStatement *p_expr) {
    return Status::OkStatus();
}
Status FuncAnalyzer::visit(Hzcc::AST::EmptyStatement *p_expr) {
    return Status::OkStatus();
}
Status FuncAnalyzer::visit(Hzcc::AST::ForStatement *p_expr) {
    return Status::OkStatus();
}
Status FuncAnalyzer::visit(Hzcc::AST::FunctionCall *p_expr) {
    return Status::OkStatus();
}
Status FuncAnalyzer::visit(Hzcc::AST::FunctionDeclNode *p_expr) {
    return Status::OkStatus();
}
Status FuncAnalyzer::visit(Hzcc::AST::DeclNode *p_expr) {
    return Status::OkStatus();
}
Status FuncAnalyzer::visit(Hzcc::AST::IfStatement *p_expr) {
    return Status::OkStatus();
}
Status FuncAnalyzer::visit(Hzcc::AST::ParamVarDecl *p_expr) {
    return Status::OkStatus();
}
Status FuncAnalyzer::visit(Hzcc::AST::ReturnNode *p_expr) {
    return Status::OkStatus();
}
Status FuncAnalyzer::visit(Hzcc::AST::WhileStatement *p_expr) {
    return Status::OkStatus();
}
Status FuncAnalyzer::visit(Hzcc::AST::LiteralExpr *p_expr) {
    return Status::OkStatus();
}
Status FuncAnalyzer::visit(Hzcc::AST::VarDecl *p_expr) {
    return Status::OkStatus();
}

void FuncAnalyzer::IncreaseCurrentStack(uint8_t p_size) {
    _current_stack_size += p_size;
    if (_current_stack_size > _max_stack_size) {
        _max_stack_size = _current_stack_size;
    }
}
void FuncAnalyzer::DecreaseCurrentStack(uint8_t p_size) {
    _current_stack_size -= p_size;
}
void FuncAnalyzer::EnableGenerateLoad() { _generate_load = true; }
void FuncAnalyzer::DisableGenerateLoad() { _generate_load = false; }
bool FuncAnalyzer::GetGenerateLoadStatus() const { return _generate_load; }
uint32_t FuncAnalyzer::GetMaxStackSize() const { return _max_stack_size; }
const std::list<std::tuple<const std::string_view, char, uint32_t>>
    &FuncAnalyzer::GetLocalVariable() {
    return _local_var_map;
}
}  // namespace Hzcc::Codegen