//
// Created by chen_ on 2022/3/27.
//
#include "Type.h"

#include <string>
#include <utility>

#include "ArrayType.h"
#include "utils/keywords_utils.h"
#include "utils/logging.h"

namespace Hzcc::AST {

constexpr static uint_fast8_t CONST_ATTR_INDEX = 0;
constexpr static uint_fast8_t EXTERN_ATTR_INDEX = 1;
constexpr static uint_fast8_t STATIC_ATTR_INDEX = 2;
constexpr static uint_fast8_t AUTO_ATTR_INDEX = 3;
constexpr static uint_fast8_t REGISTER_ATTR_INDEX = 4;
constexpr static uint_fast8_t VOLATILE_ATTR_INDEX = 5;
constexpr static uint_fast8_t SIGNED_ATTR_INDEX = 6;
constexpr static uint_fast8_t UNSIGNED_ATTR_INDEX = 7;
constexpr static uint_fast8_t INLINE_ATTR_INDEX = 8;
constexpr static uint_fast8_t RESTRICT_ATTR_INDEX = 9;

Type::Type(const std::list<TokenType>& attrs) {
    // check attrs
    for (const auto& attr : attrs) {
        // the attribute should also be a valid attribute
        DLOG_ASSERT(KeywordsUtils::IsAttribute(attr))
            << " invalid attribute: " << KeywordsUtils::TokenTypeToString(attr);
    }

    // setting attrs
    for (size_t i = 0; i < kAttributeTableSize; ++i) {
        for (auto attr : attrs) {
            if (attr == Keywords::kReversedAttributeTable[i]) {
                _attrs[i] = true;
            } else {
                _attrs[i] = false;
            }
        }
    }
}

bool Type::IsArray() const { return false; }

bool Type::IsStruct() const { return false; }

bool Type::IsFuncPtr() const { return false; }

bool Type::IsBuiltinType() const { return false; }

std::string Type::Dump() { return Name(); }

bool Type::IsConst() const { return _attrs[CONST_ATTR_INDEX]; }

std::list<TokenType> Type::GetAttributes() {
    std::list<TokenType> ret;
    for (size_t i = 0; i < kAttributeTableSize; ++i) {
        if (_attrs[i]) {
            ret.push_back(Keywords::kReversedKeywordTable[i]);
        }
    }
    return ret;
}
bool Type::operator==(const Type& rhs) const { return IsSame(rhs); }
bool Type::operator!=(const Type& type) const { return !IsSame(type); }
bool Type::IsPointer() const { return false; }
uintptr_t Type::Id() const { return reinterpret_cast<uintptr_t>(this); }
}  // namespace Hzcc::AST
