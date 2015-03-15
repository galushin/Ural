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
#include <ural/meta/functional.hpp>

namespace ural
{
namespace meta
{
    // all_of
    /** @brief Проверка, что все элементы контейнера удовлетворяют заданному
    предикату
    @tparam Container тип контейнера типов
    @tparam Predicate предикат над типами
    */
    template <class Container, class Predicate>
    struct all_of
     : std::integral_constant<bool, apply<Predicate, typename Container::head>::value
                                    && all_of<typename Container::tail, Predicate>::value>
    {};

    /** @brief Специализация для пустого списка типов
    @tparam Predicate предикат над типами
    */
    template <class Predicate>
    struct all_of<null_type, Predicate>
     : std::true_type
    {};

    // find
    /** @brief Поиск типа в контейнере типов
    @tparam Container контейнер типов
    @tparam T искомый тип
    @tparam Eq предикат над типами
    */
    template <class Container, class T, class Eq = meta::is_same>
    struct find
     : std::conditional<apply<Eq, typename Container::head, T>::value,
                        declare_type<Container>,
                        find<typename Container::tail, T, Eq>>::type
    {};

    template <class T, class Eq>
    struct find<null_type, T, Eq>
     : declare_type<null_type>
    {};

    template <class Container, class T, class Eq = meta::is_same>
    struct count
     : std::integral_constant<size_t, apply<Eq, T, typename Container::head>::value
                                      + count<typename Container::tail, T, Eq>::value>
    {};

    template <class T, class Eq>
    struct count<null_type, T, Eq>
     : std::integral_constant<size_t, 0>
    {};

    /** @brief Удаление последовательных дубликатов из списка типов
    @tparam List список типов
    @tparam Eq функция над типами, задающее равенство
    */
    template <class List, class Eq = meta::is_same>
    struct unique
    {
    private:
        typedef meta::not_fn<Eq> not_Eq;

        typedef typename List::head Head;
        typedef typename List::tail Tail;

        typedef typename ::ural::meta::find<Tail, Head, not_Eq>::type skip_head;
        typedef typename unique<skip_head>::type new_tail;

    public:
        /// @brief Тип-результат
        typedef typename push_front<new_tail, Head>::type type;
    };

    template <class Eq>
    struct unique<null_type, Eq>
    {
        typedef null_type type;
    };

    /** @brief Копирование списка типов в обратном порядке
    @tparam Container исходный список типов
    @tparam Out список типов в который осуществляется копирование
    */
    template <class Container, class Out = null_type>
    struct reverse_copy
     : reverse_copy<typename Container::tail,
                    typename push_front<Out, typename Container::head>::type>
    {};

    template <class Out>
    struct reverse_copy<null_type, Out>
     : declare_type<Out>
    {};

    // min_value
    /** @brief Поиск наименьшего значения
    @tparam List контейнер типов
    @tparam Compare функция сравнения
    @tparam Result тип, возвращаемый, если @c Container пуст
    @todo Значение для @c Result и @c Compare по умолчанию
    */
    template <class List, class Compare, class Result>
    struct min_value
    {
    private:
        typedef typename List::head Candidate;

        typedef typename std::conditional<apply<Compare, Candidate, Result>::value,
                                          Candidate, Result>::type new_result;

    public:
        /// @brief Тип-результат
        typedef typename min_value<typename List::tail, Compare, Result>::type
            type;
    };

    /** @brief Специализация для пустых контейнеров
    @tparam Compare функция сравнения
    @tparam Result тип, возвращаемый, если @c Container пуст
    */
    template <class Compare, class Result>
    struct min_value<null_type, Compare, Result>
     : declare_type<Result>
    {};

    // remove_first
    /** @brief Удаляет первое входждения типа в контейнер
    @tparam List контейнер типов
    @tparam Value тип, который нужно удалить
    @tparam Eq функция над типами, задающая равенство
    */
    template <class List, class Value, class Eq = meta::is_same>
    struct remove_first
     : std::conditional<apply<Eq, typename List::head, Value>::value,
                        pop_front<List>,
                        push_front<typename remove_first<typename List::tail, Value>::type,
                                   typename List::head>
                       >::type
    {};

    /** @brief специализация для пустых списков
    @tparam Value тип, который нужно удалить
    @tparam Eq функция над типами, задающая равенство
    */
    template <class Value, class Eq>
    struct remove_first<null_type, Value, Eq>
     : declare_type<null_type>
    {};

    /** @brief Удаление всех вхождений типа в контейнер типов
    @tparam Container контейнер типов
    @tparam T тип, который нужно удалить
    @tparam Eq предикат над типами, задающий равенство
    */
    template <class Container, class T, class Eq = meta::is_same>
    struct remove_all
    {
    private:
        typedef typename Container::head Head;
        typedef typename Container::tail Tail;

        typedef typename remove_all<Tail, T>::type new_tail;
        typedef meta::apply<Eq, Head, T> drop_head;
        typedef push_front<new_tail, Head> with_head;

    public:
        /// @brief Тип-результат
        typedef typename std::conditional<drop_head::value,
                                          declare_type<new_tail>,
                                          with_head>::type::type type;
    };

    template <class T, class Eq>
    struct remove_all<null_type, T, Eq>
     : declare_type<null_type>
    {};

    // Сортировка выбором
    /** @brief Сортировка выбором
    @tparam List Контейнер типов
    @tparam Compare функция сравнения
    @todo Значение @c Compare по умолчанию
    */
    template <class List, class Compare>
    struct selection_sort
    {
    private:
        typedef typename min_value<typename List::tail, Compare, typename List::head>::type new_head;
        typedef typename remove_first<List, new_head>::type without_new_head;
        typedef typename selection_sort<without_new_head, Compare>::type new_tail;

    public:
        /// @brief Тип-результат
        typedef typename push_front<new_tail, new_head>::type type;
    };

    /** @brief Специализация для пустого списка
    @tparam Compare функция сравнения
    */
    template <class Compare>
    struct selection_sort<null_type, Compare>
     : declare_type<null_type>
    {};

    /** @brief Линеаризация вложенных списков
    @tparam Container исходный список типов
    @tparam Out Список типов, к которому добавляются новые элементы
    @todo использовать в описательных статистиках (см. expand_depend_on)?
    */
    template <class Container, class Out = null_type>
    struct flatten
    {
    private:
        typedef typename front<Container>::type Head;
        typedef typename pop_front<Container>::type Tail;

        typedef std::is_same<Container, Head> is_atom;

        typedef push_front<Out, Container> R_atom;
        typedef flatten<Head, typename flatten<Tail, Out>::type> R_list;

    public:
        /// @brief Тип-результат
        typedef typename std::conditional<is_atom::value, R_atom, R_list>::type::type type;
    };

    template <class Out>
    struct flatten<null_type, Out>
     : declare_type<Out>
    {};

    //
    /** @brief Копирование без дубликатов
    @tparam Container исходный список типов
    @todo Возможность задавать функцию проверки равенства
    */
    template <class Container>
    struct copy_without_duplicates
    {
    private:
        typedef typename Container::head Head;
        typedef typename Container::tail Tail;

        typedef typename remove_all<Tail, Head>::type removed_head;
        typedef typename copy_without_duplicates<Tail>::type new_tail;

    public:
        /// @brief Тип-результат
        typedef typename push_front<new_tail, Head>::type type;
    };

    template <>
    struct copy_without_duplicates<null_type>
     : declare_type<null_type>
    {};
}
// namespace meta
}
// namespace ural

#endif
// Z_URAL_META_ALGO_HPP_INCLUDED
