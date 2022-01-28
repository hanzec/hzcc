//
// Created by chen_ on 2022/1/26.
//

#include "lexical.h"

namespace Mycc::Lexical {
static std::vector<LexicalToken> ParseToToken(const std::filesystem::path& source) {

  if(!std::filesystem::exists(source)) {
    throw std::runtime_error("file not found");
  }

  return {};
}
}  // namespace Mycc::Lexical
