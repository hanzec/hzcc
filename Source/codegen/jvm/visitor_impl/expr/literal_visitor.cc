#include "AST/expr/literal.h"
#include "codegen/jvm/JVMGenerator.h"
namespace Hzcc::Codegen {
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
    if (p_expr->LiteralType() == AST::LiteralType::kLiteralType_String) {
        AddToCache("ldc '" + p_expr->GetValue() + "'");
        AddToCache("invokevirtual Method java/lang/String toCharArray ()[C");
    }

    // otherwise we're using push function
    else {
        AddToCache(Utils::PushConstVal(p_expr->GetType()->GetName(),
                                       p_expr->GetDeducedValue().value()));
    }

    return Status::OkStatus();
}
}  // namespace Hzcc::Codegen
