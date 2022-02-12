#ifndef MYCC_PROTOTYPE_NODE_H
#define MYCC_PROTOTYPE_NODE_H
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "type.h"

namespace Mycc::AST {
class PortotypeNode {
 public:
 /**
  * @brief Construct a new Function Prototype For Function Node
  * 
  * @param name the name of the function
  * @param return_type the return type of the function
  * @param arguments the arguments of the function
  */
  PortotypeNode(std::string name,                              // NOLINT
                std::shared_ptr<Type> return_type,             // NOLINT
                std::vector<std::shared_ptr<Type>> arguments)  // NOLINT
      : name_(std::move(name)),
        _argument(std::move(arguments)),
        _return_type(std::move(return_type)) {}

 private:
  std::string name_;
  std::shared_ptr<Type> _return_type;
  std::vector<std::shared_ptr<Type>> _argument;
};
};      // namespace Mycc::AST
#endif  // MYCC_PROTOTYPE_NODE_H