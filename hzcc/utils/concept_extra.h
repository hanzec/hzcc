//
// Created by chen_ on 2022/11/20.
//

#ifndef HZCC_CONCEPT_EXTRA_H
#define HZCC_CONCEPT_EXTRA_H

template < template <typename...> class base,typename derived>
struct is_base_of_template_impl
{
    template<typename... Ts>
    static constexpr std::true_type  test(const base<Ts...> *);
    static constexpr std::false_type test(...);
    using type = decltype(test(std::declval<derived*>()));
};

template < template <typename...> class base,typename derived>
using is_base_of_template = typename is_base_of_template_impl<base,derived>::type;

#endif  // HZCC_CONCEPT_EXTRA_H
