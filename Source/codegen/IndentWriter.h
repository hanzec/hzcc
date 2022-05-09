//
// Created by chen_ on 2022/5/6.
//
#include <sstream>
#include <string>
#ifndef MYCC_SOURCE_CODEGEN_INDENTWRITYER_H_
#define MYCC_SOURCE_CODEGEN_INDENTWRITYER_H_
namespace Hzcc::Codegen {

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
}  // namespace Hzcc::Codegen
#endif  // MYCC_SOURCE_CODEGEN_INDENTWRITYER_H_
