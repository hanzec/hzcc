#include "macro.h"
#include <optional>
#include <string>
#include <unordered_map>

#ifndef MYCC_LEXICAL_LEXICAL_TOKEN_H
#define MYCC_LEXICAL_LEXICAL_TOKEN_H

namespace Mycc::Lexical{
  class LexicalToken {
 public:
  /**
   * @brief The TokenType enum
   * @details The token type enum of the following tokens:
   *              TokenType     |     Token String     | Token Description
   *         ------------------ | -------------------- | -----------------
   *         COMMA            |     ,                 | The token of comma
   *         DOT              |     .                 | The token of dot
   *         SEMI             |     ;                 | The token of semicolon
   *         LPAR             |     (                 | The token of left
   * parenthesis RPAR             |     )                 | The token of right
   * parenthesis LBRAK            |     [                 | The token of left
   * bracket RBRAK            |     ]                 | The token of right
   * bracket LBRACE           |     {                 | The token of left brace
   *         RBRACE           |     }                 | The token of right brace
   *         GT               |     >                 | The token of greater
   * than LT               |     <                 | The token of less than
   *         ASSIGN           |     =                 | The token of assignment
   *         PLUS             |     +                 | The token of plus
   *         MINUS            |     -                 | The token of minus
   *         STAR             |     *                 | The token of star
   *         SLASH            |     /                 | The token of slash
   *         MOD              |     %                 | The token of mod
   *         COLON            |     :                 | The token of colon
   *         QUEST           |     ?                 | The token of question
   *         TILDE            |     ~                 | The token of tilde
   *         PIPE             |     |                 | The token of pipe
   *         AMP              |     &                 | The token of ampersand
   *
   *
   *
   */
  enum PACKED Type {
    END = 0,
    kAdd = '+',
    kSub = '-',
    kMul = '*',
    kDiv = '/',
    kMod = '%',
    kAssign = '=',
    kExclamation = '!',
    kLParentheses = '(',
    kRParentheses = ')',
    kLBrace = '[',
    kRBrace = ']',
    kComma = ',',
    kSemiColon = ';',
    kColon = ':',
    kDot = '.',
    kGreater = '>',
    kLess = '<',
    kLBound = '{',
    kRBound = '}',
    kQuestionMark = '?',
    kAmpersand = '&',
    kVerticalBar = '|',

    /* Stuff with attributes */

    kType = 301,
    kChar = 302,
    kInteger = 303,
    kReal_number = 304,
    kString = 305,
    kIdentity = 306,

    /* Symbols */

    kEqual = 351,
    kNotEqual = 352,
    kGreaterEqual = 353,
    kLessEqual = 354,
    kSelfIncrement = 355,
    kSelfDecrement = 356,
    kLogicalOr = 357,
    kLogicalAnd = 358,
    kLeftShift = 359,
    kRightShift = 360,
    kAddAssign = 361,
    kSubAssign = 362,
    kMulAssign = 363,
    kDivAssign = 364,

    /* Keywords */

    kConst = 401,
    kStruct = 402,
    kFor = 403,
    kWhile = 404,
    kDo = 405,
    kIf = 406,
    kElse = 407,
    kBreak = 408,
    kContinue = 409,
    kReturn = 410,
    kSwitch = 411,
    kCase = 412,
    kDefault = 413,
    kTypedef = 414,

    kUnknown = 999
  };

  /**
   * @brief Copy Constructor of a new Lexical Token object this constructor will
   *        do following things:
   *          - Copy the token type
   *          - Copy the token location
   *          - Increate the refence count of the cached token
   *
   * @param old the old Lexical Token object
   */
  LexicalToken(const LexicalToken& old);

  /**
   * @brief The LexicalToken constructor
   * @param token_type The token type of current LexicalToken
   * @param row row number of current LexicalToken exists in the source code
   * @param col column number of current LexicalToken exists in the source code
   */
  LexicalToken(Type token_type, int row, int col) noexcept;

  /**
   * @brief The LexicalToken constructor
   * @param token_type The token type of current LexicalToken
   * @param row row number of current LexicalToken exists in the source code
   * @param col column number of current LexicalToken exists in the source code
   */
  LexicalToken(Type token_type, int row, int col,
               const std::string& line) noexcept;

  /**
   * @brief The LexicalToken constructor
   * @param token_string  the string of current LexicalToken
   * @param token_type the token type of current LexicalToken
   * @param row row number of current LexicalToken exists in the source code
   * @param col column number of current LexicalToken exists in the source code
   */
  LexicalToken(const std::string& token_string, Type token_type, int row,
               int col) noexcept;

  /**
   * @brief The LexicalToken constructor
   * @param token_string  the string of current LexicalToken
   * @param token_type the token type of current LexicalToken
   * @param row row number of current LexicalToken exists in the source code
   * @param col column number of current LexicalToken exists in the source code
   */
  LexicalToken(const std::string& token_string, Type token_type, int row,
               int col, const std::string& line) noexcept;

  /**
   * @brief The LexicalToken Destructor
   *    This destructor will tracing the reference count of current storage
   * string in _const_value_storage, If the reference count is 0, then erase the
   * token string.
   */
  ~LexicalToken() noexcept;

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
  void TokenType(Type new_type) noexcept;

  /**
   * @brief Get the token type of the token
   * @return TokenType the token type of the token
   */
  [[nodiscard]] Type TokenType() const noexcept;

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
  [[nodiscard]] std::optional<std::string> SourceLine() const noexcept;

  /**
   * @brief Get the human readable string of the token
   * @note The token string is the original string of the token and should not
   * be modified
   * @param escape whether to escape the ASCII Control char in the token string
   * @return std::string the human readable string value of the token
   */
  [[nodiscard]] std::string Value(bool escape = false) const noexcept;

 private:
  Type _token_type;
  std::pair<int, int> _token_location;
  const std::string* _token_val_ref = nullptr;
  const std::string* _source_line_val_ref = nullptr;

  // global storage pool for all the token string
  inline static std::unordered_map<std::string, int> _const_value_storage{};
};

}

#endif  // LEXICAL_TOKEN_H