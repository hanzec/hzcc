//
// Created by chen_ on 2022/3/25.
//
#include <string>
#include <string_view>
#include "AST/ast_node.h"

#ifndef MYCC_SOURCE_AST_VALUE_VALUE_H_
#define MYCC_SOURCE_AST_VALUE_VALUE_H_
namespace Mycc::AST {
class ValueNode : public ASTNode {
  public:
    enum ValueType { kChar, kInteger, kReal_number, kString };

    ValueNode(ValueType type, const std::string &value)
        : _type(type), value_(value) {}
  private:
    ValueType _type;
    std::string_view value_ ;
};

}  // namespace Mycc::AST
#endif  // MYCC_SOURCE_AST_VALUE_VALUE_H_
