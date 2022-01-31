//
// Created by chen_ on 2022/1/26.
//

#include "lexical.h"
#include "utils/Status.h"
#include "utils/fs_utils.h"
namespace Mycc::Lexical {
static Status ParseToToken(const std::filesystem::path& source, std::vector<LexicalToken> &token_stream) {

  // check file exist
  if(!std::filesystem::exists(source)) {
    return {Status::INVALID_ARGUMENT, "file not found"};
  }

  // check file is regular file and readable
  if(!std::filesystem::is_regular_file(source) || !is_readable(source)) {
    return {Status::INVALID_ARGUMENT, "file is not regular file or not readable"};
  }

  // if
  return Status::OkStatus();
}
}  // namespace Mycc::Lexical
