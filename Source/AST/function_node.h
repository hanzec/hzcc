#ifndef MYCC_FUNCTION_NODE_H
#define MYCC_FUNCTION_NODE_H

#include <algorithm>
#include <list>
#include <memory>
#include <string_view>
#include <utility>
#include <vector>

#include "type.h"

namespace Mycc::AST {
using ArgumentList =
    std::list<std::pair<std::string_view, std::shared_ptr<AST::Type>>>;

#define MYCC_ExistInArgumentList(arguments, name)                            \
    std::find_if((arguments).begin(), (arguments).end(),                     \
                 [name](const auto &arg) { return arg.first == (name); }) != \
        (arguments).end()

class FunctionNode : public ASTNode {
  public:
    /**
     * @brief Construct a new Function Prototype For Function Node
     *
     * @param name the name of the function
     * @param return_type the return type of the function
     * @param arguments the arguments of the function
     */
    FunctionNode(ArgumentList arguments,          // NOLINT
                 std::string_view function_name,  // NOLINT
                 const std::list<Lexical::Token>& attrs)
        : _argument(std::move(arguments)), _name(function_name){}
    bool set_body(std::unique_ptr<AST::ASTNode> declaration) {
        if (_body) {
            return false;
        }
        _body = std::move(declaration);
        return true;
    }

  private:
    std::string_view _name;
    std::shared_ptr<Type> _return_type;
    std::unique_ptr<ASTNode> _body;
    ArgumentList _argument;
};
}  // namespace Mycc::AST
#endif  // MYCC_FUNCTION_NODE_H