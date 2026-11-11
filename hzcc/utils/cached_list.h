//
// Created by chen_ on 2022/5/11.
//
#include <cstddef>
#include <list>
#include <stdexcept>
#include <type_traits>
#include <utility>
#ifndef HZCC_SYNTAX_TOKEN_LIST_H
#define HZCC_SYNTAX_TOKEN_LIST_H
namespace hzcc::utils {
/**
 * @brief A token queue that caches filtered tokens when consuming an item.
 * Lookahead counts only unfiltered tokens and never consumes or caches them.
 * size(), empty(), and iteration describe the underlying, unconsumed list,
 * including filtered tokens. A list containing only filtered tokens therefore
 * has no lookahead value even though empty() returns false.
 */
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

    /** @brief Create an empty queue.
     * @param filter Predicate identifying tokens to cache during pop().
     */
    explicit CachedList(CacheFilter filter) : _filter(std::move(filter)) {}

    /**
     * @brief Copy an existing token list and retain the supplied filter.
     * @param filter Predicate identifying tokens to cache instead of return.
     * @param list Tokens to copy; the original list is not consumed.
     */
    explicit CachedList(CacheFilter filter, std::list<T>& list)
        : std::list<T>(list), _filter(std::move(filter)) {}

    /**
     * @brief Consume the next unfiltered token, caching preceding filtered ones.
     * @return The consumed token.
     * @throws std::out_of_range If no unfiltered token remains; the list and
     * cache are unchanged in that case.
     */
    T pop() {
        auto next = visible_at(0);
        T token = *next;
        // Locate and copy the result before mutating either list. Splicing
        // preserves qualifier order without allocating or copying cached items.
        _cached_attributes.splice(_cached_attributes.end(), *this, this->cbegin(), next);
        this->erase(next);
        return token;
    }

    /** @brief Prepend a token without altering previously cached qualifiers.
     * @param args Arguments used to construct the token at the front.
     */
    template <class... Args,
              std::enable_if_t<std::is_constructible_v<T, Args&&...>, int> = 0>
    void push(Args... args) {
        this->emplace_front(args...);
    }

    /**
     * @brief Copy the next unfiltered token without consuming it.
     * @throws std::out_of_range If no unfiltered token remains.
     */
    [[nodiscard]] T peek() const { return *visible_at(0); }

    /**
     * @brief Reference the second unfiltered token without consuming it.
     * @throws std::out_of_range If fewer than two unfiltered tokens remain.
     */
    [[nodiscard]] const T& peek2() const { return *visible_at(1); }

    /**
     * @brief Reference the third unfiltered token without consuming it.
     * @throws std::out_of_range If fewer than three unfiltered tokens remain.
     */
    [[nodiscard]] const T& peek3() const { return *visible_at(2); }

    /**
     * @brief Transfer all cached tokens to the caller in consumption order.
     * @return Cached tokens; the internal cache is empty after this call.
     */
    std::list<T> cache_attr_list() {
        std::list<T> attributes;
        attributes.swap(_cached_attributes);
        return attributes;
    }

  private:
    /** @brief Locate an unfiltered token without changing either list.
     * @param index Zero-based position among unfiltered tokens.
     * @return Iterator to the requested token.
     * @throws std::out_of_range If fewer visible tokens remain.
     */
    typename std::list<T>::const_iterator visible_at(std::size_t index) const {
        for (auto it = this->cbegin(); it != this->cend(); ++it) {
            if (!_filter(*it)) {
                if (index == 0) {
                    return it;
                }
                --index;
            }
        }
        throw std::out_of_range("CachedList: insufficient unfiltered tokens");
    }

    CacheFilter _filter;
    std::list<T> _cached_attributes;
};
}  // namespace hzcc::utils
#endif  // HZCC_SYNTAX_TOKEN_LIST_H
