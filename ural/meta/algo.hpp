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
 @todo учесть работы по MPL11 и Hana
*/

#include <ural/meta/map.hpp>
#include <ural/meta/list.hpp>
#include <ural/meta/functional.hpp>

namespace ural
{
namespace experimental
{
/** @namespace meta
 @brief Возможности, связанные с мета-программированием.
*/
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

    /** @brief Проверка того, что тип присутствует в контейнере типов
    @tparam Container контейнер типов
    @tparam T искомый тип
    @tparam Eq предикат над типами
    */
    template <class Container, class T, class Eq = meta::is_same>
    struct contains
     : std::integral_constant<bool, !std::is_same<typename meta::find<Container, T, Eq>::type, null_type>::value>
    {};

    /** @brief Проверка того, что @c C2 является подмножеством @c C1
    @tparam C1 первый список типов
    @tparam C2 второй список типов
    @tparam Eq фукнция, определяющая равенство
    */
    template <class C1, class C2, class Eq = meta::is_same>
    struct includes
    {
    private:
        typedef typename C2::head Head;
        typedef typename C2::tail Tail;

        typedef meta::contains<C1, Head, Eq> Head_result;
        typedef meta::includes<C1, Tail, Eq> Tail_result;

        typedef typename std::conditional<!Head_result::value,
                                          std::false_type,
                                          Tail_result>::type Impl;


    public:
        enum
        {
            value = Impl::value
        };
    };

    template <class C1, class Eq>
    struct includes<C1, null_type, Eq>
     : std::true_type
    {};

    // Подсчёт числа элементов, равных данному.
    template <class Container, class T, class Eq = meta::is_same>
    struct count
     : std::integral_constant<size_t, apply<Eq, T, typename Container::head>::value
                                      + count<typename Container::tail, T, Eq>::value>
    {};

    template <class T, class Eq>
    struct count<null_type, T, Eq>
     : std::integral_constant<size_t, 0>
    {};

    // Преобразование списка типов
    template <class Input, class UnaryFunction>
    struct transform
    {
    private:
        typedef typename Input::head Head;
        typedef typename Input::tail Tail;

        typedef typename UnaryFunction::template apply<Head>::type New_head;
        typedef typename transform<Tail, UnaryFunction>::type New_tail;

    public:
        /// @brief Тип-результат
        typedef typename push_front<New_tail, New_head>::type type;
    };

    template <class UnaryFunction>
    struct transform<null_type, UnaryFunction>
     : declare_type<null_type>
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

        typedef typename meta::find<Tail, Head, not_Eq>::type skip_head;
        typedef typename unique<skip_head>::type new_tail;

    public:
        /// @brief Тип-результат
        typedef typename push_front<new_tail, Head>::type type;
    };

    template <class Eq>
    struct unique<null_type, Eq>
     : declare_type<null_type>{};

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
    @note Для типов в С++ нет естественное отношения порядка, определённого на
    этапе компиляции (на этапе выполнения есть typeid::before), поэтому для
    @c Compare не предоставляется на значение по умолчанию
    */
    template <class List, class Compare, class Result = null_type>
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

    template <class Input, class Predicate, class T_new>
    struct replace_if
    {
    private:
        // Биндим аргументы
        typedef meta::arg<0> Get_input;
        typedef meta::arg<1> Get_predicate;
        typedef meta::arg<2> Get_new_type;

        // if
        typedef bind<meta::is_same, Get_input, meta::constant<null_type>>
            Input_is_empty;

        // else
        typedef template_bind<front, Get_input> Get_head ;
        typedef template_bind<pop_front, Get_input> Get_tail;

        using Need_to_replace_head = meta::bind<Predicate, Get_head>;

        using Make_new_head
            = meta::if_then_else<Need_to_replace_head, Get_new_type, Get_head>;
        typedef template_bind<replace_if, Get_tail, Get_predicate, Get_new_type> Make_new_tail;

        typedef template_bind<push_front, Make_new_tail, Make_new_head> Make_new_list;

        // сборка
        typedef meta::if_then_else<Input_is_empty, Get_input, Make_new_list>
            Implementor;

    public:
        /// @brief Тип-результат
        typedef typename meta::apply<Implementor, Input, Predicate, T_new>::type type;
    };

    template <class Input, class T_old, class T_new>
    struct replace
    {
    private:
        typedef bind<meta::is_same, arg<0>, constant<T_old>> Predicate;

    public:
        /// @brief Тип-резульат
        typedef typename replace_if<Input, Predicate, T_new>::type type;
    };

    // Сортировка выбором
    /** @brief Сортировка выбором
    @tparam List Контейнер типов
    @tparam Compare функция сравнения
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
        typedef typename front_or<Container, Container>::type Head;
        typedef typename pop_front_or<Container, null_type>::type Tail;

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
    @tparam Eq функция проверки равенства
    */
    template <class Container, class Eq = meta::is_same>
    struct copy_without_duplicates
    {
    private:
        typedef typename Container::head Head;
        typedef typename Container::tail Tail;

        typedef typename remove_all<Tail, Head, Eq>::type removed_head;
        typedef typename copy_without_duplicates<Tail, Eq>::type new_tail;

    public:
        /// @brief Тип-результат
        typedef typename push_front<new_tail, Head>::type type;
    };

    template <>
    struct copy_without_duplicates<null_type>
     : declare_type<null_type>
    {};

    // Склеивание списков типов
    /** @brief Объединение (конкатенация) нескольких списков типов в один
    @tparam Lists списки типов
    */
    template <class... Lists>
    struct append;

    template <class L1, class... Others>
    struct append<L1, Others...>
    {
    private:
        typedef typename L1::head Head;
        typedef typename L1::tail Tail;

        typedef typename append<Others...>::type R1;
        typedef typename append<Tail, R1>::type R2;

    public:
        /// @brief Тип-результат
        typedef typename push_front<R2, Head>::type type;
    };

    template <class... Others>
    struct append<null_type, Others...>
     : append<Others...>
    {};

    template <class L1>
    struct append<L1>
     : declare_type<L1>
    {};

    template <>
    struct append<null_type>
     : declare_type<null_type>
    {};

    // Декартово произведение
    /** @brief Декартово произведение списков типов
    @tparam Lists списки типов
    */
    template <class... Lists>
    struct cartesian_product;

    template <>
    struct cartesian_product<null_type>
     : declare_type<null_type>
    {};

    template <class Input>
    struct cartesian_product<Input>
    {
    private:
        typedef typelist<typename Input::head> new_head;

        typedef typename cartesian_product<typename Input::tail>::type
            new_tail;
    public:
        /// @brief Тип-результат
        typedef typename push_front<new_tail, new_head>::type type;
    };

    template <class Input1, class... Others>
    struct cartesian_product<Input1, Others...>
    {
    private:
        typedef typename cartesian_product<Others...>::type CP_of_others;

        // @todo выразить через существующие
        template <class Types, class Tuples>
        struct combine
        {
        private:
            typedef typename Types::head Head;
            typedef typename Types::tail Tail;

            using helper = meta::template_bind<meta::push_front, meta::arg<0>,
                                               meta::constant<Head>>;

            typedef typename meta::transform<Tuples, helper>::type List1;

            typedef typename combine<Tail, Tuples>::type List2;

        public:
            using type = typename meta::append<List1, List2>::type;
        };

        template <class Tuples>
        struct combine<null_type, Tuples>
         : declare_type<null_type>
        {};

    public:
        /// @brief Тип-результат
        typedef typename combine<Input1, CP_of_others>::type type;
    };
}
// namespace meta
}
// namespace experimental
}
// namespace ural

#endif
// Z_URAL_META_ALGO_HPP_INCLUDED
