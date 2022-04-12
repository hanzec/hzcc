//
// Created by chen_ on 2022/3/27.
//
#include "Type.h"

#include <cassert>

#include "ArrayType.h"
#include "lexical/utils/symbol_utils.h"
#include "lexical/utils/token_utils.h"
#include "utils/logging.h"

namespace Mycc::AST {
std::string Type::ToString(const std::list<std::shared_ptr<Type>>& types) {
    std::string ret = "(";
    for (const auto& type : types) {
        ret += type->GetName() + ",";
    }
    return ret.replace(ret.size() - 1, 1, ")");
}

std::shared_ptr<Type> Type::GetBasicType(
    const std::string& name, const std::list<Lexical::TokenType>& attrs) {
    // shared helper function
    struct make_shared_enabler : public Type {
        explicit make_shared_enabler(const std::string& name,
                                     const std::list<Lexical::TokenType>& attrs)
            : Type(name, attrs) {}
    };

    // check if the type name is empty
    DLOG_ASSERT(!name.empty())
        << "When get pointer of basic type, the name should not be empty";

    // type name should be a basic type
    DLOG_ASSERT(Lexical::SymbolUtils::IsPrimitiveType(name.c_str()))
        << "request type name [" << name << "] is not a basic type";

    // we will avoid creating a new type if the type already exists which means
    // there will be no same type with different id
    auto type = std::make_shared<make_shared_enabler>(name, attrs);
    if (_types.find(type->GetName()) != _types.end()) {
        return _types[type->GetName()];
    } else {
        _types[type->GetName()] = type;
        return type;
    }
}

Type::Type(const std::string& name, const std::list<Lexical::TokenType>& attrs)
    : _name(name), _id(++_counter_), _is_const(false) {
    // check type name
    DLOG_ASSERT(!(name.find(' ') != std::string::npos &&
                  name.find("struct") == std::string::npos))
        << "type name cannot contain space";

    // check attrs
    for (const auto& attr : attrs) {
        // the attribute should also be a valid attribute
        DLOG_ASSERT(Lexical::TokenUtils::IsAttribute(attr))
            << " invalid attribute: "
            << Lexical::SymbolUtils::TokenTypeToString(attr);

        // handle const attribute
        if (attr == Lexical::TokenType::kConst) {
            if (_is_const) {
                DLOG(ERROR) << "type cannot be const twice";
            }
            _is_const = true;
        }
    }
}

Type::~Type() {
    // we want release type id if this is a temporary type which means we delete
    // immediately after creation
    if (_counter_ == _id + 1) {
        _counter_--;
    }
}

bool Type::IsArray() const { return false; }

bool Type::IsStruct() const { return false; }

bool Type::IsFuncPtr() const { return false; }

std::string Type::Dump() { return GetName(); }

bool Type::IsConst() const { return _is_const; }

bool Type::IsSame(const Type& type) const {
    return type._is_const == this->_is_const && this->_name == type._name;
}

bool Type::IsSame(const std::shared_ptr<Type>& type) const {
    DLOG_ASSERT(type != nullptr) << " comparing current type with nullptr!";
    return this->_name == type->_name;
}
std::string Type::GetName(bool without_attr) const {
    if (without_attr) {
        return _name;
    } else {
        return (_is_const ? "const " : "") + _name;
    }
}
std::shared_ptr<Type> Type::GetConstType() const {
    if (_is_const) {
        return std::make_shared<Type>(*this);
    } else {
        return GetBasicType(_name, {Lexical::TokenType::kConst});
    }
}

std::list<Lexical::TokenType> Type::GetAttributes() const {
    if (_is_const) {
        return {Lexical::TokenType::kConst};
    } else {
        return {};
    }
}
}  // namespace Mycc::AST
