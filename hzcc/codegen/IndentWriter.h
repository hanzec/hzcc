//
// Created by chen_ on 2022/5/6.
//
#include <sstream>
#include <string>
#ifndef HZCC_SOURCE_CODEGEN_INDENT_WRITER_H
#define HZCC_SOURCE_CODEGEN_INDENT_WRITER_H
namespace hzcc::Codegen {

class IndentWriter {
  public:
    void IncLindeIndent();
    void DecLindeIndent();

    std::string GetAllCachedLine();

    void AddToCache(const std::string& output);

  private:
    std::string _indent;
    std::stringstream _output;

    constexpr static const char* _indent_str = "    ";
};
}  // namespace hzcc::Codegen
#endif  // HZCC_SOURCE_CODEGEN_INDENT_WRITER_H
