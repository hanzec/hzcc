//
// Created by chen_ on 2022/5/11.
//
#include <list>
#include <utility>
#ifndef HZCC_SYNTAX_TOKEN_LIST_H
#define HZCC_SYNTAX_TOKEN_LIST_H
namespace hzcc::utils {
template <class T,                 // NOLINT
          typename CacheFilter,    // NOLINT
          std::enable_if_t<        // NOLINT
              std::conjunction_v<  // NOLINT
                  std::is_same<bool, std::invoke_result_t<CacheFilter, T>>>,
              int> = 0>
class CachedList : protected std::list<T> {
  public:
    using std::list<T>::end;
    using std::list<T>::size;
    using std::list<T>::begin;
    using std::list<T>::empty;
    using std::list<T>::emplace_back;

    explicit CachedList(CacheFilter filter) : _filter(std::move(filter)) {}

    /**
     * @brief construct a new Token List object with a list of tokens
     * @param list the list of tokens
     */
    explicit CachedList(CacheFilter filter, std::list<T>& list) noexcept
        : std::list<T>(list), CachedList(filter) {}

    T pop() {
        auto token = this->front();
        while (_filter(token)) {
            this->pop_front();
            _cached_attributes.push_back(token);
            token = this->front();
        }
        this->pop_front();
        return token;
    }

    template <class... Args,
              std::enable_if_t<std::is_constructible_v<T, Args&&...>, int> = 0>
    void push(Args... args) {
        emplace_front(args...);
    }

    /**
     * @brief peek the next token
     * @return the next token
     */
    [[nodiscard]] T peek() {
        while (_filter(this->front())) {
            auto token = this->front();
            this->pop_front();
            _cached_attributes.push_back(token);
        }
        return this->front();
    };

    /**
     * @brief peek the second token
     * @return the second token
     */
    [[nodiscard]] const T& peek2() const { return *(std::next(begin())); }

    /**
     * @brief peek the third token
     * @return the third token
     */
    [[nodiscard]] const T& peek3() const {
        return *(std::next(std::next(begin())));
    };

    std::list<T> cache_attr_list() {
        std::list<T> attributes;
        attributes.swap(_cached_attributes);
        return attributes;
    }

  private:
    CacheFilter _filter;
    std::list<T> _cached_attributes;
};
}  // namespace hzcc::utils
#endif  // HZCC_SYNTAX_TOKEN_LIST_H
