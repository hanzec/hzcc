#include <stdint.h>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include "macro.h"
#include "token_type.h"
#ifndef HZCC_LEXICAL_LEXICAL_TOKEN_H
#define HZCC_LEXICAL_LEXICAL_TOKEN_H

namespace hzcc {
/**
 * @brief A lexical token with source coordinates and an optional shared spelling.
 * @note Each token retains its spelling independently of the lexer and source stream.
 */
class Token {
  public:
    /**
     * @brief The LexicalToken constructor
     * @param token_type The token type of current LexicalToken
     * @param row row number of current LexicalToken exists in the source code
     * @param col column number of current LexicalToken exists in the source
     * code
     */
    Token(TokenType token_type,                            // NOLINT
          uint_fast32_t row, uint_fast32_t col) noexcept;  // NOLINT

    /**
     * @brief The LexicalToken constructor
     * @param token_string  the string of current LexicalToken
     * @param token_type the token type of current LexicalToken
     * @param row row number of current LexicalToken exists in the source code
     * @param col column number of current LexicalToken exists in the source
     * code
     */
    Token(TokenType token_type,                            // NOLINT
          std::string_view token_string,                   // NOLINT
          uint_fast32_t row, uint_fast32_t col) noexcept;  // NOLINT

    /**
     * @brief Share an immutable spelling with other tokens in a translation unit.
     * @param token_type Token category.
     * @param spelling Shared text which survives the lexer that produced the token.
     * @param row One-based source line.
     * @param col Zero-based source column.
     */
    Token(TokenType token_type, std::shared_ptr<const std::string> spelling,
          uint_fast32_t row, uint_fast32_t col) noexcept;

    /**
     * @brief Change the token type of current LexicalToken
     * @param new_type New token category; the stored spelling and source position are unchanged.
     */
    void Type(TokenType new_type) noexcept;

    /**
     * @brief Get the token type of the token
     * @return TokenType the token type of the token
     */
    [[nodiscard]] TokenType type() const noexcept;

    /**
     * @brief Get the location of the token in the source file
     * @return Stored row and column supplied at construction; the lexer uses
     * one-based rows and zero-based columns.
     */
    [[nodiscard]] Position loc() const noexcept;

    /**
     * @brief Get the human readable string of the token
     * @note The token string is the original string of the token and should not
     * be modified
     * @param escape whether to escape the ASCII Control char in the token
     * string
     * @return std::string the human readable string value of the token
     */
    [[nodiscard]] std::string to_str(bool escape = false) const noexcept;

  private:
    Position _location;
    TokenType _token_type;
    std::shared_ptr<const std::string> _token_val_ref;
};

/**
 * @brief Compare token category and source location, ignoring the stored spelling.
 * @param lhs First token to compare.
 * @param rhs Second token to compare.
 * @return True when both category and location match.
 */
inline bool operator==(const Token& lhs, const Token& rhs) {
    return lhs.type() == rhs.type() && lhs.loc() == rhs.loc();
}

}  // namespace hzcc

#endif  // HZCC_LEXICAL_LEXICAL_TOKEN_H
