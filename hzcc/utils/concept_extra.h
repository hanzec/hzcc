//
// Created by chen_ on 2022/11/20.
//

#ifndef HZCC_CONCEPT_EXTRA_H
#define HZCC_CONCEPT_EXTRA_H

/** @brief Detect inheritance from any specialization of a class template.
 * @tparam base Class template whose specializations are accepted.
 * @tparam derived Candidate derived type tested through pointer conversion.
 */
template < template <typename...> class base,typename derived>
struct is_base_of_template_impl
{
    /// @brief Select true_type when a pointer converts to a base specialization.
    template<typename... Ts>
    static constexpr std::true_type  test(const base<Ts...> *);
    /// @brief Select false_type when no base-specialization conversion matches.
    static constexpr std::false_type test(...);
    using type = decltype(test(std::declval<derived*>()));
};

template < template <typename...> class base,typename derived>
using is_base_of_template = typename is_base_of_template_impl<base,derived>::type;

#endif  // HZCC_CONCEPT_EXTRA_H
