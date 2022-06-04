//
// Created by chen_ on 2022/1/26.
//
#include <filesystem>
#include <list>

#include "utils/Status.h"
#ifndef HZCC_LEXICAL_LEXICAL_H
#define HZCC_LEXICAL_LEXICAL_H
namespace Hzcc::Lexical {
class Token;

/**
 * @brief Parse the source code and generate a list of LexicalToken
 *
 * @param source the source code to be parsed
 * @param tokenStream the output list of parsed LexicalToken
 * @param source_name the source file name, default is ""
 * @return Status return the parsing result
 */
Status ParseToToken(std::istream& source, std::list<Token>& tokenStream);

/**
 * @brief Parse the source code and generate a list of LexicalToken
 *
 * @param source the source code to be parsed
 * @param tokenStream the output list of parsed LexicalToken
 * @param source_name the source file name, default is ""
 * @return Status return the parsing result
 */
Status PreprocessSource(const std::filesystem::path& input,
                        std::ostream& output);
}  // namespace Hzcc::Lexical
#endif  // HZCC_LEXICAL_LEXICAL_H
