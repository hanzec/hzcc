////
//// Created by chen_ on 2022/5/11.
////
//
//#include <memory>
//#include <utility>
//
//#include "ast/type/Type.h"
//namespace hzcc::syntax {
//
//
//Ctx::TypePtr Ctx::getArrayType(const Ctx::TypePtr &base_type,
//                               const TokenList &attr_list,
//                               std::list<ast::ExprPtr> &shape) {
//    // convert the shape to a list of TokenType
//    AttrList attr_types;
//    for (const auto &type : attr_list) {
//        attr_types.emplace_back(type.Type());
//    }
//
//    if (base_type == nullptr || !hasType(base_type->to_str())) {
//        DLOG(ERROR) << "type not found";
//        return nullptr;
//    } else {
//        Ctx::TypePtr current_type = base_type;
//        while (!shape.empty()) {
//            auto shape_node = std::move(shape.front());
//            shape.pop_front();
//            current_type = std::make_shared<ast::ArrayType>(
//                current_type, std::move(shape_node), attr_types);
//        }
//
//        return current_type;
//    }
//}
//
//Ctx::TypePtr Ctx::getFuncPtrType(const std::string &to_str) { return nullptr; }
//
//
//
//
//
//Ctx::TypePtr Ctx::getVariableType(const std::string &to_str) {
//    if (hasVariable(to_str, false)) {
//        return _current_context.lock()->getVariableType(to_str);
//    } else {
//        DLOG(ERROR) << "Variable " << to_str << " not found";
//        return nullptr;
//    }
//}
//

//
//
//
//bool Ctx::addFunction(Position line_no, const std::string &to_str,
//                      Ctx::TypePtr &output,
//                      const std::list<Ctx::TypePtr> &argument_list) {
//    if (output == nullptr) {
//        DLOG(FATAL) << "Output type is nullptr for function " << to_str;
//    }
//
//    if (_function_def_table.find(to_str) != _function_def_table.end()) {
//        DLOG(ERROR) << "Function " << to_str << " already exists";
//        return false;
//    } else {
//        DVLOG(AST_LOG_LEVEL) << "Add new Function [" << to_str << "]";
//        _function_def_table.insert(std::make_pair(
//            to_str, std::make_tuple(output, argument_list, line_no)));
//        return true;
//    }
//}
//
//std::tuple<ast::TypePtr, std::list<ast::TypePtr>, Position>
//Ctx::func_def_info(const std::basic_string<char> &to_str) {

//}
//void Ctx::addDecl(std::unique_ptr<ast::IDeclStmt> type) {
//    if (type->is_decl() && !type->IsFuncDecl()) {
//        auto vartype = type->type();
//        _current_context.lock()->addVariable(type->Location(), type->DeclName(),
//                                             vartype);
//    }
//    _compilationUnit->addDecl(std::move(type));
//}
//
//}  // namespace hzcc::syntax
