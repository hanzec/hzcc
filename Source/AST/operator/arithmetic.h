//
// Created by chen_ on 2022/3/25.
//
#include <cassert>
#include "operator.h"

#ifndef MYCC_SOURCE_AST_OPERATOR_ARITHMETIC_H_
#define MYCC_SOURCE_AST_OPERATOR_ARITHMETIC_H_
namespace Mycc::AST {
class ArithmeticExpr : public OperatorBase {
  public:
    ArithmeticExpr(const std::string& type, std::unique_ptr<ASTNode> lhs,
               std::unique_ptr<ASTNode> rhs)
        : OperatorBase(std::move(lhs), std::move(rhs)) {
        switch (type[0]) {
            case '+':
                this->_type = ArithmeticType::kAdd;
                break;
            case '-':
                this->_type = ArithmeticType::kSub;
                break;
            case '*':
                this->_type = ArithmeticType::kMul;
                break;
            case '/':
                this->_type = ArithmeticType::kDiv;
                break;
            case '%':
                this->_type = ArithmeticType::kMod;
                break;
            default:
                assert(false);
        }
    };

  protected:
    enum ArithmeticType {
        kAdd,
        kSub,
        kMul,
        kDiv,
        kMod,
    };

  private:
    ArithmeticType _type;
};
}  // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_OPERATOR_ARITHMETIC_H_
