#include "codegen/jvm/constant_return.h"

#include <cstdint>
#include <limits>

#include "ast/expr/Expr.h"
#include "ast/type/Type.h"

namespace hzcc::codegen::jvm {
namespace {
/** @brief Select the shortest JVM instruction that loads the supplied int constant. */
std::string load_int(std::int32_t value) {
    if (value == -1) return "iconst_m1";
    if (value >= 0 && value <= 5) return "iconst_" + std::to_string(value);
    if (value >= -128 && value <= 127) return "bipush " + std::to_string(value);
    if (value >= -32768 && value <= 32767) return "sipush " + std::to_string(value);
    return "ldc " + std::to_string(value);
}
}  // namespace

StatusOr<std::string> EmitConstantReturnMethod(ast::FuncDeclStmt& function) {
    if (!function.has_body() || !function.params().empty() ||
        !ast::IsTypeOf<PrimitiveType::kInt>(function.type()) ||
        function.body_stmt()->body_stmts().size() != 1) {
        return UnimplementedError(
            "Expected a parameterless int function with a single return");
    }
    auto* ret = dynamic_cast<ast::ReturnStmt*>(function.body_stmt()->last_stmt().get());
    auto* literal =
        ret ? dynamic_cast<ast::LiteralExpr*>(ret->ret_expr().get()) : nullptr;
    if (!literal || !ast::IsTypeOf<PrimitiveType::kInt>(literal->type())) {
        return UnimplementedError("Expected an integer literal return value");
    }
    const auto value = literal->GetDeducedValue();
    if (!value || value->AsInt() < std::numeric_limits<int32_t>::min() ||
        value->AsInt() > std::numeric_limits<int32_t>::max()) {
        return OutOfRangeError("Return value does not fit in a JVM int");
    }
    return ".method public static " + std::string(function.name()) +
           " : ()I\n    .code stack 1 locals 0\n        " +
           load_int(static_cast<std::int32_t>(value->AsInt())) +
           "\n        ireturn\n    .end code\n.end method\n";
}
}  // namespace hzcc::codegen::jvm
