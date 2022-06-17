//
// Created by chen_ on 2022/5/11.
//
#include "TokenList.h"
namespace Hzcc::Syntax {
bool TokenList::empty() { return list::empty(); }
Lexical::Token TokenList::pop() {
    auto token = front();
    while (KeywordsUtils::IsAttribute(token.Type())) {
        DVLOG(SYNTAX_LOG_LEVEL)
            << "cached attribute: " << HZCC_PRETTY_PRINT_TOKEN(token);
        pop_front();
        _cached_attributes.push_back(token);
        token = front();
    }
    pop_front();
    DVLOG(SYNTAX_LOG_LEVEL) << "pop token: " << HZCC_PRETTY_PRINT_TOKEN(token);
    return token;
}

void TokenList::push(const Lexical::Token& token) {
    DVLOG(SYNTAX_LOG_LEVEL) << "push token: " << HZCC_PRETTY_PRINT_TOKEN(token);
    emplace_front(token);
}

Lexical::Token TokenList::peek() {
    while (KeywordsUtils::IsAttribute(front().Type())) {
        DVLOG(SYNTAX_LOG_LEVEL)
            << "cached attribute: " << HZCC_PRETTY_PRINT_TOKEN(front());
        auto token = front();
        pop_front();
        _cached_attributes.push_back(token);
    }
    return front();
}

const Lexical::Token& TokenList::peek2() const { return *(std::next(begin())); }
const Lexical::Token& TokenList::peek3() const {
    return *(std::next(std::next(begin())));
}
void TokenList::insert_front(std::_List_iterator<Hzcc::Lexical::Token> begin,
                             std::_List_iterator<Hzcc::Lexical::Token> end) {
    DVLOG(SYNTAX_LOG_LEVEL) << "insert_front: ";
    list::insert(list::begin(), begin, end);
}
std::list<Lexical::Token> TokenList::LoadCachedAttributes() {
    std::list<Lexical::Token> attributes;
    attributes.swap(_cached_attributes);
    DVLOG(SYNTAX_LOG_LEVEL) << "Reset cached attributes";
    return attributes;
}
}  // namespace Hzcc::Syntax
