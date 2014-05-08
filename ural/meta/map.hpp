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

/** @file ural/meta/map.hpp
 @brief Ассоциативный контейнер для типов
*/

namespace meta
{
    /** @brief Пара типов
    @tparam T1 Первый тип
    @tparam T2 Второй тип
    */
    template <class T1, class T2>
    struct pair
    {
        /// @brief Первый тип
        typedef T1 first;

        /// @brief Второй тип
        typedef T2 second;
    };

    /// @brief Ассоциативный массив типов
    template <class... Ts>
    struct map;

    /** @brief Поиск в ассоциативном массиве типов значения, соответствующего
    заданному ключу
    @tparam Container ассоциативный массив
    @tparam Key тип-ключ
    */
    template <class Container, class Key>
    struct at;

    template <class Head, class... Tail, class K>
    struct at<map<Head, Tail...>, K>
     : at<map<Tail...>, K>
    {};

    template <class V1, class... Tail, class K>
    struct at<map<pair<K, V1>, Tail...>, K>
     : ural::declare_type<V1>
    {};
}
// namespace meta

#endif
// Z_URAL_META_MAP_HPP_INCLUDED
