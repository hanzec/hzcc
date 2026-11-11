//
// Created by chen_ on 2022/5/6.
//
#include <sstream>
#include <string>
#ifndef HZCC_SOURCE_CODEGEN_INDENT_WRITER_H
#define HZCC_SOURCE_CODEGEN_INDENT_WRITER_H
namespace hzcc::Codegen {

/**
 * @brief Accumulate assembly text with four-space indentation for each appended line.
 */
class IndentWriter {
  public:
    /**
     * @brief Increase indentation by four spaces.
     */
    void IncLindeIndent();
    /**
     * @brief Remove one four-space indentation level.
     * @pre At least one indentation level is active.
     */
    void DecLindeIndent();

    /**
     * @brief Return accumulated assembly text without clearing the buffer.
     * @return A copy of the buffered assembly text.
     */
    std::string GetAllCachedLine();

    /**
     * @brief Append one indented line to the assembly buffer.
     * @param output One unindented assembly line to append.
     */
    void AddToCache(const std::string& output);

  private:
    std::string _indent;
    std::stringstream _output;

    constexpr static const char* _indent_str = "    ";
};
}  // namespace hzcc::Codegen
#endif  // HZCC_SOURCE_CODEGEN_INDENT_WRITER_H
