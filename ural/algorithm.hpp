#ifndef Z_URAL_ALGORITHM_HPP_INCLUDED
#define Z_URAL_ALGORITHM_HPP_INCLUDED

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

/** @file ural/algorithm.hpp
 @brief Обобщённые алгоритмы

 @todo Бинарный поиск: ограничения на функцию сравнения и значение
 @todo Проверка концепций + изменение имён в алгоритмах вида "*_n"
 @todo Разделить на несколько файлов, прежде всего можно вынести алгоритмы для
 контейнеров
 @todo Сгруппировать объявления переменных
 @todo Проверить возможность замены ForwardSequence на OutputSequence
 @todo Определить типы возврщаемых значений как в Range extensions
 @todo устранить дублирование в алгортмах за счёт CRTP/Фасадов
 @todo min/max/minmax для последовательностей
*/

/** @defgroup Algorithms Алгоритмы
 @brief Обобщённые операции над последовательностями.
*/

/** @defgroup NonModifyingSequenceOperations Немодифицирующие операции
 @ingroup Algorithms
 @brief Обобщённые операции не создающие новых последовательностей и не
 изменяющие порядок элементов существующих последовательностей.
*/

/** @defgroup MutatingSequenceOperations Модифицирующие операции
 @ingroup Algorithms
 @brief Операции, формирующие новые последовательности или меняющие порядок
 элементов существующих последовательностей.
*/

/** @defgroup SortingOperations Сортировка и связанные с ней операции
 @ingroup Algorithms
 @brief Алгоритмы, связанные с сортировкой и/или обработкой сортированных
 последовательностей
*/

#include <ural/math.hpp>
#include <ural/sequence/make.hpp>
#include <ural/sequence/base.hpp>
#include <ural/sequence/function_output.hpp>
#include <ural/sequence/generator.hpp>
#include <ural/sequence/outdirected.hpp>
#include <ural/sequence/reversed.hpp>
#include <ural/sequence/partition.hpp>
#include <ural/sequence/replace.hpp>
#include <ural/sequence/set_operations.hpp>
#include <ural/sequence/taken.hpp>
#include <ural/sequence/filtered.hpp>
#include <ural/sequence/transform.hpp>
#include <ural/sequence/moved.hpp>
#include <ural/sequence/uniqued.hpp>

#include <ural/functional.hpp>
#include <ural/random/c_rand_engine.hpp>
#include <ural/functional/make_callable.hpp>

#include <ural/algorithm/core.hpp>

#include <cassert>

namespace ural
{
    template <class T1, class T2>
    void swap(T1 & x, T2 & y);

namespace details
{
    class swap_fn
    {
    public:
        /** @brief Оператор применения функции.
        @details Производит неквалифицированный вызов функции @c swap. При этом
        включает перегрузки этой функции из пространства имён @c std, @c ural,
        @c boost
        */
        template <class T>
        void operator()(T & x, T & y) const
        {
            using ::std::swap;
            using ::ural::swap;
            using ::boost::swap;
            return swap(x, y);
        }
    };
    auto constexpr do_swap = swap_fn{};

    // Бинарные кучи
    template <class Size>
    Size heap_parent(Size pos)
    {
        return (pos - 1) / 2;
    }

    template <class Size>
    Size heap_child_1(Size pos)
    {
        return 2 * pos + 1;
    }

    template <class Size>
    Size heap_child_2(Size pos)
    {
        return 2 * pos + 2;
    }

    template <class RandomAccessSequence, class Size, class Compare>
    void heap_swim(RandomAccessSequence seq, Size index, Compare cmp)
    {
        for(; index > 0;)
        {
            auto const parent = heap_parent(index);

            if(cmp(seq[parent], seq[index]))
            {
                ::ural::details::do_swap(seq[parent], seq[index]);
            }

            index = parent;
        }
    }
}
// namespace details

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
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, Forward>));

            if(!in)
            {
                return in;
            }

            auto in_next = ural::next(in);

            for(; !!in_next; ++in_next, (void) ++ in)
            {
                if(cmp(*in_next, *in))
                {
                    break;
                }
            }

            return in_next;
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
        auto operator()(Forward && in, Compare cmp = Compare()) const
        -> decltype(::ural::sequence_fwd<Forward>(in))
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequenced<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, SequenceType<Forward>>));

            return this->impl(::ural::sequence_fwd<Forward>(in),
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
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Forward>));
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
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequenced<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare,
                                                             SequenceType<Forward>>));

            return this->impl(::ural::sequence_fwd<Forward>(in),
                              ::ural::make_callable(std::move(cmp)));
        }
    };

    /** @ingroup NonModifyingSequenceOperations
    @brief Тип функционального объекта для подсчёта количества элементов,
    удовлетворяющих заданному предикату.
    */
    class count_if_fn
    {
    private:
        template <class Input, class UnaryPredicate>
        static typename Input::distance_type
        impl(Input in, UnaryPredicate pred)
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectPredicate<UnaryPredicate, Input>));

            typename Input::distance_type result{0};

            for(; !!in; ++ in)
            {
                if (pred(*in))
                {
                    ++ result;
                }
            }
            return result;
        }

    public:
        /** @brief Подсчитывает количество элементов последовательности,
        удовлетворяющих предикату.
        @param in входная последовтельность
        @param pred предикат
        @return Количество элементов @c x последовательности @c in, таких, что
        <tt> pred(x) != false </tt>.
        */
        template <class Input, class UnaryPredicate>
        auto operator()(Input && in, UnaryPredicate pred) const
        -> typename decltype(::ural::sequence_fwd<Input>(in))::distance_type
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectPredicate<UnaryPredicate,
                                                              SequenceType<Input>>));

            return this->impl(::ural::sequence_fwd<Input>(in),
                              ::ural::make_callable(std::move(pred)));
        }
    };

    /** @ingroup NonModifyingSequenceOperations
    @brief Тип функционального объекта для подсчёта количества элементов,
    эквивалентных заданному значению.
    */
    class count_fn
    {
    private:
        template <class Input, class T, class BinaryPredicate>
        static typename Input::distance_type
        impl(Input in, T const & value, BinaryPredicate pred)
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<BinaryPredicate, Input, T const *>));

            return count_if_fn{}(std::move(in),
                                 std::bind(std::move(pred), ural::_1,
                                           std::ref(value)));
        }

    public:
        /** @brief Подсчитывает количество элементов последовательности, равных
        заданному значению.
        @param in входная последовтельность
        @param value значение
        @return Количество элементов @c x последовательности @c in, таких, что
        <tt> pred(x, value) == true </tt>.
        */
        template <class Input, class T,
                  class BinaryPredicate = ::ural::equal_to<>>
        auto operator()(Input && in, T const & value,
                        BinaryPredicate pred = BinaryPredicate()) const
        -> typename decltype(::ural::sequence_fwd<Input>(in))::distance_type
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<BinaryPredicate,
                                                             SequenceType<Input>,
                                                             T const *>));

            return this->impl(::ural::sequence_fwd<Input>(in), value,
                              ::ural::make_callable(std::move(pred)));
        }
    };

    // Модифицирующие алгоритмы
    /** @ingroup MutatingSequenceOperations
    @brief Тип функционального объекта для устранение последовательных
    дубликатов из последовательности.
    */
    class unique_fn
    {
    public:
        /** @brief Устранение последовательных дубликатов
        @param seq последовательность
        @param pred бинарный предикат, по умолчанию используется
        <tt> ::ural::equal_to<> </tt>, то есть оператор "равно".
        @return Последовательность, передняя пройденная часть которой содержит
        неустранённые элементы, причём исходной для неё является @c seq.
        */
        template <class ForwardSequence,
                  class BinaryPredicate = ::ural::equal_to<>>
        auto operator()(ForwardSequence && seq,
                        BinaryPredicate pred = BinaryPredicate()) const
        -> decltype(::ural::sequence_fwd<ForwardSequence>(seq))
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequenced<ForwardSequence>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<BinaryPredicate,
                                                             SequenceType<ForwardSequence>>));
            BOOST_CONCEPT_ASSERT((concepts::Permutable<SequenceType<ForwardSequence>>));

            return this->impl(::ural::sequence_fwd<ForwardSequence>(seq),
                              ::ural::make_callable(std::move(pred)));
        }

    private:
        template <class ForwardSequence, class BinaryPredicate>
        ForwardSequence
        impl(ForwardSequence seq, BinaryPredicate pred) const
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<ForwardSequence>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<BinaryPredicate, ForwardSequence>));
            BOOST_CONCEPT_ASSERT((concepts::Permutable<ForwardSequence>));

            // @todo Оптимизация
            auto us = ural::make_unique_sequence(std::move(seq), std::move(pred));

            auto result = copy_fn{}(us | ural::moved, seq);

            return result[ural::_2];
        }
    };

    // Алгоритмы над контейнерами
    /// @brief Функциональный объект для функции-члена контейнеров @c erase
    class erase_fn
    {
    public:
        /** @brief Удаление последовательности элементов из контейнера
        @param c контейнер
        @param seq последовательность элементов контейнера @c seq
        @return Аналог <tt> c.erase(seq.begin(), seq.end()) </tt>
        */
        template <class Container, class Iterator, class Policy>
        auto operator()(Container & c,
                        iterator_sequence<Iterator, Policy> seq) const
        -> typename Container::iterator
        {
            return c.erase(seq.begin(), seq.end());
        }
    };

    /** @brief Функциональный объект для удаления последовательных дубликатов
    из контейнера.
    */
    class unique_erase_fn
    {
    public:
        /** @brief Удаление последовательных дубликатов из контейнера
        @param c контейнер
        @param bin_pred бинарный предикат, с помощью которого определяются
        дубликаты
        @return @c c
        */
        template <class Container, class BinaryPredicate = ::ural::equal_to<>>
        Container &
        operator()(Container & c,
                   BinaryPredicate bin_pred = BinaryPredicate()) const
        {
            auto to_erase = ::ural::unique_fn{}(c, std::move(bin_pred));
            ::ural::erase_fn{}(c, to_erase);
            return c;
        }
    };

    /** @brief Класс функционального объекта, применяющий заданный
    функциональный объект каждого элемента последовательности.
    */
    class for_each_fn
    {
    private:
        template <class Input, class UnaryFunction>
        static tuple<Input, UnaryFunction>
        impl(Input in, UnaryFunction f)
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectCallable<UnaryFunction, Input>));

            auto r = ural::copy_fn{}(in, ural::make_function_output_sequence(std::move(f)));
            return ::ural::make_tuple(std::move(r[ural::_1]),
                                      std::move(r[ural::_2]).function());
        }

    public:
        /** @brief Применяет функциональный объект к каждому элементу
        последовательности
        @param in входная последовательность
        @param f функциональный объект
        @return Кортеж, первый компонент которого получается продвижением
        <tt> ::ural::sequence_fwd<Input>(in) </tt> до исчерпания, а второй
        --- <tt> ::ural::make_callable(std::move(f)) </tt> после его применения
        ко всем элементам последовательности.
        */
        template <class Input, class UnaryFunction>
        auto operator()(Input && in, UnaryFunction f) const
        -> tuple<decltype(::ural::sequence_fwd<Input>(in)),
                 decltype(::ural::make_callable(std::move(f)))>
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectCallable<UnaryFunction, SequenceType<Input>>));

            return for_each_fn::impl(::ural::sequence_fwd<Input>(in),
                                     ::ural::make_callable(std::move(f)));
        }
    };

    /** @ingroup NonModifyingSequenceOperations
    @brief Класс функционального объекта, проверяющего, что все элементы
    последовательности удовлетворяют предикату.
    */
    class all_of_fn
    {
    public:
        /** @brief Проверяет, что все элементы последовательности удовлетворяют
        заданному предикату
        @param in входная последовтельность
        @param pred предикат
        @return @b true, если для всех элементов @c x последовательности @c in
        выполняется <tt> pred(x) != false </tt>
        */
        template <class Input, class UnaryPredicate>
        bool operator()(Input && in, UnaryPredicate pred) const
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectPredicate<UnaryPredicate, SequenceType<Input>>));

            return !find_if_not_fn{}(std::forward<Input>(in), std::move(pred));
        }
    };

    /** @ingroup NonModifyingSequenceOperations
    @brief Класс функционального объекта, проверяющего, что ни один
    элемент последовательности не удовлетворяет предикату.
    */
    class none_of_fn
    {
    public:
        /** @brief Проверяет, что ни один элемент последовательности не
        удовлетворяет заданному предикату.
        @param in входная последовтельность
        @param pred предикат
        @return @b true, если ни для одного элемента @c x последовательности
        @c in выполняется <tt> pred(x) != false </tt>. Для пустой входной
        последовательности возвращает @b true.
        */
        template <class Input, class UnaryPredicate>
        bool operator()(Input && in, UnaryPredicate pred) const
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectPredicate<UnaryPredicate, SequenceType<Input>>));

            return !find_if_fn{}(std::forward<Input>(in), std::move(pred));
        }
    };

    /** @ingroup NonModifyingSequenceOperations
    @brief Класс функционального объекта, проверяющего, что хотя бы один
    элемент последовательности удовлетворяет предикату.
    */
    class any_of_fn
    {
    public:
        /** @brief Проверяет, что хотя бы один элемент последовательности
        удовлетворяет заданному предикату.
        @param in входная последовтельность
        @param pred предикат
        @return @b true, если для хотя бы одного элемента @c x последовательности
        @c in выполняется <tt> pred(x) != false </tt>
        */
        template <class Input, class UnaryPredicate>
        bool operator()(Input && in, UnaryPredicate pred) const
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectPredicate<UnaryPredicate, SequenceType<Input>>));

            return !none_of_fn{}(std::forward<Input>(in), std::move(pred));
        }
    };

    /** @ingroup NonModifyingSequenceOperations
    @brief Поиск первого элемента последовательности, эквивалентного одному из
    элементов другой последовательности
    */
    class find_first_of_fn
    {
    public:
        /** @brief Поиск первого появления элемента последовательности
        @param in последовательность, в которой осуществляется поиск
        @param s последовательность искомых элементов
        @param bin_pred бинарный предикат, по умолчанию используется
        <tt> ::ural::equal_to<> </tt>, то есть оператор "равно".
        @return Последовательность @c r, полученная из
        <tt> ::ural::sequence_fwd<Input>(in) </tt> продвижением до тех пор, пока
        <tt> r.front() </tt> не станет эквивалентным одному из элементов @c s.
        */
        template <class Input, class Forward,
                  class BinaryPredicate = ::ural::equal_to<>>
        auto operator()(Input && in, Forward && s,
                        BinaryPredicate bin_pred = BinaryPredicate()) const
        -> decltype(::ural::sequence_fwd<Input>(in))
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input>));
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequenced<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectPredicate<BinaryPredicate,
                                                              SequenceType<Input>,
                                                              SequenceType<Forward>>));

            return this->impl(::ural::sequence_fwd<Input>(in),
                              ::ural::sequence_fwd<Forward>(s),
                              ::ural::make_callable(std::move(bin_pred)));
        }

    private:
        template <class Input, class Forward, class BinaryPredicate>
        static Input impl(Input in, Forward s, BinaryPredicate bin_pred)
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input>));
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectPredicate<BinaryPredicate, Input, Forward>));

            for(; !!in; ++ in)
            {
                auto r = find_fn{}(s, *in, bin_pred);

                if(!!r)
                {
                    return in;
                }
            }
            return in;
        }
    };

    /** @ingroup NonModifyingSequenceOperations
    @brief Поиск первого элемента последовательности, не эквивалентного
    ни одному из элементов другой последовательности
    @todo уменьшить дублирование с find_first_of
    */
    class find_first_not_of_fn
    {
    public:
        /** @brief Поиск первого элемента, не являющегося элементом другой
        заданной последовательности
        @param in последовательность, в которой осуществляется поиск
        @param s последовательность искомых элементов
        @param bin_pred бинарный предикат, по умолчанию используется
        <tt> ::ural::equal_to<> </tt>, то есть оператор "равно"
        */
        template <class Input, class Forward,
                  class BinaryPredicate = ::ural::equal_to<>>
        auto operator()(Input && in, Forward && s,
                        BinaryPredicate bin_pred = BinaryPredicate()) const
        -> decltype(::ural::sequence_fwd<Input>(in))
        {
            return this->impl(::ural::sequence_fwd<Input>(in),
                              ::ural::sequence_fwd<Forward>(s),
                              ::ural::make_callable(std::move(bin_pred)));
        }

    private:
        template <class Input, class Forward, class BinaryPredicate>
        static Input impl(Input in, Forward s, BinaryPredicate bin_pred)
        {
            BOOST_CONCEPT_ASSERT((concepts::ReadableSequence<Input>));
            BOOST_CONCEPT_ASSERT((concepts::SinglePassSequence<Input>));

            BOOST_CONCEPT_ASSERT((concepts::ReadableSequence<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Forward>));

            typedef typename Input::reference Ref1;
            typedef typename Forward::reference Ref2;
            BOOST_CONCEPT_ASSERT((concepts::Callable<BinaryPredicate,
                                                     bool(Ref2, Ref1)>));

            for(; !!in; ++ in)
            {
                auto r = ::ural::find_fn{}(s, *in, bin_pred);

                if(!r)
                {
                    return in;
                }
            }

            return in;
        }
    };

    /** @ingroup NonModifyingSequenceOperations
    @brief Класс функционального объекта для поиска двух соседних элементов
    последовательности, удовлетворяющих заданному условию
    */
    class adjacent_find_fn
    {
    public:
        /** @brief Поиск соседних элементов, удовлетворяющих заданному условию
        @param s входная последовательность
        @param bin_pred бинарный предикат
        @return Последовательность @c r, полученная из
        <tt> ::ural::sequence_fwd<Forward>(s) </tt> продвижением до тех пор,
        пока первый непройденный и следующий за ним элементым данной
        последовательности не станут эквивалентными в смысле @c pred.
        */
        template <class Forward, class BinaryPredicate = ::ural::equal_to<>>
        auto operator()(Forward && s,
                        BinaryPredicate pred = BinaryPredicate()) const
        -> decltype(::ural::sequence_fwd<Forward>(s))
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequenced<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<BinaryPredicate, SequenceType<Forward>>));

            return this->impl(::ural::sequence_fwd<Forward>(s),
                              ::ural::make_callable(std::move(pred)));
        }
    private:
        template <class Forward, class BinaryPredicate>
        static Forward impl(Forward s, BinaryPredicate bin_pred)
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<BinaryPredicate, Forward>));

            if(!s)
            {
                return s;
            }

            auto s_next = ural::next(s);

            for(; !!s_next; ++ s_next)
            {
                if(bin_pred(*s, *s_next))
                {
                    return s;
                }
                s = s_next;
            }
            return s_next;
        }
    };

    /** @ingroup NonModifyingSequenceOperations
    @brief Класс функционального объекта для поиска первой пары соответствующих
    элементов последовательностей, которые не являются эквивалентными
    */
    class mismatch_fn
    {
    public:
        /** @brief Поиск пары несовпадающих соответствующих элементов
        @param in1 первая входная последовательность
        @param in2 вторая входная последовательность
        @param bin_pred бинарный предикат, по умолчанию используется
        <tt> ::ural::equal_to<> </tt>, то есть оператор "равно".
        @return Кортеж из двух последовательностей @c r1 и @c r2, полученных
        продвижением <tt> ::ural::sequence_fwd<Input1>(in1) </tt>
        и <tt> ::ural::sequence_fwd<Input1>(in2) </tt> соответственно синхронно
        до тех пор, пока их первые элементы не станут неэквивалентными в смысле
        @c bin_pred.
        */
        template <class Input1, class Input2,
                  class BinaryPredicate = ::ural::equal_to<>>
        auto operator()(Input1 && in1, Input2 && in2,
                        BinaryPredicate bin_pred = BinaryPredicate()) const
        -> tuple<decltype(::ural::sequence_fwd<Input1>(in1)),
                 decltype(::ural::sequence_fwd<Input2>(in2))>
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input1>));
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<BinaryPredicate,
                                                             SequenceType<Input1>,
                                                             SequenceType<Input2>>));

            return this->impl(::ural::sequence_fwd<Input1>(in1),
                              ::ural::sequence_fwd<Input2>(in2),
                              ::ural::make_callable(std::move(bin_pred)));
        }

    private:
        template <class Input1, class Input2, class BinaryPredicate>
        static tuple<Input1, Input2>
        impl(Input1 in1, Input2 in2, BinaryPredicate pred)
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input1>));
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<BinaryPredicate, Input1, Input2>));

            typedef tuple<Input1, Input2> Tuple;
            for(; !!in1 && !!in2; ++ in1, (void) ++ in2)
            {
                if(!pred(*in1, *in2))
                {
                    break;
                }
            }
            return Tuple{std::move(in1), std::move(in2)};
        }
    };

    /** @ingroup NonModifyingSequenceOperations
    @brief Класс функционального объекта проверки эквивалентности двух
    последовательностей
    */
    class equal_fn
    {
    public:
        /** @brief Проверка равенства двух последовательностей
        @param in1 первая входная последовательность
        @param in2 вторая входная последовательность
        @param bin_pred бинарный предикат, по умолчанию используется
        <tt> ::ural::equal_to<> </tt>, то есть оператор "равно"
        @return @b true, если последовательности @c in1 и @c in2 имеют равную
        длину и их соответствующие элементы удовлетворяют предикату @c bin_pred.
        */
        template <class Input1, class Input2,
                  class BinaryPredicate = ::ural::equal_to<>>
        bool operator()(Input1 && in1, Input2 && in2,
                        BinaryPredicate pred = BinaryPredicate()) const
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input1>));
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyComparable<SequenceType<Input1>,
                                                                 SequenceType<Input2>,
                                                                 BinaryPredicate>));

            return this->impl(::ural::sequence_fwd<Input1>(in1),
                              ::ural::sequence_fwd<Input2>(in2),
                              ::ural::make_callable(std::move(pred)));
        }
    private:
        template <class Input1, class Input2, class BinaryPredicate>
        static bool impl(Input1 in1, Input2 in2, BinaryPredicate pred)
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input1>));
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyComparable<Input1, Input2, BinaryPredicate>));

            auto const r = ural::mismatch_fn{}(std::move(in1), std::move(in2),
                                               std::move(pred));
            return !r[ural::_1] && !r[ural::_2];
        }
    };

    /** @ingroup NonModifyingSequenceOperations
    @brief Класс функционального объекта для поиска подпоследовательности
    */
    class search_fn
    {
    public:
        /** @brief Поиск подпоследовательности
        @param in последовательность
        @param s искомая подпоследовательность
        @param bin_pred бинарный предикат
        @return Последовательность, полученная из @c in продвижением до тех
        пора, пока @c s не станет эквивалентной в смысле @c pred префиксу её
        непройденной части.
        */
        template <class Forward1, class Forward2,
                  class BinaryPredicate = ::ural::equal_to<>>
        auto operator()(Forward1 && in, Forward2 && s,
                        BinaryPredicate bin_pred = BinaryPredicate()) const
        -> decltype(::ural::sequence_fwd<Forward1>(in))
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequenced<Forward1>));
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequenced<Forward2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyComparable<SequenceType<Forward1>,
                                                                 SequenceType<Forward2>,
                                                                 BinaryPredicate>));

            return this->impl(::ural::sequence_fwd<Forward1>(in),
                              ::ural::sequence_fwd<Forward2>(s),
                              ::ural::make_callable(std::move(bin_pred)));
        }

    private:
        template<class Forward1, class Forward2, class BinaryPredicate>
        static Forward1 impl(Forward1 in, Forward2 s, BinaryPredicate p)
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Forward1>));
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Forward2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyComparable<Forward1, Forward2, BinaryPredicate>));

            for(;; ++ in)
            {
                auto i = in;
                auto i_s = s;
                for(;; ++ i, (void) ++ i_s)
                {
                    if(!i_s)
                    {
                        return in;
                    }
                    if(!i)
                    {
                        return i;
                    }
                    if(!p(*i, *i_s))
                    {
                        break;
                    }
                }
            }
            assert(false);
        }
    };

    /** @ingroup NonModifyingSequenceOperations
    @brief Класс функционального объекта для поиска последнего вхождения
    подпоследовательности в последовательность
    */
    class find_end_fn
    {
    public:
        /** @brief Поиск последнего вхождения подпоследовательности
        @param in последовательность
        @param s подпоследовательность
        @param bin_pred бинарный предикат, по умолчанию используется
        <tt> equal_to<> </tt>, то есть оператор "равно"
        @return Последовательность @c r, полученная из
        <tt> ::ural::sequence_fwd<Forward1>(in) </tt> продвижением таким образом,
        что @c s является эквивалентом префикса непройденной части этой
        последовательности, причём @c s встречается в непройденной части
        этой последовательности ровно один раз.
        */
        template <class Forward1, class Forward2,
                  class BinaryPredicate = ::ural::equal_to<>>
        auto operator()(Forward1 && in, Forward2 && s,
                        BinaryPredicate bin_pred = BinaryPredicate()) const
        -> decltype(::ural::sequence_fwd<Forward1>(in))
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequenced<Forward1>));
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequenced<Forward2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<BinaryPredicate,
                                                             SequenceType<Forward1>,
                                                             SequenceType<Forward2>>));

            return this->impl(::ural::sequence_fwd<Forward1>(in),
                              ::ural::sequence_fwd<Forward2>(s),
                              ::ural::make_callable(std::move(bin_pred)));
        }
    private:
        template <class Forward1, class Forward2, class BinaryPredicate>
        static Forward1
        impl(Forward1 in, Forward2 s, BinaryPredicate bin_pred)
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Forward1>));
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Forward2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<BinaryPredicate, Forward1, Forward2>));

            if(!s)
            {
                return in;
            }

            auto result = ::ural::search_fn{}(in, s, bin_pred);;
            auto new_result = result;

            for(;;)
            {
                if(!new_result)
                {
                    return result;
                }
                else
                {
                    result = std::move(new_result);
                    in = result;
                    ++ in;
                    new_result = ::ural::search_fn{}(in, s, bin_pred);
                }
            }
            return result;
        }
    };

    /** @ingroup NonModifyingSequenceOperations
    @brief Функциональный объект поиска подпоследовательности одинаковых
    элементов заданной длины
    */
    class search_n_fn
    {
    public:
        /** @brief Поиска подпоследовательности одинаковых значений заданной
        длины.
        @param in входная последовательность
        @param count длина подполседовательности
        @param value значение
        @return Последовательность, полученная из @c in продвижением до тех пор,
        пока её префикс не будет содержать подряд @c count элементов,
        эквивалентных @c value.
        */
        template <class Forward, class T,
                  class BinaryPredicate = ::ural::equal_to<>>
        auto operator()(Forward && in,
                        DifferenceType<SequenceType<Forward>> count,
                        T const & value,
                        BinaryPredicate bin_pred = BinaryPredicate()) const
        -> decltype(::ural::sequence_fwd<Forward>(in))
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequenced<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyComparable<SequenceType<Forward>,
                                                                 T const *,
                                                                 BinaryPredicate>));

            return this->impl(::ural::sequence_fwd<Forward>(in),
                              std::move(count), value,
                              ::ural::make_callable(std::move(bin_pred)));
        }

    private:
        template <class Forward, class T,  class BinaryPredicate>
        static Forward impl(Forward in,
                            DifferenceType<Forward> const n,
                            T const & value,
                            BinaryPredicate bin_pred)
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyComparable<Forward, T const *, BinaryPredicate>));

            if(n == 0)
            {
                return in;
            }

            for(; !!in; ++ in)
            {
                if(!bin_pred(*in, value))
                {
                    continue;
                }

                auto candidate = in;
                auto cur_count = DifferenceType<Forward>{0};

                while(true)
                {
                    ++ cur_count;
                    if(cur_count == n)
                    {
                        return candidate;
                    }
                    ++ in;
                    if(!in)
                    {
                        return in;
                    }
                    if(!bin_pred(*in, value))
                    {
                        break;
                    }
                }
            }
            return in;
        }
    };

    // Модифицирующие последовательность алгоритмы
    /** @ingroup MutatingSequenceOperations
    @brief Класс функционального объекта копирования заданного количества
    элементов одной последовательности в другую последовательность
    @todo Что делать, если было скопировано менее @c n элементов?
    */
    class copy_n_fn
    {
    public:
        /** @brief Копирование заданного количества элементов из @c in в
        @c out по порядку.
        @param in входная последовательность
        @param n количество элементов, которые должны быть скопированы
        @param out выходная последовательность
        @return Кортеж, состоящий из непройденных частей последовательностей.
        */
        template <class Input, class Size, class Output>
        auto operator()(Input && in, Size n, Output && out) const
        -> tuple<decltype(::ural::sequence_fwd<Input>(in)),
                 decltype(::ural::sequence_fwd<Output>(out))>
        {
            // @todo проверки концепций

            auto in_n = ::ural::sequence_fwd<Input>(in) | ural::taken(n);

            auto result = ::ural::copy_fn{}(std::move(in_n),
                                            ::ural::sequence_fwd<Output>(out));
            return ural::make_tuple(std::move(result[ural::_1]).base(),
                                    std::move(result[ural::_2]));
        }
    };

    /** @ingroup MutatingSequenceOperations
    @brief Класс функционального объекта копирования элементов одной
    последовательности, которые удовлетворяют заданному предикату, в другую
    последовательность.
    */
    class copy_if_fn
    {
    public:
        /** @brief Копирование элементов одной последовательности,
        удовлетворяющих заданному предикату, в другую последовательность по
        порядку.
        @param in входная последовательность
        @param out выходная последовательность
        @param pred унарный предикат
        @return Кортеж, состоящий из непройденных частей последовательностей.
        */
        template <class Input, class Output, class Predicate>
        auto operator()(Input && in, Output && out, Predicate pred) const
        -> tuple<decltype(::ural::sequence_fwd<Input>(in)),
                 decltype(::ural::sequence_fwd<Output>(out))>
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input>));
            BOOST_CONCEPT_ASSERT((concepts::SinglePassSequenced<Output>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectPredicate<Predicate, SequenceType<Input>>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyCopyable<SequenceType<Input>,
                                                               SequenceType<Output>>));

            auto in_f = ::ural::sequence_fwd<Input>(in) | ural::filtered(pred);
            auto res = ural::copy_fn{}(std::move(in_f),
                                       ::ural::sequence_fwd<Output>(out));

            return ural::make_tuple(std::move(res[ural::_1]).base(),
                                    std::move(res[ural::_2]));
        }
    };

    // copy_backward
    /** @ingroup MutatingSequenceOperations
    @brief Класс функционального объекта копирования элементов одной
    последовательности в другую последовательность, начиная с последнего.
    */
    class copy_backward_fn
    {
    public:
        /** @brief Копирует элементы последовательности @c in в @c out в
        обратном порядке, пока одна из них не будет исчерпана.
        @param in входная последовательность
        @param out выходная последовательность
        @return Кортеж, содержащий непройденные части входной и выходной
        последовательностей (одна из них будет пустой).
        */
        template <class Bidir1, class Bidir2>
        auto operator()(Bidir1 && in, Bidir2 && out) const
        -> tuple<decltype(::ural::sequence_fwd<Bidir1>(in)),
                 decltype(::ural::sequence_fwd<Bidir2>(out))>
        {
            BOOST_CONCEPT_ASSERT((concepts::BidirectionalSequenced<Bidir1>));
            BOOST_CONCEPT_ASSERT((concepts::BidirectionalSequenced<Bidir2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyCopyable<SequenceType<Bidir1>,
                                                               SequenceType<Bidir2>>));

            return this->impl(::ural::sequence_fwd<Bidir1>(in),
                              ::ural::sequence_fwd<Bidir2>(out));
        }

    private:
        template <class Bidir1, class Bidir2>
        tuple<Bidir1, Bidir2>
        impl(Bidir1 in, Bidir2 out) const
        {
            BOOST_CONCEPT_ASSERT((concepts::BidirectionalSequence<Bidir1>));
            BOOST_CONCEPT_ASSERT((concepts::BidirectionalSequence<Bidir2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyCopyable<Bidir1, Bidir2>));

            auto res = ural::copy_fn{}(std::move(in) | ural::reversed,
                                       std::move(out) | ural::reversed);

            return ural::make_tuple(std::move(res[ural::_1].base()),
                                    std::move(res[ural::_2].base()));
        }
    };

    // move
    /** @ingroup MutatingSequenceOperations
    @brief Класс функционального объекта перемещения элементов одной
    последовательности в другую последовательность
    */
    class move_fn
    {
    public:
        /** @brief Перемещение элементов последовательности @c in в
        последовательность @c out по очереди, пока одна из них не будет
        исчерпана.
        @param in входная последовательность
        @param out выходная последовательность
        @return Кортеж, содержащий непройденные части входной и выходной
        последовательностей (одна из них будет пустой).
        */
        template <class Input, class Output>
        auto operator()(Input && in, Output && out) const
        -> tuple<decltype(::ural::sequence_fwd<Input>(in)),
                 decltype(::ural::sequence_fwd<Output>(out))>
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input>));
            BOOST_CONCEPT_ASSERT((concepts::SinglePassSequenced<Output>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyMovable<SequenceType<Input>,
                                                              SequenceType<Output>>));

            auto in_moved = ::ural::sequence_fwd<Input>(in) | ural::moved;
            auto res = ural::copy_fn{}(std::move(in_moved),
                                       ::ural::sequence_fwd<Output>(out));
            return ural::make_tuple(res[ural::_1].base(), res[ural::_2]);
        }
    };

    // move_backward
    /** @ingroup MutatingSequenceOperations
    @brief Класс функционального объекта перемещения элементов одной
    последовательности в другую последовательность, начиная с последнего.
    */
    class move_backward_fn
    {
    public:
        /** @brief Перемещает элементы последовательности @c in в @c out в
        обратном порядке, пока одна из них не будет исчерпана.
        @param in входная последовательность
        @param out выходная последовательность
        @return Кортеж, содержащий непройденные части входной и выходной
        последовательностей (одна из них будет пустой).
        */
        template <class Bidir1, class Bidir2>
        auto operator()(Bidir1 && in, Bidir2 && out) const
        -> tuple<decltype(::ural::sequence_fwd<Bidir1>(in)),
                 decltype(::ural::sequence_fwd<Bidir2>(out))>
        {
            BOOST_CONCEPT_ASSERT((concepts::BidirectionalSequenced<Bidir1>));
            BOOST_CONCEPT_ASSERT((concepts::BidirectionalSequenced<Bidir2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyMovable<SequenceType<Bidir1>,
                                                              SequenceType<Bidir2>>));

            return this->impl(::ural::sequence_fwd<Bidir1>(in),
                              ::ural::sequence_fwd<Bidir2>(out));
        }

    private:
        template <class Bidir1, class Bidir2>
        tuple<Bidir1, Bidir2>
        impl(Bidir1 in, Bidir2 out) const
        {
            BOOST_CONCEPT_ASSERT((concepts::BidirectionalSequence<Bidir1>));
            BOOST_CONCEPT_ASSERT((concepts::BidirectionalSequence<Bidir2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyMovable<Bidir1, Bidir2>));

            auto res = ural::move_fn{}(std::move(in) | ural::reversed | ural::moved,
                                       std::move(out) | ural::reversed);

            return ural::make_tuple(std::move(res[ural::_1].base().base()),
                                    std::move(res[ural::_2].base()));
        }
    };

    // 25.3.4
    /** @ingroup MutatingSequenceOperations
    @brief Класс функционального объекта преобразования элементов одной или
    двух входных последовательностей и записи полученных значений в другую
    последовательность.
    */
    class transform_fn
    {
    public:
        /** @brief Преобразование последовательности
        @param in входная последовательность
        @param out выходная последовательность
        @param f унарная функция
        @return Кортеж, содержащий непройденные части входной и выходной
        последовательностей (по меньшей мере одна из них будет пуста)
        */
        template <class Input, class Output, class UnaryFunction>
        auto operator()(Input && in, Output && out, UnaryFunction f) const
        -> tuple<decltype(::ural::sequence_fwd<Input>(in)),
                 decltype(::ural::sequence_fwd<Output>(out))>
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectCallable<UnaryFunction,
                                                             SequenceType<Input>>));

            typedef IndirectCallableResultType<UnaryFunction, SequenceType<Input>> F_result;
            BOOST_CONCEPT_ASSERT((concepts::Sequenced<Output>));
            BOOST_CONCEPT_ASSERT((concepts::OutputSequence<SequenceType<Output>, F_result>));

            return this->impl(::ural::sequence_fwd<Input>(in),
                              ::ural::sequence_fwd<Output>(out),
                              ::ural::make_callable(std::move(f)));
        }

        /** @brief Преобразование двух последовательностей
        @param in1 первая входная последовательность
        @param in2 вторая входная последовательность
        @param out выходная последовательность
        @param f бинарная функция
        @return Кортеж, содержащий непройденные части входных и выходной
        последовательностей (по меньшей мере одна из них будет пуста)
        */
        template <class Input1, class Input2, class Output, class BinaryFunction>
        auto operator()(Input1 && in1, Input2 && in2, Output && out,
                        BinaryFunction f) const
        -> tuple<decltype(::ural::sequence_fwd<Input1>(in1)),
                 decltype(::ural::sequence_fwd<Input2>(in2)),
                 decltype(::ural::sequence_fwd<Output>(out))>
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input1>));
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectCallable<BinaryFunction,
                                                             SequenceType<Input1>,
                                                             SequenceType<Input2>>));

            typedef IndirectCallableResultType<BinaryFunction, SequenceType<Input1>,
                                                SequenceType<Input2>> F_result;

            BOOST_CONCEPT_ASSERT((concepts::Sequenced<Output>));
            BOOST_CONCEPT_ASSERT((concepts::OutputSequence<SequenceType<Output>, F_result>));

            return this->impl(::ural::sequence_fwd<Input1>(in1),
                              ::ural::sequence_fwd<Input2>(in2),
                              ::ural::sequence_fwd<Output>(out),
                              ::ural::make_callable(std::move(f)));
        }

    private:
        template <class Input, class Output, class UnaryFunction>
        tuple<Input, Output>
        impl(Input in, Output out, UnaryFunction f) const
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectCallable<UnaryFunction, Input>));

            typedef IndirectCallableResultType<UnaryFunction, Input> F_result;
            BOOST_CONCEPT_ASSERT((concepts::OutputSequence<Output, F_result>));

            auto f_in = ural::make_transform_sequence(std::move(f), std::move(in));

            auto r = copy_fn{}(std::move(f_in), std::move(out));

            typedef tuple<Input, Output> Tuple;

            return Tuple{std::move(r[ural::_1].bases()[ural::_1]),
                         std::move(r[ural::_2])};
        }

        template <class Input1, class Input2, class Output, class BinaryFunction>
        tuple<Input1, Input2, Output>
        impl(Input1 in1, Input2 in2, Output out, BinaryFunction f) const
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input1>));
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectCallable<BinaryFunction, Input1, Input2>));

            typedef IndirectCallableResultType<BinaryFunction, Input1, Input2> F_result;
            BOOST_CONCEPT_ASSERT((concepts::OutputSequence<Output, F_result>));

            auto f_in = ural::make_transform_sequence(std::move(f),
                                                      std::move(in1),
                                                      std::move(in2));

            auto r = copy_fn{}(std::move(f_in), std::move(out));

            typedef tuple<Input1, Input2, Output> Tuple;

            return Tuple{std::move(r[ural::_1].bases()[ural::_1]),
                         std::move(r[ural::_1].bases()[ural::_2]),
                         std::move(r[ural::_2])};
        }
    };

    /** @ingroup MutatingSequenceOperations
    @brief Класс функционального объекта заполнения последовательности
    результатами вызова заднной функции без параметров
    */
    class generate_fn
    {
    public:
        /** @brief Заполнение последовательности результатами вызова заднной
        функции без параметров
        @param seq последовательность
        @param gen генератор, то есть функция без параметров
        @return Последовательность, полученная из @c seq продвижением до
        исчерпания.
        */
        template <class Output, class Generator>
        auto operator()(Output && seq, Generator gen) const
        -> decltype(::ural::sequence_fwd<Output>(seq))
        {
            BOOST_CONCEPT_ASSERT((concepts::Function<Generator>));
            BOOST_CONCEPT_ASSERT((concepts::SinglePassSequenced<Output>));
            BOOST_CONCEPT_ASSERT((concepts::OutputSequence<SequenceType<Output>,
                                                           ResultType<Generator>>));

            return this->impl(::ural::sequence_fwd<Output>(seq),
                              ::ural::make_callable(std::move(gen)));
        }

    private:
        template <class Output, class Generator>
        static Output
        impl(Output seq, Generator gen)
        {
            BOOST_CONCEPT_ASSERT((concepts::Function<Generator>));
            BOOST_CONCEPT_ASSERT((concepts::OutputSequence<Output, ResultType<Generator>>));

            auto r = copy_fn{}(::ural::make_generator_sequence(std::move(gen)),
                               std::move(seq));
            return r[ural::_2];
        }
    };

    /** @ingroup MutatingSequenceOperations
    @brief Класс функционального объекта для присваивания заданному количеству
    элементов последовательности результатами вызова заднной функции без
    параметров.
    */
    class generate_n_fn
    {
    public:
        /** @brief Присваивает заданному количеству элементов последовательности
        результаты вызова <tt> gen() </tt>.
        @param gen фунцкия, которая может быть вызвана без аргументов
        @param n количество
        @param out выходная последовательность
        @return Непройденная часть @c out
        @todo Что делать, если посещено менее @c n элементов?
        */
        template <class Generator, class Output>
        auto operator()(Output && out,
                        DifferenceType<SequenceType<Output>> n,
                        Generator gen) const
        -> decltype(::ural::sequence_fwd<Output>(out))
        {
            BOOST_CONCEPT_ASSERT((concepts::Function<Generator>));
            BOOST_CONCEPT_ASSERT((concepts::SinglePassSequenced<Output>));
            BOOST_CONCEPT_ASSERT((concepts::OutputSequence<SequenceType<Output>,
                                                           ResultType<Generator>>));

            auto in = ural::make_generator_sequence(::ural::make_callable(gen));
            return ::ural::copy_n_fn{}(::std::move(in), std::move(n),
                                       ::ural::sequence_fwd<Output>(out))[ural::_2];
        }
    };

    /** @ingroup MutatingSequenceOperations
    @brief Класс функционального объекта заполнения последовательности
    заданными значениями
    */
    class fill_fn
    {
    public:
        /** @brief Присваивает всем элементам последовательности заданное
        значение
        @param seq последовательность
        @param value значение
        @return Последовательность, полученная из @c seq продвижением до
        исчерпания.
        */
        template <class Output, class T>
        auto operator()(Output && seq, T const & value) const
        -> decltype(::ural::sequence_fwd<Output>(seq))
        {
            BOOST_CONCEPT_ASSERT((concepts::Semiregular<T>));
            BOOST_CONCEPT_ASSERT((concepts::Sequenced<Output>));
            BOOST_CONCEPT_ASSERT((concepts::OutputSequence<SequenceType<Output>, T>));

            return this->impl(::ural::sequence_fwd<Output>(seq), value);
        }

    private:
        template <class Output, class T>
        static Output
        impl(Output seq, T const & value)
        {
            BOOST_CONCEPT_ASSERT((concepts::Semiregular<T>));
            BOOST_CONCEPT_ASSERT((concepts::OutputSequence<Output, T>));

            return generate_fn{}(std::move(seq),
                                 ural::value_function<T const &>(value));
        }
    };

    /** @ingroup MutatingSequenceOperations
    @brief Класс функционального объекта для присваивания заданному количеству
    элементов последовательности заданного значения.
    */
    class fill_n_fn
    {
    public:
        /** @brief Присваивает заданному количеству элементов последовательности
        значение @c value
        @param out выходная последовательность
        @param n количество элементов
        @param value значение, которое должно быть присвоено элементам
        @return Непройденная часть @c out
        */
        template <class Output, class T>
        auto operator()(Output && out,
                        DifferenceType<SequenceType<Output>> n,
                        T const & value) const
        -> decltype(::ural::sequence_fwd<Output>(out))
        {
            BOOST_CONCEPT_ASSERT((concepts::Semiregular<T>));
            BOOST_CONCEPT_ASSERT((concepts::Sequenced<Output>));
            BOOST_CONCEPT_ASSERT((concepts::OutputSequence<SequenceType<Output>, T>));

            auto gen = ::ural::value_function<T const &>(value);
            return ::ural::generate_n_fn{}(std::forward<Output>(out),
                                           std::move(n), std::move(gen));
        }
    };

    /** @ingroup MutatingSequenceOperations
    @brief Тип функционального объекта копирования неповторяющихся значений
    */
    class unique_copy_fn
    {
    public:
        /** @brief Копирование неповторяющихся (подряд) значений
        @param in входная последовательность
        @param out выходная последовательность
        @param bin_pred бинарный предикат
        @return Кортеж, содержащий непройденные части последовательностей
        */
        template <class Input, class Output,
                  class BinaryPredicate = ural::equal_to<>>
        auto operator()(Input && in, Output && out,
                        BinaryPredicate bin_pred = BinaryPredicate()) const
        -> ural::tuple<decltype(::ural::sequence_fwd<Input>(in)),
                       decltype(::ural::sequence_fwd<Output>(out))>
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input>));
            BOOST_CONCEPT_ASSERT((concepts::SinglePassSequenced<Output>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<BinaryPredicate,
                                                             SequenceType<Input>>));

            auto u_in = ::ural::make_unique_sequence(std::forward<Input>(in),
                                                     std::move(bin_pred));
            auto r = ::ural::copy_fn{}(std::move(u_in),
                                       std::forward<Output>(out));
            return ::ural::make_tuple(std::move(r[ural::_1]).base(),
                                      std::move(r[ural::_2]));
        }
    };

    /** @ingroup MutatingSequenceOperations
    @brief Тип функционального объекта обмена соответствующих элементов двух
    последовательностей
    */
    class swap_ranges_fn
    {
    public:
        /** @brief Обмен соответствующих элементов последовательностей
        @param s1, s2 входные последовательности
        @return Кортеж, содержащий непройденные части последовательностей
        */
        template <class Forward1, class Forward2>
        auto operator()(Forward1 && s1, Forward2 && s2) const
        -> ural::tuple<decltype(::ural::sequence_fwd<Forward1>(s1)),
                       decltype(::ural::sequence_fwd<Forward2>(s2))>
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequenced<Forward1>));
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequenced<Forward2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlySwappable<SequenceType<Forward1>,
                                                                SequenceType<Forward2>>));

            return this->impl(::ural::sequence_fwd<Forward1>(s1),
                              ::ural::sequence_fwd<Forward2>(s2));
        }
    private:
        template <class Forward1, class Forward2>
        static ural::tuple<Forward1, Forward2>
        impl(Forward1 in1, Forward2 in2)
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Forward1>));
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Forward2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlySwappable<Forward1, Forward2>));

            for(; !!in1 && !!in2; ++ in1, (void) ++ in2)
            {
                ::ural::details::do_swap(*in1, *in2);
            }
            return ural::tuple<Forward1, Forward2>{in1, in2};
        }
    };

    /** @ingroup MutatingSequenceOperations
    @brief Тип функционального объекта обращения последовательности
    @todo Отдельный алгоритм для последовательностей с произвольным доступом
    */
    class reverse_fn
    {
    public:
        /** @brief Обращение последовательности
        @param seq последовательность
        @return Последовательность, полученна из
        <tt> ::ural::sequence_fwd<ForwardSequenced>(seq) </tt> продвижением до
        исчерпания.
        */
        template <class ForwardSequenced>
        auto operator()(ForwardSequenced && seq) const
        -> decltype(::ural::sequence_fwd<ForwardSequenced>(seq))
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequenced<ForwardSequenced>));
            BOOST_CONCEPT_ASSERT((concepts::Permutable<SequenceType<ForwardSequenced>>));

            auto s = ::ural::sequence_fwd<ForwardSequenced>(seq);

            return this->impl(std::move(s), ::ural::make_traversal_tag(s));
        }

    private:
        template <class ForwardSequence>
        void impl_n(ForwardSequence seq,
                    DifferenceType<ForwardSequence> n) const
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<decltype(seq)>));
            BOOST_CONCEPT_ASSERT((concepts::Permutable<decltype(seq)>));

            // Возможная оптимизация - попытаться выделить доп. память
            // при этом лучше сначала разворачивать, а потом обменивать?
            while(n > 1)
            {
                auto const n1 = n / 2;
                auto const n2 = n - n1;

                auto s2 = ::ural::next(seq, n2);

                // size(s2) = size(seq) - n2 = n - n2 = n1
                ::ural::swap_ranges_fn{}(seq | ural::taken(n1),
                                         s2  | ural::taken(n1))[ural::_2];

                this->impl_n(std::move(s2), n1);

                n = n1;
            }
        }

        template <class ForwardSequence>
        ForwardSequence impl(ForwardSequence seq, forward_traversal_tag) const
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<decltype(seq)>));
            BOOST_CONCEPT_ASSERT((concepts::Permutable<decltype(seq)>));

            // @todo Выделить алгоритм?
            DifferenceType<ForwardSequence> n = 0;
            auto result = seq;

            for(; !!result; ++ result)
            {
                ++ n;
            }

            this->impl_n(std::move(seq), std::move(n));

            return result;
        }

        template <class BidirectionalSequence>
        static BidirectionalSequence
        impl(BidirectionalSequence seq, bidirectional_traversal_tag)
        {
            BOOST_CONCEPT_ASSERT((concepts::BidirectionalSequence<decltype(seq)>));
            BOOST_CONCEPT_ASSERT((concepts::Permutable<decltype(seq)>));

            auto result = ::ural::exhaust_front(seq);

            for(; !!seq; ++seq)
            {
                auto seq_next = seq;
                seq_next.pop_back();

                if(!seq_next)
                {
                    break;
                }
                else
                {
                   ::ural::details::do_swap(*seq, seq.back());
                }
                seq = seq_next;
            }

            return result;
        }
    };

    /** @ingroup MutatingSequenceOperations
    @brief Тип функционального объекта копирования элементов последовательности
    в обратном порядке.
    */
    class reverse_copy_fn
    {
    public:
        /** @brief Копирование последовательности в обратном порядке
        @param in входная последовательность
        @param out выходная последовательность
        @return Кортеж, содержащий непройденные части последовательностей
        */
        template <class Bidirectional, class Output>
        auto operator()(Bidirectional && in, Output && out) const
        -> ural::tuple<decltype(::ural::sequence_fwd<Bidirectional>(in)),
                       decltype(::ural::sequence_fwd<Output>(out))>
        {
            BOOST_CONCEPT_ASSERT((concepts::BidirectionalSequenced<Bidirectional>));
            BOOST_CONCEPT_ASSERT((concepts::SinglePassSequenced<Output>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyCopyable<SequenceType<Bidirectional>,
                                                               SequenceType<Output>>));

            auto in_reversed = ::ural::sequence_fwd<Bidirectional>(in)
                             | ural::reversed;
            auto result = ural::copy_fn{}(std::move(in_reversed),
                                          ::ural::sequence_fwd<Output>(out));
            return ural::make_tuple(std::move(result[ural::_1]).base(),
                                    std::move(result[ural::_2]));
        }
    };

    /** @ingroup MutatingSequenceOperations
    @brief Тип функционального объекта "поворота" последовательности
    */
    class rotate_fn
    {
    public:
        /** @brief Обменивает местами переднюю пройденную и непройденную части
        последовательности.
        @param seq последовательность
        @post Обменивает местами переднюю пройденную и непройденную части
        последовательности.
        @return Последовательность @c r такая, что
        <tt> original(r) == original(seq) </tt> и
        <tt> rotate_fn{}(r) </tt> приведёт к тому, что будет получена исходная
        последовательность.
        */
        template <class ForwardSequence>
        auto operator()(ForwardSequence && seq) const
        -> decltype(::ural::sequence_fwd<ForwardSequence>(seq))
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequenced<ForwardSequence>));
            BOOST_CONCEPT_ASSERT((concepts::Permutable<SequenceType<ForwardSequence>>));

            return this->impl(::ural::sequence_fwd<ForwardSequence>(seq));
        }

        template <class Forward1, class Forward2>
        auto operator()(Forward1 && in1, Forward2 && in2) const
        -> ural::tuple<decltype(::ural::sequence_fwd<Forward1>(in1)),
                       decltype(::ural::sequence_fwd<Forward2>(in2))>
        {
            // @todo Проверка концепций
            return this->impl(::ural::sequence_fwd<Forward1>(in1),
                              ::ural::sequence_fwd<Forward2>(in2));
        }

    private:
        template <class Forward1, class Forward2>
        ural::tuple<Forward1, Forward2>
        impl(Forward1 in1, Forward2 in2) const
        {
            // @todo Проверка концепций

            in1.shrink_front();
            in2.shrink_front();

            if(!in1 || !in2)
            {
                return ural::tuple<Forward1, Forward2>{std::move(in1),
                                                       std::move(in2)};
            }

            auto r = ::ural::swap_ranges_fn{}(in1, in2);

            if(!r[ural::_1] && !r[ural::_2])
            {
                return r;
            }
            else if(!r[ural::_1])
            {
                assert(!r[ural::_1]);
                return this->impl(r[ural::_2].traversed_front(),
                                  ::ural::shrink_front(r[ural::_2]));
            }
            else
            {
                assert(!r[ural::_2]);
                return this->impl(::ural::shrink_front(r[ural::_1]), in2);
            }
        }

        template <class ForwardSequence>
        ForwardSequence impl(ForwardSequence seq) const
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<ForwardSequence>));
            BOOST_CONCEPT_ASSERT((concepts::Permutable<ForwardSequence>));

            auto seq_old = seq.original();

            this->impl(seq.traversed_front(), ural::shrink_front(seq));

            ural::advance(seq_old, seq.size());
            return seq_old;
        }
    };

    /** @ingroup MutatingSequenceOperations
    @brief Тип функционального объекта копирования элементов последовательности
    с поворотом.
    */
    class rotate_copy_fn
    {
    public:
        /** @brief Копирование "повёрной" последовательности
        @param in входная последовательность
        @param out выходная последовательность
        @return Кортеж, содержащий непройденные части последовательностей
        */
        template <class Forward, class Output>
        auto operator()(Forward && in, Output && out) const
        -> ural::tuple<decltype(::ural::sequence_fwd<Forward>(in)),
                       decltype(::ural::sequence_fwd<Output>(out))>
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequenced<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::SinglePassSequenced<Output>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyCopyable<SequenceType<Forward>,
                                                               SequenceType<Output>>));

            return this->impl(::ural::sequence_fwd<Forward>(in),
                              ::ural::sequence_fwd<Output>(out));
        }

    private:
        template <class Forward, class Output>
        static ural::tuple<Forward, Output>
        impl(Forward in, Output out)
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::SinglePassSequence<Output>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyCopyable<Forward, Output>));

            auto const n = ural::size(in);
            auto in_orig = ural::next(in.original(), n);

            auto in_1 = in.traversed_front();
            auto r1 = copy_fn{}(std::move(in), std::move(out));
            auto r2 = copy_fn{}(std::move(in_1), std::move(r1[ural::_2]));

            return ural::tuple<Forward, Output>{std::move(in_orig),
                                                std::move(r2[ural::_2])};
        }
    };

    /** @ingroup MutatingSequenceOperations
    @brief Тип функционального объекта для замены элементов последовательности,
    которые удовлетворяют заданному предикату, на новое значение
    */
    class replace_if_fn
    {
    public:
        /** @brief Замена элементов, удовлетворяющих предикату, новым значением
        @param seq последовательность
        @param pred унарный предикат
        @param new_value новое значение
        @post Всем элементам @c x последовательности @c seq, удовлетворяющим
        предикату @c pred, присваивается значение @c new_value.
        @return Последовательность, полученная из
        <tt> ::ural::sequence_fwd<ForwardSequenced>(seq) </tt> продвижением до
        исчерпания.
        */
        template <class ForwardSequenced, class Predicate, class T>
        auto operator()(ForwardSequenced && seq,
                        Predicate pred, T const & new_value) const
        -> decltype(::ural::sequence_fwd<ForwardSequenced>(seq))
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequenced<ForwardSequenced>));
            BOOST_CONCEPT_ASSERT((concepts::Semiregular<T>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectPredicate<Predicate, SequenceType<ForwardSequenced>>));
            BOOST_CONCEPT_ASSERT((concepts::Writable<SequenceType<ForwardSequenced>, T>));

            return this->impl(::ural::sequence_fwd<ForwardSequenced>(seq),
                              ::ural::make_callable(std::move(pred)), new_value);
        }

    private:
        template <class ForwardSequence, class Predicate, class T>
        static ForwardSequence
        impl(ForwardSequence seq, Predicate pred, T const & new_value)
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<ForwardSequence>));
            BOOST_CONCEPT_ASSERT((concepts::Semiregular<T>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectPredicate<Predicate, ForwardSequence>));
            BOOST_CONCEPT_ASSERT((concepts::Writable<ForwardSequence, T>));

            for(; !!seq; ++ seq)
            {
                if(pred(*seq))
                {
                    *seq = new_value;
                }
            }

            return seq;
        }
    };

    /** @ingroup MutatingSequenceOperations
    @brief Тип функционального объекта для замены элементов последовательности,
    эквивалентных заданному значению, на новое значение.
    */
    class replace_fn
    {
    public:
        /** @brief Замена элементов, эквивалентных заданному значению, нового
        значения
        @param seq последовательность
        @param old_value значение, которое нужно заменить.
        @param new_value новое значение
        @param bin_pred бинарный предикат
        @post Всем элементам @c x последовательности @c seq, эквивалентных
        @c old_value в смысле предиката @c bin_pred, присваивается значение
        @c new_value.
        @return Последовательность, полученная из
        <tt> ::ural::sequence_fwd<ForwardSequence>(seq) </tt> продвижением до
        исчерпания.
        */
        template <class ForwardSequenced, class T1, class T2,
                  class BinaryPredicate = ::ural::equal_to<>>
        auto operator()(ForwardSequenced && seq,
                        T1 const & old_value, T2 const & new_value,
                        BinaryPredicate bin_pred = BinaryPredicate()) const
        -> decltype(::ural::sequence_fwd<ForwardSequenced>(seq))
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequenced<ForwardSequenced>));
            BOOST_CONCEPT_ASSERT((concepts::Semiregular<T2>));
            BOOST_CONCEPT_ASSERT((concepts::Writable<SequenceType<ForwardSequenced>, T2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<BinaryPredicate,
                                                             SequenceType<ForwardSequenced>,
                                                             T1 const *>));

            return this->impl(::ural::sequence_fwd<ForwardSequenced>(seq),
                              old_value, new_value,
                              ::ural::make_callable(std::move(bin_pred)));
        }

    private:
        template <class ForwardSequence, class T1, class T2, class BinaryPredicate>
        static ForwardSequence
        impl(ForwardSequence seq, T1 const & old_value, T2 const & new_value,
             BinaryPredicate bin_pred)
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<ForwardSequence>));
            BOOST_CONCEPT_ASSERT((concepts::Semiregular<T2>));
            BOOST_CONCEPT_ASSERT((concepts::Writable<ForwardSequence, T2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<BinaryPredicate, ForwardSequence, T1 const *>));

            auto const pred = std::bind(std::move(bin_pred), ural::_1,
                                        std::cref(old_value));

            return ::ural::replace_if_fn{}(std::move(seq), std::move(pred),
                                           new_value);
        }
    };

    /** @ingroup MutatingSequenceOperations
    @brief Тип функционального объекта копирования элементов последовательности
    с заменой элементов, которые удовлетворяют заданному предикату, на новое
    значение.
    */
    class replace_copy_if_fn
    {
    public:
        /** @brief Копирование последовательности с заменой элементов,
        удовлетворяющих заданному предикату
        @param in входная последовательность
        @param out выходная последовательность
        @param pred унарный предикат, определяющий, какие элементы нужно
        заменить
        @param new_value значение, на которое нужно заменить элементы,
        удовлетворяющие предикату @c pred.
        @return Кортеж, содержащий непройденные части последовательностей
        */
        template <class Input, class Output, class Predicate, class T>
        auto operator()(Input && in, Output && out, Predicate pred,
                        T const & new_value) const
        -> ural::tuple<decltype(::ural::sequence_fwd<Input>(in)),
                       decltype(::ural::sequence_fwd<Output>(out))>
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input>));
            BOOST_CONCEPT_ASSERT((concepts::Semiregular<T>));
            BOOST_CONCEPT_ASSERT((concepts::SinglePassSequenced<Output>));
            BOOST_CONCEPT_ASSERT((concepts::Writable<SequenceType<Output>, T>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectPredicate<Predicate,
                                                              SequenceType<Input>>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyCopyable<SequenceType<Input>,
                                                               SequenceType<Output>>));

            return this->impl(::ural::sequence_fwd<Input>(in),
                              ::ural::sequence_fwd<Output>(out),
                              ::ural::make_callable(std::move(pred)),
                              new_value);
        }

    private:
        template <class Input, class Output, class Predicate, class T>
        static tuple<Input, Output>
        impl(Input in, Output out, Predicate pred, T const & new_value)
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input>));
            BOOST_CONCEPT_ASSERT((concepts::Semiregular<T>));
            BOOST_CONCEPT_ASSERT((concepts::SinglePassSequence<Output>));
            BOOST_CONCEPT_ASSERT((concepts::Writable<Output, T>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyCopyable<Input, Output>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectPredicate<Predicate, Input>));

            auto in_r = ural::make_replace_if_sequence(std::move(in),
                                                       std::move(pred),
                                                       std::cref(new_value));
            auto r = ural::copy_fn{}(std::move(in_r), std::move(out));

            return ural::make_tuple(std::move(r[ural::_1]).bases()[ural::_1],
                                    std::move(r[ural::_2]));
        }
    };

    /** @ingroup MutatingSequenceOperations
    @brief Тип функционального объекта копирования элементов последовательности
    с заменой элементов, эквивалентных заданному значению, на новое значение.
    */
    class replace_copy_fn
    {
    public:
        /** @brief Копирование с заменой
        @details Копирование с заменой элементов @c x для которых выполняется
        условие <tt> bin_pred(x, old_value) </tt> на @c new_value
        @param in входная последовательность
        @param out выходная последовательность
        @param old_value значение, которое нужно заменить
        @param new_value значение, на которое нужно заменить @c old_value
        @param bin_pred бинарный предикат, если он не задан, то используется
        <tt> equal_to<> </tt>, то есть оператор "равно"
        @return Кортеж, содержащий непройденные части последовательностей
        */
        template <class Input, class Output, class T1, class T2,
                  class BinaryPredicate = ::ural::equal_to<>>
        auto operator()(Input && in, Output && out, T1 const & old_value,
                        T2 const & new_value,
                        BinaryPredicate bin_pred = BinaryPredicate()) const
        -> ural::tuple<decltype(::ural::sequence_fwd<Input>(in)),
                       decltype(::ural::sequence_fwd<Output>(out))>
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input>));
            BOOST_CONCEPT_ASSERT((concepts::Semiregular<T2>));
            BOOST_CONCEPT_ASSERT((concepts::SinglePassSequenced<Output>));
            BOOST_CONCEPT_ASSERT((concepts::Writable<SequenceType<Output>, T2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyCopyable<SequenceType<Input>,
                                                               SequenceType<Output>>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<BinaryPredicate,
                                                             SequenceType<Input>,
                                                             T1 const *>));

            auto const pred
                = std::bind(::ural::make_callable(std::move(bin_pred)),
                            std::placeholders::_1,
                            std::cref(old_value));
            return ural::replace_copy_if_fn{}(std::forward<Input>(in),
                                              std::forward<Output>(out),
                                              std::move(pred), new_value);
        }
    };

    // Тусовка
    /** @ingroup MutatingSequenceOperations
    @brief Тип функционального объекта для случайной тасовка элементов
    последовательности.
    */
    class shuffle_fn
    {
    public:
        /** @brief Случайная тасовка элементов последовательности
        @param s последовательность произвольного доступа
        @param g генератор равномерно распределённых случайных чисел
        @return Последовательность, полученаня из
        <tt> ::ural::sequence_fwd<RASequenced>(s) </tt> продвижением до
        исчерпания.
        */
        template <class RASequenced, class URNG>
        auto operator()(RASequenced && s, URNG && g) const
        -> decltype(::ural::sequence_fwd<RASequenced>(s))
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequenced<RASequenced>));
            BOOST_CONCEPT_ASSERT((concepts::Uniform_random_number_generator<typename std::decay<URNG>::type>));
            BOOST_CONCEPT_ASSERT((concepts::Permutable<SequenceType<RASequenced>>));
            BOOST_CONCEPT_ASSERT((concepts::Convertible<ResultType<URNG>,
                                                        DifferenceType<SequenceType<RASequenced>>>));

            return this->impl(::ural::sequence_fwd<RASequenced>(s),
                              std::forward<URNG>(g));
        }

    private:
        template <class RASequence, class URNG>
        static RASequence impl(RASequence s, URNG && g)
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequence<RASequence>));
            BOOST_CONCEPT_ASSERT((concepts::Uniform_random_number_generator<typename std::decay<URNG>::type>));
            BOOST_CONCEPT_ASSERT((concepts::Permutable<RASequence>));
            BOOST_CONCEPT_ASSERT((concepts::Convertible<ResultType<URNG>, DifferenceType<RASequence>>));

            for(; !!s; ++s)
            {
                std::uniform_int_distribution<decltype(s.size())>
                    d(0, s.size() - 1);
                auto index = d(g);
                ::ural::details::do_swap(s[index], s.front());
            }

            return s;
        }
    };

    class random_shuffle_fn
    {
    public:
        /** @brief Случайная тасовка элементов последовательности
        @param s последовательность произвольного доступа
        @return Последовательность, полученная из
        <tt> ::ural::sequence_fwd<RASequence>(s) </tt> продвижением до
        исчерпания.
        */
        template <class RASequence>
        auto operator()(RASequence && s) const
        -> decltype(::ural::sequence_fwd<RASequence>(s))
        {
            ural::c_rand_engine rnd;
            return ::ural::shuffle_fn{}(std::forward<RASequence>(s), rnd);
        }
    };

    // Разделение
    /** @ingroup MutatingSequenceOperations
    @brief Тип функционального объекта для проверки того, последовательность
    является разделённой согласно предикату.
    */
    class is_partitioned_fn
    {
    public:
        /** @brief Проверка того, что последовательность является разделённой
        согласно предикату.
        @param in входная последовательность
        @param pred унарный предикат
        @return @b true, если есть все элементы @c in, удовлетворяющие предикату
        @c pred предшествуют элементам, не удовлетворяющим этому предикату.
        */
        template <class Input, class UnaryPredicate>
        bool operator()(Input && in, UnaryPredicate pred) const
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectPredicate<UnaryPredicate,
                                                              SequenceType<Input>>));

            return this->impl(::ural::sequence_fwd<Input>(in),
                              make_callable(std::move(pred)));
        }

    private:
        template <class Input, class UnaryPredicate>
        static bool impl(Input in, UnaryPredicate pred)
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectPredicate<UnaryPredicate, Input>));

            auto tail = find_if_not_fn{}(std::move(in), pred);
            return !find_if_fn{}(std::move(tail), std::move(pred));
        }
    };

    /** @ingroup MutatingSequenceOperations
    @brief Тип функционального объекта для разделения последовательности
    согласно заданному предикату
    */
    class partition_fn
    {
    public:
        /** @brief Разделение последовательности в соответствие с предикатом
        @param in последовательность
        @param pred предикат
        @post Переставляет элементы @c in таким образом, что все элемнты,
        удовлетворяющие @c pred оказываются перед элементами, не
        удовлетворяющими этому предикату.
        @return Последовательность @c r такая, что:
        <tt> original(r) == sequence_fwd<ForwardSequence>(in) </tt>,
        <tt> all_of(r.traversed_front(), pred) != false </tt> и
        <tt> none_of(r, pred) != false </tt>.
        */
        template <class ForwardSequence, class UnaryPredicate>
        auto operator()(ForwardSequence && in, UnaryPredicate pred) const
        -> decltype(::ural::sequence_fwd<ForwardSequence>(in))
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequenced<ForwardSequence>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectPredicate<UnaryPredicate,
                                                              SequenceType<ForwardSequence>>));
            BOOST_CONCEPT_ASSERT((concepts::Permutable<SequenceType<ForwardSequence>>));

            return this->impl(::ural::sequence_fwd<ForwardSequence>(in),
                              ::ural::make_callable(std::move(pred)));
        }
    private:
        template <class ForwardSequence, class UnaryPredicate>
        static ForwardSequence
        impl(ForwardSequence in, UnaryPredicate pred)
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<ForwardSequence>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectPredicate<UnaryPredicate, ForwardSequence>));
            BOOST_CONCEPT_ASSERT((concepts::Permutable<ForwardSequence>));

            // пропускаем ведущие "хорошие" элеменнов
            auto sink = find_if_not_fn{}(std::move(in), pred);

            in = sink;
            ++ in;
            in = find_if_fn{}(std::move(in), pred);

            for(; !!in; ++ in)
            {
                if(pred(*in))
                {
                    ::ural::details::do_swap(*sink, *in);
                    ++ sink;
                }
            }
            return sink;
        }
    };

    /** @ingroup MutatingSequenceOperations
    @brief Тип функционального объекта для устойчивого разделения
    последовательности согласно заданному предикату.
    @todo может быть двунаправленные как в Range extensions?
    @todo Как можно использовать partition_move
    */
    class stable_partition_fn
    {
    public:
        /** @brief Разделение последовательности в соответствие с предикатом
        @param in последовательность
        @param pred предикат
        @post Переставляет элементы @c in таким образом, что все элемнты,
        удовлетворяющие @c pred оказываются перед элементами, не
        удовлетворяющими этому предикату.
        @post Является устойчивым алгоритмом, то есть сохраняет относительный
        порядок эквивалентных элементов.
        @return Последовательность @c r такая, что:
        <tt> original(r) == sequence_fwd<ForwardSequenced>(in) </tt>,
        <tt> all_of(r.traversed_front(), pred) != false </tt> и
        <tt> none_of(r, pred) != false </tt>.
        */
        template <class ForwardSequenced, class UnaryPredicate>
        auto operator()(ForwardSequenced && in, UnaryPredicate pred) const
        -> decltype(::ural::sequence_fwd<ForwardSequenced>(in))
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequenced<ForwardSequenced>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectPredicate<UnaryPredicate,
                                                              SequenceType<ForwardSequenced>>));
            BOOST_CONCEPT_ASSERT((concepts::Permutable<SequenceType<ForwardSequenced>>));

            return this->impl(::ural::sequence_fwd<ForwardSequenced>(in),
                              ::ural::make_callable(std::move(pred)));
        }

    private:
        template <class ForwardSequence, class UnaryPredicate>
        ForwardSequence
        impl_inplace(ForwardSequence in, UnaryPredicate pred) const
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<ForwardSequence>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectPredicate<UnaryPredicate, ForwardSequence>));
            BOOST_CONCEPT_ASSERT((concepts::Permutable<ForwardSequence>));

            auto const n = ural::size(in);

            assert(!!in);
            assert(n > 0);
            assert(!pred(*in));
            assert(!in.traversed_front());

            auto const s_orig = ural::shrink_front(in);

            if(n == 1)
            {
                return s_orig;
            }

            // Разделяем первую половину
            auto const n_left = n/2;
            auto s = ural::next(s_orig, n_left);

            auto r_left = this->impl_inplace(s.traversed_front(), pred);

            // Разделяем вторую половину
            auto s_right = find_if_not_fn{}(ural::shrink_front(s), pred);

            if(!!s_right)
            {
                auto r_right = this->impl_inplace(ural::shrink_front(s_right), pred);
                ural::advance(s_right, ural::size(r_right.traversed_front()));
            }

            // Поворачиваем
            auto r = ::ural::rotate_fn{}(ural::shrink_front(r_left),
                                         s_right.traversed_front());

            // Возвращаем результат
            auto nt = ::ural::size(r_left.traversed_front());
            nt += ::ural::size(r[ural::_1].traversed_front());

            return ural::next(s_orig, nt);
        }

        template <class ForwardSequence, class UnaryPredicate>
        ForwardSequence
        impl(ForwardSequence in, UnaryPredicate pred) const
        {
            // @todo Проверка концепций

            in.shrink_front();
            in = find_if_not_fn{}(std::move(in), pred);

            if(!in)
            {
                return in;
            }

            // Разделяем на месте
            auto s = ural::shrink_front(std::move(in));
            auto r = this->impl_inplace(std::move(s), pred);
            auto const nt = ural::size(r.traversed_front());
            return ural::next(in, nt);
        }
    };

    /** @ingroup MutatingSequenceOperations
    @brief Тип функционального объекта для копирования элементов
    последовательности в две разные последовательности в зависимости от того,
    удовлетворяют ли они предикату.
    */
    class partition_copy_fn
    {
    public:
        /** @brief Копирование элементов последовательности в две разные
        последовательности в зависимости от того, удовлетворяют ли они
        заданному предикату.
        @param in входная последовательность
        @param out_true, выходная последовательность для элементов,
        удовлетворяющих предикату @c pred.
        @param out_false, выходная последовательность для элементов, не
        удовлетворяющих предикату @c pred.
        @return Кортеж, содержащий непройденные части последовательностей.
        */
        template <class Input, class Output1, class Output2, class UnaryPredicate>
        auto operator()(Input && in, Output1 && out_true, Output2 && out_false,
                        UnaryPredicate pred) const
        -> ural::tuple<decltype(::ural::sequence_fwd<Input>(in)),
                       decltype(::ural::sequence_fwd<Output1>(out_true)),
                       decltype(::ural::sequence_fwd<Output2>(out_false))>
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input>));
            BOOST_CONCEPT_ASSERT((concepts::SinglePassSequenced<Output1>));
            BOOST_CONCEPT_ASSERT((concepts::SinglePassSequenced<Output2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyCopyable<SequenceType<Input>,
                                                               SequenceType<Output1>>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyCopyable<SequenceType<Input>,
                                                               SequenceType<Output2>>));

            return this->impl(::ural::sequence_fwd<Input>(in),
                              ::ural::sequence_fwd<Output1>(out_true),
                              ::ural::sequence_fwd<Output2>(out_false),
                              ::ural::make_callable(std::move(pred)));
        }

    private:
        template <class Input, class Output1, class Output2, class UnaryPredicate>
        static ural::tuple<Input, Output1, Output2>
        impl(Input in, Output1 out_true, Output2 out_false, UnaryPredicate pred)
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input>));
            BOOST_CONCEPT_ASSERT((concepts::SinglePassSequence<Output1>));
            BOOST_CONCEPT_ASSERT((concepts::SinglePassSequence<Output2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyCopyable<Input, Output1>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyCopyable<Input, Output2>));

            auto out = ural::make_partition_sequence(std::move(out_true),
                                                     std::move(out_false),
                                                     std::move(pred));
            auto r = copy_fn{}(std::move(in), std::move(out));

            typedef ural::tuple<Input, Output1, Output2> Tuple;
            return Tuple(r[ural::_1], r[ural::_2].true_sequence(),
                         r[ural::_2].false_sequence());
        }
    };

    /** @ingroup MutatingSequenceOperations
    @brief Тип функционального объекта для поиска точки разделения
    последовательности
    */
    class partition_point_fn
    {
    public:
        /** @brief Поиск точки разделения
        @param in последовательность
        @param pred предикат
        @pre <tt> is_partitioned(in, pred) </tt>
        @return Последовательность @c r такая, что:
        <tt> original(r) == sequence_fwd<ForwardSequence>(in) </tt>,
        <tt> all_of(r.traversed_front(), pred) != false </tt> и
        <tt> none_of(r, pred) != false </tt>.
        */
        template <class ForwardSequence, class Predicate>
        auto operator()(ForwardSequence && in, Predicate pred) const
        -> decltype(::ural::sequence_fwd<ForwardSequence>(in))
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequenced<ForwardSequence>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectPredicate<Predicate,
                                                              SequenceType<ForwardSequence>>));

            return this->impl(::ural::sequence_fwd<ForwardSequence>(in),
                              ::ural::make_callable(std::move(pred)));
        }

    private:
        template <class ForwardSequence, class Predicate>
        static ForwardSequence
        impl(ForwardSequence in, Predicate pred)
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<ForwardSequence>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectPredicate<Predicate, ForwardSequence>));

            // @todo Нужен ли этот вызов?
            in.shrink_front();
            return find_if_not_fn{}(std::move(in), std::move(pred));
        }
    };

    // Бинарные кучи
    /** @ingroup SortingOperations
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
        @return Последовательность @c r такая, что
        <tt> original(r) == sequence(seq) </tt>, <tt> r.traversed_front() </tt>
        является наибольшим префиксом @c seq, который является бинарной кучей.
        */
        template <class RASequenced, class Compare = ural::less<>>
        auto operator()(RASequenced && seq, Compare cmp = Compare()) const
        -> decltype(::ural::sequence_fwd<RASequenced>(seq))
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequenced<RASequenced>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, SequenceType<RASequenced>>));

            return this->impl(::ural::sequence_fwd<RASequenced>(seq),
                              ::ural::make_callable(std::move(cmp)));
        }

    private:
        template <class RASequence, class Compare>
        static RASequence
        impl(RASequence seq, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequence<RASequence>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, RASequence>));

            // Пустая последовательность - куча
            if(!seq)
            {
                return seq;
            }

            auto const n = seq.size();

            auto index = 1;

            for(; index != n; ++ index)
            {
                auto const p = ural::details::heap_parent(index);

                if(cmp(seq[p], seq[index]))
                {
                    break;
                }
            }

            return std::move(seq) + index;
        }
    };

    /** @ingroup SortingOperations
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
        template <class RASequenced, class Compare = ::ural::less<>>
        bool operator()(RASequenced && seq, Compare cmp = Compare()) const
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequenced<RASequenced>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, SequenceType<RASequenced>>));

            return this->impl(::ural::sequence_fwd<RASequenced>(seq),
                              ::ural::make_callable(std::move(cmp)));
        }

    private:
        template <class RASequence, class Compare>
        static bool
        impl(RASequence seq, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequence<RASequence>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, RASequence>));

            return !::ural::is_heap_until_fn{}(seq, cmp);
        }
    };

    class heap_sink_fn
    {
    public:
        template <class RASequence, class Size, class Compare = ::ural::less<>>
        void operator()(RASequence && seq, Size first, Size last,
                        Compare cmp = Compare()) const
        {
            return this->impl(::ural::sequence_fwd<RASequence>(seq),
                              first, last,
                              ::ural::make_callable(std::move(cmp)));
        }

    private:
        template <class RASequence, class Size, class Compare>
        static void
        update_largest(RASequence seq, Size & largest, Size candidate,
                       Size last, Compare cmp)
        {
            if(candidate < last && cmp(seq[largest], seq[candidate]))
            {
                largest = candidate;
            }
        }

        template <class RASequence, class Size, class Compare>
        void impl(RASequence seq, Size first, Size last, Compare cmp) const
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequence<RASequence>));
            BOOST_CONCEPT_ASSERT((concepts::ReadableSequence<RASequence>));
            // @todo Проверить, что можно обменивать

            // @todo Требования к Size

            BOOST_CONCEPT_ASSERT((concepts::Callable<Compare, bool(decltype(*seq), decltype(*seq))>));

            assert(ural::to_signed(last) <= seq.size());

            for(; first != last;)
            {
                auto largest = first;
                update_largest(seq, largest, details::heap_child_1(first), last, cmp);
                update_largest(seq, largest, details::heap_child_2(first), last, cmp);

                if(largest == first)
                {
                    return;
                }

                ::ural::details::do_swap(seq[largest], seq[first]);

                first = largest;
            }
        }
    };

    /** @ingroup SortingOperations
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
        @post <tt> is_heap(std::forward<RASequenced>(seq), cmp) </tt>
        @return Последовательность, полученная из std::forward<RASequence>(seq)
        путём продвижения до исчерпания.
        */
        template <class RASequenced, class Compare = ural::less<>>
        auto operator()(RASequenced && seq, Compare cmp = Compare()) const
        -> decltype(::ural::sequence_fwd<RASequenced>(seq))
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequenced<RASequenced>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<SequenceType<RASequenced>, Compare>));

            return this->impl(::ural::sequence_fwd<RASequenced>(seq),
                              ::ural::make_callable(std::move(cmp)));
        }

    private:
        template <class RASequence, class Compare>
        static RASequence
        impl(RASequence seq, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequence<RASequence>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<RASequence, Compare>));

            for(auto n = seq.size() / 2; n > 0; -- n)
            {
                heap_sink_fn{}(seq, n - 1, seq.size(), cmp);
            }

            assert(is_heap_fn{}(seq, cmp));

            return seq += seq.size();
        }
    };

    /** @ingroup SortingOperations
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
        template <class RASequenced, class Compare = ural::less<>>
        void operator()(RASequenced && seq, Compare cmp = Compare()) const
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequenced<RASequenced>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<SequenceType<RASequenced>, Compare>));

            return this->impl(::ural::sequence_fwd<RASequenced>(seq),
                              ::ural::make_callable(std::move(cmp)));
        }

    private:
        template <class RASequence, class Compare>
        static void
        impl(RASequence seq, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequence<RASequence>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<RASequence, Compare>));

            assert(::ural::is_heap_until_fn{}(seq, cmp).size() <= 1);

            if(seq.size() >= 1)
            {
                ::ural::details::heap_swim(seq, seq.size() - 1, cmp);
            }

            assert(is_heap_fn{}(seq, cmp));
        }
    };

    /** @ingroup SortingOperations
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
        <tt>::ural::sequence_fwd<RASequenced>(seq)</tt> путём продвижения до
        исчерпания.
        */
        template <class RASequenced, class Compare = ural::less<>>
        auto operator()(RASequenced && seq, Compare cmp = Compare()) const
        -> decltype(::ural::sequence_fwd<RASequenced>(seq))
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequenced<RASequenced>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<SequenceType<RASequenced>, Compare>));

            return this->impl(::ural::sequence_fwd<RASequenced>(seq),
                              ::ural::make_callable(std::move(cmp)));
        }

    private:
        template <class RASequence, class Compare>
        static RASequence
        impl(RASequence seq, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequence<RASequence>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<RASequence, Compare>));

            assert(is_heap_fn{}(seq, cmp));
            auto const N = seq.size();

            if(N > 1)
            {
                ::ural::details::do_swap(seq[0], seq[N-1]);
                heap_sink_fn{}(seq, 0*N, N-1, cmp);
            }

            return seq += N;
        }
    };

    /** @ingroup SortingOperations
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
        <tt>::ural::sequence_fwd<RASequenced>(seq)</tt> путём продвижения до
        исчерпания.
        */
        template <class RASequenced, class Compare = ural::less<>>
        auto operator()(RASequenced && seq, Compare cmp = Compare()) const
        -> decltype(::ural::sequence_fwd<RASequenced>(seq))
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequenced<RASequenced>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<SequenceType<RASequenced>, Compare>));

            return this->impl(::ural::sequence_fwd<RASequenced>(seq),
                              ::ural::make_callable(std::move(cmp)));
        }

    private:
        template <class RASequence, class Compare>
        static RASequence
        impl(RASequence seq, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequence<RASequence>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<RASequence, Compare>));

            assert(is_heap_fn{}(seq, cmp));
            for(auto n = seq.size(); n > 0; --n)
            {
                pop_heap_fn{}(seq, cmp);
                seq.pop_back();
            }
            assert(!seq);
            assert(::ural::is_sorted_fn{}(seq.traversed_back(), cmp));

            auto result = seq.traversed_back();
            result += result.size();
            return result;
        }
    };

    class heap_select_fn
    {
    public:
        template <class RASequence, class Compare = ::ural::less<>>
        auto operator()(RASequence && s, Compare cmp = Compare()) const
        -> decltype(::ural::sequence_fwd<RASequence>(s))
        {
            // @todo Подумать, что можно возвращать из impl
            auto seq = ::ural::sequence_fwd<RASequence>(s);
            this->impl(seq, ::ural::make_callable(std::move(cmp)));
            seq += seq.size();
            return seq;
        }

    private:
        template <class RASequence, class Compare>
        static void
        impl(RASequence s, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequence<RASequence>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<RASequence, Compare>));

            if(!s)
            {
                return;
            }

            ++ s;
            auto s1 = s.traversed_front();

            if(!s1 || !s)
            {
                return;
            }

            make_heap_fn{}(s1, cmp);

            for(; !!s; ++ s)
            {
                if(cmp(*s, *s1))
                {
                    ::ural::details::do_swap(*s, *s1);
                    auto const n = s1.size();
                    heap_sink_fn{}(s1, 0*n, n, cmp);
                }
            }
            pop_heap_fn{}(s1, cmp);
        }
    };
    auto constexpr heap_select = heap_select_fn{};

    /** @ingroup SortingOperations
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
        @pre Элементы @c e последовательности @c in должны быть разделены
        относительно предиката <tt> cmp(e, value) </tt>.
        @return Последовательность @c r такая, что
        <tt> original(r) == sequence(in) </tt> и <tt> r.traversed_front() </tt>
        является наибольшей возможной последовательностью, всё элементы @c x
        которой удовлетворяют условию <tt> cmp(x, value) </tt>.
        */
        template <class Forward, class T, class Compare = ::ural::less<>>
        auto operator()(Forward && in, T const & value,
                        Compare cmp = Compare()) const
        -> decltype(::ural::sequence_fwd<Forward>(in))
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequenced<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, T const *,
                                                             SequenceType<Forward>>));

            return this->impl(::ural::sequence_fwd<Forward>(in), value,
                              ::ural::make_callable(std::move(cmp)));
        }
    private:
        template <class Forward, class T, class Compare>
        static Forward
        impl(Forward in, T const & value, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, T const *, Forward>));

            auto pred = std::bind(std::move(cmp), ural::_1, std::cref(value));
            return ::ural::partition_point_fn{}(std::move(in), std::move(pred));
        }
    };

    /** @ingroup SortingOperations
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
        <tt> original(r) == sequence(in) </tt> и <tt> r.traversed_front() </tt>
        является наибольшей возможной последовательностью, всё элементы @c x
        которой удовлетворяют условию <tt> !cmp(value, x) </tt>.
        */
        template <class Forward, class T, class Compare = ::ural::less<>>
        auto operator()(Forward && in, T const & value,
                        Compare cmp = Compare()) const
        -> decltype(::ural::sequence_fwd<Forward>(in))
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequenced<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, T const *,
                                                             SequenceType<Forward>>));

            return this->impl(::ural::sequence_fwd<Forward>(in), value,
                              ::ural::make_callable(std::move(cmp)));
        }
    private:
        template <class RASequence, class T, class Compare>
        static RASequence
        impl(RASequence in, T const & value, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<RASequence>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, T const *, RASequence>));

            auto pred = ural::not_fn(std::bind(std::move(cmp), std::cref(value), ural::_1));
            return ::ural::partition_point_fn{}(std::move(in), std::move(pred));
        }
    };

    /** @ingroup SortingOperations
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
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequenced<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, T const *,
                                                             SequenceType<Forward>>));

            return this->impl(::ural::sequence_fwd<Forward>(in), value,
                              ::ural::make_callable(std::move(cmp)));
        }

    private:
        template <class Forward, class T, class Compare>
        static bool impl(Forward in, T const & value, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, T const *, Forward>));

            in = lower_bound_fn{}(std::move(in), value, cmp);

            return !!in && !cmp(value, *in);
        }
    };

    /** @ingroup SortingOperations
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
        <tt> original(r) == sequence(in) </tt>, причём все элементы @c r
        эквивалентны @c value в смысле отношения @c cmp.
        */
        template <class Forward, class T, class Compare = ::ural::less<>>
        auto operator()(Forward && in, T const & value,
                        Compare cmp = Compare()) const
        -> decltype(::ural::sequence_fwd<Forward>(in))
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequenced<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, T const *,
                                                             SequenceType<Forward>>));

            return this->impl(::ural::sequence_fwd<Forward>(in), value,
                              ::ural::make_callable(std::move(cmp)));
        }

    private:
        template <class Forward, class T, class Compare>
        static Forward impl(Forward in, T const & value, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, T const *, Forward>));

            // @todo Оптимизация
            auto lower = lower_bound_fn{}(in, value, cmp);
            auto upper = upper_bound_fn{}(in, value, cmp);

            auto n_lower = lower.traversed_front().size();
            auto n_upper = in.size() - upper.traversed_front().size();

            in += n_lower;
            in.pop_back(n_upper);
            return in;
        }
    };

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
        template <class RASequenced, class Compare>
        auto operator()(RASequenced && s, Compare cmp) const
        -> decltype(::ural::sequence_fwd<RASequenced>(s))
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequenced<RASequenced>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<SequenceType<RASequenced>, Compare>));

            return this->impl(::ural::sequence_fwd<RASequenced>(s),
                              ::ural::make_callable(std::move(cmp)));
        }

    private:
        template <class RASequence, class Compare>
        static RASequence impl(RASequence s, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequence<RASequence>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<RASequence, Compare>));

            if(!s)
            {
                return s;
            }

            typedef decltype(s.size()) Index;

            for(Index i = 1; i != s.size(); ++ i)
            for(Index j = i; j > 0; -- j)
            {
                if(cmp(s[j], s[j-1]))
                {
                    ::ural::details::do_swap(s[j], s[j-1]);
                }
                else
                {
                    break;
                }
            }

            s += s.size();
            return s;
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
        template <class RASequenced, class Compare = ::ural::less<>>
        auto operator()(RASequenced && s, Compare cmp = Compare()) const
        -> decltype(::ural::sequence_fwd<RASequenced>(s))
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequenced<RASequenced>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<SequenceType<RASequenced>, Compare>));

            return this->impl(::ural::sequence_fwd<RASequenced>(s),
                              ::ural::make_callable(std::move(cmp)));
        }
    private:
        template <class RASequence, class Compare>
        static RASequence impl(RASequence s, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequence<RASequence>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<RASequence, Compare>));

            return ::ural::insertion_sort_fn{}(std::move(s), std::move(cmp));
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
        template <class RASequenced, class Compare = ::ural::less<>>
        auto operator()(RASequenced && s, Compare cmp = Compare()) const
        -> decltype(::ural::sequence_fwd<RASequenced>(s))
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequenced<RASequenced>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<SequenceType<RASequenced>, Compare>));

            return this->impl(::ural::sequence_fwd<RASequenced>(s),
                              ::ural::make_callable(std::move(cmp)));
        }

    private:
        template <class RASequence, class Compare>
        static RASequence impl(RASequence s, Compare cmp)
        {
            return ::ural::insertion_sort_fn{}(std::move(s), std::move(cmp));
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
        <tt> ::ural::sequence_fwd<RASequenced>(s) </tt> продвижением до
        исчерпания.
        */
        template <class RASequenced, class Compare = ::ural::less<>>
        auto operator()(RASequenced && s,
                        DifferenceType<SequenceType<RASequenced>> part,
                        Compare cmp = Compare()) const
        -> decltype(::ural::sequence_fwd<RASequenced>(s))
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequenced<RASequenced>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<SequenceType<RASequenced>, Compare>));

            // @todo Подумать, что можно возвращать из impl
            auto seq = ::ural::sequence_fwd<RASequenced>(s);
            this->impl(seq, std::move(part),
                       ::ural::make_callable(std::move(cmp)));
            seq += seq.size();
            return seq;
        }

    private:
        template <class RASequence, class Compare>
        static void
        impl(RASequence s, DifferenceType<RASequence> const part, Compare cmp)
        {
            make_heap_fn{}(s, cmp);

            s.shrink_front();
            auto const s_old = s;
            s += part;

            for(auto i = s; !!i; ++ i)
            {
                if(cmp(*i, *s_old))
                {
                    ::ural::details::do_swap(*s_old, *i);
                    heap_sink_fn{}(s.traversed_front(), 0*part, part, cmp);
                }
            }

            sort_heap_fn{}(s.traversed_front(), cmp);
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
        auto operator()(Input && in, RASequence && out,
                        Compare cmp = Compare()) const
        -> decltype(::ural::sequence_fwd<RASequence>(out))
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input>));
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequenced<RASequence>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyCopyable<SequenceType<Input>,
                                                               SequenceType<RASequence>>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<SequenceType<RASequence>, Compare>));

            return this->impl(::ural::sequence_fwd<Input>(in),
                              ::ural::sequence_fwd<RASequence>(out),
                              ::ural::make_callable(std::move(cmp)));
        }

    private:
        template <class Input, class RASequence, class Compare>
        static RASequence
        impl(Input in, RASequence out, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input>));
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequence<RASequence>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyCopyable<Input, RASequence>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<RASequence, Compare>));

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
        <tt> ::ural::sequence_fwd<RASequenced>(s) </tt>, продвижением до
        исчерпания.
        @todo перегрузка, получающая номер элемента
        */
        template <class RASequenced, class Compare = ::ural::less<>>
        auto operator()(RASequenced && s, Compare cmp = Compare()) const
        -> decltype(::ural::sequence_fwd<RASequenced>(s))
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequenced<RASequenced>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<SequenceType<RASequenced>, Compare>));

            return this->impl(::ural::sequence_fwd<RASequenced>(s),
                              ::ural::make_callable(std::move(cmp)));
        }

    private:
        template <class RASequence, class Compare>
        static RASequence impl(RASequence s, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequence<RASequence>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<RASequence, Compare>));

            return heap_select_fn{}(std::move(s), std::move(cmp));
        }
    };

    /** @ingroup SortingOperations
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
        auto operator()(Input1 && in1, Input2 && in2, Output && out,
                        Compare cmp = Compare()) const
        -> tuple<decltype(::ural::sequence_fwd<Input1>(in1)),
                 decltype(::ural::sequence_fwd<Input2>(in2)),
                 decltype(::ural::sequence_fwd<Output>(out))>
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input1>));
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input2>));
            BOOST_CONCEPT_ASSERT((concepts::SinglePassSequenced<Output>));
            BOOST_CONCEPT_ASSERT((concepts::Mergeable<SequenceType<Input1>,
                                                      SequenceType<Input2>,
                                                      SequenceType<Output>,
                                                      Compare>));

            auto inputs = ::ural::merged(::ural::sequence_fwd<Input1>(in1),
                                         ::ural::sequence_fwd<Input2>(in2),
                                         ::ural::make_callable(std::move(cmp)));
            auto result = ural::copy_fn{}(std::move(inputs),
                                          ::ural::sequence_fwd<Output>(out));

            return ural::make_tuple(std::move(result[ural::_1]).first_base(),
                                    std::move(result[ural::_1]).second_base(),
                                    std::move(result[ural::_2]));
        }
    };

    /** @ingroup SortingOperations
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
        <tt>::ural::sequence_fwd<Bidirectional>(seq)</tt> путём продвижения до
        исчерпания.
        */
        template <class Bidirectional, class Compare = ::ural::less<>>
        auto operator()(Bidirectional && s, Compare cmp = Compare()) const
        -> decltype(::ural::sequence_fwd<Bidirectional>(s))
        {
            BOOST_CONCEPT_ASSERT((concepts::BidirectionalSequenced<Bidirectional>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<SequenceType<Bidirectional>, Compare>));

            // @todo Подумать, что возвращать из impl
            auto seq = ::ural::sequence_fwd<Bidirectional>(s);
            this->impl(seq, ::ural::make_callable(std::move(cmp)));
            seq += seq.size();
            return seq;
        }

    private:
        template <class BidirectionalSequence, class Compare>
        static void impl(BidirectionalSequence s, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::BidirectionalSequence<BidirectionalSequence>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<BidirectionalSequence, Compare>));

            auto s1 = s.traversed_front();
            auto s2 = ural::shrink_front(s);

            auto n1 = ural::size(s1);
            auto n2 = ural::size(s2);

            if(!s1 || !s2)
            {
                return;
            }

            assert(::ural::is_sorted_fn{}(s1, cmp));
            assert(::ural::is_sorted_fn{}(s2, cmp));

            if(n1 + n2 == 2)
            {
                if(cmp(*s2, *s1))
                {
                    ::ural::details::do_swap(*s1, *s2);
                }
                return;
            }

            auto s1_cut = s1;
            auto s2_cut = s2;

            if(n1 > n2)
            {
                auto n11 = n1 / 2;
                s1_cut += n11;
                s2_cut = lower_bound_fn{}(s2, *s1_cut, cmp);
            }
            else
            {
                auto n21 = n2 / 2;
                s2_cut += n21;
                s1_cut = upper_bound_fn{}(s1, *s2_cut, cmp);
            }

            ::ural::rotate_fn{}(s1_cut, s2_cut.traversed_front());

            auto s_new = s.original();

            auto n11 = ural::size(s1_cut.traversed_front());
            auto n12 = ural::size(s1_cut);
            auto n21 = ural::size(s2_cut.traversed_front());

            ural::advance(s_new, n11 + n21);

            auto s1_new = s_new.traversed_front();
            auto s2_new = ural::shrink_front(s_new);

            ural::advance(s1_new, n11);
            ural::advance(s2_new, n12);

            inplace_merge_fn::impl(s1_new, cmp);
            inplace_merge_fn::impl(s2_new, cmp);
        }
    };

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
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input1>));
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare,
                                                             SequenceType<Input1>,
                                                             SequenceType<Input2>>));

            return this->impl(::ural::sequence_fwd<Input1>(in1),
                              ::ural::sequence_fwd<Input2>(in2),
                              ::ural::make_callable(std::move(cmp)));
        }

    private:
        template <class Input1, class  Input2, class Compare>
        static bool
        impl(Input1 in1, Input2 in2, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input1>));
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input2>));
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

    /** @ingroup NonModifyingSequenceOperations
    @brief Функциональный объект для проверки того, что одна последовательность
    является перестановкой другой
    */
    class is_permutation_fn
    {
    public:
        /** @brief Проверка того, что одна последовательность являются
        перестановкой другой.
        @param s1, s2 прямые последовательности
        @param pred бинарный предикат
        @return @b true, если для каждого элемента @c s1 существует эквивалентый
        в смысле @c pred элемент @c s2.
        */
        template <class Forward1, class Forward2,
                  class BinaryPredicate = ::ural::equal_to<>>
        bool operator()(Forward1 && s1, Forward2 && s2,
                        BinaryPredicate pred = BinaryPredicate()) const
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequenced<Forward1>));
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequenced<Forward2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyComparable<SequenceType<Forward1>,
                                                                 SequenceType<Forward2>,
                                                                 BinaryPredicate>));

            return this->impl(::ural::sequence_fwd<Forward1>(s1),
                              ::ural::sequence_fwd<Forward2>(s2),
                              ::ural::make_callable(std::move(pred)));
        }

    private:
        template <class Forward1, class Forward2, class BinaryPredicate>
        static bool
        impl(Forward1 s1, Forward2 s2, BinaryPredicate pred)
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Forward1>));
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Forward2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyComparable<Forward1, Forward2, BinaryPredicate>));

            std::tie(s1, s2) = ural::mismatch_fn{}(std::move(s1), std::move(s2),
                                                   pred);

            s1.shrink_front();
            s2.shrink_front();

            if(ural::size(s1) != ural::size(s2))
            {
                return false;
            }

            for(; !!s1; ++ s1)
            {
                // Пропускаем элементы, которые уже встречались
                if(!!find_fn{}(s1.traversed_front(), *s1, pred))
                {
                    continue;
                }

                auto s = s1;
                ++ s;
                auto const n1 = 1 + count_fn{}(s, *s1, pred);
                auto const n2 = count_fn{}(s2, *s1, pred);

                if(n1 != n2)
                {
                    return false;
                }
            }
            return true;
        }
    };

    // Операции с множествами
    /** @ingroup SortingOperations
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
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input1>));
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare,
                                                             SequenceType<Input1>,
                                                             SequenceType<Input2>>));

            return this->impl(::ural::sequence_fwd<Input1>(in1),
                              ::ural::sequence_fwd<Input2>(in2),
                              ::ural::make_callable(std::move(cmp)));
        }

    private:
        template <class Input1, class  Input2, class Compare>
        static bool impl(Input1 in1, Input2 in2, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input1>));
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<Input2>));
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

    /** @ingroup SortingOperations
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
        auto operator()(Input1 && in1, Input2 && in2, Output && out,
                        Compare cmp = Compare()) const
        -> tuple<decltype(::ural::sequence_fwd<Input1>(in1)),
                 decltype(::ural::sequence_fwd<Input2>(in2)),
                 decltype(::ural::sequence_fwd<Output>(out))>
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input1>));
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare,
                                                             SequenceType<Input1>,
                                                             SequenceType<Input2>>));
            BOOST_CONCEPT_ASSERT((concepts::Mergeable<SequenceType<Input1>,
                                                      SequenceType<Input2>,
                                                      SequenceType<Output>,
                                                      Compare>));

            auto in
                = ::ural::make_set_union_sequence(std::forward<Input1>(in1),
                                                  std::forward<Input2>(in2),
                                                  ::ural::make_callable(std::move(cmp)));
            auto r = ural::copy_fn{}(std::move(in),
                                     ::ural::sequence_fwd<Output>(out));
            return make_tuple(std::move(r[ural::_1]).first_base(),
                              std::move(r[ural::_1]).second_base(),
                              std::move(r[ural::_2]));
        }
    };

    /** @ingroup SortingOperations
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
        auto operator()(Input1 && in1, Input2 && in2, Output && out,
                        Compare cmp = Compare()) const
        -> tuple<decltype(::ural::sequence_fwd<Input1>(in1)),
                 decltype(::ural::sequence_fwd<Input2>(in2)),
                 decltype(::ural::sequence_fwd<Output>(out))>
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input1>));
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare,
                                                             SequenceType<Input1>,
                                                             SequenceType<Input2>>));
            BOOST_CONCEPT_ASSERT((concepts::Mergeable<SequenceType<Input1>,
                                                      SequenceType<Input2>,
                                                      SequenceType<Output>,
                                                      Compare>));

            auto in
                = ::ural::make_set_intersection_sequence(std::forward<Input1>(in1),
                                                         std::forward<Input2>(in2),
                                                         ::ural::make_callable(std::move(cmp)));
            auto r = ural::copy_fn{}(std::move(in),
                                     ::ural::sequence_fwd<Output>(out));
            return make_tuple(std::move(r[ural::_1]).first_base(),
                              std::move(r[ural::_1]).second_base(),
                              std::move(r[ural::_2]));
        }
    };

    /** @ingroup SortingOperations
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
        auto operator()(Input1 && in1, Input2 && in2, Output && out,
                        Compare cmp = Compare()) const
        -> tuple<decltype(::ural::sequence_fwd<Input1>(in1)),
                 decltype(::ural::sequence_fwd<Input2>(in2)),
                 decltype(::ural::sequence_fwd<Output>(out))>
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input1>));
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare,
                                                             SequenceType<Input1>,
                                                             SequenceType<Input2>>));
            BOOST_CONCEPT_ASSERT((concepts::Mergeable<SequenceType<Input1>,
                                                      SequenceType<Input2>,
                                                      SequenceType<Output>,
                                                      Compare>));

            auto in
                = ::ural::make_set_difference_sequence(std::forward<Input1>(in1),
                                                       std::forward<Input2>(in2),
                                                       ::ural::make_callable(std::move(cmp)));
            auto r = ural::copy_fn{}(std::move(in),
                                     ::ural::sequence_fwd<Output>(out));
            return make_tuple(std::move(r[ural::_1]).first_base(),
                              std::move(r[ural::_1]).second_base(),
                              std::move(r[ural::_2]));
        }
    };

    /** @ingroup SortingOperations
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
        auto operator()(Input1 && in1, Input2 && in2, Output && out,
                        Compare cmp = Compare()) const
        -> tuple<decltype(::ural::sequence_fwd<Input1>(in1)),
                 decltype(::ural::sequence_fwd<Input2>(in2)),
                 decltype(::ural::sequence_fwd<Output>(out))>
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input1>));
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input2>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare,
                                                             SequenceType<Input1>,
                                                             SequenceType<Input2>>));
            BOOST_CONCEPT_ASSERT((concepts::Mergeable<SequenceType<Input1>,
                                                      SequenceType<Input2>,
                                                      SequenceType<Output>,
                                                      Compare>));

            auto in = ::ural::make_set_symmetric_difference_sequence(std::forward<Input1>(in1),
                                                                     std::forward<Input2>(in2),
                                                                     ::ural::make_callable(std::move(cmp)));
            auto r = ::ural::copy_fn{}(std::move(in),
                                       ::ural::sequence_fwd<Output>(out));
            return make_tuple(std::move(r[ural::_1]).first_base(),
                              std::move(r[ural::_1]).second_base(),
                              std::move(r[ural::_2]));
        }
    };

    // Поиск наибольшего и наименьшего
    /** @ingroup SortingOperations
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
        @pre @c Compare должент быть <tt> Relation<FunctionType<Comp>, T> </tt>
        @return Если <tt> cmp(y, x) </tt>, то возвращает @c y, иначе --- @c x.
        */
        template <class T, class Compare>
        constexpr T const &
        operator()(T const & x, T const & y, Compare cmp) const
        {
            BOOST_CONCEPT_ASSERT((concepts::Relation<FunctionType<Compare>, T>));

            return cmp(y, x) ? y : x;
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
        @param cmp функция сравнения
        @pre <tt> values.begin() != values.end() </tt>
        @return Наименьшее значение в списке инициализаторов
        */
        template <class T, class Compare>
        constexpr T
        operator()(std::initializer_list<T> values, Compare cmp) const
        {
            BOOST_CONCEPT_ASSERT((concepts::Semiregular<T>));
            BOOST_CONCEPT_ASSERT((concepts::Relation<FunctionType<Compare>, T>));

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

    /** @ingroup SortingOperations
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
        @pre @c Compare должент быть <tt> Relation<FunctionType<Comp>, T> </tt>
        @return Наибольший из @c x и @c y, если они равны, то возвращает @c x.
        */
        template <class T, class Compare>
        constexpr T const &
        operator()(T const & x, T const & y, Compare cmp) const
        {
            BOOST_CONCEPT_ASSERT((concepts::Relation<FunctionType<Compare>, T>));

            return cmp(x, y) ? y : x;
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
            BOOST_CONCEPT_ASSERT((concepts::Relation<FunctionType<Compare>, T>));

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

    /** @ingroup SortingOperations
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
        @pre @c Compare должент быть <tt> Relation<FunctionType<Comp>, T> </tt>
        Если <tt> cmp(y, x) </tt>, то возвращает <tt> {y, x} </tt>, иначе
        --- <tt> {x, y} </tt>.
        */
        template <class T, class Compare>
        constexpr std::pair<T const &, T const &>
        operator()(T const & x, T const & y, Compare cmp) const
        {
            BOOST_CONCEPT_ASSERT((concepts::Relation<FunctionType<Compare>, T>));

            using Pair = std::pair<T const &, T const &>;
            return cmp(y, x) ? Pair(y, x) : Pair(x, y);
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
            BOOST_CONCEPT_ASSERT((concepts::Relation<FunctionType<Compare>, T>));

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

    /** @ingroup SortingOperations
    @brief Тип функционального объекта для поиска наименьшего элемента
    последовательности.
    */
    class min_element_fn
    {
    private:
        template <class ForwardSequence, class Compare>
        static ForwardSequence
        impl(ForwardSequence in, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<ForwardSequence>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, ForwardSequence>));

            if(!in)
            {
                return in;
            }

            auto cmp_s = ural::compare_by(ural::dereference<>{}, std::move(cmp));

            ::ural::min_element_accumulator<ForwardSequence, decltype(cmp_s)>
                acc(in++, cmp_s);

            auto seq = in | ural::outdirected;

            acc = ::ural::for_each_fn{}(std::move(seq), std::move(acc))[ural::_2];

            return acc.result();
        }
    public:
        /** @brief Поиск наименьшего элемента последовательности.
        @param in входная последовательность
        @param cmp функция сравнения, по умолчанию используется
        <tt> ural::less<> </tt>, то есть оператор "меньше".
        @return Последовательность @c r такая, что
        <tt> original(r) == sequence(in) </tt>, а для любого элемента @c x
        последовательности @c in, выполняется условие
        <tt> cmp(x, r.front()) == false </tt>, а среди элементов
        последовательности <tt> r.traversed_front() </tt> нет элементов,
        эквивалентных <tt> r.front() </tt>.
        */
        template <class Forward, class Compare = ::ural::less<>>
        auto operator()(Forward && in, Compare cmp = Compare()) const
        -> decltype(::ural::sequence_fwd<Forward>(in))
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequenced<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare,
                                                             SequenceType<Forward>>));

            return this->impl(::ural::sequence_fwd<Forward>(in),
                              ::ural::make_callable(std::move(cmp)));
        }
    };

    /** @ingroup SortingOperations
    @brief Тип функционального объекта для поиска наибольшего элемента
    последовательности.
    */
    class max_element_fn
    {
    private:
        template <class ForwardSequence, class Compare>
        static ForwardSequence
        impl(ForwardSequence in, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<ForwardSequence>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, ForwardSequence>));

            auto transposed_cmp = ural::make_binary_reverse_args(std::move(cmp));

            return ::ural::min_element_fn{}(std::move(in),
                                            std::move(transposed_cmp));
        }

    public:
        /** @brief Поиск наибольшего элемента последовательности.
        @param in входная последовательность
        @param cmp функция сравнения, по умолчанию используется
        <tt> ural::less<> </tt>, то есть оператор "меньше".
        @return Последовательность @c r такая, что
        <tt> original(r) == sequence(in) </tt>, а для любого элемента @c x
        последовательности @c in, выполняется условие
        <tt> cmp(r.front(), x) == false </tt>, а среди элементов
        последовательности <tt> r.traversed_front() </tt> нет элементов,
        эквивалентных <tt> r.front() </tt>.
        */
        template <class Forward, class Compare = ::ural::less<>>
        auto operator()(Forward && in, Compare cmp = Compare()) const
        -> decltype(::ural::sequence_fwd<Forward>(in))
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequenced<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare,
                                                             SequenceType<Forward>>));

            return this->impl(::ural::sequence_fwd<Forward>(in),
                              ::ural::make_callable(std::move(cmp)));
        }
    };

    /** @ingroup SortingOperations
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
        auto operator()(Forward && in, Compare cmp = Compare()) const
        -> ural::tuple<decltype(::ural::sequence_fwd<Forward>(in)),
                       decltype(::ural::sequence_fwd<Forward>(in))>
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequenced<Forward>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare,
                                                             SequenceType<Forward>>));

            return this->impl(::ural::sequence_fwd<Forward>(in),
                              ::ural::make_callable(std::move(cmp)));
        }

    private:
        template <class ForwardSequence, class Compare>
        static tuple<ForwardSequence, ForwardSequence>
        impl(ForwardSequence in, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<ForwardSequence>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, ForwardSequence>));

            typedef tuple<ForwardSequence, ForwardSequence> Tuple;

            if(!in)
            {
                return Tuple{in, in};
            }

            auto cmp_min = ural::compare_by(ural::dereference<>{}, std::cref(cmp));
            auto cmp_max = ural::make_binary_reverse_args(cmp_min);

            ::ural::min_element_accumulator<ForwardSequence, decltype(cmp_min)>
                acc_min(in, std::move(cmp_min));
            ::ural::min_element_accumulator<ForwardSequence, decltype(cmp_max)>
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

    // Перестановки
    /** @ingroup SortingOperations
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
        template <class BiSequenced, class Compare = ::ural::less<>>
        bool operator()(BiSequenced && s, Compare cmp = Compare()) const
        {
            BOOST_CONCEPT_ASSERT((concepts::BidirectionalSequenced<BiSequenced>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare,
                                                             SequenceType<BiSequenced>>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<SequenceType<BiSequenced>, Compare>));

            return this->impl(::ural::sequence_fwd<BiSequenced>(s),
                              ::ural::make_callable(std::move(cmp)));

        }

    private:
        template <class BiSequence, class Compare>
        bool impl(BiSequence s, Compare cmp) const
        {
            BOOST_CONCEPT_ASSERT((concepts::BidirectionalSequence<BiSequence>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, BiSequence>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<BiSequence, Compare>));

            if(!s)
            {
                return false;
            }

            auto s1 = ural::next(s);

            if(!s1)
            {
                return false;
            }

            auto r = ::ural::is_sorted_until_fn{}(s | ural::reversed, cmp);

            if(!r)
            {
                ::ural::reverse_fn{}(std::move(s));
                return false;
            }
            else
            {
                auto r1 = r;
                auto r2 = s | ural::reversed;

                for(; cmp(*r2, *r1); ++r2)
                {}

                ::ural::details::do_swap(*r1, *r2);
                ural::reverse_fn{}(r1.traversed_front().base());

                return true;
            }
        }
    };

    /** @ingroup SortingOperations
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
        template <class BiSequenced, class Compare = ::ural::less<>>
        bool operator()(BiSequenced && s, Compare cmp = Compare()) const
        {
            BOOST_CONCEPT_ASSERT((concepts::BidirectionalSequenced<BiSequenced>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare,
                                                             SequenceType<BiSequenced>>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<SequenceType<BiSequenced>, Compare>));

            return this->impl(::ural::sequence_fwd<BiSequenced>(s),
                              ::ural::make_callable(std::move(cmp)));

        }

    private:
        template <class BiSequence, class Compare>
        bool impl(BiSequence s, Compare cmp) const
        {
            BOOST_CONCEPT_ASSERT((concepts::BidirectionalSequence<BiSequence>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<Compare, BiSequence>));
            BOOST_CONCEPT_ASSERT((concepts::Sortable<BiSequence, Compare>));

            auto constexpr f = next_permutation_fn{};
            return f(std::move(s), ::ural::not_fn(std::move(cmp)));
        }
    };

    /** @ingroup MutatingSequenceOperations
    @brief Тип функционального объекта, устраняющего из последовательности
    элементы, удовлетворяющих заданному предикату.
    */
    class remove_if_fn
    {
    public:
        /** @brief Устраняет из @c seq элементы, удовлетворяющие предикату
        @c pred.
        @param seq входная последовательность
        @param pred унарный предикат
        @return Последовательность, передняя пройденная часть которой содержит
        неустранённые элементы, причём исходной для неё является @c seq.
        */
        template <class ForwardSequence, class Predicate>
        auto operator()(ForwardSequence && seq, Predicate pred) const
        -> decltype(::ural::sequence_fwd<ForwardSequence>(seq))
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequenced<ForwardSequence>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectPredicate<Predicate, SequenceType<ForwardSequence>>));
            BOOST_CONCEPT_ASSERT((concepts::Permutable<SequenceType<ForwardSequence>>));

            return this->impl(::ural::sequence_fwd<ForwardSequence>(seq),
                              ::ural::make_callable(std::move(pred)));
        }

    private:
        template <class ForwardSequence, class Predicate>
        ForwardSequence
        impl(ForwardSequence in, Predicate pred) const
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<ForwardSequence>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectPredicate<Predicate, ForwardSequence>));
            BOOST_CONCEPT_ASSERT((concepts::Permutable<ForwardSequence>));

            auto out = find_if_fn{}(std::move(in), pred);

            if(!out)
            {
                return out;
            }

            auto in_filtered = ural::next(out) | ural::removed_if(std::move(pred));

            return ural::move_fn{}(in_filtered, out)[ural::_2];
        }
    };

    /** @ingroup MutatingSequenceOperations
    @brief Тип функционального объекта, устраняющего из последовательности
    элементы, находящиеся в заднному отношении с заданным значением.
    */
    class remove_fn
    {
    public:
        /** @brief Устраняет из @c seq элементы, эквивалентные в смысле @c pred
        @c value.
        @param seq входная последовательность
        @param value значение
        @param pred бинарный предикат, определяющий эквивалентность элементов
        @return Последовательность, передняя пройденная часть которой содержит
        неустранённые элементы, причём исходной для неё является @c seq.
        */
        template <class ForwardSequence, class T,
                  class BinaryPredicate = ::ural::equal_to<>>
        auto operator()(ForwardSequence && seq, T const & value,
                        BinaryPredicate pred = BinaryPredicate()) const
        -> decltype(::ural::sequence_fwd<ForwardSequence>(seq))
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequenced<ForwardSequence>));
            BOOST_CONCEPT_ASSERT((concepts::Permutable<SequenceType<ForwardSequence>>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<BinaryPredicate,
                                                             SequenceType<ForwardSequence>,
                                                             T const *>));

            return this->impl(::ural::sequence_fwd<ForwardSequence>(seq),
                              value, ::ural::make_callable(std::move(pred)));
        }

    private:
        template <class ForwardSequence, class T, class BinaryPredicate>
        ForwardSequence
        impl(ForwardSequence in, T const & value,
             BinaryPredicate pred) const
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<ForwardSequence>));
            BOOST_CONCEPT_ASSERT((concepts::Permutable<ForwardSequence>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<BinaryPredicate, ForwardSequence, T const *>));

            auto pred_1 = std::bind(std::move(pred), ural::_1, std::cref(value));
            return remove_if_fn{}(in, std::move(pred_1));
        }
    };

    /** @ingroup MutatingSequenceOperations
    @brief Тип функционального объекта, выполняющего копирование элементов одной
    последовательности в другую за исключением удовлетворяющих заданному
    предикату.
    */
    class remove_copy_if_fn
    {
    public:
        /** @brief Копирование элементов @c x одной последовательности в другую
        за исключением удовлетворяющих предикату @c pred.
        @param in входная последовательность
        @param out выходная последовательность
        @param pred унарный предикат
        @return Кортеж, содержащий непройденные части последовательности
        */
        template <class Input, class Output, class Predicate>
        auto operator()(Input && in, Output && out, Predicate pred) const
        -> tuple<decltype(::ural::sequence_fwd<Input>(in)),
                 decltype(::ural::sequence_fwd<Output>(out))>
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input>));
            BOOST_CONCEPT_ASSERT((concepts::SinglePassSequenced<Output>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectPredicate<Predicate,
                                                              SequenceType<Input>>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyCopyable<SequenceType<Input>,
                                                               SequenceType<Output>>));

            return ural::copy_if_fn{}(std::forward<Input>(in),
                                      std::forward<Output>(out),
                                      ural::not_fn(std::move(pred)));
        }
    };

    /** @ingroup MutatingSequenceOperations
    @brief Тип функционального объекта, выполняющего копирование элементов одной
    последовательности в другую за исключением эквивалентных заданному значению.
    */
    class remove_copy_fn
    {
    public:
        /** @brief Копирование элементов @c x одной последовательности в другую
        за исключением удовлетворяющих <tt> bin_pred(x, value) != false </tt>.
        @param in входная последовательность
        @param out выходная последовательность
        @param value значение, которое не нужно копировать
        @param bin_pred бинарный предикат, по умолчанию используется
        <tt> ::ural::equal_to<> </tt>, то есть оператор "равно"
        @return Кортеж, содержащий непройденные части последовательности
        */
        template <class Input, class Output, class T,
                  class BinaryPredicate = ::ural::equal_to<>>
        auto operator()(Input && in, Output && out, T const & value,
                        BinaryPredicate bin_pred = BinaryPredicate()) const
        -> tuple<decltype(::ural::sequence_fwd<Input>(in)),
                 decltype(::ural::sequence_fwd<Output>(out))>
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequenced<Input>));
            BOOST_CONCEPT_ASSERT((concepts::SinglePassSequenced<Output>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyCopyable<SequenceType<Input>,
                                                               SequenceType<Output>>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<BinaryPredicate,
                                                             SequenceType<Input>,
                                                             T const *>));

            auto pred = std::bind(::ural::make_callable(std::move(bin_pred)),
                                  ural::_1, std::cref(value));
            return ::ural::remove_copy_if_fn{}(std::forward<Input>(in),
                                               std::forward<Output>(out),
                                               std::move(pred));
        }
    };

    // Алгоритмы над контейнерами
    class remove_if_erase_fn
    {
    public:
        /** Физически удаляет элементы, удовлетворяющие предикату, из контейнера
        @brief Оператор вызова функции
        @param c контейнер
        @param pred предикат
        @return @c c
        */
        template <class Container, class Predicate>
        Container & operator()(Container & c, Predicate pred) const
        {
            auto to_erase = remove_if_fn{}(c, pred);
            erase_fn{}(c, to_erase);
            return c;
        }
    };

    class remove_erase_fn
    {
    public:
        /** Физически удаляет элементы, равные заданному значению
        @brief Оператор вызова функции
        @param target контейнер
        @param value значение
        @return @c target
        */
        template <class Container, class Value>
        Container & operator()(Container & target, Value const & value) const
        {
            auto to_erase = remove_fn{}(target, value);

            erase_fn{}(target, to_erase);

            return target;
        }
    };

    auto constexpr const erase = erase_fn{};
    auto constexpr const remove_erase = remove_erase_fn{};

    auto constexpr const remove_if_erase = remove_if_erase_fn{};

    auto constexpr const unique_erase = unique_erase_fn{};

    class move_if_noexcept_fn
    {
    public:
        template <class Input, class Output>
        auto operator()(Input && in, Output && out) const
        -> tuple<decltype(::ural::sequence_fwd<Input>(in)),
                 decltype(::ural::sequence_fwd<Output>(out))>
        {
            return this->impl(::ural::sequence_fwd<Input>(in),
                              ::ural::sequence_fwd<Output>(out));
        }

        template <class T>
        constexpr
        typename std::conditional<!std::is_nothrow_move_constructible<T>::value
                                  && std::is_move_constructible<T>::value,
                                  T const &, T &&>::type
        operator()(T & x) const
        {
            return std::move_if_noexcept(x);
        }

    private:
        template <class Input, class Output>
        tuple<Input, Output>
        impl(Input in, Output out) const
        {
            auto r = ural::copy_fn{}(std::move(in) | transformed(cref = *this),
                                     std::move(out));
            return ural::make_tuple(r[ural::_1].bases()[ural::_1],
                                    r[ural::_2]);
        }
    };
    constexpr auto move_if_noexcept = move_if_noexcept_fn{};

    inline namespace
    {
        // 25.2 Немодифицирующие
        // 25.2.1-3 Кванторы
        constexpr auto const all_of = all_of_fn{};
        constexpr auto const none_of = none_of_fn{};
        constexpr auto const any_of = any_of_fn{};

        // 25.2.4 for_each
        constexpr auto const for_each = for_each_fn{};

        // 25.2.5 Поиск
        constexpr auto const find = find_fn{};
        constexpr auto const find_if = find_if_fn{};
        constexpr auto const find_if_not = find_if_not_fn{};

        // 25.2.6 Поиск конца подпоследовательности (find_end)
        constexpr auto const find_end = find_end_fn{};

        // 25.2.7 Поиск первого вхождения
        constexpr auto const find_first_of = find_first_of_fn{};
        constexpr auto const find_first_not_of = find_first_not_of_fn{};

        // 25.2.8 Поиск соседних одинаковых элементов
        constexpr auto const adjacent_find = adjacent_find_fn{};

        // 25.2.9 Подсчёт
        constexpr auto const count = count_fn{};
        constexpr auto const count_if = count_if_fn{};

        // 25.2.10 Поиск несовпадения
        constexpr auto const mismatch = mismatch_fn{};

        // 25.2.11 Равенство
        constexpr auto const equal = equal_fn{};

        // 25.2.12 Являются ли две последовательности перестановками?
        constexpr auto const is_permutation = is_permutation_fn{};

        // 25.2.13 Поиск подпоследовательностей
        constexpr auto const search = search_fn{};
        constexpr auto const search_n = search_n_fn{};

        // 25.3 Модифицирующие алгоритмы
        // 25.3.1 Копирование
        constexpr auto const copy = copy_fn{};
        constexpr auto const copy_n = copy_n_fn{};
        constexpr auto const copy_if = copy_if_fn{};
        constexpr auto const copy_backward = copy_backward_fn{};

        // 25.3.2 Перемещение
        constexpr auto const move = move_fn{};
        constexpr auto const move_backward = move_backward_fn{};

        // 25.3.3 Обмен
        constexpr auto const swap_ranges = swap_ranges_fn{};

        // 25.3.4 Преобразование
        constexpr auto const transform = transform_fn{};

        // 25.3.5 Замена
        constexpr auto const replace = replace_fn{};
        constexpr auto const replace_if = replace_if_fn{};
        constexpr auto const replace_copy = replace_copy_fn{};
        constexpr auto const replace_copy_if = replace_copy_if_fn{};

        // 25.3.6 Заполнение
        constexpr auto const fill = fill_fn{};
        constexpr auto const fill_n = fill_n_fn{};

        // 25.3.7 Порождение
        constexpr auto const generate = generate_fn{};
        constexpr auto const generate_n = generate_n_fn{};

        // 25.3.8 Удаление
        constexpr auto const remove = remove_fn{};
        constexpr auto const remove_if = remove_if_fn{};
        constexpr auto const remove_copy = remove_copy_fn{};
        constexpr auto const remove_copy_if = remove_copy_if_fn{};

        // 25.3.9 Устранение последовательных дубликатов
        constexpr auto const unique = unique_fn{};
        constexpr auto const unique_copy = unique_copy_fn{};

        // 25.3.10 Обращение
        constexpr auto const reverse = reverse_fn{};
        constexpr auto const reverse_copy = reverse_copy_fn{};

        // 25.3.11 Вращение
        constexpr auto const rotate = rotate_fn{};
        constexpr auto const rotate_copy = rotate_copy_fn{};

        // 25.3.12 Тасовка
        constexpr auto const shuffle = shuffle_fn{};
        constexpr auto const random_shuffle = random_shuffle_fn{};

        // 25.3.13 Разделение
        constexpr auto const is_partitioned = is_partitioned_fn{};
        constexpr auto const partition = partition_fn{};
        constexpr auto const stable_partition = stable_partition_fn{};
        constexpr auto const partition_copy = partition_copy_fn{};
        // @todo partition_move
        constexpr auto const partition_point = partition_point_fn{};

        // 25.4 Сортировка и связанные с ним операции
        // 25.4.1 Сортировка
        // @todo объект для insertion_sort?

        // 25.4.1.1 Быстрая сортировка
        constexpr auto const sort = sort_fn{};

        // 25.4.1.2 Устойчивая сортировка
        constexpr auto const stable_sort = stable_sort_fn{};

        // 25.4.1.3 Частичная сортировка
        constexpr auto const partial_sort = partial_sort_fn{};

        // 25.4.1.4 Частичная сортировка с копированием
        constexpr auto const partial_sort_copy = partial_sort_copy_fn{};

        // 25.4.1.5 Проверка сортированности
        constexpr auto const is_sorted = is_sorted_fn{};
        constexpr auto const is_sorted_until = is_sorted_until_fn{};

        // 25.4.2 N-ый элемент
        constexpr auto const nth_element = nth_element_fn{};

        // 25.4.3 Бинарный поиск
        constexpr auto const lower_bound = lower_bound_fn{};
        constexpr auto const upper_bound = upper_bound_fn{};
        constexpr auto const equal_range = equal_range_fn{};
        constexpr auto const binary_search = binary_search_fn{};

        // 25.4.4 Слияние
        constexpr auto const merge = merge_fn{};
        // @todo merge_move
        constexpr auto const inplace_merge = inplace_merge_fn{};

        // 25.4.5 Операции с сортированными множествами
        constexpr auto const includes = includes_fn{};
        constexpr auto const set_union = set_union_fn{};
        constexpr auto const set_intersection = set_intersection_fn{};
        constexpr auto const set_difference = set_difference_fn{};
        constexpr auto const set_symmetric_difference = set_symmetric_difference_fn{};

        // 25.4.6 Операции с бинарными кучами
        constexpr auto const push_heap = push_heap_fn{};
        constexpr auto const pop_heap = pop_heap_fn{};
        constexpr auto const make_heap = make_heap_fn{};
        constexpr auto const sort_heap = sort_heap_fn{};
        constexpr auto const is_heap = is_heap_fn{};
        constexpr auto const is_heap_until = is_heap_until_fn{};

        // 25.4.7 Наибольшее и наименьшее значение
        constexpr auto const min = min_fn{};
        constexpr auto const max = max_fn{};
        constexpr auto const minmax = minmax_fn{};
        constexpr auto const min_element = min_element_fn{};
        constexpr auto const max_element = max_element_fn{};
        constexpr auto const minmax_element = minmax_element_fn{};

        // 25.4.8 Лексикографическое сравнение
        constexpr auto const lexicographical_compare = lexicographical_compare_fn{};

        // 25.4.9 Порождение перестановка
        constexpr auto const next_permutation = next_permutation_fn{};
        constexpr auto const prev_permutation = prev_permutation_fn{};
    }
}
// namespace ural

#endif
// Z_URAL_ALGORITHM_HPP_INCLUDED
