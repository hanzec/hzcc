//
// Created by chen_ on 2022/1/26.
//

#include "Token.h"

#include <glog/logging.h>
#include <magic_enum.hpp>
#include <string>
#include <utility>
#include <cctype>
#include <ostream>

#include "parser/lexical/utils/symbol_utils.h"
#include "utils/logging.h"
#include "enums.h"
#include "parser/common/keywords.h"
#include "parser/common/token_type.h"
#include "utils/constexpr_utils.h"

#define HZCC_INSERT_TO_CACHE(NEW_CACHE, ASSIGN_TO)                         \
    {                                                                      \
        auto ref = _global_val_storage.find((NEW_CACHE));                  \
        if (ref != _global_val_storage.end()) {                            \
            (ASSIGN_TO) = ref->second;                                     \
        } else {                                                           \
            DVLOG(LEXICAL_LOG)                                       \
                << "insert new source (NEW_CACHE) cache: [" << (NEW_CACHE) \
                << "]";                                                    \
            auto new_item = std::make_shared<std::string>((NEW_CACHE));    \
            (ASSIGN_TO) = _global_val_storage.emplace(*new_item, new_item) \
                              .first->second;                              \
        }                                                                  \
    }

namespace hzcc {
Token::Token(TokenType token_type,                           // NOLINT
             uint_fast32_t row, uint_fast32_t col) noexcept  // NOLINT
    : _token_type(token_type), _location(std::make_pair(row, col)) {}

Token::Token(TokenType token_type,                           // NOLINT
             std::string_view token_string,                  // NOLINT
             uint_fast32_t row, uint_fast32_t col) noexcept  // NOLINT
    : Token(token_type, row, col) {
#ifdef HZCC_ENABLE_RUNTIME_CHECK
    if (!token_string.empty()) {
        HZCC_INSERT_TO_CACHE(token_string, _token_val_ref)
    } else {
        LOG(FATAL) << "token string is empty";
    }
#else
    HZCC_INSERT_TO_CACHE(token_string, _token_val_ref);
#endif
}

bool Token::IsSymbol() const noexcept {
    return magic_enum::enum_integer(_token_type) < 400;
}

bool Token::IsKeyword() const noexcept {
    return magic_enum::enum_integer(_token_type) > 400;
}

bool Token::IsAttribute() const noexcept {
    return utils::search_table(_token_type,
                               parser_common::kReversedAttributeTable) != -1;
}

void Token::Type(TokenType new_type) noexcept { _token_type = new_type; }

TokenType Token::Type() const noexcept { return _token_type; }

std::string Token::to_str(bool escape) const noexcept {
    if (_token_val_ref == nullptr) {
        return to_string(_token_type);
    } else {
        if (escape) {
            std::stringstream ss;
            if (_token_type == TokenType::kChar) ss << "\'";
            if (_token_type == TokenType::kString) ss << "\"";
            for (auto& c : *_token_val_ref) {
                if (std::iscntrl(c)) {
                    ss << lexical::SymbolUtils::ASCIIControlCodeToString(c);
                } else {
                    ss << c;
                }
            }
            if (_token_type == TokenType::kChar) ss << "\'";
            if (_token_type == TokenType::kString) ss << "\"";
            return ss.str();
        } else {
            return {*_token_val_ref};
        }
    }
}

Position Token::loc() const noexcept { return _location; }

}  // namespace hzcc
