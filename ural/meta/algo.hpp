#ifndef Z_URAL_META_ALGO_HPP_INCLUDED
#define Z_URAL_META_ALGO_HPP_INCLUDED

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

/** @file ural/meta/algo.hpp
 @brief Алгоритмы для списков типов
*/

#include <ural/meta/list.hpp>

namespace ural
{
namespace meta
{
    // @todo Обобщить
    template <class T1, class T2>
    struct is_not_same
     : std::integral_constant<bool, !std::is_same<T1, T2>::value>
    {};

    // Удаление последовательных дубликатов
    template <class List>
    struct unique;

    template <>
    struct unique<null_type>
    {
        typedef null_type type;
    };

    template <class Head, class Tail>
    struct unique<list<Head, Tail>>
    {
    private:
        typedef typename ::ural::meta::find<Tail, Head, is_not_same>::type skip_head;
        typedef typename unique<skip_head>::type new_tail;

    public:
        typedef list<Head, new_tail> type;
    };
}
// namespace meta
}
// namespace ural

#endif
// Z_URAL_META_ALGO_HPP_INCLUDED
