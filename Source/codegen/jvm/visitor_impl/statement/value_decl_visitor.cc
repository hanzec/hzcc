//
// Created by chen_ on 2022/4/10.
//
#include "AST/statement/value_decl.h"
#include "AST/type/ArrayType.h"
#include "codegen/jvm/JVMGenerator.h"
#include "codegen/jvm/utils/ConstValUtils.h"
#include "codegen/jvm/utils/TypeUtils.h"

namespace Hzcc::Codegen {
Status JVMGenerator::visit(Hzcc::AST::VarDecl *p_expr) {
    static std::unordered_map<std::string, const char *> kTypeName = {
        {"int", "int"},     {"char", "char"},     {"bool", "bool"},
        {"float", "float"}, {"double", "double"}, {"long", "long"}};

    /** #####################################################################
     *  ### Runtime Assertion                                             ###
     *  ##################################################################### */

    /** #####################################################################
     *  ### Code Generation                                               ###
     *  ##################################################################### */
    // current variable type is array, we need to initialize it
    if (p_expr->GetType()->IsArray()) {
        auto type =
            std::dynamic_pointer_cast<AST::ArrayType>(p_expr->GetType());
        DLOG_ASSERT(type)
            << "Current type " << p_expr->GetType()->Dump()
            << "  cannot cast to array type but IsArray() return true";

        // push array size
        if (type->HasDeduceSize()) {
            AddToCache(Utils::PushConstVal(static_cast<int>(type->GetSize())));
        } else {
            HZCC_JVM_Visit_Node(type->GetArraySizeNode());
        }

        // generate push command
        AddToCache(
            std::string("newarray ") +
            kTypeName[Utils::RemoveAttribute(type->GetBaseType()->GetName())]);

        // push array to stack
        auto var_name = p_expr->GetName();
        AddToCache("astore " + std::to_string(GetStackID(var_name)));
    }

    // if the expression contains a initializer, we need to push it
    if (p_expr->HasInitExpr()) {
        // visit the initializer expression
        HZCC_JVM_Use_Deduced_IF_POSSIBLE(p_expr->GetInitExpr());

        // push the initializer value
        AddToCache(SaveToVariable(p_expr->GetName()));
    }

    return Status::OkStatus();
}


}  // namespace Hzcc::Codegen
