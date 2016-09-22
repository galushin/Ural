#ifndef Z_URAL_ALGORITHM_SORTING_HPP_INCLUDED
#define Z_URAL_ALGORITHM_SORTING_HPP_INCLUDED

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

/** @file ural/algorithm/sorting.hpp
 @brief Алгоритмы, связанные с сортировкой
*/

/** @defgroup SortingOperations Сортировка и связанные с ней операции
 @ingroup Algorithms
 @brief Алгоритмы, связанные с сортировкой и/или обработкой сортированных
 последовательностей.
*/

/** @defgroup BinarySearch Бинарный поиск
 @ingroup SortingOperations
 @brief Представляют собой различные варианты бинарного поиска и предполагают,
 что последовательность, в которой производится поиск, является разделённой
 относительно предиката, формируемого за счёт связывания ключа поиска и
 одного из аргументов функции сравнения. Эти алгоритмы работают для прямых
 последовательностей и минимизируют число сравнений: оно будет логарифмическим
 для всех видов последовательностей. Особенно хорошо для этих алгоритмов
 подходят последовательности произвольного доступа, так как для них будет
 производится логарифмическое число перемещений по структуре данных. Для
 последовательностей, не предоставляющих произвольный доступ, будет производится
 линейное число перемещений.
*/

/** @defgroup SetOperations Операции над множествами
 @ingroup SortingOperations
 @brief Операции над сортированными множествами и мульти-множествами
*/

/** @defgroup HeapOperations Операции над кучами
 @ingroup SortingOperations
 @brief Операции, связанные с бинарными кучами. То есть структурами данных,
 имеющих операции вставку элемента за логорифмическое время, а также
 определения наибольшего элемента и его удаления за постоянное время.
*/

/** @defgroup MinMaxOperations Наибольшие и наименьшие элементы
 @ingroup SortingOperations
 @brief Операции поиска наибольших и наименьших элементов последовательностей.
*/

/** @defgroup PermutationGenerators Генераторы перестановок
 @ingroup SortingOperations
 @brief Операции порождения перестановок в лексикографическом порядке.
*/

#include <ural/algorithm/non_modifying.hpp>
#include <ural/sequence/adaptors/outdirected.hpp>
#include <ural/sequence/adaptors/set_operations.hpp>

namespace ural
{
inline namespace v1
{
    // Проверка отсортированности
    /** @ingroup SortingOperations
    @brief Тип функционального объекта для пропуска отсортированной части
    последовательности
    */
    class is_sorted_until_fn
    {
    private:
        template <class Forward, class Compare>
        static Forward
        impl(Forward in, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardCursor<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, Forward>));

            auto cmp_2_1 = ::ural::experimental::make_binary_reverse_args(std::move(cmp));

            in = ural::adjacent_find_fn{}(std::move(in), std::move(cmp_2_1));

            if(!!in)
            {
                ++ in;
            }

            return in;
        }

    public:
        /** @brief Пропуск отсортированной части последовательности
        @param in входная последовательность.
        @param cmp функция сравнения, по умолчанию используется
        <tt> ural::less<> </tt>, то есть оператор "меньше".
        @return Последовательность @c r, такая, что
        <tt> original(r) == in </tt>,
        <tt> r.traversed_front() </tt> является отсортированным в соответствии с
        @c cmp и <tt> cmp(r.traversed_front().back(), r.front()) == false </tt>.
        */
        template <class Forward, class Compare = ::ural::less<>>
        cursor_type_t<Forward>
        operator()(Forward && in, Compare cmp = Compare()) const
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, cursor_type_t<Forward>>));

            return this->impl(::ural::cursor_fwd<Forward>(in),
                              ::ural::make_callable(std::move(cmp)));
        }
    };

    /** @ingroup SortingOperations
    @brief Тип функционального объекта для проверки того, что последовательность
    является упорядоченной
    */
    class is_sorted_fn
    {
    private:
        template <class Forward, class Compare>
        static bool
        impl(Forward in, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardCursor<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, Forward>));

            return !is_sorted_until_fn{}(std::move(in), std::move(cmp));
        }

    public:
        /** @brief Проверка того, что последовательность является упорядоченной
        @param in входная последовательность
        @param cmp функция сравнения, по умолчанию используется
        <tt> ural::less<> </tt>, то есть оператор "меньше"
        @return @b true, если для любых двух элементов @c x и @c y
        последовательности @c in, таких, что @c x предшествует @c y, выполняется
        условие <tt> cmp(x, y) != false </tt>
        */
        template <class Forward, class Compare = ural::less<>>
        bool operator()(Forward && in, Compare cmp = Compare()) const
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare,
                                                             cursor_type_t<Forward>>));

            return this->impl(::ural::cursor_fwd<Forward>(in),
                              ::ural::make_callable(std::move(cmp)));
        }
    };

    // Бинарные кучи
    /** @ingroup HeapOperations
    @brief Тип функционального поиска префикса последовательности, являющегося
    бинарной кучей.
    */
    class is_heap_until_fn
    {
    public:
        /** @brief Поиск префикса последовательности, являющегося бинарной
        кучей.
        @param seq последовательность
        @param cmp функция сравнения
        @return Курсор @c r такой, что <tt> original(r) == cursor(seq) </tt>,
        <tt> r.traversed_front() </tt> является наибольшим префиксом @c seq,
        который является бинарной кучей.
        */
        template <class RASequence, class Compare = ural::less<>>
        cursor_type_t<RASequence>
        operator()(RASequence && seq, Compare cmp = Compare()) const
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequence<RASequence>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, cursor_type_t<RASequence>>));

            return this->impl(::ural::cursor_fwd<RASequence>(seq),
                              ::ural::make_callable(std::move(cmp)));
        }

    private:
        template <class RACursor, class Compare>
        static RACursor
        impl(RACursor cur, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessCursor<RACursor>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, RACursor>));

            // Пустая последовательность - куча
            if(!cur)
            {
                return cur;
            }

            auto const n = cur.size();

            auto index = 1;

            for(; index != n; ++ index)
            {
                auto const p = ural::details::heap_parent(index);

                if(cmp(cur[p], cur[index]))
                {
                    break;
                }
            }

            return std::move(cur) + index;
        }
    };

    /** @ingroup HeapOperations
    @brief Тип функционального объекта для проверки того, что последовательность
    является бинарной кучей.
    */
    class is_heap_fn
    {
    public:
        /** @brief Проверка того, что последовательность является бинарной
        кучей.
        @param seq последовательность
        @param cmp функция сравнения
        @return @b true, если @c seq является кучей, иначе --- @b false.
        */
        template <class RASequence, class Compare = ::ural::less<>>
        bool operator()(RASequence && seq, Compare cmp = Compare()) const
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequence<RASequence>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, cursor_type_t<RASequence>>));

            return this->impl(::ural::cursor_fwd<RASequence>(seq),
                              ::ural::make_callable(std::move(cmp)));
        }

    private:
        template <class RACursor, class Compare>
        static bool
        impl(RACursor cur, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessCursor<RACursor>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, RACursor>));

            return !::ural::is_heap_until_fn{}(cur, cmp);
        }
    };

    class heap_sink_fn
    {
    public:
        template <class RASequence, class Compare = ::ural::less<>>
        void operator()(RASequence && seq,
                        difference_type_t<cursor_type_t<RASequence>> first,
                        difference_type_t<cursor_type_t<RASequence>> last,
                        Compare cmp = Compare()) const
        {
            return this->impl(::ural::cursor_fwd<RASequence>(seq),
                              std::move(first), std::move(last),
                              ::ural::make_callable(std::move(cmp)));
        }

    private:
        template <class RACursor, class Compare>
        static void
        update_largest(RACursor cur,
                       difference_type_t<RACursor> & largest,
                       difference_type_t<RACursor> candidate,
                       difference_type_t<RACursor> last,
                       Compare cmp)
        {
            if(candidate < last && cmp(cur[largest], cur[candidate]))
            {
                largest = candidate;
            }
        }

        template <class RACursor, class Compare>
        void impl(RACursor cur,
                  difference_type_t<RACursor> first,
                  difference_type_t<RACursor> last,
                  Compare cmp) const
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessCursor<RACursor>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<RACursor, Compare>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, RACursor>));

            assert(ural::to_signed(last) <= cur.size());

            for(; first != last;)
            {
                auto largest = first;
                update_largest(cur, largest, details::heap_child_1(first), last, cmp);
                update_largest(cur, largest, details::heap_child_2(first), last, cmp);

                if(largest == first)
                {
                    return;
                }

                ::ural::indirect_swap(cur, largest, cur, first);

                first = largest;
            }
        }
    };

    /** @ingroup HeapOperations
    @brief Тип функционального объекта для добавления элемента в кучу.
    */
    class push_heap_fn
    {
    public:
        /** Добавление нового элемента в бинарную кучу
        @brief Оператор вызова функции
        @param seq последовательность произвольного доступа
        @param cmp функция сравнения. По умолчанию используется
        <tt> ::ural::less<>{} </tt>, то есть операто "меньше".
        @pre @c seq не пуста
        @pre Первые <tt> seq.size() - 1 </tt> элементов @c seq образуют бинарную
        кучу по отношению @c cmp
        @post <tt> is_heap(seq, cmp) </tt>
        */
        template <class RASequence, class Compare = ural::less<>>
        cursor_type_t<RASequence>
        operator()(RASequence && seq, Compare cmp = Compare()) const
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequence<RASequence>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<cursor_type_t<RASequence>, Compare>));

            return this->impl(::ural::cursor_fwd<RASequence>(seq),
                              ::ural::make_callable(std::move(cmp)));
        }

    private:
        template <class RACursor, class Compare>
        RACursor
        impl(RACursor cur, Compare cmp) const
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessCursor<RACursor>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<RACursor, Compare>));

            assert(::ural::is_heap_until_fn{}(cur, cmp).size() <= 1);

            if(cur.size() >= 1)
            {
                ::ural::details::heap_swim(cur, cur.size() - 1, cmp);
            }

            assert(is_heap_fn{}(cur, cmp));

            cur += cur.size();
            return cur;
        }
    };

    /** @ingroup HeapOperations
    @brief Тип функционального объекта для удаления первого элемента из кучи
    */
    class pop_heap_fn
    {
    public:
        /** @brief Удаление первого элемента из кучи
        @param seq последовательность, представляющая собой бинарную кучу
        @param cmp функция сравнения, по умолчанию используется
        <tt> less<> </tt>, то есть оператор "меньше".
        @return Последовательность, полученная
        <tt>::ural::cursor_fwd<RASequence>(seq)</tt> путём продвижения до
        исчерпания.
        */
        template <class RASequence, class Compare = ural::less<>>
        cursor_type_t<RASequence>
        operator()(RASequence && seq, Compare cmp = Compare()) const
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequence<RASequence>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<cursor_type_t<RASequence>, Compare>));

            return this->impl(::ural::cursor_fwd<RASequence>(seq),
                              ::ural::make_callable(std::move(cmp)));
        }

    private:
        template <class RACursor, class Compare>
        static RACursor
        impl(RACursor cur, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessCursor<RACursor>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<RACursor, Compare>));

            assert(is_heap_fn{}(cur, cmp));
            auto const N = cur.size();

            if(N > 1)
            {
                ::ural::indirect_swap(cur, 0, cur, N-1);
                heap_sink_fn{}(cur, 0*N, N-1, cmp);
            }

            return cur += N;
        }
    };

    /** @ingroup HeapOperations
    @brief Тип функционального объекта для преобразование последовательности
    в бинарную кучу
    */
    class make_heap_fn
    {
    public:
        /** @brief Преобразование последовательности в бинарную кучу
        @param seq последовательность
        @param cmp функция сравнения, по умолчанию используется
        <tt> less<> </tt>, то есть оператор "меньше".
        @post <tt> is_heap(std::forward<RASequence>(seq), cmp) </tt>
        @return Последовательность, полученная из std::forward<RASequence>(seq)
        путём продвижения до исчерпания.
        */
        template <class RASequence, class Compare = ural::less<>>
        cursor_type_t<RASequence>
        operator()(RASequence && seq, Compare cmp = Compare()) const
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequence<RASequence>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<cursor_type_t<RASequence>, Compare>));

            return this->impl(::ural::cursor_fwd<RASequence>(seq),
                              ::ural::make_callable(std::move(cmp)));
        }

    private:
        template <class RACursor, class Compare>
        static RACursor
        impl(RACursor cur, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessCursor<RACursor>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<RACursor, Compare>));

            for(auto n = cur.size() / 2; n > 0; -- n)
            {
                heap_sink_fn{}(cur, n - 1, cur.size(), cmp);
            }

            assert(is_heap_fn{}(cur, cmp));

            return cur += cur.size();
        }
    };

    /** @ingroup HeapOperations
    @brief Тип функционального объекта преобразования последовательности
    в бинарную кучу.
    */
    class sort_heap_fn
    {
    public:
        /** @brief Преобразование последовательности в бинарную кучу.
        @param seq последовательность произвольного доступа
        @param cmp функция сравнения, по умолчанию используется
        <tt> less<> </tt>, то есть оператор "меньше"
        @post <tt> is_sorted(seq, cmp) </tt>
        @return Последовательность, полученная
        <tt>::ural::cursor_fwd<RASequence>(seq)</tt> путём продвижения до
        исчерпания.
        */
        template <class RASequence, class Compare = ural::less<>>
        cursor_type_t<RASequence>
        operator()(RASequence && seq, Compare cmp = Compare()) const
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequence<RASequence>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<cursor_type_t<RASequence>, Compare>));

            return this->impl(::ural::cursor_fwd<RASequence>(seq),
                              ::ural::make_callable(std::move(cmp)));
        }

    private:
        template <class RACursor, class Compare>
        static RACursor
        impl(RACursor cur, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessCursor<RACursor>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<RACursor, Compare>));

            assert(is_heap_fn{}(cur, cmp));
            for(auto n = cur.size(); n > 0; --n)
            {
                pop_heap_fn{}(cur, cmp);
                cur.pop_back();
            }
            assert(!cur);
            assert(::ural::is_sorted_fn{}(cur.traversed_back(), cmp));

            auto result = cur.traversed_back();
            result += result.size();
            return result;
        }
    };

    class heap_select_fn
    {
    public:
        template <class RASequence, class Compare = ::ural::less<>>
        cursor_type_t<RASequence>
        operator()(RASequence && seq, Compare cmp = Compare()) const
        {
            // @todo Подумать, что можно возвращать из impl
            auto cur = ::ural::cursor_fwd<RASequence>(seq);
            this->impl(cur, ::ural::make_callable(std::move(cmp)));
            cur += cur.size();
            return cur;
        }

    private:
        template <class RACursor, class Compare>
        static void
        impl(RACursor cur, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessCursor<RACursor>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<RACursor, Compare>));

            if(!cur)
            {
                return;
            }

            ++ cur;
            auto cur1 = cur.traversed_front();

            if(!cur1 || !cur)
            {
                return;
            }

            make_heap_fn{}(cur1, cmp);

            for(; !!cur; ++ cur)
            {
                if(cmp(*cur, *cur1))
                {
                    ::ural::indirect_swap(cur, cur1);
                    auto const n = cur1.size();
                    heap_sink_fn{}(cur1, 0*n, n, cmp);
                }
            }
            pop_heap_fn{}(cur1, cmp);
        }
    };

    // Сортировка
    /** @ingroup SortingOperations
    @brief Тип функционального объекта для сортировки вставками
    */
    class insertion_sort_fn
    {
    public:
        /** @brief Сортировка вставками
        @param s сортируемая последовательность
        @param cmp функция сравнения, по умолчанию используется
        <tt> less<> </tt>, то есть оператор "меньше".
        @return Последовательность, полученная из @c s путём продвижения до
        исчерпания.
        */
        template <class RASequence, class Compare>
        cursor_type_t<RASequence>
        operator()(RASequence && s, Compare cmp) const
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequence<RASequence>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<cursor_type_t<RASequence>, Compare>));

            return this->impl(::ural::cursor_fwd<RASequence>(s),
                              ::ural::make_callable(std::move(cmp)));
        }

    private:
        template <class RACursor, class Compare>
        static RACursor impl(RACursor cur, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessCursor<RACursor>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<RACursor, Compare>));

            if(!cur)
            {
                return cur;
            }

            typedef decltype(cur.size()) Index;

            for(Index i = 1; i != cur.size(); ++ i)
            for(Index j = i; j > 0; -- j)
            {
                if(cmp(cur[j], cur[j-1]))
                {
                    ural::indirect_swap(cur, j, cur, j-1);
                }
                else
                {
                    break;
                }
            }

            cur += cur.size();
            return cur;
        }
    };

    /** @ingroup SortingOperations
    @brief Тип функционального объекта для устойчивой сортировки
    */
    class stable_sort_fn
    {
    public:
        /** @brief Устойчивая сортировка, то есть сортировка, сохраняющая
        относительный порядок эквивалентных элементов
        @param s сортируемая последовательность
        @param cmp функция сравнения, по умолчанию используется
        <tt> less<> </tt>, то есть оператор "меньше".
        @return Последовательность, полученная из @c s путём продвижения до
        исчерпания.
        */
        template <class RASequence, class Compare = ::ural::less<>>
        cursor_type_t<RASequence>
        operator()(RASequence && s, Compare cmp = Compare()) const
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequence<RASequence>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<cursor_type_t<RASequence>, Compare>));

            return this->impl(::ural::cursor_fwd<RASequence>(s),
                              ::ural::make_callable(std::move(cmp)));
        }

    private:
        template <class RACursor, class Compare>
        static RACursor impl(RACursor cur, Compare cmp)
        {
            return ::ural::insertion_sort_fn{}(std::move(cur), std::move(cmp));
        }
    };

    /** @ingroup SortingOperations
    @brief Тип функционального объекта для частичной сортировки
    */
    class partial_sort_fn
    {
    public:
        // @todo перегрузка, сортирующая traversed_front
        /** @brief Частичная сортировка
        @param s входная последовательность
        @param part количество начальных элементов, которые должны быть
        отсортированны
        @param cmp функция сравнения, по умолчанию используется
        <tt> less<> </tt>, то есть оператор "меньше".
        @return Последовательность, полученная из
        <tt> ::ural::cursor_fwd<RASequence>(s) </tt> продвижением до
        исчерпания.
        */
        template <class RASequence, class Compare = ::ural::less<>>
        cursor_type_t<RASequence>
        operator()(RASequence && s,
                   difference_type_t<cursor_type_t<RASequence>> part,
                   Compare cmp = Compare()) const
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequence<RASequence>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<cursor_type_t<RASequence>, Compare>));

            // @todo Подумать, что можно возвращать из impl
            auto seq = ::ural::cursor_fwd<RASequence>(s);
            this->impl(seq, std::move(part),
                       ::ural::make_callable(std::move(cmp)));
            seq += seq.size();
            return seq;
        }

    private:
        template <class RACursor, class Compare>
        static void
        impl(RACursor cur, difference_type_t<RACursor> const part, Compare cmp)
        {
            make_heap_fn{}(cur, cmp);

            cur.shrink_front();
            auto const cur_old = cur;
            cur += part;

            for(auto i = cur; !!i; ++ i)
            {
                if(cmp(*i, *cur_old))
                {
                    ::ural::indirect_swap(cur_old, i);
                    heap_sink_fn{}(cur.traversed_front(), 0*part, part, cmp);
                }
            }

            sort_heap_fn{}(cur.traversed_front(), cmp);
        }
    };

    /** @ingroup SortingOperations
    @brief Тип функционального объекта для копирование наименьших элементов
    одной последовательности в другую по порядку
    */
    class partial_sort_copy_fn
    {
    public:
        /** @brief Копирование наименьших элементов @c in в @c out по порядку
        @param in входная последовательность
        @param out выходная последовательность
        @param cmp функция сравнения, по умолчанию используется
        <tt> less<> </tt>, то есть оператор "меньше".
        */
        template <class Input, class RASequence, class Compare = ::ural::less<>>
        cursor_type_t<RASequence>
        operator()(Input && in, RASequence && out,
                   Compare cmp = Compare()) const
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input>));
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequence<RASequence>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyCopyable<cursor_type_t<Input>,
                                                               cursor_type_t<RASequence>>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<cursor_type_t<RASequence>, Compare>));

            return this->impl(::ural::cursor_fwd<Input>(in),
                              ::ural::cursor_fwd<RASequence>(out),
                              ::ural::make_callable(std::move(cmp)));
        }

    private:
        template <class Input, class RACursor, class Compare>
        static RACursor
        impl(Input in, RACursor out, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::InputCursor<Input>));
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessCursor<RACursor>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyCopyable<Input, RACursor>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<RACursor, Compare>));

            out.shrink_front();
            std::tie(in, out) = copy_fn{}(std::move(in), std::move(out));

            auto to_sort = out.traversed_front();
            auto const part = to_sort.size();

            make_heap_fn{}(to_sort, cmp);

            for(; !!in; ++ in)
            {
                if(cmp(*in, *to_sort))
                {
                    *to_sort = *in;
                    heap_sink_fn{}(to_sort, 0*part, part, cmp);
                }
            }

            sort_heap_fn{}(std::move(to_sort), cmp);

            return out;
        }
    };

    /** @ingroup SortingOperations
    @brief Тип функционального объекта для быстрой сортировки
    */
    class sort_fn
    {
    public:
        /** @brief Сортировка со сложностью <tt> N log(N) </tt>
        @param s сортируемая последовательность
        @param cmp функция сравнения, по умолчанию используется
        <tt> less<> </tt>, то есть оператор "меньше".
        @return Последовательность, полученная из @c s путём продвижения до
        исчерпания.
        */
        template <class RASequence, class Compare = ::ural::less<>>
        cursor_type_t<RASequence>
        operator()(RASequence && s, Compare cmp = Compare()) const
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequence<RASequence>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<cursor_type_t<RASequence>, Compare>));

            return this->impl(::ural::cursor_fwd<RASequence>(s),
                              ::ural::make_callable(std::move(cmp)));
        }
    private:
        template <class RACursor, class Compare>
        static RACursor impl(RACursor cur, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessCursor<RACursor>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<RACursor, Compare>));

            auto const n = ural::size(cur);
            return ::ural::partial_sort_fn{}(std::move(cur), n, std::move(cmp));
        }
    };

    /** @ingroup SortingOperations
    @brief Тип функционального объекта для определение N-го элемента
    сортированной последовательности.
    */
    class nth_element_fn
    {
    public:
        /** @brief Определение N-го элемента сортированной последовательности
        @details Переупорядочение последовательности таким образом, что
        на месте, соответствующему началу непройденной части последовательности
        оказывается элемент, который занимал бы это место после полной
        сортировки последовательности (с учётом передней пройденной части).
        Кроме того, элементы передней пройденной части последовательности
        становятся оказываются меньшими, чем элементы непройденной части
        последовательности.
        @param s последовательность
        @param cmp функция сравнения, по умолчанию используется
        <tt> less<> </tt>, то есть оператор "меньше".
        @return Последовательность, полученная из
        <tt> ::ural::cursor_fwd<RASequence>(s) </tt>, продвижением до
        исчерпания.
        @todo перегрузка, получающая номер элемента
        */
        template <class RASequence, class Compare = ::ural::less<>>
        cursor_type_t<RASequence>
        operator()(RASequence && s, Compare cmp = Compare()) const
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequence<RASequence>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<cursor_type_t<RASequence>, Compare>));

            return this->impl(::ural::cursor_fwd<RASequence>(s),
                              ::ural::make_callable(std::move(cmp)));
        }

    private:
        template <class RACursor, class Compare>
        static RACursor impl(RACursor cur, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessCursor<RACursor>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<RACursor, Compare>));

            return heap_select_fn{}(std::move(cur), std::move(cmp));
        }
    };

    /** @ingroup BinarySearch
    @brief Тип функционального объекта для поиска нижней грани
    */
    class lower_bound_fn
    {
    public:
        /** @brief Поиск нижней грани
        @param in последовательность
        @param value значение
        @param cmp функция сравнения, по умолчанию используется
        <tt> less<> </tt>, то есть оператор "меньше"
        @pre Элементы @c e курсора @c in должны быть разделены относительно
        предиката <tt> cmp(e, value) </tt>.
        @return Курсор @c r такой, что <tt> original(r) == cursor(in) </tt> и
        <tt> r.traversed_front() </tt> является наибольшей возможной
        последовательностью, всё элементы @c x которой удовлетворяют условию
        <tt> cmp(x, value) </tt>.
        */
        template <class Forward, class T, class Compare = ::ural::less<>>
        cursor_type_t<Forward>
        operator()(Forward && in, T const & value, Compare cmp = Compare()) const
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, T const *,
                                                             cursor_type_t<Forward>>));

            return this->impl(::ural::cursor_fwd<Forward>(in), value,
                              ::ural::make_callable(std::move(cmp)));
        }
    private:
        template <class Forward, class T, class Compare>
        static Forward
        impl(Forward in, T const & value, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardCursor<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, T const *, Forward>));

            auto pred = [&](auto const & x) { return cmp(x, value); };
            return ::ural::partition_point_fn{}(std::move(in), std::move(pred));
        }
    };

    /** @ingroup BinarySearch
    @brief Тип функционального объекта для поиска верхней грани
    */
    class upper_bound_fn
    {
    public:
        /** @brief Поиск верхней грани
        @param in последовательность
        @param value значение
        @param cmp функция сравнения, по умолчанию используется
        <tt> less<> </tt>, то есть оператор "меньше"
        @pre Элементы @c e последовательности @c in должны быть разделены
        относительно предиката <tt> cmp(e, value) </tt>.
        @return Последовательность @c r такая, что
        <tt> original(r) == cursor(in) </tt> и <tt> r.traversed_front() </tt>
        является наибольшей возможной последовательностью, всё элементы @c x
        которой удовлетворяют условию <tt> !cmp(value, x) </tt>.
        */
        template <class Forward, class T, class Compare = ::ural::less<>>
        cursor_type_t<Forward>
        operator()(Forward && in,
                   T const & value, Compare cmp = Compare()) const
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, T const *,
                                                             cursor_type_t<Forward>>));

            return this->impl(::ural::cursor_fwd<Forward>(in), value,
                              ::ural::make_callable(std::move(cmp)));
        }
    private:
        template <class RACursor, class T, class Compare>
        static RACursor
        impl(RACursor in, T const & value, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardCursor<RACursor>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, T const *, RACursor>));

            auto pred = [&](auto const & x) { return !cmp(value, x); };

            return ::ural::partition_point_fn{}(std::move(in), std::move(pred));
        }
    };

    /** @ingroup BinarySearch
    @brief Тип функционального объекта для поиска подпоследовательности
    элементов, эквивалентных данному
    */
    class equal_range_fn
    {
    public:
        /** @brief Поиск подпоследовательности элементов, эквивалентных данному
        значению
        @param in входная последовательность
        @param value значение
        @param cmp функция сравнения, по умолчанию используется
        <tt> less<> </tt>, то есть оператор "меньше".
        @pre Элементы @c e последовательности @c in должны быть разделены
        относительно предиката <tt> cmp(e, value) </tt>.
        @return Последовательность @c r такая, что
        <tt> original(r) == cursor(in) </tt>, причём все элементы @c r
        эквивалентны @c value в смысле отношения @c cmp.
        */
        template <class Forward, class T, class Compare = ::ural::less<>>
        TraversedFrontType<cursor_type_t<Forward>>
        operator()(Forward && in,
                   T const & value, Compare cmp = Compare()) const
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, T const *,
                                                             cursor_type_t<Forward>>));

            return this->impl(::ural::cursor_fwd<Forward>(in), value,
                              ::ural::make_callable(std::move(cmp)));
        }

    private:
        template <class Forward, class T, class Compare>
        TraversedFrontType<Forward>
        impl(Forward in, T const & value, Compare cmp) const
        {
            return this->impl(std::move(in), value, std::move(cmp),
                              ::ural::make_cursor_tag(in));
        }

        template <class Forward, class T, class Compare>
        TraversedFrontType<Forward>
        impl(Forward in, T const & value, Compare cmp,
                            finite_forward_cursor_tag) const
        {
            BOOST_CONCEPT_ASSERT((concepts::FiniteForwardCursor<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, T const *, Forward>));

            auto upper = upper_bound_fn{}(in, value, cmp).traversed_front();

            return lower_bound_fn{}(upper, value, cmp);
        }

        template <class Bidirectional, class T, class Compare>
        Bidirectional impl(Bidirectional in, T const & value, Compare cmp,
                                  bidirectional_cursor_tag) const
        {
            BOOST_CONCEPT_ASSERT((concepts::BidirectionalCursor<Bidirectional>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, T const *, Bidirectional>));

            auto lower = lower_bound_fn{}(in, value, cmp);
            auto upper = upper_bound_fn{}(in, value, cmp);

            auto n_lower = ural::size(lower.traversed_front());
            auto n_upper = ural::size(in) - ural::size(upper.traversed_front());

            ural::advance(in, n_lower);
            ural::pop_back_n(in, n_upper);

            return in;
        }
    };

    /** @ingroup BinarySearch
    @brief Тип функционального объекта для проверки того, что значение
    принадлежит упорядоченной последовательности.
    */
    class binary_search_fn
    {
    public:
        /** @brief Проверка того, что значение принадлежит упорядоченной
        последовательности.
        @param in последовательность
        @param value значение
        @param cmp функция сравнения, по умолчанию используется
        <tt> less<> </tt>, то есть оператор "меньше".
        @pre Элементы @c e последовательности @c in должны быть разделены
        относительно предиката <tt> cmp(e, value) </tt>.
        @return @b true, если существует элементы @c e последовательности @c in
        эквивалентный @c value, то есть такой, для которого выполняется условие
        <tt> cmp(e, value) == cmp(value, e) == false </tt>.
        */
        template <class Forward, class T, class Compare = ::ural::less<>>
        bool operator()(Forward && in, T const & value,
                        Compare cmp = Compare()) const
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, T const *,
                                                             cursor_type_t<Forward>>));

            return this->impl(::ural::cursor_fwd<Forward>(in), value,
                              ::ural::make_callable(std::move(cmp)));
        }

    private:
        template <class Forward, class T, class Compare>
        static bool impl(Forward in, T const & value, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardCursor<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, T const *, Forward>));

            in = lower_bound_fn{}(std::move(in), value, cmp);

            return !!in && !cmp(value, *in);
        }
    };

    // Слияние
    /** @ingroup SetOperations
    @brief Тип функционального объекта для слияния сортированных
    последовательности.
    */
    class merge_fn
    {
    public:
        /** @brief Слияние двух последовательностей путём копирования в выходную
        последовательность
        @param in1 первая входная последовательность
        @param in1 вторая входная последовательность
        @param out выходная последовательность
        @param cmp функция сравнения, по умолчанию используется
        <tt> less<> </tt>, то есть оператор "меньше".
        @return Кортеж, содержащий непройденные части последовательностей.
        */
        template <class Input1, class Input2, class Output,
                  class Compare = ::ural::less<>>
        tuple<cursor_type_t<Input1>, cursor_type_t<Input2>, cursor_type_t<Output>>
        operator()(Input1 && in1, Input2 && in2, Output && out,
                   Compare cmp = Compare()) const
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input1>));
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input2>));
            BOOST_CONCEPT_ASSERT((concepts::SinglePassSequence<Output>));
            BOOST_CONCEPT_ASSERT((concepts::Mergeable<cursor_type_t<Input1>,
                                                      cursor_type_t<Input2>,
                                                      cursor_type_t<Output>,
                                                      Compare>));

            auto inputs = ::ural::experimental::merged(::ural::cursor_fwd<Input1>(in1),
                                                       ::ural::cursor_fwd<Input2>(in2),
                                                       ::ural::make_callable(std::move(cmp)));
            auto result = ural::copy_fn{}(std::move(inputs),
                                          ::ural::cursor_fwd<Output>(out));

            return ural::make_tuple(std::move(result[ural::_1]).first_base(),
                                    std::move(result[ural::_1]).second_base(),
                                    std::move(result[ural::_2]));
        }
    };

    /** @ingroup SetOperations
    @brief Тип функционального объекта для слияния сортированных частей
    последовательности.
    */
    class inplace_merge_fn
    {
    public:
        /** @brief Слияние передней пройденной и непройденной части
        последовательности.
        @param s последовательность
        @param cmp функция сравнения, по умолчанию используется
        <tt> less<> </tt>, то есть оператор "меньше".
        @return Последовательность, полученная
        <tt>::ural::cursor_fwd<Bidirectional>(seq)</tt> путём продвижения до
        исчерпания.
        */
        template <class Bidirectional, class Compare = ::ural::less<>>
        cursor_type_t<Bidirectional>
        operator()(Bidirectional && s, Compare cmp = Compare()) const
        {
            BOOST_CONCEPT_ASSERT((concepts::BidirectionalSequence<Bidirectional>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<cursor_type_t<Bidirectional>, Compare>));

            // @todo Возвращать из impl последовательность
            auto seq = ::ural::cursor_fwd<Bidirectional>(s);
            this->impl(seq, ::ural::make_callable(std::move(cmp)));

            seq.exhaust_front();
            return seq;
        }

    private:
        template <class BidirectionalCursor, class Compare>
        void impl(BidirectionalCursor cur, Compare cmp) const
        {
            BOOST_CONCEPT_ASSERT((concepts::BidirectionalCursor<BidirectionalCursor>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<BidirectionalCursor, Compare>));

            auto cur1 = cur.traversed_front();
            auto cur2 = ural::shrink_front_copy(cur);

            auto n1 = ural::size(cur1);
            auto n2 = ural::size(cur2);

            if(!cur1 || !cur2)
            {
                return;
            }

            assert(::ural::is_sorted_fn{}(cur1, cmp));
            assert(::ural::is_sorted_fn{}(cur2, cmp));

            if(n1 + n2 == 2)
            {
                if(cmp(*cur2, *cur1))
                {
                    ::ural::indirect_swap(cur1, cur2);
                }
                return;
            }

            auto cur1_cut = cur1;
            auto cur2_cut = cur2;

            if(n1 > n2)
            {
                ural::advance(cur1_cut, n1 / 2);
                cur2_cut = lower_bound_fn{}(cur2, *cur1_cut, cmp);
            }
            else
            {
                ural::advance(cur2_cut, n2 / 2);
                cur1_cut = upper_bound_fn{}(cur1, *cur2_cut, cmp);
            }

            ::ural::rotate_fn{}(cur1_cut, cur2_cut.traversed_front());

            auto cur_new = cur.original();

            auto n11 = ural::size(cur1_cut.traversed_front());
            auto n12 = ural::size(cur1_cut);
            auto n21 = ural::size(cur2_cut.traversed_front());

            ural::advance(cur_new, n11 + n21);

            auto cur1_new = cur_new.traversed_front();
            auto cur2_new = ural::shrink_front_copy(cur_new);

            ural::advance(cur1_new, n11);
            ural::advance(cur2_new, n12);

            this->impl(cur1_new, cmp);
            this->impl(cur2_new, cmp);
        }
    };

    // Операции с множествами
    /** @ingroup SetOperations
    @brief Тип функционального объекта для проверки того, что одно множество
    является подмножеством другого.
    */
    class includes_fn
    {
    public:
        /** @brief Проверка того, что одно множество включает другое как
        подмножество.
        @param in1 первая последовательность
        @param in2 вторая последовательность
        @param cmp функция сравнения, по умолчанию используется
        <tt> less<> </tt>, то есть оператор "меньше".
        @return @b true, если @c in1 включает все элементы @c in2, иначе ---
        @b false.
        */
        template <class Input1, class  Input2, class Compare = ::ural::less<>>
        bool operator()(Input1 && in1, Input2 && in2,
                        Compare cmp = Compare()) const
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input1>));
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare,
                                                             cursor_type_t<Input1>,
                                                             cursor_type_t<Input2>>));

            return this->impl(::ural::cursor_fwd<Input1>(in1),
                              ::ural::cursor_fwd<Input2>(in2),
                              ::ural::make_callable(std::move(cmp)));
        }

    private:
        template <class Input1, class  Input2, class Compare>
        static bool impl(Input1 in1, Input2 in2, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::InputCursor<Input1>));
            BOOST_CONCEPT_ASSERT((concepts::InputCursor<Input2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, Input1, Input2>));

            for(; !!in1 && !!in2;)
            {
                if(cmp(*in1, *in2))
                {
                    ++ in1;
                }
                else if(cmp(*in2, *in1))
                {
                    return false;
                }
                else
                {
                    ++ in1;
                    ++ in2;
                }
            }

            return !in2;
        }
    };

    /** @ingroup SetOperations
    @brief Тип функционального объекта для построения объединения двух множеств.
    */
    class set_union_fn
    {
    public:
        /** @brief Теоретико-множественное объединение двух множеств
        @param in1 первая входная последовательность
        @param in2 вторая входная последовательность
        @param out выходная последовательность
        @param cmp функция сравнения, по умолчанию используется
        <tt> less<> </tt>, то есть оператор "меньше".
        @return Непройденные части входных и выходной последовательностей
        */
        template <class Input1, class Input2, class Output,
                  class Compare = ::ural::less<>>
        tuple<cursor_type_t<Input1>, cursor_type_t<Input2>, cursor_type_t<Output>>
        operator()(Input1 && in1, Input2 && in2, Output && out,
                        Compare cmp = Compare()) const
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input1>));
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare,
                                                             cursor_type_t<Input1>,
                                                             cursor_type_t<Input2>>));
            BOOST_CONCEPT_ASSERT((concepts::Mergeable<cursor_type_t<Input1>,
                                                      cursor_type_t<Input2>,
                                                      cursor_type_t<Output>,
                                                      Compare>));

            auto in
                = ::ural::experimental::make_set_union_cursor(std::forward<Input1>(in1),
                                                              std::forward<Input2>(in2),
                                                              ::ural::make_callable(std::move(cmp)));
            auto r = ural::copy_fn{}(std::move(in),
                                     ::ural::cursor_fwd<Output>(out));
            return make_tuple(std::move(r[ural::_1]).first_base(),
                              std::move(r[ural::_1]).second_base(),
                              std::move(r[ural::_2]));
        }
    };

    /** @ingroup SetOperations
    @brief Тип функционального объекта для построения пересечения двух множеств.
    */
    class set_intersection_fn
    {
    public:
        /** @brief Теоретико-множественное пересечение двух множеств
        @param in1 первая входная последовательность
        @param in2 вторая входная последовательность
        @param out выходная последовательность
        @param cmp функция сравнения, по умолчанию используется
        <tt> less<> </tt>, то есть оператор "меньше".
        @return Непройденные части входных и выходной последовательностей
        */
        template <class Input1, class Input2, class Output,
                  class Compare = ::ural::less<>>
        tuple<cursor_type_t<Input1>, cursor_type_t<Input2>, cursor_type_t<Output>>
        operator()(Input1 && in1, Input2 && in2, Output && out,
                   Compare cmp = Compare()) const
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input1>));
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare,
                                                             cursor_type_t<Input1>,
                                                             cursor_type_t<Input2>>));
            BOOST_CONCEPT_ASSERT((concepts::Mergeable<cursor_type_t<Input1>,
                                                      cursor_type_t<Input2>,
                                                      cursor_type_t<Output>,
                                                      Compare>));

            auto in
                = ::ural::experimental::make_set_intersection_cursor(std::forward<Input1>(in1),
                                                                     std::forward<Input2>(in2),
                                                                     ::ural::make_callable(std::move(cmp)));
            auto r = ural::copy_fn{}(std::move(in),
                                     ::ural::cursor_fwd<Output>(out));
            return make_tuple(std::move(r[ural::_1]).first_base(),
                              std::move(r[ural::_1]).second_base(),
                              std::move(r[ural::_2]));
        }
    };

    /** @ingroup SetOperations
    @brief Тип функционального объекта для построения разности двух множеств.
    */
    class set_difference_fn
    {
    public:
        /** @brief Теоретико-множественная разность двух множеств
        @param in1 первая входная последовательность
        @param in2 вторая входная последовательность
        @param out выходная последовательность
        @param cmp функция сравнения, по умолчанию используется
        <tt> less<> </tt>, то есть оператор "меньше".
        @return Непройденные части входных и выходной последовательностей
        */
        template <class Input1, class Input2, class Output,
                  class Compare = ::ural::less<>>
        tuple<cursor_type_t<Input1>, cursor_type_t<Input2>, cursor_type_t<Output>>
        operator()(Input1 && in1, Input2 && in2, Output && out,
                        Compare cmp = Compare()) const
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input1>));
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare,
                                                             cursor_type_t<Input1>,
                                                             cursor_type_t<Input2>>));
            BOOST_CONCEPT_ASSERT((concepts::Mergeable<cursor_type_t<Input1>,
                                                      cursor_type_t<Input2>,
                                                      cursor_type_t<Output>,
                                                      Compare>));

            auto in
                = ::ural::experimental::make_set_difference_cursor(std::forward<Input1>(in1),
                                                       std::forward<Input2>(in2),
                                                       ::ural::make_callable(std::move(cmp)));
            auto r = ural::copy_fn{}(std::move(in),
                                     ::ural::cursor_fwd<Output>(out));
            return make_tuple(std::move(r[ural::_1]).first_base(),
                              std::move(r[ural::_1]).second_base(),
                              std::move(r[ural::_2]));
        }
    };

    /** @ingroup SetOperations
    @brief Тип функционального объекта для построения симметричной разности
    двух множеств.
    */
    class set_symmetric_difference_fn
    {
    public:
        /** @brief Теоретико-множественная симметричная разность двух множеств
        @param in1 первая входная последовательность
        @param in2 вторая входная последовательность
        @param out выходная последовательность
        @param cmp функция сравнения, по умолчанию используется
        <tt> less<> </tt>, то есть оператор "меньше".
        @return Непройденные части входных и выходной последовательностей
        */
        template <class Input1, class Input2, class Output,
                  class Compare = ::ural::less<>>
        tuple<cursor_type_t<Input1>, cursor_type_t<Input2>, cursor_type_t<Output>>
        operator()(Input1 && in1, Input2 && in2, Output && out,
                   Compare cmp = Compare()) const
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input1>));
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare,
                                                             cursor_type_t<Input1>,
                                                             cursor_type_t<Input2>>));
            BOOST_CONCEPT_ASSERT((concepts::Mergeable<cursor_type_t<Input1>,
                                                      cursor_type_t<Input2>,
                                                      cursor_type_t<Output>,
                                                      Compare>));

            auto in = ::ural::experimental::make_set_symmetric_difference_cursor(std::forward<Input1>(in1),
                                                                     std::forward<Input2>(in2),
                                                                     ::ural::make_callable(std::move(cmp)));
            auto r = ::ural::copy_fn{}(std::move(in),
                                       ::ural::cursor_fwd<Output>(out));
            return make_tuple(std::move(r[ural::_1]).first_base(),
                              std::move(r[ural::_1]).second_base(),
                              std::move(r[ural::_2]));
        }
    };

    // Поиск наибольшего и наименьшего
    /** @ingroup MinMaxOperations
    @brief Тип функционального объекта для поиска наименьшего значения
    */
    class min_fn
    {
    public:
        /** @brief Определение наименьшего из двух значений
        @param x, y аргументы
        @pre @c T должен быть @c TotallyOrdered
        @return Наименьший из @c x и @c y, если они равны, то возвращает @c x.
        */
        template <class T>
        constexpr T const & operator()(T const & x, T const & y) const
        {
            BOOST_CONCEPT_ASSERT((concepts::TotallyOrdered<T>));

            return (*this)(x, y, ural::less<>{});
        }

        /** @brief Определение наименьшего из двух значений
        @param x, y аргументы
        @param cmp функция сравнения
        @pre @c Compare должент быть <tt> Relation<function_type_t<Comp>, T> </tt>
        @return Если <tt> cmp(y, x) </tt>, то возвращает @c y, иначе --- @c x.
        */
        template <class T, class Compare>
        constexpr T const &
        operator()(T const & x, T const & y, Compare cmp) const
        {
            BOOST_CONCEPT_ASSERT((concepts::Relation<function_type_t<Compare>, T>));

            return ::ural::make_callable(std::move(cmp))(y, x) ? y : x;
        }

        /** @brief Определение наименьшего из значений в списке инициализаторов
        @param values список инициализаторов
        @pre <tt> values.begin() != values.end() </tt>
        @return Наименьшее значение в списке инициализаторов
        */
        template <class T>
        constexpr T
        operator()(std::initializer_list<T> values) const
        {
            BOOST_CONCEPT_ASSERT((concepts::TotallyOrdered<T>));
            BOOST_CONCEPT_ASSERT((concepts::Semiregular<T>));

            return (*this)(values, ::ural::less<>{});
        }

        /** @brief Определение наименьшего из значений в списке инициализаторов
        @param values список инициализаторов
        @pre <tt> values.begin() != values.end() </tt>
        @return Наименьшее значение в списке инициализаторов
        */
        template <class T, class Compare = ::ural::less<>>
        constexpr T
        operator()(std::initializer_list<T> values, Compare cmp) const
        {
            BOOST_CONCEPT_ASSERT((concepts::Semiregular<T>));
            BOOST_CONCEPT_ASSERT((concepts::Relation<function_type_t<Compare>, T>));

            return values.size() > 0
                   ? this->impl(values.begin() + 1, values.end(),
                                std::move(cmp), *(values.begin()))
                   : throw std::logic_error("Must be not empty");
        }

    private:
        template <class T, class Compare>
        constexpr T
        impl(T const * first, T const * last, Compare cmp, T result) const
        {
            return first == last
                   ? std::move(result)
                   : this->impl(first + 1, last, std::move(cmp),
                                (*this)(result, *first, cmp));
        }
    };

    /** @ingroup MinMaxOperations
    @brief Тип функционального объекта для поиска наибольшего значения
    @note А. Степанов считает, что при эквивалентности аргументов нужно
    возвращать второй, чтобы обеспечить устойчивость.
    */
    class max_fn
    {
    public:
        /** @brief Определение наибольшего из двух значений
        @pre @c T должен быть @c TotallyOrdered
        @param x, y аргументы
        @return Если <tt> cmp(x, y) </tt>, то возвращает @c y, иначе --- @c x.
        */
        template <class T>
        constexpr T const & operator()(T const & x, T const & y) const
        {
            BOOST_CONCEPT_ASSERT((concepts::TotallyOrdered<T>));

            return (*this)(x, y, ural::less<>{});
        }

        /** @brief Определение наибольшего из двух значений
        @param x, y аргументы
        @param cmp функция сравнения
        @pre @c Compare должент быть <tt> Relation<function_type_t<Comp>, T> </tt>
        @return Наибольший из @c x и @c y, если они равны, то возвращает @c x.
        */
        template <class T, class Compare>
        constexpr T const &
        operator()(T const & x, T const & y, Compare cmp) const
        {
            BOOST_CONCEPT_ASSERT((concepts::Relation<function_type_t<Compare>, T>));

            return ::ural::make_callable(std::move(cmp))(x, y) ? y : x;
        }

        /** @brief Определение наибольшего из значений в списке инициализаторов
        @param values список инициализаторов
        @pre <tt> values.begin() != values.end() </tt>
        @return Наибольшее значение в списке инициализаторов
        */
        template <class T>
        constexpr T
        operator()(std::initializer_list<T> values) const
        {
            BOOST_CONCEPT_ASSERT((concepts::Semiregular<T>));
            BOOST_CONCEPT_ASSERT((concepts::TotallyOrdered<T>));

            return (*this)(values, ::ural::less<>{});
        }

        /** @brief Определение наибольшего из значений в списке инициализаторов
        @param values список инициализаторов
        @param cmp функция сравнения
        @pre <tt> values.begin() != values.end() </tt>
        @return Наибольшее значение в списке инициализаторов
        */
        template <class T, class Compare>
        constexpr T
        operator()(std::initializer_list<T> values, Compare cmp) const
        {
            BOOST_CONCEPT_ASSERT((concepts::Semiregular<T>));
            BOOST_CONCEPT_ASSERT((concepts::Relation<function_type_t<Compare>, T>));

            return values.size() > 0
                   ? this->impl(values.begin() + 1, values.end(),
                                std::move(cmp), *(values.begin()))
                   : throw std::logic_error("Must be not empty");
        }

    private:
        template <class T, class Compare>
        constexpr T
        impl(T const * first, T const * last, Compare cmp, T result) const
        {
            return first == last
                   ? std::move(result)
                   : this->impl(first + 1, last, std::move(cmp),
                                (*this)(result, *first, cmp));
        }
    };

    /** @ingroup MinMaxOperations
    @brief Тип функционального объекта для поиска наименьшего и наибольшего
    значений одновременно.
    */
    class minmax_fn
    {
    public:
        /** @brief Определение наименьшего и наименьшего из двух значений
        @pre @c T должен быть @c TotallyOrdered
        @param x, y аргументы
        @return Если <tt> y < x </tt>, то возвращает <tt> {y, x} </tt>, иначе
        --- <tt> {x, y} </tt>.
        */
        template <class T>
        constexpr std::pair<T const &, T const &>
        operator()(T const & x, T const & y) const
        {
            BOOST_CONCEPT_ASSERT((concepts::TotallyOrdered<T>));

            return (*this)(x, y, ural::less<>{});
        }

        /** @brief Определение наименьшего и наименьшего из двух значений
        @param x, y аргументы
        @param cmp функция сравнения
        @pre @c Compare должент быть <tt> Relation<function_type_t<Comp>, T> </tt>
        Если <tt> cmp(y, x) </tt>, то возвращает <tt> {y, x} </tt>, иначе
        --- <tt> {x, y} </tt>.
        */
        template <class T, class Compare>
        constexpr std::pair<T const &, T const &>
        operator()(T const & x, T const & y, Compare cmp) const
        {
            BOOST_CONCEPT_ASSERT((concepts::Relation<function_type_t<Compare>, T>));

            using Pair = std::pair<T const &, T const &>;
            return ::ural::make_callable(std::move(cmp))(y, x) ? Pair(y, x) : Pair(x, y);
        }

        /** @brief Определение наименьшего и наибольшего из значений в списке
        инициализаторов
        @param values список инициализаторов
        @pre <tt> values.begin() != values.end() </tt>
        @return <tt> std::pair<T, T>(x, y) </tt>, где @c x --- наименьшее, а
        @c y --- наибольшее из значений в списке инициализаторов.
        */
        template <class T>
        constexpr std::pair<T, T>
        operator()(std::initializer_list<T> values) const
        {
            BOOST_CONCEPT_ASSERT((concepts::TotallyOrdered<T>));

            return (*this)(values, ::ural::less<>{});
        }

        /** @brief Определение наименьшего и наибольшего из значений в списке
        инициализаторов
        @param values список инициализаторов
        @param cmp функция сравнения
        @pre <tt> values.begin() != values.end() </tt>
        @return <tt> std::pair<T, T>(x, y) </tt>, где @c x --- наименьшее, а
        @c y --- наибольшее из значений в списке инициализаторов.
        */
        template <class T, class Compare>
        constexpr std::pair<T, T>
        operator()(std::initializer_list<T> values, Compare cmp) const
        {
            BOOST_CONCEPT_ASSERT((concepts::Semiregular<T>));
            BOOST_CONCEPT_ASSERT((concepts::Relation<function_type_t<Compare>, T>));

            return values.size() > 0
                   ? this->impl(values.begin() + 1, values.end(),
                                std::move(cmp),
                                std::make_pair(*(values.begin()), *(values.begin())))
                   : throw std::logic_error("Must be not empty");
        }

    private:
        template <class T, class Compare>
        constexpr std::pair<T, T>
        impl(T const * first, T const * last, Compare cmp,
             std::pair<T, T> result) const
        {
            return first == last
                   ? std::move(result)
                   : this->impl(first + 1, last, std::move(cmp),
                                this->update_minmax_pair(*first, cmp, result));
        }

        template <class T, class Compare>
        constexpr std::pair<T, T>
        update_minmax_pair(T x, Compare cmp, std::pair<T, T> p) const
        {
            return cmp(x, p.first)
                   ? std::make_pair(std::move(x), std::move(p.second))
                   : cmp(p.second, x) ? std::make_pair(std::move(p.first), std::move(x))
                                      : std::move(p);
        }
    };

    /** @ingroup MinMaxOperations
    @brief Тип функционального объекта для поиска наименьшего элемента
    последовательности.
    */
    class min_element_fn
    {
    private:
        template <class Forward, class Compare>
        static Forward
        impl(Forward in, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardCursor<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, Forward>));

            if(!in)
            {
                return in;
            }

            auto cmp_s = ::ural::experimental::compare_by(ural::experimental::dereference<>{}, std::move(cmp));

            ::ural::experimental::min_element_accumulator<Forward, decltype(cmp_s)>
                acc(in++, cmp_s);

            auto seq = in | ::ural::experimental::outdirected;

            acc = ::ural::for_each_fn{}(std::move(seq), std::move(acc))[ural::_2];

            return acc.result();
        }
    public:
        /** @brief Поиск наименьшего элемента последовательности.
        @param in входная последовательность
        @param cmp функция сравнения, по умолчанию используется
        <tt> ural::less<> </tt>, то есть оператор "меньше".
        @return Последовательность @c r такая, что
        <tt> original(r) == cursor(in) </tt>, а для любого элемента @c x
        последовательности @c in, выполняется условие
        <tt> cmp(x, r.front()) == false </tt>, а среди элементов
        последовательности <tt> r.traversed_front() </tt> нет элементов,
        эквивалентных <tt> r.front() </tt>.
        */
        template <class Forward, class Compare = ::ural::less<>>
        cursor_type_t<Forward>
        operator()(Forward && in, Compare cmp = Compare()) const
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare,
                                                             cursor_type_t<Forward>>));

            return this->impl(::ural::cursor_fwd<Forward>(in),
                              ::ural::make_callable(std::move(cmp)));
        }
    };

    /** @ingroup MinMaxOperations
    @brief Тип функционального объекта для поиска наибольшего элемента
    последовательности.
    */
    class max_element_fn
    {
    private:
        template <class Forward, class Compare>
        static Forward
        impl(Forward in, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardCursor<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, Forward>));

            auto transposed_cmp = ::ural::experimental::make_binary_reverse_args(std::move(cmp));

            return ::ural::min_element_fn{}(std::move(in),
                                            std::move(transposed_cmp));
        }

    public:
        /** @brief Поиск наибольшего элемента последовательности.
        @param in входная последовательность
        @param cmp функция сравнения, по умолчанию используется
        <tt> ural::less<> </tt>, то есть оператор "меньше".
        @return Последовательность @c r такая, что
        <tt> original(r) == cursor(in) </tt>, а для любого элемента @c x
        последовательности @c in, выполняется условие
        <tt> cmp(r.front(), x) == false </tt>, а среди элементов
        последовательности <tt> r.traversed_front() </tt> нет элементов,
        эквивалентных <tt> r.front() </tt>.
        */
        template <class Forward, class Compare = ::ural::less<>>
        cursor_type_t<Forward>
        operator()(Forward && in, Compare cmp = Compare()) const
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare,
                                                             cursor_type_t<Forward>>));

            return this->impl(::ural::cursor_fwd<Forward>(in),
                              ::ural::make_callable(std::move(cmp)));
        }
    };

    /** @ingroup MinMaxOperations
    @brief Тип функционального объекта для поиска наименьшего и наибольшего
    элементов последовательности.
    */
    class minmax_element_fn
    {
    public:
        /** @brief Поиск наибольшего и наименьшего элементов последовательности.
        Выполняется быстрее, чем выполнение алгоритмов @c min_element и
        @c max_element по-отдельности.
        @param in входная последовательность
        @param cmp функция сравнения, по умолчанию используется
        <tt> ural::less<> </tt>, то есть оператор "меньше".
        @return Кортеж, первый элемент которого совпадает с
        <tt> min_element(std::forward<Forward>(in), cmp) </tt>, а второй
        --- с <tt> max_element(std::forward<Forward>(in), cmp) </tt>
        */
        template <class Forward, class Compare = ::ural::less<>>
        tuple<cursor_type_t<Forward>, cursor_type_t<Forward>>
        operator()(Forward && in, Compare cmp = Compare()) const
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare,
                                                             cursor_type_t<Forward>>));

            return this->impl(::ural::cursor_fwd<Forward>(in),
                              ::ural::make_callable(std::move(cmp)));
        }

    private:
        template <class Forward, class Compare>
        static tuple<Forward, Forward>
        impl(Forward in, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardCursor<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, Forward>));

            using Tuple = tuple<Forward, Forward>;

            if(!in)
            {
                return Tuple{in, in};
            }

            auto cmp_min = ::ural::experimental::compare_by(::ural::experimental::dereference<>{}, std::cref(cmp));
            auto cmp_max = ::ural::experimental::make_binary_reverse_args(cmp_min);

            ::ural::experimental::min_element_accumulator<Forward, decltype(cmp_min)>
                acc_min(in, std::move(cmp_min));
            ::ural::experimental::min_element_accumulator<Forward, decltype(cmp_max)>
                acc_max(in, std::move(cmp_max));
            ++ in;

            for(; !!in; ++ in)
            {
                auto in_next = in;
                ++ in_next;

                // остался только один элемент
                if(!in_next)
                {
                    if(acc_min.update(in) == false)
                    {
                        acc_max(in);
                    }
                    break;
                }

                // осталось как минимум два элемента
                if(cmp(*in, *in_next))
                {
                    acc_min(in);
                    acc_max(in_next);
                }
                else
                {
                    acc_min(in_next);
                    acc_max(in);
                }

                in = in_next;
            }

            return Tuple{acc_min.result(), acc_max.result()};
        }
    };

    // Лексикографическое упорядочение
    /** @ingroup SortingOperations
    @brief Тип функционального объекта, проверяющего, что одна
    последовательность лексикографически предшествует другой.
    */
    class lexicographical_compare_fn
    {
    public:
        /** @brief Проверка того, что @c in1 лексикографически предшествует
        @c in2.
        @param in1 первая входная последовательность
        @param in2 вторая входная последовательность
        @param cmp функция сравнения, по умолчанию используется
        <tt> less<> </tt>, то есть оператор "меньше".
        @return @b true, если @c in1 лексикографически предшествует @c in2
        */
        template <class Input1, class  Input2, class Compare = ::ural::less<>>
        bool operator()(Input1 && in1, Input2 && in2,
                        Compare cmp = Compare()) const
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input1>));
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare,
                                                             cursor_type_t<Input1>,
                                                             cursor_type_t<Input2>>));

            return this->impl(::ural::cursor_fwd<Input1>(in1),
                              ::ural::cursor_fwd<Input2>(in2),
                              ::ural::make_callable(std::move(cmp)));
        }

    private:
        template <class Input1, class  Input2, class Compare>
        static bool
        impl(Input1 in1, Input2 in2, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::InputCursor<Input1>));
            BOOST_CONCEPT_ASSERT((concepts::InputCursor<Input2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, Input1, Input2>));

            for(; !!in1 && !!in2; ++ in1, (void) ++ in2)
            {
                if(cmp(*in1, *in2))
                {
                    return true;
                }
                else if(cmp(*in2, *in1))
                {
                    return false;
                }
            }
            return !in1 && !!in2;
        }
    };

    // Перестановки
    /** @ingroup PermutationGenerators
    @brief Тип функционального объекта, порождающего лексикографически
    следующей перестановки.
    */
    class next_permutation_fn
    {
    public:
        /** @brief Порождение лексикографически следующей перестановки. Если
        такой перестановки нет, то выполняет обращение последовательности.
        @param s последовательность
        @param cmp функция сравнения, по умолчанию используется
        <tt> ural::less<> </tt>, то есть оператор "меньше".
        @return Если последовательность @c s в начале выполнения операции
        упорядочена по убыванию, то @b false, иначе @b --- @b true.
        */
        template <class BiSequence, class Compare = ::ural::less<>>
        bool operator()(BiSequence && s, Compare cmp = Compare()) const
        {
            BOOST_CONCEPT_ASSERT((concepts::BidirectionalSequence<BiSequence>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare,
                                                             cursor_type_t<BiSequence>>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<cursor_type_t<BiSequence>, Compare>));

            return this->impl(::ural::cursor_fwd<BiSequence>(s),
                              ::ural::make_callable(std::move(cmp)));

        }

    private:
        template <class BiCursor, class Compare>
        bool impl(BiCursor cur, Compare cmp) const
        {
            BOOST_CONCEPT_ASSERT((concepts::BidirectionalCursor<BiCursor>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, BiCursor>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<BiCursor, Compare>));

            if(!cur)
            {
                return false;
            }

            auto cur1 = ural::next(cur);

            if(!cur1)
            {
                return false;
            }

            auto r = ::ural::is_sorted_until_fn{}(cur | ::ural::experimental::reversed, cmp);

            if(!r)
            {
                ::ural::reverse_fn{}(std::move(cur));
                return false;
            }
            else
            {
                auto r1 = r;
                auto r2 = cur | ::ural::experimental::reversed;

                for(; cmp(*r2, *r1); ++r2)
                {}

                ::ural::indirect_swap(r1, r2);
                ural::reverse_fn{}(r1.traversed_front().base());

                return true;
            }
        }
    };

    /** @ingroup PermutationGenerators
    @brief Тип функционального объекта, порождающего лексикографически
    предыдущей перестановки.
    */
    class prev_permutation_fn
    {
    public:
        /** @brief Порождение лексикографически предыдущей перестановки. Если
        такой перестановки нет, то выполняет обращение последовательности.
        @param s последовательность
        @param cmp функция сравнения, по умолчанию используется
        <tt> ural::less<> </tt>, то есть оператор "меньше"
        @return Если последовательность @c s в начале выполнения операции
        упорядочена по возрастанию, то @b false, иначе @b --- @b true.
        */
        template <class BiSequence, class Compare = ::ural::less<>>
        bool operator()(BiSequence && s, Compare cmp = Compare()) const
        {
            BOOST_CONCEPT_ASSERT((concepts::BidirectionalSequence<BiSequence>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare,
                                                             cursor_type_t<BiSequence>>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<cursor_type_t<BiSequence>, Compare>));

            return this->impl(::ural::cursor_fwd<BiSequence>(s),
                              ::ural::make_callable(std::move(cmp)));

        }

    private:
        template <class BiCursor, class Compare>
        bool impl(BiCursor cur, Compare cmp) const
        {
            BOOST_CONCEPT_ASSERT((concepts::BidirectionalCursor<BiCursor>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, BiCursor>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<BiCursor, Compare>));

            auto constexpr f = next_permutation_fn{};
            return f(std::move(cur), ::ural::not_fn(std::move(cmp)));
        }
    };
}
// namespace v1
}
// namespace ural

#endif
// Z_URAL_ALGORITHM_SORTING_HPP_INCLUDED
