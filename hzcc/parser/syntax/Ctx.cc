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
//    if (base_type == nullptr || !hasType(base_type->Name())) {
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
//Ctx::TypePtr Ctx::getFuncPtrType(const std::string &name) { return nullptr; }
//
//
//
//
//
//Ctx::TypePtr Ctx::getVariableType(const std::string &name) {
//    if (hasVariable(name, false)) {
//        return _current_context.lock()->getVariableType(name);
//    } else {
//        DLOG(ERROR) << "Variable " << name << " not found";
//        return nullptr;
//    }
//}
//
//std::pair<bool, int> Ctx::getVariableInfo(const std::basic_string<char> &name) {
//    DLOG_ASSERT(name.empty() == false) << "Variable name cannot be empty";
//    DLOG_ASSERT(hasVariable(name, false))
//        << "Variable " << name << " not found";
//
//    if (_current_context.lock() != nullptr &&
//        _current_context.lock()->hasVariable(name, false)) {
//        return std::make_pair(
//            false, _current_context.lock()->getVariableDeclLine(name));
//    } else {
//        DLOG(ERROR) << "Variable " << name << " not found";
//        return std::make_pair(false, 0);
//    }
//}
//
//void Ctx::addVariable(Position pos, const std::string &name,
//                      Ctx::TypePtr &variable_type) {
//    DLOG_ASSERT(_current_context.lock() != nullptr)
//        << " should never call add_var when in root context";
//    DLOG_ASSERT(!_current_context.lock()->hasVariable(name, true))
//        << "variable already exists";
//    _current_context.lock()->addVariable(pos, name, variable_type);
//}
//
//bool Ctx::hasFunction(const std::string &name) {
//    if (_function_def_table.find(name) != _function_def_table.end()) {
//        return true;
//    } else {
//        return false;
//    }
//}
//
//bool Ctx::hasFunctionBody(const std::string &name) {
//    if (_function_def_table.find(name + "_decl") != _function_def_table.end()) {
//        return true;
//    } else {
//        return false;
//    }
//}
//
//bool Ctx::addFunction(Position line_no, const std::string &name,
//                      Ctx::TypePtr &output,
//                      const std::list<Ctx::TypePtr> &argument_list) {
//    if (output == nullptr) {
//        DLOG(FATAL) << "Output type is nullptr for function " << name;
//    }
//
//    if (_function_def_table.find(name) != _function_def_table.end()) {
//        DLOG(ERROR) << "Function " << name << " already exists";
//        return false;
//    } else {
//        DVLOG(AST_LOG_LEVEL) << "Add new Function [" << name << "]";
//        _function_def_table.insert(std::make_pair(
//            name, std::make_tuple(output, argument_list, line_no)));
//        return true;
//    }
//}
//
//std::tuple<ast::TypePtr, std::list<ast::TypePtr>, Position>
//Ctx::func_def_info(const std::basic_string<char> &name) {
//    if (hasFunction(name)) {
//        return {
//            _function_def_table[name],
//        };
//    } else {
//        DVLOG(SYNTAX_LOG_LEVEL) << "Function " << name << " does not exist";
//        return {nullptr, {}, {-1, -1}};
//    }
//}
//void Ctx::addDecl(std::unique_ptr<ast::IDeclStmt> type) {
//    if (type->IsDeclNode() && !type->IsFuncDecl()) {
//        auto vartype = type->type();
//        _current_context.lock()->addVariable(type->Location(), type->DeclName(),
//                                             vartype);
//    }
//    _compilationUnit->addDecl(std::move(type));
//}
//
//}  // namespace hzcc::syntax
