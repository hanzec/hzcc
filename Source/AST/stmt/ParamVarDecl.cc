//
// Created by chen_ on 2022/3/29.
//

#include "ParamVarDecl.h"

#include "EmptyStmt.h"
namespace Hzcc::AST {
ParamVarDecl::ParamVarDecl(std::shared_ptr<Type> type,
                           const std::string_view& name,
                           const Position& loc)  // NO_LINT
    : VarDecl(loc, std::move(type), name, std::make_unique<EmptyStmt>()){
          /** ############################################################
           *  ### Runtime Assertion                                   ###
           *  ###########################################################*/
          // already checked in VarDecl
      };

const char* ParamVarDecl::NodeName() const { return "ParamVarDecl"; }
Status ParamVarDecl::visit(ASTVisitor& visitor) { return visitor.visit(this); }

}  // namespace Hzcc::AST
