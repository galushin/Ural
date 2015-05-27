#ifndef Z_URAL_ALGORITHM_NON_MODIFYING_HPP_INCLUDED
#define Z_URAL_ALGORITHM_NON_MODIFYING_HPP_INCLUDED

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

/** @file ural/algorithm/non_modifying.hpp
 @brief Алгоритмы, не модифицирующие последовательность
*/

/** @defgroup NonModifyingSequenceOperations Немодифицирующие операции
 @ingroup Algorithms
 @brief Обобщённые операции не создающие новых последовательностей и не
 изменяющие порядок элементов существующих последовательностей.
*/

#include <ural/sequence/function_output.hpp>

#include <ural/algorithm/core.hpp>

namespace ural
{
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
    @brief Класс функционального объекта, применяющий заданный
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
}
//namespace ural

#endif
// Z_URAL_ALGORITHM_NON_MODIFYING_HPP_INCLUDED
