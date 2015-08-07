#ifndef Z_URAL_SEQUENCE_MAP_HPP_INCLUDED
#define Z_URAL_SEQUENCE_MAP_HPP_INCLUDED

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

/** @file ural/sequence/map.hpp
 @brief Адапторы последовательностей для ассоциативных контейнеров
*/

#include <ural/sequence/adaptors/transform.hpp>
#include <ural/utility.hpp>
#include <utility>

namespace ural
{
    /** @brief Тип-тэг для создания последовательностей ключей и отображаемых
    значений ассоциативного контейнера
    @tparam Index индекс последовательности: 0 --- ключи, 1 --- отображаемые
    значения
    */
    template <size_t Index>
    struct map_sequence_helper{};

    /** @brief Создание последовательностей ключей или отображаемых значений
    отображений ассоциативного контейнера
    @tparam Index индекс последовательности: 0 --- ключи, 1 --- отображаемые
    значения

    @param seq (под)последовательность элементов ассоциативного контейнера
    */
    template <class Sequence, size_t Index>
    auto operator|(Sequence && seq, map_sequence_helper<Index>)
    -> decltype(make_transform_sequence(tuple_get<Index>{}, std::forward<Sequence>(seq)))
    {
        return make_transform_sequence(tuple_get<Index>{}, std::forward<Sequence>(seq));
    }

    namespace
    {
        /** @brief Объект-тэг для создания последовательности ключей ассоциативного
        контейнера */
        constexpr auto & map_keys = odr_const<map_sequence_helper<0>>;

        /** @brief Объект-тэг для создания последовательности отоброжаемых значений
        ассоциативного контейнера */
        constexpr auto & map_values = odr_const<map_sequence_helper<1>>;
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_MAP_HPP_INCLUDED
