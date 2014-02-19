#ifndef Z_URAL_UTILITY_HPP_INCLUDED
#define Z_URAL_UTILITY_HPP_INCLUDED

#include <ural/defs.hpp>
#include <ural/type_traits.hpp>

namespace ural
{
    template <class T, T... Ints>
    struct integer_sequence
    {
        typedef T value_type;

        static constexpr std::size_t size() noexcept
        {
            return sizeof...(Ints);
        }
    };

    template<std::size_t... Ints>
    using index_sequence = integer_sequence<std::size_t, Ints...>;

    template <class T, T first, T last, T... Rs>
    struct make_int_sequence_helper
     : make_int_sequence_helper<T, first+1, last, Rs..., first>
    {};

    template <class T, T first, T... Rs>
    struct make_int_sequence_helper<T, first, first, Rs...>
     : declare_type<integer_sequence<T, Rs...> >
    {};

    template<class T, T N>
    using make_integer_sequence = typename make_int_sequence_helper<T, 0, N>::type;

    template<std::size_t N>
    using make_index_sequence = make_integer_sequence<std::size_t, N>;

    template<class... T>
    using index_sequence_for = make_index_sequence<sizeof...(T)>;

    template <typename F, typename Tuple, size_t... I>
    auto apply_impl(F&& f, Tuple&& t, index_sequence<I...>)
        -> decltype(std::forward<F>(f)(std::get<I>(std::forward<Tuple>(t))...))
    {
        return std::forward<F>(f)(std::get<I>(std::forward<Tuple>(t))...);
    }

    template <typename F, typename Tuple>
    auto apply(F&& f, Tuple&& t)
    -> decltype(apply_impl(std::forward<F>(f), std::forward<Tuple>(t),
                           make_index_sequence<std::tuple_size<decay_t<Tuple>>::value>{}))
    {
        using Indices = make_index_sequence<std::tuple_size<decay_t<Tuple>>::value>;
        return apply_impl(std::forward<F>(f), std::forward<Tuple>(t), Indices{});
    }
}
// namespace ural

#endif
// Z_URAL_UTILITY_HPP_INCLUDED
