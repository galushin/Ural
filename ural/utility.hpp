#ifndef Z_URAL_UTILITY_HPP_INCLUDED
#define Z_URAL_UTILITY_HPP_INCLUDED

/*  This file is part of Ural.

    Ural is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Ural is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Ural.  If not, see <http://www.gnu.org/licenses/>.
*/

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
