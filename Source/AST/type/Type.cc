//
// Created by chen_ on 2022/3/27.
//
#include "Type.h"

#include <string>
#include <utility>

#include "ArrayType.h"
#include "BaseType.h"
#include "lexical/utils/symbol_utils.h"
#include "syntax/utils/token_utils.h"
#include "utils/logging.h"

namespace Hzcc::AST {

constexpr static int CONST_ATTR_INDEX = 0;

Type::BaseTypePtr Type::GetBaseType(const std::string& name) {
    if (_cached_b_types.find(name) != _cached_b_types.end()) {
        return _cached_b_types[name];
    } else {
        return _cached_b_types.emplace(name, std::make_shared<BaseType>(name))
            .first->second;
    }
}

std::shared_ptr<Type> Type::GetTypeOf(
    const std::shared_ptr<Type>& other_type,
    const std::list<Lexical::TokenType>& attrs) {
    // shared helper function
    struct make_shared_enabler : public Type {
        explicit make_shared_enabler(const std::shared_ptr<BaseType>& name,
                                     const std::list<Lexical::TokenType>& attrs)
            : Type(name, attrs) {}
    };

    // check if the type name is empty
    DLOG_ASSERT(other_type != nullptr)
        << "Type::GetTypeOf: other_type is nullptr";

    // check if the type is already cached
    auto new_type =
        std::make_shared<make_shared_enabler>(other_type->_base_type, attrs);
    if (_cached_types.find(new_type->GetName()) != _cached_types.end()) {
        return _cached_types[new_type->GetName()];
    } else {
        return _cached_types.emplace(new_type->GetName(), new_type)
            .first->second;
    }
}

std::shared_ptr<Type> Type::GetTypeOf(
    const std::string& name, const std::list<Lexical::TokenType>& attrs) {
    // shared helper function
    struct make_shared_enabler : public Type {
        explicit make_shared_enabler(const std::string& name,
                                     const std::list<Lexical::TokenType>& attrs)
            : Type(Type::GetBaseType(name), attrs) {}
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
    if (_cached_types.find(type->GetName()) != _cached_types.end()) {
        return _cached_types[type->GetName()];
    } else {
        _cached_types[type->GetName()] = type;
        return type;
    }
}

Type::Type(BaseTypePtr base_type, const std::list<Lexical::TokenType>& attrs)
    : _base_type(std::move(base_type)), _id(++_counter_) {
    // check attrs
    for (const auto& attr : attrs) {
        // the attribute should also be a valid attribute
        DLOG_ASSERT(Syntax::TokenUtils::IsAttribute(attr))
            << " invalid attribute: "
            << Lexical::SymbolUtils::TokenTypeToString(attr);
    }

    // setting attrs
    for (size_t i = 0; i < kVariableAttrTableSize; ++i) {
        for (auto attr : attrs) {
            if (attr == Syntax::TokenUtils::kAtrributeTable[i]) {
                _attrs[i] = true;
            } else {
                _attrs[i] = false;
            }
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

bool Type::IsConst() const { return _attrs[CONST_ATTR_INDEX]; }

bool Type::IsSame(const Type& type) const {
    return type._attrs == this->_attrs && this->_base_type == type._base_type;
}

bool Type::IsSame(const std::shared_ptr<Type>& type) const {
    DLOG_ASSERT(type != nullptr) << " comparing current type with nullptr!";
    return type->_attrs == this->_attrs && this->_base_type == type->_base_type;
}

std::string Type::GetName(bool without_attr) const {
    if (without_attr) {
        return _base_type->GetName();
    } else {
        return (IsConst() ? "const " : "") + _base_type->GetName();
    }
}
std::shared_ptr<Type> Type::GetConstType() {
    if (IsConst()) {
        return shared_from_this();
    } else {
        return GetTypeOf(shared_from_this(), {Lexical::TokenType::kConst});
    }
}
std::list<Lexical::TokenType> Type::GetAttributes() {
    std::list<Lexical::TokenType> ret;
    for (size_t i = 0; i < kVariableAttrTableSize; ++i) {
        if (_attrs[i]) {
            ret.push_back(Syntax::TokenUtils::kAtrributeTable[i]);
        }
    }
    return ret;
}
}  // namespace Hzcc::AST
