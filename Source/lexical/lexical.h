//
// Created by chen_ on 2022/1/26.
//

#include <filesystem>
#include <memory>
#include <regex>
#include <unordered_set>

#include "macro.h"
#include "utils/Status.h"
#include "lexical_limit.h"
#include "lexical/LexicalToken.h"

#ifndef MYCC_LEXICAL_H
#define MYCC_LEXICAL_H

namespace Mycc::Lexical {
/**
 * @brief Parse the source code and generate a list of LexicalToken
 *
 * @param source the source code to be parsed
 * @param tokenStream the output list of parsed LexicalToken
 * @param source_name the source file name, default is ""
 * @return Status return the parsing result
 */
Status ParseToToken(std::istream& source,
                    std::vector<LexicalToken>& tokenStream,
                    const std::string& source_name = "");

/**
 * @brief  
 * 
 * @param source 
 * @param output 
 * @param source_name 
 * @return Status 
 */
Status PreprocessSource(std::istream& source, std::ostream& output,
                        const std::string& source_name = "");

}  // namespace Mycc::Lexical
#endif  // MYCC_LEXICAL_H
