//
// Created by chen_ on 2022/1/26.
//

#include "Token.h"

#include <glog/logging.h>

#include <cctype>
#include <magic_enum/magic_enum.hpp>
#include <ostream>
#include <string>
#include <utility>

#include "enums.h"
#include "parser/common/keywords.h"
#include "parser/common/token_type.h"
#include "parser/lexical/utils/symbol_utils.h"
#include "utils/constexpr_utils.h"
#include "utils/logging.h"

namespace hzcc {
Token::Token(TokenType token_type,                           // NOLINT
             uint_fast32_t row, uint_fast32_t col) noexcept  // NOLINT
    : _location(row, col), _token_type(token_type) {}

Token::Token(TokenType token_type,                           // NOLINT
             std::string_view token_string,                  // NOLINT
             uint_fast32_t row, uint_fast32_t col) noexcept  // NOLINT
    : Token(token_type, std::make_shared<const std::string>(token_string), row, col) {}

Token::Token(TokenType token_type, std::shared_ptr<const std::string> spelling,
             uint_fast32_t row, uint_fast32_t col) noexcept
    : _location(row, col), _token_type(token_type), _token_val_ref(std::move(spelling)) {}

void Token::Type(TokenType new_type) noexcept { _token_type = new_type; }

TokenType Token::type() const noexcept { return _token_type; }

std::string Token::to_str(bool escape) const noexcept {
    if (_token_val_ref == nullptr) {
        auto idx = magic_enum::enum_integer(_token_type);

        // 0-128 is ascii
        if (idx <= 128) {
            // convert to char then to string
            return std::string(1, static_cast<char>(idx));
        }

        // literal
        else if (idx >= magic_enum::enum_integer(TokenType::LitType_START) &&
                 idx <= magic_enum::enum_integer(TokenType::LitType_END)) {
            return kLiteralsSymbol[idx -
                                   magic_enum::enum_integer(TokenType::LitType_START)];
        }

        // type specifier
        else if (idx >= magic_enum::enum_integer(TokenType::TypeSpec_START) &&
                 idx <= magic_enum::enum_integer(TokenType::TypeSpec_END)) {
            return kTypeSpecifier[idx -
                                  magic_enum::enum_integer(TokenType::TypeSpec_START)];
        }

        // operators
        else if (idx >= magic_enum::enum_integer(TokenType::Op_START) &&
                 idx <= magic_enum::enum_integer(TokenType::Op_END)) {
            return kOperatorTable[idx - magic_enum::enum_integer(TokenType::Op_START)];
        }

        // qualifiers
        else if (idx >= magic_enum::enum_integer(TokenType::Qual_START) &&
                 idx <= magic_enum::enum_integer(TokenType::Qual_END)) {
            return kQualifierTable[idx - magic_enum::enum_integer(TokenType::Qual_START)];
        }

        // keywords
        else if (idx >= magic_enum::enum_integer(TokenType::Keyword_START) &&
                 idx <= magic_enum::enum_integer(TokenType::Keyword_END)) {
            return kKeywordTable[idx -
                                 magic_enum::enum_integer(TokenType::Keyword_START)];
        }

        // others
        else {
#ifdef HZCC_ENABLE_RUNTIME_CHECK
            LOG(WARNING) << "unknown token type: "
                         << magic_enum::enum_integer(_token_type);
#endif
            return "unknown token";
        }

    } else {
        if (escape) {
            std::stringstream ss;
            if (_token_type == TokenType::Char_Lit) ss << "\'";
            if (_token_type == TokenType::Str_Lit) ss << "\"";
            for (auto& c : *_token_val_ref) {
                if (std::iscntrl(static_cast<unsigned char>(c))) {
                    ss << lexical::SymbolUtils::ASCIIControlCodeToString(c);
                } else {
                    ss << c;
                }
            }
            if (_token_type == TokenType::Char_Lit) ss << "\'";
            if (_token_type == TokenType::Str_Lit) ss << "\"";
            return ss.str();
        } else {
            return {*_token_val_ref};
        }
    }
}

Position Token::loc() const noexcept { return _location; }

}  // namespace hzcc
