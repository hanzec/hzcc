//
// Created by chen_ on 2022/5/11.
//
#include <list>
#include <utility>

#include "lexical/Token.h"
#include "utils/logging.h"

#ifndef HZCC_SYNTAX_TOKEN_LIST_H
#define HZCC_SYNTAX_TOKEN_LIST_H
namespace Hzcc {
namespace Lexical {
class Token;
}
namespace Syntax {
class TokenList : protected std::list<Lexical::Token> {
  public:
    /**
     * @brief expose the iterator of the list
     */
    using std::list<Lexical::Token>::end;

    /**
     * @brief expose the iterator of the list
     */
    using std::list<Lexical::Token>::size;

    /**
     * @brief expose the iterator of the list
     */
    using std::list<Lexical::Token>::begin;

    explicit TokenList(std::list<Lexical::Token>& list)
        : std::list<Lexical::Token>(list) {}

    bool empty();

    void push(const Lexical::Token& token);

    std::list<Lexical::Token> LoadCachedAttributes();

    Lexical::Token pop();

    [[nodiscard]] Lexical::Token peek();

    void insert_front(std::_List_iterator<Hzcc::Lexical::Token> begin,
                      std::_List_iterator<Hzcc::Lexical::Token> end);

    [[nodiscard]] const Lexical::Token& peek2() const;

    [[nodiscard]] const Lexical::Token& peek3() const;

  private:
    std::list<Lexical::Token> _cached_attributes;
};

}  // namespace Syntax
}  // namespace Hzcc
#endif  // HZCC_SYNTAX_TOKEN_LIST_H
