#include "AST/expr/LiteralExpr.h"
#include "codegen/jvm/JVMGenerator.h"
namespace Hzcc::Codegen {
/**
 * @brief Emit JVM assembly for a LiteralExpr node in the legacy backend.
 * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
 * @return OK on success, or the first backend/pass error.
 */
Status JVMGenerator::visit(Hzcc::AST::LiteralExpr *p_expr) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    DLOG_ASSERT(p_expr != nullptr) << "p_expr is nullptr";

    /** #####################################################################
     *  ### Code Generation                                               ###
     *  ##################################################################### */

    // for string literal we push value use LDC then call java method to get
    // char array
    if (p_expr->GetLiteralType() == AST::LiteralType::kLiteralType_String) {
        AddToCache("ldc '" + p_expr->GetValue() + "'");
        AddToCache("invokevirtual Method java/lang/String toCharArray ()[C");
    }

    // otherwise we're using push function
    else {
        AddToCache(Utils::PushConstVal(p_expr->RetType()->GetName(),
                                       p_expr->GetDeducedValue().value()));
    }

    return Status::OkStatus();
}
}  // namespace Hzcc::Codegen
