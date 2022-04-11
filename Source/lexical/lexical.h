//
// Created by chen_ on 2022/1/26.
//

#ifndef MYCC_LEXICAL_H
#define MYCC_LEXICAL_H
#include <list>

#include "utils/Status.h"
namespace Mycc::Lexical {
// forward declare of Token
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

}  // namespace Mycc::Lexical
#endif  // MYCC_LEXICAL_H
