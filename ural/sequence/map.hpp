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
 @todo Унифицировать @c map_keys_functor и @c map_values_functor
*/

#include <utility>

namespace ural
{
    /** @brief Функциональный объект, извлекающий первый компонент пары
    */
    struct map_keys_functor
    {
    public:
        //@{
        /** @brief Оператор применения
        @param x пара
        @return ссылка на первый компонент пары
        */
        template <class T1, class T2>
        constexpr T1 & operator()(std::pair<T1, T2> & x) const
        {
            return x.first;
        }

        template <class T1, class T2>
        constexpr T1 const & operator()(std::pair<T1, T2> const & x) const
        {
            return x.first;
        }
        //@}
    };

    struct map_keys_helper{};

    template <class Sequence>
    auto operator|(Sequence && seq, map_keys_helper)
    -> decltype(make_transform_sequence(map_keys_functor{}, std::forward<Sequence>(seq)))
    {
        return make_transform_sequence(map_keys_functor{}, std::forward<Sequence>(seq));
    }

    constexpr map_keys_helper map_keys{};

    /** @brief Функциональный объект, извлекающий второй компонент пары
    */
    struct map_values_functor
    {
    public:
        //@{
        /** @brief Оператор применения
        @param x пара
        @return ссылка на первый компонент пары
        */
        template <class T1, class T2>
        constexpr T2 & operator()(std::pair<T1, T2> & x) const
        {
            return x.second;
        }

        template <class T1, class T2>
        constexpr T2 const & operator()(std::pair<T1, T2> const & x) const
        {
            return x.second;
        }
        //@}
    };

    struct map_values_helper{};

    template <class Sequence>
    auto operator|(Sequence && seq, map_values_helper)
    -> decltype(make_transform_sequence(map_values_functor{}, std::forward<Sequence>(seq)))
    {
        return make_transform_sequence(map_values_functor{}, std::forward<Sequence>(seq));
    }

    constexpr map_values_helper map_values{};
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_MAP_HPP_INCLUDED
