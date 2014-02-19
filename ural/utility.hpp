#ifndef Z_URAL_UTILITY_HPP_INCLUDED
#define Z_URAL_UTILITY_HPP_INCLUDED

#include <ural/defs.hpp>

namespace ural
{
    template <class T, T... Ints>
    class integer_sequence
    {};

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
}
// namespace ural

#endif
// Z_URAL_UTILITY_HPP_INCLUDED
