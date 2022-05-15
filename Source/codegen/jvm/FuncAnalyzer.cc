//
// Created by chen_ on 2022/5/3.
//

#include "FuncAnalyzer.h"

#include <algorithm>

#include "AST/expr/array.h"
#include "AST/expr/cast.h"
#include "AST/expr/decl_ref.h"
#include "AST/expr/operator/arithmetic.h"
#include "AST/expr/operator/assign.h"
#include "AST/expr/operator/bitwise.h"
#include "AST/expr/operator/comma.h"
#include "AST/expr/operator/logical.h"
#include "AST/expr/operator/relational.h"
#include "AST/expr/sizeof.h"
#include "AST/expr/unary.h"
#include "AST/statement/compound.h"
#include "AST/statement/empty.h"
#include "AST/statement/function_call.h"
#include "AST/statement/param_val_decl.h"
#include "AST/statement/return.h"
#include "AST/statement/value_decl.h"
#include "AST/type/ArrayType.h"
#include "codegen/jvm/utils/macro.h"
namespace Hzcc::Codegen::JVM {

Status FuncAnalyzer::visit(Hzcc::AST::CastExpr *p_expr) {
    // firs we get the type of the expr
    HZCC_JVM_Visit_Node(p_expr->GetCastExpr());

    // then we get the type of the cast
    if (_generate_load) {
        // avoid generate instruction like "i2i, "c2i"
        if (Utils::GetTypeName(p_expr->GetCastExpr()->GetType(), true) !=
            Utils::GetTypeName(p_expr->GetType())) {
            // "i2f" or "f2i" or "c2f" or "f2c" do not change the stack
        }
    }
    return Status::OkStatus();
}

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
        (!p_expr->GetLHS()->GetType()->IsArray() &&
         !p_expr->GetLHS()->GetType()->IsFuncPtr() &&
         Utils::GetTypeName(p_expr->GetLHS()->GetType()) == "i") &&
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

Status FuncAnalyzer::visit(Hzcc::AST::AccessExpr *p_expr) {
    return Status::OkStatus();
}
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
Status FuncAnalyzer::visit(Hzcc::AST::TernaryExpr *p_expr) {
    return Status::OkStatus();
}
Status FuncAnalyzer::visit(Hzcc::AST::DeclRefExpr *p_expr) {
    // Generate push instruction if needed
    if (_generate_load) {
        if (p_expr->GetType()->IsArray()) {
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

Status FuncAnalyzer::visit(Hzcc::AST::SizeofExpr *p_expr) {
    /**
     * SizeofExpr Expression will push one stack slot.
     * TODO not verified yet
     */

    // Then we consume the two stack slots and produce one stack slot.
    ModifyStackSize(1);

    return Status::OkStatus();
}
Status FuncAnalyzer::visit(Hzcc::AST::UnaryExpr *p_expr) {
    if (p_expr->GetUnaryType() == AST::kUnaryType_UnaryMinus) {
        HZCC_JVM_GENERATE_LOAD_INSTR(HZCC_JVM_Visit_Node(p_expr->GetExpr()));
    } else {
        // visit node
        HZCC_JVM_NOT_GENERATE_LOAD_INSTR(
            HZCC_JVM_Visit_Node(p_expr->GetExpr()));

        auto var_name = ConsumeReturnStack();
        if (_global_vars.find(var_name) == _global_vars.end() &&
            Utils::GetTypeName(p_expr->GetExpr()->GetType()) == "i") {
            ModifyStackSize(LoadFromVariable(var_name));
        } else {
            ModifyStackSize(LoadFromVariable(var_name));
            ModifyStackSize(3);
            ModifyStackSize(SaveToVariable(var_name));
        }
    }
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
    AddLocalVariable(p_expr->GetName(), Utils::GetTypeName(p_expr->GetType()),
                     p_expr->GetLine());
    return Status::OkStatus();
}
Status FuncAnalyzer::visit(Hzcc::AST::ReturnNode *p_expr) {
    _has_return = true;
    HZCC_JVM_GENERATE_LOAD_INSTR(
        HZCC_LEAVE_RET_ON_STACK(HZCC_JVM_Visit_Node(p_expr->GetReturnVal())));
    return Status::OkStatus();
}
Status FuncAnalyzer::visit(Hzcc::AST::WhileStatement *p_expr) {
    return Status::OkStatus();
}
Status FuncAnalyzer::visit(Hzcc::AST::LiteralExpr *p_expr) {
    ModifyStackSize(1);
    return Status::OkStatus();
}
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

void FuncAnalyzer::ModifyStackSize(int p_size) {
    _current_stack_size += p_size;
    if (_current_stack_size > _max_stack_size) {
        _max_stack_size = _current_stack_size;
    }
}
void FuncAnalyzer::DecreaseCurrentStack(uint8_t p_size) {
    _current_stack_size -= p_size;
}
uint32_t FuncAnalyzer::GetMaxStackSize() const { return _max_stack_size + 20; }

bool FuncAnalyzer::HasValidReturn() const { return _has_return; }

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
FuncAnalyzer::FuncAnalyzer(
    const std::unordered_map<std::string, std::string> &global_vars)
    : _global_vars(global_vars) {
    EnterScope();
}

void FuncAnalyzer::PushReturnStack(const std::string &stackID) {
    _return_stack.push_back(stackID);
}

std::string FuncAnalyzer::ConsumeReturnStack() {
    DLOG_ASSERT(!_return_stack.empty()) << " Return stack is empty";
    auto poped_stack = _return_stack.back();
    _return_stack.pop_back();
    return poped_stack;
}

/** #####################################################################
 *  ### Symbol Table Functions                                         ###
 *  ##################################################################### */
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

bool FuncAnalyzer::IsGlobal(const std::string &p_name) {
    return _global_vars.find(p_name) != _global_vars.end();
}

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

void FuncAnalyzer::AddLocalVariable(const std::string &name,
                                    const std::string &type, uint32_t line_no) {
    _symbol_table_stack.back()->AddSymbol(name, type, line_no);
}

void FuncAnalyzer::EnterScope() {
    auto new_scope = std::make_shared<ScopedSymbolTable>();
    _symbol_table_stack.emplace_back(new_scope);
    _saved_symbol_table.emplace_back(_symbol_table_stack.back());
}

void FuncAnalyzer::LeaveScope() { _symbol_table_stack.pop_back(); }

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

/** #####################################################################
 *  ### Scoped Symbol Table                                           ###
 *  ##################################################################### */

void FuncAnalyzer::ScopedSymbolTable::AddSymbol(const std::string &name,
                                                const std::string &type,
                                                uint32_t line_no) {
    _symbol_table.emplace_back(name, type, line_no);
}

bool FuncAnalyzer::ScopedSymbolTable::HasSymbol(const std::string &name) {
    return std::find_if(_symbol_table.begin(), _symbol_table.end(),
                        [&](auto &t) { return std::get<0>(t) == name; }) !=
           _symbol_table.end();
}
std::string FuncAnalyzer::ScopedSymbolTable::GetSymbolType(
    const std::string &name) {
    DLOG_ASSERT(HasSymbol(name)) << " Variable " << name << " not found";
    return std::get<1>(
        *std::find_if(_symbol_table.begin(), _symbol_table.end(),
                      [&](auto &t) { return std::get<0>(t) == name; }));
}
std::list<FuncAnalyzer::LocalSymbol>
FuncAnalyzer::ScopedSymbolTable::GetSymbols() {
    return _symbol_table;
}
}  // namespace Hzcc::Codegen::JVM
