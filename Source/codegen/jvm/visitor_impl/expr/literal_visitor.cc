#include "AST/expr/literal.h"
#include "codegen/jvm/JVMGenerator.h"
namespace Hzcc::Codegen {
Status JVMGenerator::visit(Hzcc::AST::LiteralExpr *p_expr) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    DLOG_ASSERT(p_expr != nullptr) << "p_expr is nullptr";
    DLOG_ASSERT(p_expr->GetDeducedValue().has_value())
        << "LiteralExpr(line: " << p_expr->GetLine()
        << ") has no deduced value";

    /** #####################################################################
     *  ### Code Generation                                               ###
     *  ##################################################################### */

    AddToCache(Utils::PushConstVal(p_expr->GetType()->GetName(),
                                   p_expr->GetDeducedValue().value()));
    return Status::OkStatus();
}
}  // namespace Hzcc::Codegen