//
// Created by chen_ on 2022/5/4.
//
#include <algorithm>

#include "AST/expr/DeclRefExpr.h"
#include "codegen/jvm/JVMGenerator.h"
namespace Hzcc::Codegen {
/**
 * @brief Emit JVM assembly for a DeclRefExpr node in the legacy backend.
 * @param p_expr Non-null borrowed AST node; ownership remains with the compilation unit.
 * @return OK on success, or the first backend/pass error.
 */
Status JVMGenerator::visit(Hzcc::AST::DeclRefExpr *p_expr) {
    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */
    DLOG_ASSERT(p_expr != nullptr) << "p_expr is nullptr";

    /** #####################################################################
     *  ### Code Generation                                               ###
     *  ##################################################################### */
    // Generate push instruction if needed
    if (_generate_load || _under_compare) {
        if (p_expr->RetType()->IsArray()) {
            auto var_name = p_expr->VarName();
            PushReturnStack(var_name);
            // Local variable have higher priority than global variable
            if (IsLocalVar(var_name)) {
                AddToCache("aload " + std::to_string(GetStackID(var_name)));
            } else {
                if (IsGlobalVar(var_name)) {
                    auto var_type = GetVarType(var_name);
                    std::transform(var_type.begin(), var_type.end(),
                                   var_type.begin(), ::toupper);
                    AddToCache("getstatic Field " + GetCurrentClassName() +
                               " " + var_name + " " + var_type);
                } else {
                    DLOG(FATAL) << "Unknown variable name: " << var_name;
                    return {Status::Code::NOT_FOUND,
                            "Unknown variable name " + var_name};
                }
            }
        } else {
            AddToCache(LoadFromVariable(p_expr->VarName()));
        }
    } else {
        PushReturnStack(p_expr->VarName());
    }

    return Status::OkStatus();
}
}  // namespace Hzcc::Codegen
