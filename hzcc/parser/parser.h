//
// Created by hanzech on 12/20/22.
//

#ifndef HZCC_PARSER_H
#define HZCC_PARSER_H
#include "ast/CompilationUnit.h"
#include "common/Token.h"
#include "common/name_utils.h"
#include "utils/cached_list.h"
#include "utils/status/status.h"
#include "utils/status/statusor.h"
namespace hzcc {
using TokenList = utils::CachedList<Token, std::function<bool(Token)>>;
namespace lexical {
/**
 * @brief Tokenize a seekable source stream from its beginning.
 * @param source Source stream; its position is advanced during tokenization.
 * @return Tokens on success, or the first lexical or input error.
 * @note Qualifier tokens are cached by TokenList when consumed by the parser.
 */
StatusOr<TokenList> ParseToToken(std::istream& source) noexcept;

/**
 * @brief Reserved interface for lexical preprocessing.
 * @param input Source text to preprocess.
 * @param output Destination for preprocessed text.
 * @return Preprocessing status.
 * @note This overload currently has no implementation; it is not a usable API.
 */
Status PreprocessSource(const std::istream& input, std::ostream& output);
}  // namespace lexical

namespace syntax {
/**
 * @brief Reserved interface for syntax preprocessing.
 * @param input Source text to preprocess.
 * @param output Destination for preprocessed text.
 * @return Preprocessing status.
 * @note This overload currently has no implementation; it is not a usable API.
 */
Status PreprocessSource(std::istream& input, std::ostream& output);

/**
 * @brief Consume tokens and register parsed declarations in a compilation unit.
 * @param source Token list consumed from the front as parsing progresses.
 * @param compile_unit Non-null compilation unit receiving declarations.
 * @return OK after consuming all tokens, or the first parsing error.
 * @note On error, consumed tokens and previously registered declarations are
 * not rolled back.
 */
Status GenerateAST(TokenList& source,
                   std::shared_ptr<ast::CompilationUnit> compile_unit);
}  // namespace syntax
}  // namespace hzcc
#endif  // HZCC_PARSER_H
