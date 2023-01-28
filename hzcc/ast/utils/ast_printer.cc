//
// Created by chen_ on 2022/12/30.
//

#include "ast_printer.h"

#include <glog/logging.h>
#include <magic_enum.hpp>
#include <algorithm>
#include <array>
#include <memory>
#include <ostream>
#include <utility>
#include <vector>

#include "ast/CompilationUnit.h"
#include "ast/Stmt.h"
#include "ast/expr/Expr.h"
#include "ast/type/Type.h"
#include "enums.h"
#include "macro.h"

namespace hzcc::ast {
Status AstPrinter::visit(CompoundStmt* p_stmt) {
    for (const auto& statement : p_stmt->get_body_stmts()) {
        add_indent(statement == p_stmt->get_body_stmts().back() ? "  `"
                                                                : "  |");
        HZCC_CHECK_OK_OR_RETURN(statement->visit(*this));

        remove_indent();
    }
    return NoError();
}

Status AstPrinter::visit(DoStmt* p_stmt) {
    add_indent(" |");
    HZCC_CHECK_OK_OR_RETURN(p_stmt->cond_stmt()->visit(*this));
    remove_indent();

    add_indent(" `");
    HZCC_CHECK_OK_OR_RETURN(p_stmt->body_stmt()->visit(*this));
    remove_indent();
    return NoError();
}

Status AstPrinter::visit(ForStmt* p_stmt) {
    add_indent(" |");
    HZCC_CHECK_OK_OR_RETURN(p_stmt->init_stmt()->visit(*this));
    remove_indent();

    // print condition
    add_indent(" |");
    HZCC_CHECK_OK_OR_RETURN(p_stmt->cond_stmt()->visit(*this));

    // print step
    add_indent(" |");
    HZCC_CHECK_OK_OR_RETURN(p_stmt->step_stmt()->visit(*this));

    // print body
    add_indent(" `");
    HZCC_CHECK_OK_OR_RETURN(p_stmt->body_stmt()->visit(*this));
    remove_indent();
    return NoError();
}

Status AstPrinter::visit(IfStmt* p_stmt) {
    add_indent(" |");
    HZCC_CHECK_OK_OR_RETURN(p_stmt->cond_stmt()->visit(*this));
    remove_indent();

    // print body
    add_indent(p_stmt->has_else_if() && p_stmt->has_else() ? " `" : " |");
    HZCC_CHECK_OK_OR_RETURN(p_stmt->body_stmt()->visit(*this));
    remove_indent();

    // print else if
    for (auto& else_if : p_stmt->else_if_stmts()) {
        add_indent(else_if == p_stmt->else_if_stmts().back() &&
                           p_stmt->has_else()
                       ? " `"
                       : " |");
        HZCC_CHECK_OK_OR_RETURN(else_if.first->visit(*this));
        remove_indent();
    }

    // print else
    if (p_stmt->has_else()) {
        add_indent(" `");
        HZCC_CHECK_OK_OR_RETURN(p_stmt->else_stmt()->visit(*this));
        remove_indent();
    }
    return NoError();
}

Status AstPrinter::visit(ReturnStmt* p_stmt) {  // print node if available
    if (p_stmt->has_body()) {
        add_indent(" `");
        HZCC_CHECK_OK_OR_RETURN(p_stmt->ret_expr()->visit(*this));
        remove_indent();
    }
    return NoError();
}

Status AstPrinter::visit(WhileStmt* p_stmt) {
    add_indent(" |");
    HZCC_CHECK_OK_OR_RETURN(p_stmt->cond_stmt()->visit(*this));
    remove_indent();

    // print body
    add_indent(" `");
    HZCC_CHECK_OK_OR_RETURN(p_stmt->body_stmt()->visit(*this));
    remove_indent();
    return NoError();
}

Status AstPrinter::visit(VarDecl* p_stmt) {
    _out << p_stmt->decl_name() << " " << p_stmt->declType()->Name();
    if (p_stmt->has_init()) {
        add_indent(" `");
        HZCC_CHECK_OK_OR_RETURN(p_stmt->init_expr()->visit(*this));
        remove_indent();
    }
    return NoError();
}

Status AstPrinter::visit(FuncDeclStmt* p_stmt) {
    // print its arguments
    _out << " (";
    for (const auto& arg : p_stmt->params()) {
        _out << arg->declType()->Dump();
        if (arg != p_stmt->params().back()) {
            _out << ",";
        }
    }
    _out << ")";

    // print argument node
    if (!p_stmt->params().empty()) {
        for (const auto& arg : p_stmt->params()) {
            add_indent(arg == p_stmt->params().back() && p_stmt->has_body()
                           ? " `"
                           : " |");
            HZCC_CHECK_OK_OR_RETURN(arg->visit(*this));
            remove_indent();
        }
    }

    // print its body
    if (p_stmt->has_body()) {
        add_indent(" `");
        HZCC_CHECK_OK_OR_RETURN(p_stmt->body_stmt()->visit(*this));
        remove_indent();
    } else {
        _out << ")";
    }
    return NoError();
}

Status AstPrinter::visit(UnaryOperator* p_expr) {
    static std::array<const char*, magic_enum::enum_count<UnaryType>()>
        kUnaryOpSTR = {"-", "++(Pre)", "--(Pre)", "++(Post)", "--(Post)",
                       "&", "*",       "!",       "~"};

    // print unary operator
    _out << kUnaryOpSTR[magic_enum::enum_integer(p_expr->GetUnaryType())] << " "
         << p_expr->expr()->retType()->Name();

    // print its operand
    add_indent(" `");
    HZCC_CHECK_OK_OR_RETURN(p_expr->expr()->visit(*this));
    remove_indent();
    return NoError();
}

Status AstPrinter::visit(TernaryExpr* p_expr) {
    // type
    _out << p_expr->retType()->Name();

    // node
    add_indent(" |");
    HZCC_CHECK_OK_OR_RETURN(p_expr->cond_expr()->visit(*this));
    remove_indent();

    add_indent(" |");
    HZCC_CHECK_OK_OR_RETURN(p_expr->true_expr()->visit(*this));
    remove_indent();

    add_indent(" `");
    HZCC_CHECK_OK_OR_RETURN(p_expr->false_expr()->visit(*this));
    remove_indent();

    return NoError();
}

Status AstPrinter::visit(LiteralExpr* p_expr) {
    switch (p_expr->literal_type()) {
        case LiteralType::kLiteralType_Char:
            _out << "char \'" << p_expr->get_literal_val() << "\'";
            break;
        case LiteralType::kLiteralType_Real_number:
            _out << "real_number " << p_expr->get_literal_val();
            break;
        case LiteralType::kLiteralType_String:
            _out << "string \"" << p_expr->get_literal_val() << "\"";
            break;
        case LiteralType::kLiteralType_Integer:
            _out << "int " << p_expr->get_literal_val();
            break;
        case kLiteralType_Max:
            DLOG_ASSERT(false) << "unexpected literal type [kLiteralType_Max]";
    }
    return NoError();
}

Status AstPrinter::visit(FuncCallStmt* p_stmt) {
    _out << p_stmt->name();
    if (!p_stmt->args().empty()) {
        for (auto& arg : p_stmt->args()) {
            add_indent(arg == p_stmt->args().back() ? " ` " : " | ");
            HZCC_CHECK_OK_OR_RETURN(arg->visit(*this));
            remove_indent();
        }
    }
    return NoError();
}

Status AstPrinter::visit(DeclRefExpr* p_expr) {
    _out << p_expr->var_name() << " " << p_expr->retType()->Name();
    return NoError();
}

Status AstPrinter::visit(ArraySubscriptExpr* p_expr) {
    _out << p_expr->retType()->Name();

    add_indent(" |");
    HZCC_CHECK_OK_OR_RETURN(p_expr->base_expr()->visit(*this));
    remove_indent();

    add_indent(" `");
    HZCC_CHECK_OK_OR_RETURN(p_expr->subscript_expr()->visit(*this));
    remove_indent();

    return NoError();
}

Status AstPrinter::visit(RelationalExpr* p_expr) {
    // lhs type
    _out << " " + p_expr->lhs()->retType()->Name();

    // symbol type
    switch (p_expr->op_type()) {
        case RelationalType::kRelationalType_Equal:
            _out << " == ";
            break;
        case RelationalType::kRelationalType_NEqual:
            _out << " != ";
            break;
        case RelationalType::kRelationalType_Less:
            _out << " < ";
            break;
        case RelationalType::kRelationalType_LessEqual:
            _out << " <= ";
            break;
        case RelationalType::kRelationalType_Greater:
            _out << " > ";
            break;
        case RelationalType::kRelationalType_GreaterEqual:
            _out << " >= ";
            break;
        default:
            DLOG(FATAL) << "Unknown relational biop: " << p_expr->op_type();
    }

    add_indent(" |");
    HZCC_CHECK_OK_OR_RETURN(p_expr->lhs()->visit(*this));
    remove_indent();

    add_indent(" `");
    HZCC_CHECK_OK_OR_RETURN(p_expr->rhs()->visit(*this));
    remove_indent();

    return NoError();
}

Status AstPrinter::visit(LogicalExpr* p_expr) {
    // print logical type
    _out << (p_expr->op_type() == kLogicalType_And ? "&&" : "||");

    // print lhs
    add_indent(" |");
    HZCC_CHECK_OK_OR_RETURN(p_expr->lhs()->visit(*this));
    remove_indent();

    // print rhs
    add_indent(" `");
    HZCC_CHECK_OK_OR_RETURN(p_expr->rhs()->visit(*this));
    remove_indent();

    return NoError();
}

Status AstPrinter::visit(AssignExpr* p_expr) {
    _out << p_expr->lhs()->retType()->Name();

    // print symbol
    switch (p_expr->op_type()) {
        case kAssignType_Assign:
            _out << " = ";
            break;
        case kAssignType_AddAssign:
            _out << " += ";
            break;
        case kAssignType_SubAssign:
            _out << " -= ";
            break;
        case kAssignType_MulAssign:
            _out << " *= ";
            break;
        case kAssignType_DivAssign:
            _out << " /= ";
            break;
        case kAssignType_ModAssign:
            _out << " %= ";
            break;
        case kAssignType_LShiftAssign:
            _out << " <<= ";
            break;
        case kAssignType_RShiftAssign:
            _out << " >>= ";
            break;
        case kAssignType_AndAssign:
            _out << " &= ";
            break;
        case kAssignType_OrAssign:
            _out << " |= ";
            break;
        case kAssignType_XorAssign:
            _out << " ^= ";
            break;
        default:
            DLOG(FATAL) << "Unknown assign biop: " << p_expr->op_type();
    }

    // print LHS and RHS
    add_indent(" |");
    HZCC_CHECK_OK_OR_RETURN(p_expr->lhs()->visit(*this));
    remove_indent();

    add_indent(" `");
    HZCC_CHECK_OK_OR_RETURN(p_expr->rhs()->visit(*this));
    remove_indent();

    return NoError();
}

Status AstPrinter::visit(ArithmeticExpr* p_expr) {
    static const std::array<const char*,
                            magic_enum::enum_count<ArithmeticType>()>
        kArithmeticTypeStr = {" + ", " - ", " * ", " / ", " % "};

    // print node info
    _out << kArithmeticStr[magic_enum::enum_integer(p_expr->op_type())] << ' '
         << p_expr->lhs()->retType()->Name();

    // print LHS and RHS info
    add_indent(" |");
    HZCC_CHECK_OK_OR_RETURN(p_expr->lhs()->visit(*this));
    remove_indent();

    add_indent(" `");
    HZCC_CHECK_OK_OR_RETURN(p_expr->rhs()->visit(*this));
    remove_indent();

    return NoError();
}

Status AstPrinter::visit(ExplicitCastExpr* p_expr) {
    _out << '[' << p_expr->cast_expr()->retType()->Name() << "]->["
         << p_expr->retType()->Name() << "]";

    add_indent(" `");
    HZCC_CHECK_OK_OR_RETURN(p_expr->cast_expr()->visit(*this));
    remove_indent();

    return NoError();
}

Status AstPrinter::visit(hzcc::ast::ImplicitCastExpr* p_expr) {
    _out << '[' << p_expr->cast_expr()->retType()->Name() << "]->["
         << p_expr->retType()->Name() << "]";

    add_indent(" `");
    HZCC_CHECK_OK_OR_RETURN(p_expr->cast_expr()->visit(*this));
    remove_indent();

    return NoError();
}

void AstPrinter::PrintStmtHeader(Stmt* p_stmt) {
    // print node info
    _out << std::endl
         << get_indent() << '-' << p_stmt->NodeName() << " ["
         << std::to_string(p_stmt->Id()) << "] "
         << "<line:" << std::to_string(p_stmt->loc().first)
         << ", col:" << std::to_string(p_stmt->loc().second) << "> ";
}

void AstPrinter::add_indent(std::string_view str) {
    _indent.emplace_back(_indent.back() + std::string(str));
    std::replace(_indent.back().begin(), _indent.back().end(), '`', ' ');
}
AstPrinter::AstPrinter(std::ostream& output) : _out(output) {
    _indent.emplace_back("");
}
Status AstPrinter::visit(hzcc::ast::CompilationUnit* p_expr) {
    _out << "Compilation Unit [" << p_expr->GetFileName() << "]";
    add_indent(" |");
    for (auto& func : p_expr->get_decls()) {
        add_indent((func == p_expr->get_decls().back() ? "`" : "|"));
        HZCC_CHECK_OK_OR_RETURN(func.second->visit(*this));
        remove_indent();
    }
    remove_indent();
    return NoError();
}

}  // namespace hzcc::ast
