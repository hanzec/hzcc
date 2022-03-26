//
// Created by chen_ on 2022/3/25.
//
#include <cassert>
#include <memory>

#include "AST/ast_node.h"
#include "operator.h"
#ifndef MYCC_SOURCE_AST_OPERATOR_ASSIGN_H_
#define MYCC_SOURCE_AST_OPERATOR_ASSIGN_H_
namespace Mycc::AST {
class AssignExpr : public OperatorBase {
  public:
    AssignExpr(const std::string& type, std::unique_ptr<ASTNode> lhs,
               std::unique_ptr<ASTNode> rhs)
        : OperatorBase(std::move(lhs), std::move(rhs)) {
        switch (type[0]) {
            case '=':
                this->_type = AssignType::kAssign;
                break;
            case '+':
                this->_type = AssignType::kAddAssign;
                break;
            case '-':
                this->_type = AssignType::kSubAssign;
                break;
            case '*':
                this->_type = AssignType::kMulAssign;
                break;
            case '/':
                this->_type = AssignType::kDivAssign;
                break;
            case '%':
                this->_type = AssignType::kModAssign;
                break;
            case '<':
                this->_type = AssignType::kLShiftAssign;
                break;
            case '>':
                this->_type = AssignType::kRShiftAssign;
                break;
            case '&':
                this->_type = AssignType::kAndAssign;
                break;
            case '|':
                this->_type = AssignType::kOrAssign;
                break;
            case '^':
                this->_type = AssignType::kXorAssign;
                break;
            default:
                assert(false);
        }
    };

  protected:
    enum AssignType {
        kAssign,
        kAddAssign,
        kSubAssign,
        kMulAssign,
        kDivAssign,
        kModAssign,
        kLShiftAssign,
        kRShiftAssign,
        kAndAssign,
        kOrAssign,
        kXorAssign,
    };

  private:
    AssignType _type;
};
}  // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_OPERATOR_ASSIGN_H_
