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
    struct unique
    {
    private:
        typedef typename List::head Head;
        typedef typename List::tail Tail;

        typedef typename ::ural::meta::find<Tail, Head, is_not_same>::type skip_head;
        typedef typename unique<skip_head>::type new_tail;

    public:
        typedef typename push_front<Head, new_tail>::type type;
    };

    template <>
    struct unique<null_type>
    {
        typedef null_type type;
    };

    // Обращение
    template <class Container, class Out>
    struct reverse_copy
     : reverse_copy<typename Container::tail,
                    typename push_front<typename Container::head, Out>::type>
    {};

    template <class Out>
    struct reverse_copy<null_type, Out>
     : declare_type<Out>
    {};

    // Сортировка выбором
    /** @brief Сортировка выбором
    @tparam List Контейнер типов
    @tparam Compare функция сравнения
    */
    template <class List, template <class, class> class Compare>
    struct selection_sort
    {
    private:
        typedef typename min_value<typename List::tail, Compare, typename List::head>::type new_head;
        typedef typename remove_first<List, new_head>::type without_new_head;
        typedef typename selection_sort<without_new_head, Compare>::type new_tail;

    public:
        /// @brief Тип-результат
        typedef typename push_front<new_head, new_tail>::type type;
    };

    /** @brief Специализация для пустого списка
    @tparam Compare функция сравнения
    */
    template <template <class, class> class Compare>
    struct selection_sort<null_type, Compare>
     : declare_type<null_type>
    {};
}
// namespace meta
}
// namespace ural

#endif
// Z_URAL_META_ALGO_HPP_INCLUDED
