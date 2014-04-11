#ifndef Z_URAL_TUPLE_HPP_INCLUDED
#define Z_URAL_TUPLE_HPP_INCLUDED

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

/** @file ural/tuple.hpp
 @brief Кортежи --- гетерогенный контейнер фиксированного на этапе компиляции
 размера.
*/

#include <tuple>

#include <ural/placeholders.hpp>

namespace ural
{
    /** @brief Кортеж
    @tparam Ts типы элементов кортежа
    */
    template <class... Ts>
    class tuple
     : public std::tuple<Ts...>
    {
        typedef std::tuple<Ts...> Base;
    public:
        /** Инициализирует каждый элемент с помощью соответствующих
        конструкторов без аргументов.
        @brief Конструктор без аргументов
        */
        constexpr tuple()
         : Base{}
        {}

        /** Инициализирует каждый элемент с помощью конструктора копирования
        на основе соответсвующего аргумента из @c args
        @brief Конструктор
        @param args аргументы
        */
        constexpr explicit tuple(Ts const & ... args)
         : Base(args...)
        {}

        template <class... Us>
        constexpr explicit tuple(Us &&... args)
         : Base(std::forward<Us>(args)...)
        {}

        //@{
        /** @brief Доступ к элементам по "статическому индексу"
        @tparam Index номер элемента
        @return Ссылка на элемент с номером @c Index
        */
        template <size_t Index>
        constexpr typename std::tuple_element<Index, Base>::type const &
        operator[](ural::placeholder<Index>) const
        {
            return std::get<Index>(*this);
        }

        template <size_t Index>
        typename std::tuple_element<Index, Base>::type &
        operator[](ural::placeholder<Index>)
        {
            return std::get<Index>(*this);
        }
        //@}
    };

    template <class... Args>
    constexpr tuple<Args && ...>
    forward_as_tuple(Args &&... args) noexcept
    {
        return tuple<Args && ...>(std::forward<Args>(args)...);
    }


namespace details
{
    template <class Tuple, class UnaryPredicate, size_t Index>
    bool any_of(Tuple const &, UnaryPredicate,
                placeholder<Index>, placeholder<Index>)
    {
        return false;
    }

    template <class Tuple, class UnaryPredicate, size_t First, size_t Last>
    bool any_of(Tuple const & x, UnaryPredicate pred,
                placeholder<First>,
                placeholder<Last> last)
    {
        return pred(std::get<First>(x))
             || ::ural::details::any_of(x, std::move(pred),
                                        placeholder<First+1>{}, last);
    }
}
// namespace details
    template <class Tuple, class UnaryPredicate>
    typename std::enable_if<(std::tuple_size<Tuple>::value > 0), bool>::type
    any_of(Tuple const & x, UnaryPredicate pred)
    {
        return ural::details::any_of(x, pred, placeholder<0>{},
                                     placeholder<std::tuple_size<Tuple>::value>{});
    }
}
// namespace ural

namespace std
{
    template <class... Ts>
    struct tuple_size<ural::tuple<Ts...>>
     : std::tuple_size<std::tuple<Ts...>>
    {};

    template <size_t I, class... Ts>
    struct tuple_element<I, ural::tuple<Ts...>>
     : std::tuple_element<I, std::tuple<Ts...>>
    {};
}
// namespace std

#endif
// Z_URAL_TUPLE_HPP_INCLUDED
