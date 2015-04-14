#ifndef Z_URAL_META_MAP_HPP_INCLUDED
#define Z_URAL_META_MAP_HPP_INCLUDED

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

/** @file ural/meta/map.hpp
 @brief Ассоциативный контейнер для типов
*/
namespace ural
{
namespace meta
{
    /** @brief Пара типов
    @tparam T1 Первый тип
    @tparam T2 Второй тип
    @todo Покрыть тестами
    */
    template <class T1, class T2>
    struct pair
    {
        /// @brief Первый тип
        typedef T1 first;

        /// @brief Второй тип
        typedef T2 second;
    };

    struct make_pair
    {
        template <class T1, class T2>
        struct apply
         : declare_type<pair<T1, T2>>
        {};
    };

    /** @brief Ассоциативный массив типов
    @tparam Ts список типов
    @todo Покрыть тестами
    @todo Более строгие ограничения для Ts
    */
    template <class... Ts>
    struct map
     : public ::ural::typelist<Ts...>
    {};
}
// namespace meta
}
// namespace ural

#endif
// Z_URAL_META_MAP_HPP_INCLUDED
