//
// Created by chen_ on 2022/12/17.
//
#ifndef HZCC_COMMON_KEYWORDS_H
#define HZCC_COMMON_KEYWORDS_H
#include <array>
#include <magic_enum.hpp>

#include "macro.h"
#include "token_type.h"
namespace hzcc::parser_common {
/**
 * Number of special symbols in C language.
 */
static constexpr const int kSingleCharSymbolTableSize = 23;

/**
 * Special Symbol's table.
 */
constexpr std::array<const char, kSingleCharSymbolTableSize> kSingleCharSymbol{
    '+', '-', '*', '/', '%', '&', '|', '!', '=', '.', ',', '~',
    '<', '>', '?', ':', ';', '{', '}', '(', ')', '[', ']'};
}  // namespace hzcc::parser_common

#endif  // HZCC_COMMON_KEYWORDS_H
