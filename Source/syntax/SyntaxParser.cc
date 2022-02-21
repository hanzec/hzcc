#include <algorithm>

#include "syntax/syntax.h"

namespace Mycc::Syntax {

Status GenertateAST(AST::ASTContext& context,
                    std::vector<Lexical::LexicalToken>& tokens) {
  using Block = struct {
    std::vector<Lexical::LexicalToken>::iterator begin;
    std::vector<Lexical::LexicalToken>::iterator end;
  };
                     
  return Status::OkStatus();
}



Status SplitToBlocks(std::vector<std::vector<Lexical::LexicalToken>>& blocks,
                     std::vector<Lexical::LexicalToken>& tokens) {
  // first reverse the tokens
  std::reverse(tokens.begin(), tokens.end());

  // then split the tokens to blocks
  for (std::vector<Lexical::LexicalToken> block; !tokens.empty();
       blocks.push_back(block)) {
    for (auto& token = tokens.back(); tokens.empty();
         tokens.pop_back()) {
      if (token.) 
    }
  }

  return Status::OkStatus();
}

}  // namespace Mycc::Syntax
