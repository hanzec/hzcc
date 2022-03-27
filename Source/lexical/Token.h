#include <string>
#include <unordered_map>

#include "lexical/token_type.h"

#ifndef MYCC_LEXICAL_LEXICAL_TOKEN_H
#define MYCC_LEXICAL_LEXICAL_TOKEN_H

namespace Mycc::Lexical {
class Token {
  public:
    /**
     * @brief Copy Constructor of a new Lexical Token object this constructor
     * will do following things:
     *          - Copy the token type
     *          - Copy the token location
     *          - Increate the refence count of the cached token
     *
     * @param old the old Lexical Token object
     */
    Token(const Token& old);

    /**
     * @brief The LexicalToken constructor
     * @param token_type The token type of current LexicalToken
     * @param row row number of current LexicalToken exists in the source code
     * @param col column number of current LexicalToken exists in the source
     * code
     */
    Token(TokenType token_type, int row, int col) noexcept;

    /**
     * @brief The LexicalToken constructor
     * @param token_type The token type of current LexicalToken
     * @param row row number of current LexicalToken exists in the source code
     * @param col column number of current LexicalToken exists in the source
     * code
     */
    Token(TokenType token_type, int row, int col,
          const std::string& line) noexcept;

    /**
     * @brief The LexicalToken constructor
     * @param token_string  the string of current LexicalToken
     * @param token_type the token type of current LexicalToken
     * @param row row number of current LexicalToken exists in the source code
     * @param col column number of current LexicalToken exists in the source
     * code
     */
    Token(const std::string& token_string, TokenType token_type, int row,
          int col) noexcept;

    /**
     * @brief The LexicalToken constructor
     * @param token_string  the string of current LexicalToken
     * @param token_type the token type of current LexicalToken
     * @param row row number of current LexicalToken exists in the source code
     * @param col column number of current LexicalToken exists in the source
     * code
     */
    Token(const std::string& token_string, TokenType token_type, int row,
          int col, const std::string& line) noexcept;

    /**
     * @brief The LexicalToken Destructor
     *    This destructor will tracing the reference count of current storage
     * string in _const_value_storage, If the reference count is 0, then erase
     * the token string.
     */
    ~Token() noexcept;

    /**
     * @brief Check whether the current LexicalToken is a reserved keyword
     * @return true if the current LexicalToken is a reserved keyword, otherwise
     * false
     */
    [[nodiscard]] bool IsSymbol() const noexcept;

    /**
     * @brief Check whether the current LexicalToken is a symbol
     * @return true if the current LexicalToken is a symbol, otherwise false
     */
    [[nodiscard]] bool IsKeyword() const noexcept;

    /**
     * @brief Change the token type of current LexicalToken
     * @param token_type the new token type of current LexicalToken
     */
    void Type(TokenType new_type) noexcept;

    /**
     * @brief Get the token type of the token
     * @return TokenType the token type of the token
     */
    [[nodiscard]] TokenType Type() const noexcept;

    /**
     * @brief Get the location of the token in the source file
     * @return std::pair<int, int> the location of the token in the
     *         source file which is a three tuple variable (row, column)
     */
    [[nodiscard]] std::pair<int, int> Location() const noexcept;

    /**
     * @brief Get the cached source line of the token
     *
     * @return std::optional<std::string>
     */
    [[nodiscard]] std::string SourceLine() const noexcept;

    /**
     * @brief Get the human readable string of the token
     * @note The token string is the original string of the token and should not
     * be modified
     * @param escape whether to escape the ASCII Control char in the token
     * string
     * @return std::string the human readable string value of the token
     */
    [[nodiscard]] std::string Value(bool escape = false) const noexcept;

  private:
    TokenType _token_type;
    std::pair<int, int> _token_location;
    const std::string* _token_val_ref = nullptr;
    const std::string* _source_line_val_ref = nullptr;

    // global storage pool for all the token string
    inline static std::unordered_map<std::string, int> _const_value_storage{};
};

inline bool operator==(const Token& lhs, const Token& rhs) {
    return lhs.Type() == rhs.Type() && lhs.Location() == rhs.Location();
}

}  // namespace Mycc::Lexical

#endif  // LEXICAL_TOKEN_H