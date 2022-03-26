//
// Created by chen_ on 2022/3/25.
//
#include "operator.h"

#ifndef MYCC_SOURCE_AST_OPERATOR_BITWISE_H_
#define MYCC_SOURCE_AST_OPERATOR_BITWISE_H_
namespace Mycc::AST {
class BitwiseExpr : public OperatorBase {
  public:
    BitwiseExpr(const std::string& type, std::unique_ptr<ASTNode> lhs,
                std::unique_ptr<ASTNode> rhs)
        : OperatorBase(std::move(rhs), std::move(lhs)) {
        switch (type[0]) {
            case '&':
                this->_type = BitwiseType::kAND;
                break;
            case '|':
                this->_type = BitwiseType::kOR;
                break;
            case '^':
                this->_type = BitwiseType::kXOR;
                break;
            case '<':
                this->_type = BitwiseType::kLSHIFT;
                break;
            case '>':
                this->_type = BitwiseType::kRSHIFT;
                break;
                default:
                    assert(false);
        }
    }

  protected:
    enum BitwiseType {
        kAND,
        kOR,
        kXOR,
        kLSHIFT,
        kRSHIFT,
    };

  private:
    BitwiseType _type;
};
}  // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_OPERATOR_BITWISE_H_
