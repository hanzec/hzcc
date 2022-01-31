//
// Created by chen_ on 2022/1/26.
//


#include <utility>
#include "lexical.h"

namespace Mycc::Lexical {
LexicalToken::LexicalToken(std::string &token_string,
                           std::tuple<std::filesystem::path, int, int> token_location) noexcept
    : _token_location(std::move(token_location)),
      _value(std::make_shared<std::string_view>(token_string)) {
  for (const auto &token_matcher : _token_matchers) {
    // check if matcher match the token
    if (std::regex_match(token_string, token_matcher.first)) {
      _token_type = token_matcher.second;
      break;
    }
  }
}

LexicalToken::TokenType LexicalToken::GetTokenType() const noexcept {
  return _token_type;
}

const std::shared_ptr<std::string_view>& LexicalToken::GetTokenValue() const noexcept {
  return _value;
}

std::tuple<std::filesystem::path, int, int> LexicalToken::GetLocation() const noexcept {
  return _token_location;
}

}  // namespace MyCC::Lexical
