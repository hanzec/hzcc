//
// Created by chen_ on 2022/1/26.
//

#include "lexical/Token.h"

#include <glog/logging.h>

#include <optional>
#include <string>
#include <utility>

#include "lexical/utils/symbol_utils.h"
#include "utils/logging.h"

namespace Hzcc::Lexical {
Token::Token(const Token& old)
    : _token_type(old._token_type),
      _token_val_ref(old._token_val_ref),
      _location(old._location),
      _source_line_val_ref(old._source_line_val_ref) {
    // token string
    if (old._token_val_ref !=
        nullptr) {  // Speical symbol does not have string value
        auto ref = _const_value_storage.find(*_token_val_ref);
        if (ref != _const_value_storage.end()) {
            ref->second++;
        } else {
            DLOG(ERROR)
                << "Token value is not in const value storage when copy.";
        }
    }

    // source line cache
    if (old._source_line_val_ref != nullptr) {
        auto ref = _const_value_storage.find(*_source_line_val_ref);
        if (ref != _const_value_storage.end()) {
            ref->second++;
        } else {
            DLOG(ERROR) << "Source linke cache is not in const value "
                           "storage when copy.";
        }
    }
}

Token::Token(TokenType token_type, uint_fast32_t row,
             uint_fast32_t col) noexcept
    : _token_type(token_type), _location(std::make_pair(row, col)) {}

Token::Token(TokenType token_type, uint_fast32_t row, uint_fast32_t col,
             const std::string& line) noexcept
    : Token(token_type, row, col) {
    auto ref = _const_value_storage.find(line);
    if (ref != _const_value_storage.end()) {
        ref->second += 1;
        _source_line_val_ref = &ref->first;
        DVLOG(LEXICAL_LOG_LEVEL)
            << "increasing recount source line cache: [" << line << "]";
    } else {
        DVLOG(LEXICAL_LOG_LEVEL)
            << "insert new source line cache: [" << line << "]";
        _source_line_val_ref =
            &(_const_value_storage.emplace(line, 1).first->first);
    }
}

Token::Token(const std::string& token_string, TokenType token_type,
             uint_fast32_t row, uint_fast32_t col) noexcept
    : _token_type(token_type), _location(std::make_pair(row, col)) {
    auto ref = _const_value_storage.find(token_string);
    if (ref != _const_value_storage.end()) {
        ref->second += 1;
        _token_val_ref = &ref->first;
        DVLOG(LEXICAL_LOG_LEVEL)
            << "increasing recount for string: [" << token_string << "]";
    } else {
        DVLOG(LEXICAL_LOG_LEVEL)
            << "insert new token string: [" << token_string << "]";
        _token_val_ref =
            &(_const_value_storage.emplace(token_string, 1).first->first);
    }
}

Token::Token(const std::string& token_string, TokenType token_type,
             uint_fast32_t row, uint_fast32_t col,
             const std::string& line) noexcept
    : Token(token_string, token_type, row, col) {
    auto ref = _const_value_storage.find(line);
    if (ref != _const_value_storage.end()) {
        ref->second += 1;
        _source_line_val_ref = &ref->first;
        DVLOG(LEXICAL_LOG_LEVEL)
            << "increasing recount source line cache: [" << line << "]";
    } else {
        DVLOG(LEXICAL_LOG_LEVEL)
            << "insert new source line cache: [" << line << "]";
        _source_line_val_ref =
            &(_const_value_storage.emplace(line, 1).first->first);
    }
}

Token::~Token() noexcept {
    // clean up token string cache
    if (_token_val_ref !=
        nullptr) {  // Special symbol does not have string value
        auto ref = _const_value_storage.find(*_token_val_ref);
        if (ref != _const_value_storage.end()) {
            ref->second -= 1;
            if (ref->second == 0) {
                _const_value_storage.erase(ref);
            }
        } else {
            LOG(FATAL) << "[Internal] Token value reference not found";
        }
    }

    // clean up source line cache when cache is available
    if (_source_line_val_ref != nullptr) {
        auto ref = _const_value_storage.find(*_source_line_val_ref);
        if (ref != _const_value_storage.end()) {
            ref->second -= 1;
            if (ref->second == 0) {
                _const_value_storage.erase(ref);
            }
        } else {
            LOG(FATAL) << "[Internal] Token value reference not found";
        }
    }
}

bool Token::IsSymbol() const noexcept { return _token_type < 400; }

bool Token::IsKeyword() const noexcept { return _token_type > 400; }

void Token::Type(TokenType new_type) noexcept { _token_type = new_type; }

TokenType Token::Type() const noexcept { return _token_type; }

std::string Token::Value(bool escape) const noexcept {
    if (_token_val_ref == nullptr) {
        return SymbolUtils::TokenTypeToString(_token_type);
    } else {
        if (escape) {
            std::stringstream ss;
            if (_token_type == TokenType::kChar) ss << "\'";
            if (_token_type == TokenType::kString) ss << "\"";
            for (auto& c : *_token_val_ref) {
                if (std::iscntrl(c)) {
                    ss << SymbolUtils::ASCIIControlCodeToString(c);
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

const std::pair<uint_fast32_t, uint_fast32_t>& Token::Location()
    const noexcept {
    return _location;
}

std::string Token::SourceLine() const noexcept {
    return _source_line_val_ref == nullptr ? "NO_SOURCE_LINE"
                                           : std::string(*_source_line_val_ref);
}

}  // namespace Hzcc::Lexical
