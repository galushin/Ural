#ifndef Z_URAL_ALGORITHM_MUTATING_HPP_INCLUDED
#define Z_URAL_ALGORITHM_MUTATING_HPP_INCLUDED

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
 @brief Алгоритмы, модифицирующие последовательность
*/

/** @defgroup MutatingSequenceOperations Модифицирующие операции
 @ingroup Algorithms
 @brief Операции, формирующие новые последовательности или меняющие порядок
 элементов существующих последовательностей.
*/

#include <ural/sequence/generator.hpp>
#include <ural/sequence/partition.hpp>
#include <ural/sequence/uniqued.hpp>
#include <ural/sequence/replace.hpp>
#include <ural/sequence/transform.hpp>
#include <ural/sequence/moved.hpp>
#include <ural/sequence/reversed.hpp>
#include <ural/sequence/filtered.hpp>
#include <ural/sequence/taken.hpp>

#include <ural/algorithm/core.hpp>

namespace ural
{
    // @todo нужно либо удалить, либо реализовать
    template <class T1, class T2>
    void swap(T1 & x, T2 & y);

/// @cond false
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
/// @endcond

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

    // Обмен
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
}
// namespace ural

#endif
// Z_URAL_ALGORITHM_MUTATING_HPP_INCLUDED
