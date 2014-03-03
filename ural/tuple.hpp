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
}
// namespace ural

#endif
// Z_URAL_TUPLE_HPP_INCLUDED
