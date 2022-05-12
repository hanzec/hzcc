//
// Created by chen_ on 2022/5/11.
//
#include <list>
#include <utility>

#include "lexical/Token.h"
#include "utils/logging.h"
#ifndef FACT_CLASS_SOURCE_SYNTAX_TOKENLISTS_H_
#define FACT_CLASS_SOURCE_SYNTAX_TOKENLISTS_H_
namespace Hzcc {
namespace Lexical {
class Token;
}
namespace Syntax {
class TokenList : protected std::list<Lexical::Token> {
  public:
    using std::list<Lexical::Token>::begin;
    using std::list<Lexical::Token>::end;
    using std::list<Lexical::Token>::size;

    explicit TokenList(std::list<Lexical::Token>& list)
        : std::list<Lexical::Token>(list) {}

    bool empty();

    void push(const Lexical::Token& token);

    std::list<Lexical::Token> LoadCachedAttributes();

    Lexical::Token pop();

    [[nodiscard]] Lexical::Token peek();

    std::_List_iterator<Hzcc::Lexical::Token> insert_front(
        std::_List_iterator<Hzcc::Lexical::Token> begin,
        std::_List_iterator<Hzcc::Lexical::Token> end);

    [[nodiscard]] const Lexical::Token& peek2() const;

    [[nodiscard]] const Lexical::Token& peek3() const;

  private:
    std::list<Lexical::Token> _cached_attributes;
};

}  // namespace Syntax
}  // namespace Hzcc
#endif  // FACT_CLASS_SOURCE_SYNTAX_TOKENLISTS_H_
