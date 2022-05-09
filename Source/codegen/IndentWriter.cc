//
// Created by chen_ on 2022/5/6.
//

#include "IndentWriter.h"

#include <cstring>
namespace Hzcc::Codegen {

std::string IndentWriter::GetAllCachedLine() { return _output.str(); }

void IndentWriter::IncLindeIndent() { _indent += _indent_str; }

void IndentWriter::DecLindeIndent() {
    _indent = _indent.substr(0, _indent.length() - 4);
}

void IndentWriter::AddToCache(const std::string& output) {
    _output << _indent << output << std::endl;
}

}  // namespace Hzcc::Codegen