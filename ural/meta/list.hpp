#ifndef Z_URAL_META_LIST_HPP_INCLUDED
#define Z_URAL_META_LIST_HPP_INCLUDED

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

/** @file ural/meta/list.hpp
 @brief Спиоск типов
*/

namespace ural
{
namespace meta
{
    template <class Head, class Tail>
    struct list
    {
        typedef Head head;
        typedef Tail tail;
    };

    template <class... Types>
    struct make_list;

    template <>
    struct make_list<>
     : declare_type<null_type>
    {};

    template <class T1, class... Ts>
    struct make_list<T1, Ts...>
     : declare_type<list<T1, typename make_list<Ts...>::type>>
    {};

    template <class Container, class T>
    struct find;

    template <class T>
    struct find<null_type, T>
     : declare_type<null_type>
    {};

    template <class Head, class Tail>
    struct find<list<Head, Tail>, Head>
     : declare_type<list<Head, Tail>>
    {};

    template <class Head, class Tail, class T>
    struct find<list<Head, Tail>, T>
     : find<Tail, T>
    {};

    template <class Container, class Out = null_type>
    struct copy_without_duplicates;

    template <class Out>
    struct copy_without_duplicates<null_type, Out>
     : declare_type<Out>
    {};

    template <class Head, class Tail, class Out>
    struct copy_without_duplicates<list<Head, Tail>, Out>
    {
    private:
        typedef typename find<Out, Head>::type Pos;

        typedef typename std::conditional<std::is_same<Pos, null_type>::value,
                                          list<Head, Out>, Out>::type new_out;

    public:
        typedef typename copy_without_duplicates<Tail, new_out>::type type;
    };
}
// namespace meta
}
// namespace ural

#endif
// Z_URAL_META_LIST_HPP_INCLUDED
