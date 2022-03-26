//
// Created by chen_ on 2022/3/25.
//
#include <cassert>
#include "operator.h"
#ifndef MYCC_SOURCE_AST_OPERATOR_LOGICAL_H_
#define MYCC_SOURCE_AST_OPERATOR_LOGICAL_H_
namespace Mycc::AST {
class LogicalExpr : public OperatorBase {
  public:
    LogicalExpr(const std::string& type, std::unique_ptr<ASTNode> lhs,
                std::unique_ptr<ASTNode> rhs)
        : OperatorBase(std::move(lhs), std::move(rhs)) {
        switch (type[0]) {
            case '&':
                this->_type = LogicalType::kAnd;
                break;
            case '|':
                this->_type = LogicalType::kOr;
                break;
            case '=':
                this->_type = LogicalType::kEqual;
                break;
            case '!':
                this->_type = LogicalType::kNotEqual;
                break;
            default:
                assert(false);
        }
    };

  protected:
    enum LogicalType {
        kAnd,
        kOr,
        kEqual,
        kNotEqual,
    };

  private:
    LogicalType _type;
};
}  // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_OPERATOR_LOGICAL_H_
