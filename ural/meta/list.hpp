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

#include <cstddef>

namespace ural
{
namespace meta
{
    // Вставка нового элемента в начало
    template <class Container, class Value>
    struct push_front;

    template <class Value, template<class...> class Container, class... Args>
    struct push_front<Container<Args...>, Value>
     : declare_type<Container<Value, Args...>>
    {};

    // Удаление первого элемента
    template <class Container>
    struct pop_front
    {
    private:
        template <class U>
        static null_type pop_front_helper(...);

        template <class U>
        static typename U::tail
        pop_front_helper(void*);

    public:
        typedef decltype(pop_front_helper<Container>(nullptr)) type;
    };

    // Первый элемент
    template <class Container>
    struct front
    {
    private:
        template <class U>
        static U front_helper(...);

        template <class U>
        static typename U::head
        front_helper(void*);

    public:
        typedef decltype(front_helper<Container>(nullptr)) type;
    };

    // at
    /** @brief Доступ по индексу
    @tparam Container тип контейнера типов
    @tparam Index номер элемента
    @pre @c Index меньше количества элементов в контейнере
    */
    template <class Container, size_t Index>
    struct at
     : at<typename Container::tail, Index - 1>
    {};

    /** @brief Специализация для <tt> Index == 0 </tt>
    @tparam Container тип контейнера типов
    */
    template <class Container>
    struct at<Container, 0u>
     : declare_type<typename Container::head>
    {};

    // Алгоритмы
    // all_of
    /** @brief Проверка, что все элементы контейнера удовлетворяют заданному
    предикату
    @tparam Container тип контейнера типов
    @tparam Predicate тип-предикат
    */
    template <class Container, template <class> class Predicate>
    struct all_of
     : std::integral_constant<bool, Predicate<typename Container::head>::value
                                    && all_of<typename Container::tail, Predicate>::value>
    {};

    /** @brief Специализация для пустого списка типов
    @tparam Predicate тип-предикат
    */
    template <template <class> class Predicate>
    struct all_of<null_type, Predicate>
     : std::true_type
    {};

    // find
    template <class Container, class T,
              template <class, class> class Eq = std::is_same>
    struct find
     : std::conditional<Eq<typename Container::head, T>::value,
                        declare_type<Container>,
                        find<typename Container::tail, T, Eq>>::type
    {};

    template <class T, template <class, class> class Eq>
    struct find<null_type, T, Eq>
     : declare_type<null_type>
    {};

    // Копирование без дубликатов
    template <class Container, class Out = null_type>
    struct copy_without_duplicates
    {
    private:
        typedef typename Container::head Head;
        typedef typename Container::tail Tail;

        typedef typename find<Out, Head>::type Pos;

        typedef typename std::conditional<std::is_same<Pos, null_type>::value,
                                          Container, Out>::type new_out;

    public:
        /// @brief Тип-результат
        typedef typename copy_without_duplicates<Tail, new_out>::type type;
    };

    template <class Out>
    struct copy_without_duplicates<null_type, Out>
     : declare_type<Out>
    {};

    // min_value
    /** @brief Поиск наименьшего значения
    @tparam List контейнер типов
    @tparam Compare функция сравнения
    @tparam Result тип, возвращаемый, если @c Container пуст
    */
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

    /** @brief Специализация для пустых контейнеров
    @tparam Compare функция сравнения
    @tparam Result тип, возвращаемый, если @c Container пуст
    */
    template <template <class, class> class Compare, class Result>
    struct min_value<null_type, Compare, Result>
     : declare_type<Result>
    {};

    // remove_first
    /** @brief Удаляет первое входждения типа в контейнер
    @tparam List контейнер типов
    @tparam Value тип, который нужно удалить
    */
    template <class List, class Value>
    struct remove_first
     : std::conditional<std::is_same<typename List::head, Value>::value,
                        pop_front<List>,
                        push_front<remove_first<typename List::tail, Value>,
                                   typename List::head>
                       >::type
    {};

    /** @brief специализация для пустых списков
    @tparam Value тип, который нужно удалить
    */
    template <class Value>
    struct remove_first<null_type, Value>
     : declare_type<null_type>
    {};
}
// namespace meta
}
// namespace ural

#endif
// Z_URAL_META_LIST_HPP_INCLUDED
