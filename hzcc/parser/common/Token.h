#include <stdint.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <string_view>
#include <utility>

#include "macro.h"
#include "token_type.h"
#ifndef HZCC_LEXICAL_LEXICAL_TOKEN_H
#define HZCC_LEXICAL_LEXICAL_TOKEN_H

namespace hzcc {
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
     * @brief Change the token type of current LexicalToken
     * @param token_type the new token type of current LexicalToken
     */
    void Type(TokenType new_type) noexcept;

    /**
     * @brief Get the token type of the token
     * @return TokenType the token type of the token
     */
    [[nodiscard]] TokenType type() const noexcept;

    /**
     * @brief Get the location of the token in the source file
     * @return std::pair<int, int> the location of the token in the
     *         source file which is a three tuple variable (row, column)
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
    std::shared_ptr<std::string> _token_val_ref = nullptr;

    // global storage pool for all the token string
    inline static std::unordered_map<std::string_view,
                                     std::shared_ptr<std::string>>
        _global_val_storage{};
};

inline bool operator==(const Token& lhs, const Token& rhs) {
    return lhs.type() == rhs.type() && lhs.loc() == rhs.loc();
}

}  // namespace hzcc

#endif  // HZCC_LEXICAL_LEXICAL_TOKEN_H
