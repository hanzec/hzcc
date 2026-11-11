//
// Created by chen_ on 2022/5/6.
//

#include "IndentWriter.h"

#include <cstring>
namespace Hzcc::Codegen {

/**
 * @brief Return accumulated assembly text without clearing the buffer.
 * @return A copy of the buffered assembly text.
 */
std::string IndentWriter::GetAllCachedLine() { return _output.str(); }

/**
 * @brief Increase indentation by four spaces.
 */
void IndentWriter::IncLindeIndent() { _indent += _indent_str; }

/**
 * @brief Remove one four-space indentation level.
 * @pre At least one indentation level is active.
 */
void IndentWriter::DecLindeIndent() {
    _indent = _indent.substr(0, _indent.length() - 4);
}

/**
 * @brief Append one indented line to the assembly buffer.
 * @param output One unindented assembly line to append.
 */
void IndentWriter::AddToCache(const std::string& output) {
    _output << _indent << output << std::endl;
}

}  // namespace Hzcc::Codegen
