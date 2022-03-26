//
// Created by chen_ on 2022/3/25.
//
#include <cassert>
#include "operator.h"

#ifndef MYCC_SOURCE_AST_OPERATOR_RELATIONAL_H_
#define MYCC_SOURCE_AST_OPERATOR_RELATIONAL_H_
namespace Mycc::AST {
class RelationalExpr : public OperatorBase {
  public:
    RelationalExpr(const std::string& type,std::unique_ptr<ASTNode> lhs, std::unique_ptr<ASTNode> rhs)
        : OperatorBase(std::move(rhs),std::move(lhs)){
        if (type == "==") {
            this->_type = RelationalType::EQ;
        } else if (type == "!=") {
            this->_type = RelationalType::NE;
        } else if (type == "<") {
            this->_type = RelationalType::LT;
        } else if (type == "<=") {
            this->_type = RelationalType::LE;
        } else if (type == ">") {
            this->_type = RelationalType::GT;
        } else if (type == ">=") {
            this->_type = RelationalType::GE;
        } else {
            assert(false);
        }
    }

  protected:
    enum RelationalType{
        LT,
        GT,
        LE,
        GE,
        EQ,
        NE
    };

  private:
    RelationalType _type;
};
}  // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_OPERATOR_RELATIONAL_H_
