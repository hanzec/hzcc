#ifndef HZCC_CODEGEN_JVM_CONSTANT_RETURN_H
#define HZCC_CODEGEN_JVM_CONSTANT_RETURN_H

#include <string>

#include "utils/status/statusor.h"

namespace hzcc::ast {
class FuncDeclStmt;
}

namespace hzcc::codegen::jvm {
/** @brief Generate a JVM method for a parameterless int function returning one integer
 * literal.
 * @param function Borrowed AST function, including its body.
 * @return Krakatau method text or an error for an unsupported body, value or signature.
 */
StatusOr<std::string> EmitConstantReturnMethod(ast::FuncDeclStmt& function);
}  // namespace hzcc::codegen::jvm
#endif  // HZCC_CODEGEN_JVM_CONSTANT_RETURN_H
