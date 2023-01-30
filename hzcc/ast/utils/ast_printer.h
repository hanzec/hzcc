//
// Created by chen_ on 2022/12/30.
//

#ifndef HZCC_AST_PRINTER_H
#define HZCC_AST_PRINTER_H
#include <list>
#include <iosfwd>
#include <string>
#include <string_view>

#include "ast/visitor.h"
#include "utils/status/status.h"

namespace hzcc::ast {
class ArithmeticExpr;
class ArraySubscriptExpr;
class AssignExpr;
class BitwiseExpr;
class BreakStmt;
class CastExpr;
class CommaExpr;
class CompilationUnit;
class CompoundStmt;
class ContinueStmt;
class DeclRefExpr;
class DeclStmt;
class DoStmt;
class EmptyStmt;
class ExplicitCastExpr;
class ForStmt;
class FuncCallStmt;
class FuncDeclStmt;
class IDeclStmt;
class IfStmt;
class ImplicitCastExpr;
class LiteralExpr;
class LogicalExpr;
class MemberExpr;
class ParamVarDecl;
class RelationalExpr;
class ReturnStmt;
class SizeofExpr;
class Stmt;
class TernaryExpr;
class TypeProxyExpr;
class UnaryOperator;
class VarDecl;
class WhileStmt;
}  // namespace hzcc

namespace hzcc::ast {
class AstPrinter : public Visitor{
  public:
    explicit AstPrinter(std::ostream& output);
    ~AstPrinter() override = default;
    Status visit(ArithmeticExpr* p_expr) override;
    Status visit(AssignExpr* p_expr) override;
    Status visit(BitwiseExpr* p_expr) override;
    Status visit(CommaExpr* p_expr) override;
    Status visit(LogicalExpr* p_expr) override;
    Status visit(RelationalExpr* p_expr) override;
    Status visit(MemberExpr* p_expr) override;
    Status visit(ArraySubscriptExpr* p_expr) override;
    Status visit(TernaryExpr* p_expr) override;
    Status visit(DeclRefExpr* p_expr) override;
    Status visit(LiteralExpr* p_expr) override;
    Status visit(SizeofExpr* p_expr) override;
    Status visit(UnaryOperator* p_expr) override;
    Status visit(TypeProxyExpr* p_expr) override;
    Status visit(BreakStmt* p_stmt) override;
    Status visit(CompoundStmt* p_stmt) override;
    Status visit(ContinueStmt* p_stmt) override;
    Status visit(IDeclStmt* p_stmt) override;
    Status visit(DeclStmt* p_stmt) override;
    Status visit(DoStmt* p_stmt) override;
    Status visit(EmptyStmt* p_stmt) override;
    Status visit(ForStmt* p_stmt) override;
    Status visit(FuncCallStmt* p_stmt) override;
    Status visit(FuncDeclStmt* p_stmt) override;
    Status visit(IfStmt* p_stmt) override;
    Status visit(ParamVarDecl* p_stmt) override;
    Status visit(ReturnStmt* p_stmt) override;
    Status visit(WhileStmt* p_stmt) override;
    Status visit(VarDecl* p_stmt) override;

    Status visit(hzcc::ast::ExplicitCastExpr* p_expr) override;

    Status visit(hzcc::ast::ImplicitCastExpr* p_expr) override;

    Status visit(hzcc::ast::CompilationUnit* p_expr) override;
  protected:
    void add_indent(std::string_view str);
    void remove_indent();
    std::string_view get_indent();

    void PrintStmtHeader(Stmt* p_stmt);

  private:
    std::ostream &_out;
    std::list<std::string> _indent;


};
}
#endif  // HZCC_AST_PRINTER_H
