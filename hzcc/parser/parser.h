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
 * @brief Parse the source code and generate a list of LexicalToken
 *
 * @param source the source code to be parsed
 * @param source_name the source file to_str
 * @return StatusOr<std::list<Token>> the list of LexicalToken or the error
 */
StatusOr<TokenList> ParseToToken(std::istream& source) noexcept;

/**
 * @brief Parse the source code and generate a list of LexicalToken
 *
 * @param source the source code to be parsed
 * @param tokenStream the output list of parsed LexicalToken
 * @param source_name the source file to_str, default is ""
 * @return Status return the parsing result
 */
Status PreprocessSource(const std::istream& input, std::ostream& output);
}  // namespace lexical

namespace syntax {
/**
 * @brief Preprocess the source code
 * @param input
 * @param output
 * @return
 */
Status PreprocessSource(std::istream& input, std::ostream& output);

/**
 * @brief Generate the ast from the source code
 * @param source the source code to be parsed
 */
Status GenerateAST(TokenList& source,
                   std::shared_ptr<ast::CompilationUnit> compile_unit);
}  // namespace syntax
}  // namespace hzcc
#endif  // HZCC_PARSER_H
