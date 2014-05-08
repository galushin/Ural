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
 @todo Обобщить алгоритмы на любые контейнеры типов
*/

#include <ural/type_traits.hpp>
#include <ural/defs.hpp>

namespace ural
{
namespace meta
{
    // Список
    /** @brief Список типов
    @tparam Head первый элемент списка
    @tparam Tail хвост списка --- список остальных элементов или @c null_type
    */
    template <class Head, class Tail>
    struct list
    {
        /// @brief Голова списка типов --- первый элемент.
        typedef Head head;

        /// @brief Хвост списка типов --- остальные элементы или @c null_type
        typedef Tail tail;
    };

    /** @brief Класс-характеристика для создания <tt> meta::list </tt> по
    переменному количеству типов-аргументов
    @tparam Types типы
    */
    template <class... Types>
    struct make_list;

    /// @brief Специализация для пустого списка
    template <>
    struct make_list<>
     : declare_type<null_type>
    {};

    /** @brief Специализация для непустого списка
    @tparam T1 первый тип
    @tparam Ts остальные типы
    */
    template <class T1, class... Ts>
    struct make_list<T1, Ts...>
     : declare_type<list<T1, typename make_list<Ts...>::type>>
    {};

    // at
    template <class Container, size_t Index>
    struct at
     : at<typename Container::tail, Index - 1>
    {};

    template <class Container>
    struct at<Container, 0u>
     : declare_type<typename Container::head>
    {};

    // Алгоритмы
    // all_of
    template <class Container, template <class> class Predicate>
    struct all_of;

    template <template <class> class Predicate>
    struct all_of<null_type, Predicate>
     : std::true_type
    {};

    template <class Head, class Tail, template <class> class Predicate>
    struct all_of<list<Head, Tail>, Predicate>
     : std::integral_constant<bool, Predicate<Head>::value && all_of<Tail, Predicate>::value>
    {};

    // find
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

    // Копирование без дубликатов
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
        /// @brief Тип-результат
        typedef typename copy_without_duplicates<Tail, new_out>::type type;
    };

    // min_value
    template <class List, template <class, class> class Compare, class Result>
    struct min_value
    {
    private:
        typedef typename List::head Candidate;

        typedef typename std::conditional<Compare<Candidate, Result>::value,
                                          Candidate, Result>::type new_result;

    public:
        /// @brief Тип-результат
        typedef typename min_value<typename List::tail, Compare, Result>::type
            type;
    };

    template <template <class, class> class Compare, class Result>
    struct min_value<null_type, Compare, Result>
     : declare_type<Result>
    {};

    // remove_first
    template <class List, class Value>
    struct remove_first;

    template <class Value>
    struct remove_first<null_type, Value>
     : declare_type<null_type>
    {};

    template <class Head, class Tail>
    struct remove_first<list<Head, Tail>, Head>
     : declare_type<Tail>
    {};

    template <class Head, class Tail, class Value>
    struct remove_first<list<Head, Tail>, Value>
     : list<Head, typename remove_first<Tail, Value>::type>
    {};

    // Сортировка выбором
    template <class List, template <class, class> class Compare>
    struct selection_sort
    {
    private:
        typedef typename min_value<typename List::tail, Compare, typename List::head>::type new_head;
        typedef typename remove_first<List, new_head>::type without_new_head;
        typedef typename selection_sort<without_new_head, Compare>::type new_tail;

    public:
        /// @brief Тип-результат
        typedef list<new_head, new_tail> type;
    };

    template <template <class, class> class Compare>
    struct selection_sort<null_type, Compare>
     : declare_type<null_type>
    {};
}
// namespace meta
}
// namespace ural

#endif
// Z_URAL_META_LIST_HPP_INCLUDED
