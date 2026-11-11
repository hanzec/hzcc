//
// Created by chen_ on 2022/5/3.
//

#include "FuncAnalyzer.h"

#include <algorithm>

#include "AST/expr/ArraySubscriptExpr.h"
#include "AST/expr/CastExpr.h"
#include "AST/expr/DeclRefExpr.h"
#include "AST/expr/SizeofExpr.h"
#include "AST/expr/UnaryOperator.h"
#include "AST/expr/operator/RelationalExpr.h"
#include "AST/expr/operator/arithmetic.h"
#include "AST/expr/operator/assign.h"
#include "AST/expr/operator/bitwise.h"
#include "AST/expr/operator/comma.h"
#include "AST/expr/operator/logical.h"
#include "AST/statement/CompoundStmt.h"
#include "AST/statement/FuncCallStmt.h"
#include "AST/statement/ParamVarDecl.h"
#include "AST/statement/ReturnStmt.h"
#include "AST/statement/VarDecl.h"
#include "AST/statement/empty.h"
#include "AST/type/ArrayType.h"
#include "codegen/jvm/utils/macro.h"
namespace Hzcc::Codegen::JVM {

/**
 * @brief Analyze stack and symbol effects for a CastExpr node in the legacy backend.
 * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
 * @return OK on success, or the first backend/pass error.
 */
Status FuncAnalyzer::visit(Hzcc::AST::CastExpr *p_expr) {
    // firs we get the type of the cast
    HZCC_JVM_Visit_Node(p_expr->GetCastExpr());

    // then we get the type of the cast
    if (_generate_load) {
        // avoid generate instruction like "i2i, "c2i"
        if (Utils::GetTypeName(p_expr->GetCastExpr()->RetType(), true) !=
            Utils::GetTypeName(p_expr->RetType())) {
            // "i2f" or "f2i" or "c2f" or "f2c" do not change the stack
        }
    }
    return Status::OkStatus();
}

/**
 * @brief Analyze stack and symbol effects for a ArithmeticExpr node in the legacy backend.
 * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
 * @return OK on success, or the first backend/pass error.
 */
Status FuncAnalyzer::visit(Hzcc::AST::ArithmeticExpr *p_expr) {
    /**
     * Arithmetic Expression will consume two stack slots and produce one stack
     * slot. Which means decrease the current stack by 1.
     */

    HZCC_JVM_GENERATE_LOAD_INSTR({
        HZCC_JVM_Visit_Node(p_expr->GetLHS());
        HZCC_JVM_Visit_Node(p_expr->GetRHS());
    })

    // Then we consume the two stack slots and produce one stack slot.
    DecreaseCurrentStack(1);

    return Status::OkStatus();
}

/**
 * @brief Analyze stack and symbol effects for a AssignExpr node in the legacy backend.
 * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
 * @return OK on success, or the first backend/pass error.
 */
Status FuncAnalyzer::visit(Hzcc::AST::AssignExpr *p_expr) {
    /**
     * Stack change for Assign Expression will be determined by the RHS
     * expression and the LHS expression. Also depends on the type of the LHS
     */

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
        // "IINC" will not change the stack size.
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
                ModifyStackSize(1);  // for aload
            }
            // for primitive type we can directly use the value
            else {
                ModifyStackSize(LoadFromVariable(var_name));
            }
        }

        // we evaluate the RHS
        HZCC_LEAVE_RET_ON_STACK(                          // NOLINT
            HZCC_JVM_GENERATE_LOAD_INSTR(                 // NOLINT
                HZCC_JVM_Visit_Node(p_expr->GetRHS())));  // NOLINT

        // here we push instructions to generate the assignment
        if (p_expr->GetAssignType() != AST::kAssignType_Assign) {
            // we then do the operation
            // arithmetic expression will consume  stack size 2 and produce 1
            ModifyStackSize(-1);
        }

        // if requested, we will duplicate the value and leave result on stack
        if (_request_leave) {
            ModifyStackSize(1);  // for "dup_x2" or "dup"
        }

        // we push the value back to the stack
        ModifyStackSize(SaveToVariable(var_name));
    }

    return Status::OkStatus();
}

/**
 * @brief Analyze stack and symbol effects for a BitwiseExpr node in the legacy backend.
 * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
 * @return OK on success, or the first backend/pass error.
 */
Status FuncAnalyzer::visit(Hzcc::AST::BitwiseExpr *p_expr) {
    /**
     * BitwiseExpr Expression will consume two stack slots and produce one stack
     * slot. Which means decrease the current stack by 1.
     * TODO not verified yet
     */

    // First we evaluate the left expression.
    HZCC_JVM_Visit_Node(p_expr->GetLHS());

    // Then we evaluate the right expression.
    HZCC_JVM_Visit_Node(p_expr->GetRHS());

    // Then we consume the two stack slots and produce one stack slot.
    DecreaseCurrentStack(1);

    return Status::OkStatus();
}

/**
 * @brief Analyze stack and symbol effects for a LogicalExpr node in the legacy backend.
 * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
 * @return OK on success, or the first backend/pass error.
 */
Status FuncAnalyzer::visit(Hzcc::AST::LogicalExpr *p_expr) {
    /**
     * LogicalExpr Expression will consume two stack slots and produce one stack
     * slot. Which means decrease the current stack by 1.
     * TODO not verified yet
     */

    // First we evaluate the left expression.
    HZCC_JVM_Visit_Node(p_expr->GetLHS());

    // Then we evaluate the right expression.
    HZCC_JVM_Visit_Node(p_expr->GetRHS());

    // Then we consume the two stack slots and produce one stack slot.
    DecreaseCurrentStack(1);

    return Status::OkStatus();
}

/**
 * @brief Analyze stack and symbol effects for a CommaExpr node in the legacy backend.
 * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
 * @return OK on success, or the first backend/pass error.
 */
Status FuncAnalyzer::visit(Hzcc::AST::CommaExpr *p_expr) {
    /**
     * CommaExpr Expression will not consume or pop any stack.
     * TODO not verified yet
     */

    // First we evaluate the left expression.
    HZCC_JVM_Visit_Node(p_expr->GetLHS());

    // Then we evaluate the right expression.
    HZCC_JVM_Visit_Node(p_expr->GetRHS());

    return Status::OkStatus();
}

/**
 * @brief Analyze stack and symbol effects for a RelationalExpr node in the legacy backend.
 * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
 * @return OK on success, or the first backend/pass error.
 */
Status FuncAnalyzer::visit(Hzcc::AST::RelationalExpr *p_expr) {
    /**
     * RelationalExpr Expression will consume two stack slots and produce one
     * stack slot. Which means decrease the current stack by 1.
     * TODO not verified yet
     */

    // First we evaluate the left expression.
    HZCC_JVM_Visit_Node(p_expr->GetLHS());

    // Then we evaluate the right expression.
    HZCC_JVM_Visit_Node(p_expr->GetRHS());

    // Then we consume the two stack slots and produce one stack slot.
    DecreaseCurrentStack(1);

    return Status::OkStatus();
}

/**
 * @brief Accept a AccessExpr node without analyzing it in this unfinished visitor.
 * @param p_expr Borrowed node pointer; this handler does not inspect or traverse it.
 * @return Success without changing stack or symbol state.
 */
Status FuncAnalyzer::visit(Hzcc::AST::AccessExpr *p_expr) {
    return Status::OkStatus();
}
/**
 * @brief Analyze stack and symbol effects for a ArraySubscriptExpr node in the legacy backend.
 * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
 * @return OK on success, or the first backend/pass error.
 */
Status FuncAnalyzer::visit(Hzcc::AST::ArraySubscriptExpr *p_expr) {
    // first visit DeclRefExpr
    HZCC_JVM_GENERATE_LOAD_INSTR(HZCC_JVM_Visit_Node(p_expr->GetArrayBase()));

    // load array base
    HZCC_JVM_GENERATE_LOAD_INSTR(HZCC_JVM_Visit_Node(p_expr->GetSubscript()));

    // generate load instr
    if (_generate_load) {
        // for "x_aload" consume 2 stack slots and produce 1 stack slot
        ModifyStackSize(-1);
    }

    return Status::OkStatus();
}
/**
 * @brief Accept a TernaryExpr node without analyzing it in this unfinished visitor.
 * @param p_expr Borrowed node pointer; this handler does not inspect or traverse it.
 * @return Success without changing stack or symbol state.
 */
Status FuncAnalyzer::visit(Hzcc::AST::TernaryExpr *p_expr) {
    return Status::OkStatus();
}
/**
 * @brief Analyze stack and symbol effects for a DeclRefExpr node in the legacy backend.
 * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
 * @return OK on success, or the first backend/pass error.
 */
Status FuncAnalyzer::visit(Hzcc::AST::DeclRefExpr *p_expr) {
    // Generate push instruction if needed
    if (_generate_load) {
        if (p_expr->RetType()->IsArray()) {
            ModifyStackSize(1);
            PushReturnStack(p_expr->VarName());
        } else {
            ModifyStackSize(LoadFromVariable(p_expr->VarName()));
        }
    } else {
        PushReturnStack(p_expr->VarName());
    }

    return Status::OkStatus();
}

/**
 * @brief Analyze stack and symbol effects for a SizeofExpr node in the legacy backend.
 * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
 * @return OK on success, or the first backend/pass error.
 */
Status FuncAnalyzer::visit(Hzcc::AST::SizeofExpr *p_expr) {
    /**
     * SizeofExpr Expression will push one stack slot.
     * TODO not verified yet
     */

    // Then we consume the two stack slots and produce one stack slot.
    ModifyStackSize(1);

    return Status::OkStatus();
}
/**
 * @brief Analyze stack and symbol effects for a UnaryExpr node in the legacy backend.
 * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
 * @return OK on success, or the first backend/pass error.
 */
Status FuncAnalyzer::visit(Hzcc::AST::UnaryExpr *p_expr) {
    if (p_expr->GetUnaryType() == AST::kUnaryType_UnaryMinus) {
        HZCC_JVM_GENERATE_LOAD_INSTR(HZCC_JVM_Visit_Node(p_expr->GetExpr()));
    } else {
        // visit node
        HZCC_JVM_NOT_GENERATE_LOAD_INSTR(
            HZCC_JVM_Visit_Node(p_expr->GetExpr()));

        auto var_name = ConsumeReturnStack();
        if (_global_vars.find(var_name) == _global_vars.end() &&
            Utils::GetTypeName(p_expr->GetExpr()->RetType()) == "i") {
            ModifyStackSize(LoadFromVariable(var_name));
        } else {
            ModifyStackSize(LoadFromVariable(var_name));
            ModifyStackSize(3);
            ModifyStackSize(SaveToVariable(var_name));
        }
    }
    return Status::OkStatus();
}

/**
 * @brief Analyze stack and symbol effects for a CompoundStmt node in the legacy backend.
 * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
 * @return OK on success, or the first backend/pass error.
 */
Status FuncAnalyzer::visit(Hzcc::AST::CompoundStmt *p_expr) {
    /**
     * CompoundStmt is a list of statements. We will visit each stmt
     * in the list in order to get the stack size and variables;
     */

    for (auto &stmt : p_expr->GetBodyStatements()) {
        HZCC_JVM_Visit_Node(stmt);
    }
    return Status::OkStatus();
}

/**
 * @brief Accept a BreakStmt node without analyzing it in this unfinished visitor.
 * @param p_expr Borrowed node pointer; this handler does not inspect or traverse it.
 * @return Success without changing stack or symbol state.
 */
Status FuncAnalyzer::visit(Hzcc::AST::BreakStmt *p_expr) {
    return Status::OkStatus();
}
/**
 * @brief Accept a ContinueStmt node without analyzing it in this unfinished visitor.
 * @param p_expr Borrowed node pointer; this handler does not inspect or traverse it.
 * @return Success without changing stack or symbol state.
 */
Status FuncAnalyzer::visit(Hzcc::AST::ContinueStmt *p_expr) {
    return Status::OkStatus();
}
/**
 * @brief Accept a DoStatement node without analyzing it in this unfinished visitor.
 * @param p_expr Borrowed node pointer; this handler does not inspect or traverse it.
 * @return Success without changing stack or symbol state.
 */
Status FuncAnalyzer::visit(Hzcc::AST::DoStatement *p_expr) {
    return Status::OkStatus();
}
/**
 * @brief Accept a EmptyStatement node without analyzing it in this unfinished visitor.
 * @param p_expr Borrowed node pointer; this handler does not inspect or traverse it.
 * @return Success without changing stack or symbol state.
 */
Status FuncAnalyzer::visit(Hzcc::AST::EmptyStatement *p_expr) {
    return Status::OkStatus();
}
/**
 * @brief Accept a ForStmt node without analyzing it in this unfinished visitor.
 * @param p_expr Borrowed node pointer; this handler does not inspect or traverse it.
 * @return Success without changing stack or symbol state.
 */
Status FuncAnalyzer::visit(Hzcc::AST::ForStmt *p_expr) {
    return Status::OkStatus();
}
/**
 * @brief Analyze stack and symbol effects for a FuncCallStmt node in the legacy backend.
 * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
 * @return OK on success, or the first backend/pass error.
 */
Status FuncAnalyzer::visit(Hzcc::AST::FuncCallStmt *p_expr) {
    // generate ars push
    for (auto &arg : p_expr->GetArgsNode()) {
        HZCC_JVM_GENERATE_LOAD_INSTR(HZCC_JVM_Visit_Node(arg));
    }

    // consume args
    ModifyStackSize(-1 * (int)p_expr->GetArgsNode().size());

    // push return value if needed
    if (p_expr->GetType()->GetName() != "void") {
        ModifyStackSize(1);
    }

    // pop if does not need return value
    if (!_generate_load && p_expr->GetType()->GetName() != "void") {
        ModifyStackSize(-1);
    }

    return Status::OkStatus();
}

/**
 * @brief Accept a FuncDeclStmt node without analyzing it in this unfinished visitor.
 * @param p_expr Borrowed node pointer; this handler does not inspect or traverse it.
 * @return Success without changing stack or symbol state.
 */
Status FuncAnalyzer::visit(Hzcc::AST::FuncDeclStmt *p_expr) {
    return Status::OkStatus();
}
/**
 * @brief Accept a DeclStmt node without analyzing it in this unfinished visitor.
 * @param p_expr Borrowed node pointer; this handler does not inspect or traverse it.
 * @return Success without changing stack or symbol state.
 */
Status FuncAnalyzer::visit(Hzcc::AST::DeclStmt *p_expr) {
    return Status::OkStatus();
}
/**
 * @brief Accept a IfStmt node without analyzing it in this unfinished visitor.
 * @param p_expr Borrowed node pointer; this handler does not inspect or traverse it.
 * @return Success without changing stack or symbol state.
 */
Status FuncAnalyzer::visit(Hzcc::AST::IfStmt *p_expr) {
    return Status::OkStatus();
}
/**
 * @brief Analyze stack and symbol effects for a ParamVarDecl node in the legacy backend.
 * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
 * @return OK on success, or the first backend/pass error.
 */
Status FuncAnalyzer::visit(Hzcc::AST::ParamVarDecl *p_expr) {
    AddLocalVariable(p_expr->GetName(), Utils::GetTypeName(p_expr->GetType()),
                     p_expr->GetLine());
    return Status::OkStatus();
}
/**
 * @brief Analyze stack and symbol effects for a ReturnStmt node in the legacy backend.
 * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
 * @return OK on success, or the first backend/pass error.
 */
Status FuncAnalyzer::visit(Hzcc::AST::ReturnStmt *p_expr) {
    _has_return = true;
    HZCC_JVM_GENERATE_LOAD_INSTR(
        HZCC_LEAVE_RET_ON_STACK(HZCC_JVM_Visit_Node(p_expr->GetReturnVal())));
    return Status::OkStatus();
}
/**
 * @brief Accept a WhileStmt node without analyzing it in this unfinished visitor.
 * @param p_expr Borrowed node pointer; this handler does not inspect or traverse it.
 * @return Success without changing stack or symbol state.
 */
Status FuncAnalyzer::visit(Hzcc::AST::WhileStmt *p_expr) {
    return Status::OkStatus();
}
/**
 * @brief Analyze stack and symbol effects for a LiteralExpr node in the legacy backend.
 * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
 * @return OK on success, or the first backend/pass error.
 */
Status FuncAnalyzer::visit(Hzcc::AST::LiteralExpr *p_expr) {
    ModifyStackSize(1);
    return Status::OkStatus();
}
/**
 * @brief Analyze stack and symbol effects for a VarDecl node in the legacy backend.
 * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
 * @return OK on success, or the first backend/pass error.
 */
Status FuncAnalyzer::visit(Hzcc::AST::VarDecl *p_expr) {
    AddLocalVariable(p_expr->GetName(), Utils::GetTypeName(p_expr->GetType()),
                     p_expr->GetLine());

    // current variable type is array, we need to initialize it
    if (p_expr->GetType()->IsArray()) {
        auto type =
            std::dynamic_pointer_cast<AST::ArrayType>(p_expr->GetType());
        DLOG_ASSERT(type)
            << "Current type " << p_expr->GetType()->Dump()
            << "  cannot cast to array type but IsArray() return true";

        // push array size
        if (type->HasDeduceSize()) {
            ModifyStackSize(1);
        } else {
            HZCC_JVM_Visit_Node(type->GetArraySizeNode());
        }

        // push array to stack
        ModifyStackSize(-1);
    }

    // if the expression contains a initializer, we need to push it
    if (p_expr->HasInitExpr()) {
        // visit the initializer expression
        HZCC_JVM_Visit_Node(p_expr->GetInitExpr());

        // push the initializer value
        ModifyStackSize(SaveToVariable(p_expr->GetName()));
    }
    return Status::OkStatus();
}

/**
 * @brief Apply a signed operand-stack adjustment and update the observed maximum.
 * @param p_size Number of slots by which the operand-stack estimate is adjusted.
 */
void FuncAnalyzer::ModifyStackSize(int p_size) {
    _current_stack_size += p_size;
    if (_current_stack_size > _max_stack_size) {
        _max_stack_size = _current_stack_size;
    }
}
/**
 * @brief Subtract slots from the current operand-stack estimate.
 * @param p_size Number of slots by which the operand-stack estimate is adjusted.
 */
void FuncAnalyzer::DecreaseCurrentStack(uint8_t p_size) {
    _current_stack_size -= p_size;
}
/**
 * @brief Return the observed maximum operand-stack size plus the legacy safety margin of 20.
 * @return Maximum observed slot count plus 20.
 */
uint32_t FuncAnalyzer::GetMaxStackSize() const { return _max_stack_size + 20; }

/**
 * @brief Report whether a return statement has been visited.
 * @return True when the stated condition holds; otherwise false.
 */
bool FuncAnalyzer::HasValidReturn() const { return _has_return; }

/**
 * @brief Compute the operand-stack change caused by storing a variable.
 * @param name Variable or symbol name to register or look up.
 * @return Signed stack-size adjustment.
 */
int FuncAnalyzer::SaveToVariable(const std::string &name) {
    if (IsLocal(name)) {
        auto var_type = GetLocalVarType(name);
        if (var_type.find('[') != std::string::npos) {
            return -3;
        } else {
            return -1;
        }
    } else if (_global_vars.find(name) != _global_vars.end()) {
        auto var_type = _global_vars.at(name);
        if (var_type.find('[') != std::string::npos) {
            return -3;
        } else {
            return -1;
        }
    } else {
        LOG(FATAL) << "Variable " << name << " not found";
    }
}

/**
 * @brief Compute the operand-stack change caused by loading a variable.
 * @param name Variable or symbol name to register or look up.
 * @return Signed stack-size adjustment.
 */
int FuncAnalyzer::LoadFromVariable(const std::string &name) {
    if (IsLocal(name)) {
        auto var_type = GetLocalVarType(name);
        if (var_type.find('[') != std::string::npos) {
            return -1;
        } else {
            return 1;
        }
    } else if (_global_vars.find(name) != _global_vars.end()) {
        return 1;
    } else {
        LOG(FATAL) << "Variable " << name << " not found";
    }
}
/**
 * @brief Initialize FuncAnalyzer with the supplied backend state.
 * @param global_vars Borrowed global name-to-type table; it must outlive this analyzer.
 */
FuncAnalyzer::FuncAnalyzer(
    const std::unordered_map<std::string, std::string> &global_vars)
    : _global_vars(global_vars) {
    EnterScope();
}

/**
 * @brief Push an intermediate result identifier onto the return-value stack.
 * @param stackID Identifier of the intermediate result to retain.
 */
void FuncAnalyzer::PushReturnStack(const std::string &stackID) {
    _return_stack.push_back(stackID);
}

/**
 * @brief Pop and return the most recent intermediate result identifier.
 * @return Most recently pushed result identifier.
 * @pre The corresponding stack or scope list is nonempty.
 */
std::string FuncAnalyzer::ConsumeReturnStack() {
    DLOG_ASSERT(!_return_stack.empty()) << " Return stack is empty";
    auto poped_stack = _return_stack.back();
    _return_stack.pop_back();
    return poped_stack;
}

/**
 * @brief Look up a variable in active lexical scopes, searching from the innermost scope.
 * @param p_name Variable name to look up.
 * @return True when the stated condition holds; otherwise false.
 */
bool FuncAnalyzer::IsLocal(const std::string &p_name) {
    // iterate from the top of the stack to the bottom
    for (auto iter = _symbol_table_stack.rbegin();
         iter != _symbol_table_stack.rend(); ++iter) {
        if ((*iter)->HasSymbol(p_name)) {
            return true;
        }
    }

    return false;
}

/**
 * @brief Test whether a variable is present in the global variable table.
 * @param p_name Variable name to look up.
 * @return True when the stated condition holds; otherwise false.
 */
bool FuncAnalyzer::IsGlobal(const std::string &p_name) {
    return _global_vars.find(p_name) != _global_vars.end();
}

/**
 * @brief Return the type of the nearest visible local declaration.
 * @param p_name Variable name to look up.
 * @return Type descriptor of the nearest visible local variable.
 */
std::string FuncAnalyzer::GetLocalVarType(const std::string &p_name) {
    DLOG_ASSERT(IsLocal(p_name)) << " Variable " << p_name << " not found";

    // iterate from the top of the stack to the bottom
    for (auto iter = _symbol_table_stack.rbegin();
         iter != _symbol_table_stack.rend(); ++iter) {
        if ((*iter)->HasSymbol(p_name)) {
            return (*iter)->GetSymbolType(p_name);
        }
    }

    return "";
}

/**
 * @brief Append a local declaration to the active scope.
 * @param name Variable or symbol name to register or look up.
 * @param type JVM type or descriptor used by this operation.
 * @param line_no Source line of the variable declaration.
 */
void FuncAnalyzer::AddLocalVariable(const std::string &name,
                                    const std::string &type, uint32_t line_no) {
    _symbol_table_stack.back()->AddSymbol(name, type, line_no);
}

/**
 * @brief Push a new lexical scope for subsequent declarations or labels.
 */
void FuncAnalyzer::EnterScope() {
    auto new_scope = std::make_shared<ScopedSymbolTable>();
    _symbol_table_stack.emplace_back(new_scope);
    _saved_symbol_table.emplace_back(_symbol_table_stack.back());
}

/**
 * @brief Pop the innermost scope after its contents have been processed.
 * @pre The corresponding stack or scope list is nonempty.
 */
void FuncAnalyzer::LeaveScope() { _symbol_table_stack.pop_back(); }

/**
 * @brief Build slot-indexed lists of variable names, types and declaration lines.
 * @return Slot-indexed lists of (name, type, declaration line) tuples.
 */
FuncAnalyzer::SymbolTable FuncAnalyzer::GetLocalVariable() {
    FuncAnalyzer::SymbolTable ret;

    // since the first element of SymbolTable is the local variable table, we
    // add all the local variables to the return table
    int local_var_count = 0;
    for (const auto &stack : _saved_symbol_table) {
        // for local variable
        if (stack == _symbol_table_stack.front()) {
            for (const auto &pair : stack->GetSymbols()) {
                std::list<std::tuple<std::basic_string<char>,
                                     std::basic_string<char>, unsigned long>>
                    tmp;

                tmp.emplace_back(pair);
                ret.push_back(tmp);
                local_var_count++;
            }
        }

        // for local temp variable
        else {
            int stack_id = 0;
            for (const auto &pair : stack->GetSymbols()) {
                if (ret.at(stack_id + local_var_count).empty()) {
                    std::list<
                        std::tuple<std::basic_string<char>,
                                   std::basic_string<char>, unsigned long>>
                        tmp;
                    tmp.emplace_back(pair);
                } else {
                    ret.at(stack_id + local_var_count).emplace_back(pair);
                }
                stack_id++;
            }
        }
    }

    return ret;
}

/**
 * @brief Append a symbol name, type and declaration line to this scope.
 * @param name Variable or symbol name to register or look up.
 * @param type JVM type or descriptor used by this operation.
 * @param line_no Source line of the variable declaration.
 */
void FuncAnalyzer::ScopedSymbolTable::AddSymbol(const std::string &name,
                                                const std::string &type,
                                                uint32_t line_no) {
    _symbol_table.emplace_back(name, type, line_no);
}

/**
 * @brief Test whether this scope contains the given symbol name.
 * @param name Variable or symbol name to register or look up.
 * @return True when the stated condition holds; otherwise false.
 */
bool FuncAnalyzer::ScopedSymbolTable::HasSymbol(const std::string &name) {
    return std::find_if(_symbol_table.begin(), _symbol_table.end(),
                        [&](auto &t) { return std::get<0>(t) == name; }) !=
           _symbol_table.end();
}
/**
 * @brief Return the type recorded for a symbol in this scope.
 * @param name Variable or symbol name to register or look up.
 * @return Type string stored for the named declaration.
 */
std::string FuncAnalyzer::ScopedSymbolTable::GetSymbolType(
    const std::string &name) {
    DLOG_ASSERT(HasSymbol(name)) << " Variable " << name << " not found";
    return std::get<1>(
        *std::find_if(_symbol_table.begin(), _symbol_table.end(),
                      [&](auto &t) { return std::get<0>(t) == name; }));
}
/**
 * @brief Return a copy of the declarations recorded in this scope.
 * @return A copy of this scope's declarations.
 */
std::list<FuncAnalyzer::LocalSymbol>
FuncAnalyzer::ScopedSymbolTable::GetSymbols() {
    return _symbol_table;
}
}  // namespace Hzcc::Codegen::JVM
