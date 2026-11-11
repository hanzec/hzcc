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
/**
 * @brief Write a tree representation of AST nodes to a borrowed output stream.
 * @details The stream must outlive this visitor; indentation is maintained during traversal.
 */
class AstPrinter : public Visitor{
  public:
    /**
     * @brief Initialize a printer at the root indentation level.
     * @param output Borrowed destination stream; ownership is retained by the caller.
     */
    explicit AstPrinter(std::ostream& output);
    /**
     * @brief Release indentation storage without closing the borrowed stream.
     */
    ~AstPrinter() override = default;
    /**
     * @brief Write the tree representation of a ArithmeticExpr node.
     * @param p_expr Non-null borrowed node to print.
     * @return Success, or the first error reported by a visited child.
     */
    Status visit(ArithmeticExpr* p_expr) override;
    /**
     * @brief Write the tree representation of a AssignExpr node.
     * @param p_expr Non-null borrowed node to print.
     * @return Success, or the first error reported by a visited child.
     */
    Status visit(AssignExpr* p_expr) override;
    /**
     * @brief Write the tree representation of a BitwiseExpr node.
     * @param p_expr Non-null borrowed node to print.
     * @return Success, or the first error reported by a visited child.
     */
    Status visit(BitwiseExpr* p_expr) override;
    /**
     * @brief Write the tree representation of a CommaExpr node.
     * @param p_expr Non-null borrowed node to print.
     * @return Success, or the first error reported by a visited child.
     */
    Status visit(CommaExpr* p_expr) override;
    /**
     * @brief Write the tree representation of a LogicalExpr node.
     * @param p_expr Non-null borrowed node to print.
     * @return Success, or the first error reported by a visited child.
     */
    Status visit(LogicalExpr* p_expr) override;
    /**
     * @brief Write the tree representation of a RelationalExpr node.
     * @param p_expr Non-null borrowed node to print.
     * @return Success, or the first error reported by a visited child.
     */
    Status visit(RelationalExpr* p_expr) override;
    /**
     * @brief Write the tree representation of a MemberExpr node.
     * @param p_expr Non-null borrowed node to print.
     * @return Success, or the first error reported by a visited child.
     */
    Status visit(MemberExpr* p_expr) override;
    /**
     * @brief Write the tree representation of a ArraySubscriptExpr node.
     * @param p_expr Non-null borrowed node to print.
     * @return Success, or the first error reported by a visited child.
     */
    Status visit(ArraySubscriptExpr* p_expr) override;
    /**
     * @brief Write the tree representation of a TernaryExpr node.
     * @param p_expr Non-null borrowed node to print.
     * @return Success, or the first error reported by a visited child.
     */
    Status visit(TernaryExpr* p_expr) override;
    /**
     * @brief Write the tree representation of a DeclRefExpr node.
     * @param p_expr Non-null borrowed node to print.
     * @return Success, or the first error reported by a visited child.
     */
    Status visit(DeclRefExpr* p_expr) override;
    /**
     * @brief Write the tree representation of a LiteralExpr node.
     * @param p_expr Non-null borrowed node to print.
     * @return Success, or the first error reported by a visited child.
     */
    Status visit(LiteralExpr* p_expr) override;
    /**
     * @brief Write the tree representation of a SizeofExpr node.
     * @param p_expr Non-null borrowed node to print.
     * @return Success, or the first error reported by a visited child.
     */
    Status visit(SizeofExpr* p_expr) override;
    /**
     * @brief Write the tree representation of a UnaryOperator node.
     * @param p_expr Non-null borrowed node to print.
     * @return Success, or the first error reported by a visited child.
     */
    Status visit(UnaryOperator* p_expr) override;
    /**
     * @brief Reject direct printing of a type-proxy node.
     * @param p_expr Borrowed type-proxy node.
     * @return An internal-error status because type proxies are printed through their owners.
     */
    Status visit(TypeProxyExpr* p_expr) override;
    /**
     * @brief Write the tree representation of a BreakStmt node.
     * @param p_stmt Non-null borrowed node to print.
     * @return Success, or the first error reported by a visited child.
     */
    Status visit(BreakStmt* p_stmt) override;
    /**
     * @brief Write the tree representation of a CompoundStmt node.
     * @param p_stmt Non-null borrowed node to print.
     * @return Success, or the first error reported by a visited child.
     */
    Status visit(CompoundStmt* p_stmt) override;
    /**
     * @brief Write the tree representation of a ContinueStmt node.
     * @param p_stmt Non-null borrowed node to print.
     * @return Success, or the first error reported by a visited child.
     */
    Status visit(ContinueStmt* p_stmt) override;
    /**
     * @brief Write the tree representation of a DeclStmt node.
     * @param p_stmt Non-null borrowed node to print.
     * @return Success, or the first error reported by a visited child.
     */
    Status visit(DeclStmt* p_stmt) override;
    /**
     * @brief Write the tree representation of a DoStmt node.
     * @param p_stmt Non-null borrowed node to print.
     * @return Success, or the first error reported by a visited child.
     */
    Status visit(DoStmt* p_stmt) override;
    /**
     * @brief Skip an empty statement without emitting tree text.
     * @param p_stmt Borrowed placeholder statement.
     * @return Success.
     */
    Status visit(EmptyStmt* p_stmt) override;
    /**
     * @brief Write the tree representation of a ForStmt node.
     * @param p_stmt Non-null borrowed node to print.
     * @return Success, or the first error reported by a visited child.
     */
    Status visit(ForStmt* p_stmt) override;
    /**
     * @brief Write the tree representation of a FuncCallStmt node.
     * @param p_stmt Non-null borrowed node to print.
     * @return Success, or the first error reported by a visited child.
     */
    Status visit(FuncCallStmt* p_stmt) override;
    /**
     * @brief Write the tree representation of a FuncDeclStmt node.
     * @param p_stmt Non-null borrowed node to print.
     * @return Success, or the first error reported by a visited child.
     */
    Status visit(FuncDeclStmt* p_stmt) override;
    /**
     * @brief Write the tree representation of a IfStmt node.
     * @param p_stmt Non-null borrowed node to print.
     * @return Success, or the first error reported by a visited child.
     */
    Status visit(IfStmt* p_stmt) override;
    /**
     * @brief Write the tree representation of a ParamVarDecl node.
     * @param p_stmt Non-null borrowed node to print.
     * @return Success, or the first error reported by a visited child.
     */
    Status visit(ParamVarDecl* p_stmt) override;
    /**
     * @brief Write the tree representation of a ReturnStmt node.
     * @param p_stmt Non-null borrowed node to print.
     * @return Success, or the first error reported by a visited child.
     */
    Status visit(ReturnStmt* p_stmt) override;
    /**
     * @brief Write the tree representation of a WhileStmt node.
     * @param p_stmt Non-null borrowed node to print.
     * @return Success, or the first error reported by a visited child.
     */
    Status visit(WhileStmt* p_stmt) override;
    /**
     * @brief Write the tree representation of a VarDecl node.
     * @param p_stmt Non-null borrowed node to print.
     * @return Success, or the first error reported by a visited child.
     */
    Status visit(VarDecl* p_stmt) override;

    /**
     * @brief Write the tree representation of a ExplicitCastExpr node.
     * @param p_expr Non-null borrowed node to print.
     * @return Success, or the first error reported by a visited child.
     */
    Status visit(hzcc::ast::ExplicitCastExpr* p_expr) override;

    /**
     * @brief Write the tree representation of a ImplicitCastExpr node.
     * @param p_expr Non-null borrowed node to print.
     * @return Success, or the first error reported by a visited child.
     */
    Status visit(hzcc::ast::ImplicitCastExpr* p_expr) override;

    /**
     * @brief Write the tree representation of a CompilationUnit node.
     * @param p_expr Non-null borrowed node to print.
     * @return Success, or the first error reported by a visited child.
     */
    Status visit(hzcc::ast::CompilationUnit* p_expr) override;
  protected:
    /**
     * @brief Push an indentation prefix extending the current tree branch.
     * @param str Branch marker copied onto the current prefix.
     */
    void add_indent(std::string_view str);
    /**
     * @brief Pop the current indentation prefix after visiting a subtree.
     * @pre A matching add_indent() call has left a non-root entry on the stack.
     */
    void remove_indent();
    /**
     * @brief Borrow the current indentation prefix.
     * @return A view valid until the corresponding stack entry is removed.
     */
    std::string_view get_indent();

    /**
     * @brief Write a node's kind, identity, and source position.
     * @param p_stmt Non-null borrowed statement whose header is emitted.
     */
    void print_stmt_hdr(Stmt* p_stmt);

  private:
    std::ostream &_out;
    std::list<std::string> _indent;
};
}
#endif  // HZCC_AST_PRINTER_H
