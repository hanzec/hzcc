//
// Created by chen_ on 2022/1/26.
//

#ifndef MYCC_LEXICAL_H
#define MYCC_LEXICAL_H

#include <filesystem>
#include <memory>
#include <regex>

#include "macro.h"
#include "utils/Status.h"
namespace Mycc::Lexical {
class LexicalToken {
 public:
  /**
   * @brief The TokenType enum
   * @details The token type enum of the following tokens:
   *              TokenType     |     Token String     | Token Description
   *         ------------------ | -------------------- | -----------------
   *         Add                | "+"                  | Addition
   *         Sub                | "-"                  | Subtraction
   *         Mul                | "*"                  | Multiplication
   *         Div                | "/"                  | Division
   *         Number             | "0", ”0.1" etc.      | Numbers
   *         Identifiers        | "a", "while" etc.    | Identifiers
   */
  enum PACKED TokenType {
    kTokenType_Add,
    kTokenType_Sub,
    kTokenType_Mul,
    kTokenType_Div,
    kTokenType_Number,
    kTokenType_Identifiers,
    kTokenType_TokenCount
  };

  /**
   * @brief The LexicalToken constructor
   * @param token_string the human readable string of the token
   * @param token_location the location of the token in the source file which is a three tuple
   *                       variable (line, column, offset)
   */
  explicit LexicalToken(std::string& token_string,
                        std::tuple<std::filesystem::path, int, int> token_location) noexcept;

  /**
   * @brief Get the token type of the token
   * @return TokenType the token type of the token
   */
  [[nodiscard]] TokenType GetTokenType() const noexcept;

  /**
   * @brief Get the location of the token in the source file
   * @return std::tuple<std::filesystem::path, int, int> the location of the token in the
   *         source file which is a three tuple variable (line, column, offset)
   */
  [[nodiscard]] std::tuple<std::filesystem::path, int, int> GetLocation() const noexcept;

  /**
   * @brief Get the human readable string of the token
   * @note The token string is the original string of the token and should not be modified
   * @return const std::shared_ptr<std::string_view>& the human readable string value of the
   * token
   */
  [[nodiscard]] const std::shared_ptr<std::string_view>& GetTokenValue() const noexcept;

 protected:
  /**
   * @brief The regex pattern of the token
   */
  inline const static std::array<std::pair<std::regex, TokenType>,
                                 TokenType::kTokenType_TokenCount>
      _token_matchers = {
          std::make_pair(std::regex("\\+"), kTokenType_Add),
          std::make_pair(std::regex("-"), kTokenType_Sub),
          std::make_pair(std::regex("\\*"), kTokenType_Mul),
          std::make_pair(std::regex("/"), kTokenType_Div),
          std::make_pair(std::regex("[0-9]+\\.?[0-9]*"), kTokenType_Number),
          std::make_pair(std::regex("[a-zA-Z_][a-zA-Z0-9_]*"), kTokenType_Identifiers)};

 private:
  TokenType _token_type;
  std::shared_ptr<std::string_view> _value;
  std::tuple<std::filesystem::path, int, int> _token_location;
};

/**
 *
 * @param source
 * @return
 */
static std::vector<LexicalToken> ParseToToken(const std::filesystem::path& source);

}  // namespace Mycc::Lexical
#endif  // MYCC_LEXICAL_H
