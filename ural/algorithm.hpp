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
*/

/** @defgroup Algorithms Алгоритмы
 @brief Обобщённые операции над последовательностями.
*/

/** @defgroup NonModifyingSequenceOperations Немодифицирующие операции
 @ingroup Algorithms
 @brief Обобщённые операции не создающие новых последовательностей и не
 изменяющие порядок элементов существующих последовательностей.
*/

/** @defgroup MutatingSequenceOperations
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
#include <ural/sequence/replace.hpp>
#include <ural/sequence/set_operations.hpp>
#include <ural/sequence/taken.hpp>
#include <ural/sequence/filtered.hpp>
#include <ural/sequence/transform.hpp>
#include <ural/sequence/moved.hpp>
#include <ural/sequence/make.hpp>
#include <ural/sequence/uniqued.hpp>

#include <ural/functional.hpp>
#include <ural/random/c_rand_engine.hpp>
#include <ural/functional/make_callable.hpp>

#include <ural/algorithm/copy.hpp>
#include <ural/algorithm/details/algo_base.hpp>

namespace ural
{
    // Модифицирующие алгоритмы
    class unique_fn
    {
    public:
        /** @brief Устранение последовательных дубликатов
        @param seq последовательность
        */
        template <class ForwardSequence>
        auto operator()(ForwardSequence && seq) const
        -> decltype(ural::sequence_fwd<ForwardSequence>(seq))
        {
            return (*this)(std::forward<ForwardSequence>(seq), ural::equal_to<>{});
        }

        /** @brief Устранение последовательных дубликатов
        @param seq последовательность
        @param pred бинарный предикат
        */
        template <class ForwardSequence, class BinaryPredicate>
        auto operator()(ForwardSequence && seq, BinaryPredicate pred) const
        -> decltype(ural::sequence_fwd<ForwardSequence>(seq))
        {
            return this->impl(ural::sequence_fwd<ForwardSequence>(seq),
                              make_callable(std::move(pred)));
        }

    private:
        template <class ForwardSequence, class BinaryPredicate>
        ForwardSequence
        impl(ForwardSequence seq, BinaryPredicate pred) const
        {
            typedef typename ForwardSequence::value_type Value;

            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<ForwardSequence>));
            BOOST_CONCEPT_ASSERT((concepts::ReadableSequence<ForwardSequence>));
            BOOST_CONCEPT_ASSERT((concepts::WritableSequence<ForwardSequence, Value>));
            BOOST_CONCEPT_ASSERT((concepts::Callable<BinaryPredicate, bool(Value, Value)>));

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
        template <class Container, class Iterator, class Policy>
        auto operator()(Container & c,
                        iterator_sequence<Iterator, Policy> seq) const
        -> typename Container::iterator
        {
            return c.erase(seq.begin(), seq.end());
        }
    };

    class unique_erase_fn
    {
    public:
        /** @brief Удаление последовательных дубликатов из контейнера
        @param c контейнер
        @return @c c
        */
        template <class Container>
        Container & operator()(Container & c) const
        {
            return (*this)(c, ural::equal_to<>{});
        }

        /** @brief Удаление последовательных дубликатов из контейнера
        @param c контейнер
        @param bin_pred бинарный предикат, с помощью которого определяются
        дубликаты
        @return @c c
        */
        template <class Container, class BinaryPredicate>
        Container &
        operator()(Container & c, BinaryPredicate bin_pred) const
        {
            auto to_erase = unique_fn{}(c, std::move(bin_pred));
            erase_fn{}(c, to_erase);
            return c;
        }
    };

    class for_each_fn
    {
    private:
        template <class Input, class UnaryFunction>
        static UnaryFunction impl(Input in, UnaryFunction f)
        {
            BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassSequence<Input>));
            BOOST_CONCEPT_ASSERT((ural::concepts::ReadableSequence<Input>));
            BOOST_CONCEPT_ASSERT((ural::concepts::Callable<UnaryFunction, void(decltype(*in))>));

            auto r = ural::copy_fn{}(in, ural::make_function_output_sequence(std::move(f)));
            return r[ural::_2].function();
        }

    public:
        /** @brief Применяет функциональный объект к каждому элементу
        последовательности
        @param in входная последовательность
        @param f функциональный объект
        @return @c f
        */
        template <class Input, class UnaryFunction>
        auto operator()(Input && in, UnaryFunction f) const
        -> decltype(ural::make_callable(std::move(f)))
        {
            return for_each_fn::impl(sequence_fwd<Input>(in),
                                     ural::make_callable(std::move(f)));
        }
    };

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
            return !find_if_not_fn{}(std::forward<Input>(in), std::move(pred));
        }
    };

    class find_first_of_fn
    {
    public:
        template <class Input, class Forward>
        auto operator()(Input && in, Forward && s) const
        -> decltype(sequence_fwd<Input>(in))
        {
            return (*this)(std::forward<Input>(in),
                           std::forward<Forward>(s),
                           ural::equal_to<>{});
        }

        template <class Input, class Forward, class BinaryPredicate>
        auto operator()(Input && in, Forward && s, BinaryPredicate bin_pred) const
        -> decltype(sequence_fwd<Input>(in))
        {
            return this->impl(sequence_fwd<Input>(in),
                              sequence_fwd<Forward>(s),
                              make_callable(std::move(bin_pred)));
        }

    private:
        template <class Input, class Forward, class BinaryPredicate>
        static Input impl(Input in, Forward s, BinaryPredicate bin_pred)
        {
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

    /**
    @todo уменьшить дублирование с find_first_of
    */
    class find_first_not_of_fn
    {
    public:
        template <class Input, class Forward>
        auto operator()(Input && in, Forward && s) const
        -> decltype(sequence_fwd<Input>(in))
        {
            return (*this)(sequence_fwd<Input>(in), sequence_fwd<Forward>(s),
                           ::ural::equal_to<>{});
        }

        template <class Input, class Forward, class BinaryPredicate>
        auto operator()(Input && in, Forward && s, BinaryPredicate bin_pred) const
        -> decltype(sequence_fwd<Input>(in))
        {
            return this->impl(sequence_fwd<Input>(in), sequence_fwd<Forward>(s),
                              ural::make_callable(std::move(bin_pred)));
        }

    private:
        template <class Input, class Forward, class BinaryPredicate>
        static Input impl(Input in, Forward s, BinaryPredicate bin_pred)
        {
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

    class adjacent_find_fn
    {
    public:
        template <class Forward>
        auto operator()(Forward && s) const
        -> decltype(sequence_fwd<Forward>(s))
        {
            return (*this)(std::forward<Forward>(s), ::ural::equal_to<>{});
        }

        template <class Forward, class BinaryPredicate>
        auto operator()(Forward && s, BinaryPredicate pred) const
        -> decltype(sequence_fwd<Forward>(s))
        {
            return this->impl(sequence_fwd<Forward>(s),
                              ural::make_callable(std::move(pred)));
        }
    private:
        template <class Forward, class BinaryPredicate>
        static Forward impl(Forward s, BinaryPredicate pred)
        {
            if(!s)
            {
                return s;
            }

            auto s_next = ural::next(s);

            for(; !!s_next; ++ s_next)
            {
                if(pred(*s, *s_next))
                {
                    return s;
                }
                s = s_next;
            }
            return s_next;
        }
    };

    class mismatch_fn
    {
    public:
        template <class Input1, class Input2>
        auto operator()(Input1 && in1, Input2 && in2) const
        -> tuple<decltype(sequence_fwd<Input1>(in1)),
                 decltype(sequence_fwd<Input2>(in2))>
        {
            return (*this)(std::forward<Input1>(in1),
                           std::forward<Input2>(in2), ural::equal_to<>());
        }

        template <class Input1, class Input2, class BinaryPredicate>
        auto operator()(Input1 && in1, Input2 && in2, BinaryPredicate pred) const
        -> tuple<decltype(sequence_fwd<Input1>(in1)),
                 decltype(sequence_fwd<Input2>(in2))>
        {
            return this->impl(sequence_fwd<Input1>(in1),
                              sequence_fwd<Input2>(in2),
                              make_callable(std::move(pred)));
        }

    private:
        template <class Input1, class Input2, class BinaryPredicate>
        static tuple<Input1, Input2>
        impl(Input1 in1, Input2 in2, BinaryPredicate pred)
        {
            BOOST_CONCEPT_ASSERT((concepts::SinglePassSequence<Input1>));
            BOOST_CONCEPT_ASSERT((concepts::ReadableSequence<Input1>));
            BOOST_CONCEPT_ASSERT((concepts::SinglePassSequence<Input2>));
            BOOST_CONCEPT_ASSERT((concepts::ReadableSequence<Input2>));
            BOOST_CONCEPT_ASSERT((concepts::Callable<BinaryPredicate,
                                                     bool(decltype(*in1), decltype(*in2))>));

            typedef tuple<Input1, Input2> Tuple;
            for(; !!in1 && !!in2; ++ in1, ++ in2)
            {
                if(!pred(*in1, *in2))
                {
                    break;
                }
            }
            return Tuple{std::move(in1), std::move(in2)};
        }
    };

    class equal_fn
    {
    public:
        template <class Input1, class Input2>
        bool operator()(Input1 && in1, Input2 && in2) const
        {
            return (*this)(std::forward<Input1>(in1),
                           std::forward<Input2>(in2), ural::equal_to<>());
        }

        template <class Input1, class Input2, class BinaryPredicate>
        bool operator()(Input1 && in1, Input2 && in2, BinaryPredicate pred) const
        {
            return this->impl(sequence_fwd<Input1>(in1),
                              sequence_fwd<Input2>(in2),
                              ural::make_callable(std::move(pred)));
        }
    private:
        template <class Input1, class Input2, class BinaryPredicate>
        static bool impl(Input1 in1, Input2 in2, BinaryPredicate pred)
        {
            BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassSequence<Input1>));
            BOOST_CONCEPT_ASSERT((ural::concepts::ReadableSequence<Input1>));
            BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassSequence<Input2>));
            BOOST_CONCEPT_ASSERT((ural::concepts::ReadableSequence<Input2>));
            BOOST_CONCEPT_ASSERT((ural::concepts::Callable<BinaryPredicate,
                                                           bool(decltype(*in1), decltype(*in2))>));

            auto const r = ural::mismatch_fn{}(std::move(in1), std::move(in2),
                                               std::move(pred));
            return !r[ural::_1] && !r[ural::_2];
        }
    };

    class search_fn
    {
    public:
        template <class Forward1, class Forward2>
        auto operator()(Forward1 && in, Forward2 && s) const
        -> decltype(sequence_fwd<Forward1>(in))
        {
            return (*this)(std::forward<Forward1>(in),
                           std::forward<Forward2>(s),
                           ural::equal_to<>{});
        }

        template <class Forward1, class Forward2, class BinaryPredicate>
        auto operator()(Forward1 && in, Forward2 && s, BinaryPredicate bin_pred) const
        -> decltype(sequence_fwd<Forward1>(in))
        {
            return this->impl(sequence_fwd<Forward1>(in),
                              sequence_fwd<Forward2>(s),
                              ural::make_callable(std::move(bin_pred)));
        }

    private:
        template<class Forward1, class Forward2, class BinaryPredicate>
        static Forward1 impl(Forward1 in, Forward2 s, BinaryPredicate p)
        {
            BOOST_CONCEPT_ASSERT((ural::concepts::ForwardSequence<Forward1>));
            BOOST_CONCEPT_ASSERT((ural::concepts::ReadableSequence<Forward1>));
            BOOST_CONCEPT_ASSERT((ural::concepts::ForwardSequence<Forward2>));
            BOOST_CONCEPT_ASSERT((ural::concepts::ReadableSequence<Forward2>));

            BOOST_CONCEPT_ASSERT((ural::concepts::Callable<BinaryPredicate,
                                                           bool(decltype(*in), decltype(*s))>));

            for(;; ++ in)
            {
                auto i = in;
                auto i_s = s;
                for(;; ++ i, ++ i_s)
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

    class find_end_fn
    {
    public:
        template <class Forward1, class Forward2>
        auto operator()(Forward1 && in, Forward2 && s) const
        -> decltype(sequence_fwd<Forward1>(in))
        {
            return (*this)(std::forward<Forward1>(in),
                           std::forward<Forward2>(s),
                           ural::equal_to<>{});
        }

        template <class Forward1, class Forward2, class BinaryPredicate>
        auto operator()(Forward1 && in, Forward2 && s, BinaryPredicate bin_pred) const
        -> decltype(sequence_fwd<Forward1>(in))
        {
            return this->impl(sequence_fwd<Forward1>(in),
                              sequence_fwd<Forward2>(s),
                              make_callable(std::move(bin_pred)));
        }
    private:
        template <class Forward1, class Forward2, class BinaryPredicate>
        static Forward1
        impl(Forward1 in, Forward2 s, BinaryPredicate bin_pred)
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Forward1>));
            BOOST_CONCEPT_ASSERT((concepts::ReadableSequence<Forward1>));
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Forward2>));
            BOOST_CONCEPT_ASSERT((concepts::ReadableSequence<Forward2>));

            BOOST_CONCEPT_ASSERT((ural::concepts::Callable<BinaryPredicate,
                                                           bool(decltype(*in), decltype(*s))>));
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

    class search_n_fn
    {
    public:
        template <class Forward, class Size, class T>
        auto operator()(Forward && in, Size count, T const & value) const
        -> decltype(sequence_fwd<Forward>(in))
        {
            return (*this)(std::forward<Forward>(in), std::move(count),
                           value, ural::equal_to<>{});
        }

        template <class Forward, class Size, class T,  class BinaryPredicate>
        auto operator()(Forward && in, Size count, T const & value,
                        BinaryPredicate bin_pred) const
        -> decltype(sequence_fwd<Forward>(in))
        {
            return this->impl(sequence_fwd<Forward>(in),
                              std::move(count), value,
                              ural::make_callable(std::move(bin_pred)));
        }

    private:
        template <class Forward, class Size, class T,  class BinaryPredicate>
        static Forward impl(Forward in, Size const n, T const & value,
                            BinaryPredicate bin_pred)
        {
            BOOST_CONCEPT_ASSERT((ural::concepts::ForwardSequence<Forward>));
            BOOST_CONCEPT_ASSERT((ural::concepts::ReadableSequence<Forward>));
            BOOST_CONCEPT_ASSERT((ural::concepts::Callable<BinaryPredicate, bool(decltype(*in), T)>));

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
                Size cur_count = 0;

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
    class copy_n_fn
    {
    public:
        template <class Input, class Size, class Output>
        auto operator()(Input && in, Size n, Output && out) const
        -> tuple<decltype(::ural::sequence_fwd<Input>(in)),
                 decltype(::ural::sequence_fwd<Output>(out))>
        {
            auto in_n = ::ural::sequence_fwd<Input>(in) | ural::taken(n);

            auto result = ::ural::copy_fn{}(std::move(in_n),
                                            ::ural::sequence_fwd<Output>(out));
            return ural::make_tuple(std::move(result[ural::_1]).base(),
                                    std::move(result[ural::_2]));
        }
    };

    class copy_if_fn
    {
    public:
        template <class Input, class Output, class Predicate>
        auto operator()(Input && in, Output && out, Predicate pred) const
        -> tuple<decltype(sequence_fwd<Input>(in)),
                 decltype(sequence_fwd<Output>(out))>
        {
            auto in_f = sequence_fwd<Input>(in) | ural::filtered(pred);
            auto res = ural::copy_fn{}(std::move(in_f), sequence_fwd<Output>(out));

            return ural::make_tuple(std::move(res[ural::_1]).base(),
                                    std::move(res[ural::_2]));
        }
    };

    class move_fn
    {
    public:
        template <class Input, class Output>
        auto operator()(Input && in, Output && out) const
        -> tuple<decltype(sequence_fwd<Input>(in)),
                 decltype(sequence_fwd<Output>(out))>
        {
            auto in_moved = ural::sequence_fwd<Input>(in) | ural::moved;
            auto res = ural::copy_fn{}(std::move(in_moved), sequence_fwd<Output>(out));
            return ural::make_tuple(res[ural::_1].base(), res[ural::_2]);
        }
    };

    // copy_backward
    class copy_backward_fn
    {
    public:
        template <class Bidir1, class Bidir2>
        auto operator()(Bidir1 && in, Bidir2 && out) const
        -> tuple<decltype(sequence_fwd<Bidir1>(in)),
                 decltype(sequence_fwd<Bidir2>(out))>
        {
            return this->impl(sequence_fwd<Bidir1>(in),
                              sequence_fwd<Bidir2>(out));
        }

    private:
        template <class Bidir1, class Bidir2>
        tuple<Bidir1, Bidir2>
        impl(Bidir1 in, Bidir2 out) const
        {
            auto res = ural::copy_fn{}(std::move(in) | ural::reversed,
                                       std::move(out) | ural::reversed);

            return ural::make_tuple(std::move(res[ural::_1].base()),
                                    std::move(res[ural::_2].base()));
        }
    };

    auto constexpr copy_backward = copy_backward_fn{};

    // move_backward
    class move_backward_fn
    {
    public:
        template <class Bidir1, class Bidir2>
        auto operator()(Bidir1 && in, Bidir2 && out) const
        -> tuple<decltype(sequence_fwd<Bidir1>(in)),
                 decltype(sequence_fwd<Bidir2>(out))>
        {
            return this->impl(sequence_fwd<Bidir1>(in),
                              sequence_fwd<Bidir2>(out));
        }

    private:
        template <class Bidir1, class Bidir2>
        tuple<Bidir1, Bidir2>
        impl(Bidir1 in, Bidir2 out) const
        {
            auto res = ural::move_fn{}(std::move(in) | ural::reversed | ural::moved,
                                       std::move(out) | ural::reversed);

            return ural::make_tuple(std::move(res[ural::_1].base().base()),
                                    std::move(res[ural::_2].base()));
        }
    };

    // 25.3.4
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
        -> tuple<decltype(sequence_fwd<Input>(in)),
                 decltype(sequence_fwd<Output>(out))>
        {
            return this->impl(sequence_fwd<Input>(in),
                              sequence_fwd<Output>(out),
                              ural::make_callable(std::move(f)));
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
        -> tuple<decltype(sequence_fwd<Input1>(in1)),
                 decltype(sequence_fwd<Input2>(in2)),
                 decltype(sequence_fwd<Output>(out))>
        {
            return this->impl(sequence_fwd<Input1>(in1),
                              sequence_fwd<Input2>(in2),
                              sequence_fwd<Output>(out),
                              ural::make_callable(std::move(f)));
        }

    private:
        template <class Input, class Output, class UnaryFunction>
        tuple<Input, Output>
        impl(Input in, Output out, UnaryFunction f) const
        {
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

    class generate_fn
    {
    public:
        template <class ForwardSequence, class Generator>
        auto operator()(ForwardSequence && seq, Generator gen) const
        -> decltype(ural::sequence_fwd<ForwardSequence>(seq))
        {
            return this->impl(ural::sequence_fwd<ForwardSequence>(seq),
                              ural::make_callable(std::move(gen)));
        }

    private:
        template <class ForwardSequence, class Generator>
        static ForwardSequence
        impl(ForwardSequence seq, Generator gen)
        {
            // По сути, это проверка концепции для типа Generator
            typedef decltype(gen()) result_type;

            BOOST_CONCEPT_ASSERT((concepts::SinglePassSequence<ForwardSequence>));
            BOOST_CONCEPT_ASSERT((concepts::WritableSequence<ForwardSequence, result_type>));

            auto r = copy_fn{}(ural::make_generator_sequence(std::move(gen)),
                               std::move(seq));
            return r[ural::_2];
        }
    };

    class generate_n_fn
    {
    public:
        template <class Generator, class N, class Output>
        auto operator()(Generator gen, N n, Output && out) const
        -> decltype(ural::sequence_fwd<Output>(out))
        {
            auto in = ural::make_generator_sequence(ural::make_callable(gen))
                    | ural::taken(n);
            return ural::copy_fn{}(std::move(in),
                                   ural::sequence_fwd<Output>(out))[ural::_2];
        }
    };

    class fill_fn
    {
    public:
        template <class ForwardSequence, class T>
        auto operator()(ForwardSequence && seq, T const & value) const
        -> decltype(sequence_fwd<ForwardSequence>(seq))
        {
            return this->impl(sequence_fwd<ForwardSequence>(seq), value);
        }

    private:
        template <class ForwardSequence, class T>
        static ForwardSequence
        impl(ForwardSequence seq, T const & value)
        {
            BOOST_CONCEPT_ASSERT((concepts::SinglePassSequence<ForwardSequence>));
            BOOST_CONCEPT_ASSERT((concepts::WritableSequence<ForwardSequence, T const &>));

            return generate_fn{}(std::move(seq),
                                 ural::value_functor<T const &>(value));
        }
    };

    /** @brief Тип функционального объекта копирования неповторяющихся значений
    */
    class unique_copy_fn
    {
    public:
        template <class Input, class Output>
        auto operator()(Input && in, Output && out) const
        -> ural::tuple<decltype(sequence_fwd<Input>(in)),
                       decltype(sequence_fwd<Output>(out))>
        {
            return (*this)(std::forward<Input>(in), std::forward<Output>(out),
                           ::ural::equal_to<>{});
        }

        template <class Input, class Output, class BinaryPredicate>
        auto operator()(Input && in, Output && out, BinaryPredicate bin_pred) const
        -> ural::tuple<decltype(sequence_fwd<Input>(in)),
                       decltype(sequence_fwd<Output>(out))>
        {
            auto u_in = ::ural::make_unique_sequence(std::forward<Input>(in),
                                                     std::move(bin_pred));
            auto r = ural::copy_fn{}(std::move(u_in), std::forward<Output>(out));
            return ural::make_tuple(std::move(r[ural::_1]).base(),
                                    std::move(r[ural::_2]));
        }
    };

    class reverse_fn
    {
    public:
        template <class BidirectionalSequence>
        void operator()(BidirectionalSequence && seq) const
        {
            return this->impl(sequence_fwd<BidirectionalSequence>(seq));
        }

    private:
        template <class BidirectionalSequence>
        static void impl(BidirectionalSequence seq)
        {
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
        }
    };

    class reverse_copy_fn
    {
    public:
        template <class Bidirectional, class Output>
        auto operator()(Bidirectional && in, Output && out) const
        -> ural::tuple<decltype(::ural::sequence_fwd<Bidirectional>(in)),
                       decltype(::ural::sequence_fwd<Output>(out))>
        {
            auto in_reversed = ::ural::sequence_fwd<Bidirectional>(in)
                             | ural::reversed;
            auto result = ural::copy_fn{}(std::move(in_reversed),
                                          ::ural::sequence_fwd<Output>(out));
            return ural::make_tuple(std::move(result[ural::_1]).base(),
                                    std::move(result[ural::_2]));
        }
    };

    class rotate_copy_fn
    {
    public:
        template <class Forward, class Output>
        auto operator()(Forward && in, Output && out) const
        -> ural::tuple<decltype(sequence_fwd<Forward>(in)),
                       decltype(sequence_fwd<Output>(out))>
        {
            return this->impl(sequence_fwd<Forward>(in),
                              sequence_fwd<Output>(out));
        }

    private:
        template <class Forward, class Output>
        static ural::tuple<Forward, Output>
        impl(Forward in, Output out)
        {
            auto const n = ural::size(in);
            auto in_orig = ural::next(in.original(), n);

            auto in_1 = in.traversed_front();
            auto r1 = copy_fn{}(std::move(in), std::move(out));
            auto r2 = copy_fn{}(in_1, std::move(r1[ural::_2]));

            return ural::tuple<Forward, Output>{std::move(in_orig),
                                                std::move(r2[ural::_2])};
        }
    };

    class replace_if_fn
    {
    public:
        template <class ForwardSequence, class Predicate, class T>
        void operator()(ForwardSequence && seq,
                        Predicate pred, T const & new_value) const
        {
            return this->impl(sequence_fwd<ForwardSequence>(seq),
                              make_callable(std::move(pred)), new_value);
        }

    private:
        template <class ForwardSequence, class Predicate, class T>
        static void
        impl(ForwardSequence seq, Predicate pred, T const & new_value)
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<ForwardSequence>));
            BOOST_CONCEPT_ASSERT((concepts::ReadableSequence<ForwardSequence>));
            BOOST_CONCEPT_ASSERT((concepts::WritableSequence<ForwardSequence, T>));
            BOOST_CONCEPT_ASSERT((concepts::Callable<Predicate, bool(decltype(*seq))>));

            for(; !!seq; ++ seq)
            {
                if(pred(*seq))
                {
                    *seq = new_value;
                }
            }
        }
    };

    class replace_fn
    {
    public:
        template <class ForwardSequence, class T>
        void operator()(ForwardSequence && seq,
                        T const & old_value, T const & new_value) const
        {
            return (*this)(std::forward<ForwardSequence>(seq),
                           old_value, new_value, ural::equal_to<>{});
        }

        template <class ForwardSequence, class T, class BinaryPredicate>
        void operator()(ForwardSequence && seq,
                        T const & old_value, T const & new_value,
                        BinaryPredicate bin_pred) const
        {
            return this->impl(sequence_fwd<ForwardSequence>(seq),
                              old_value, new_value,
                              make_callable(std::move(bin_pred)));
        }

    private:
        template <class ForwardSequence, class T, class BinaryPredicate>
        static void
        impl(ForwardSequence seq, T const & old_value, T const & new_value,
             BinaryPredicate bin_pred)
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<ForwardSequence>));
            BOOST_CONCEPT_ASSERT((concepts::ReadableSequence<ForwardSequence>));
            BOOST_CONCEPT_ASSERT((concepts::WritableSequence<ForwardSequence, T>));

            BOOST_CONCEPT_ASSERT((concepts::Callable<BinaryPredicate, bool(decltype(*seq), T)>));

            auto const pred = std::bind(std::move(bin_pred), ural::_1,
                                        std::cref(old_value));

            return ::ural::replace_if_fn{}(std::move(seq), std::move(pred),
                                           new_value);
        }
    };

    class replace_copy_if_fn
    {
    public:
        template <class Input, class Output, class Predicate, class T>
        auto operator()(Input && in, Output && out, Predicate pred,
                        T const & new_value) const
        -> ural::tuple<decltype(sequence_fwd<Input>(in)),
                       decltype(sequence_fwd<Output>(out))>
        {
            return this->impl(sequence_fwd<Input>(in),
                              sequence_fwd<Output>(out),
                              ural::make_callable(std::move(pred)),
                              new_value);
        }

    private:
        template <class Input, class Output, class Predicate, class T>
        static tuple<Input, Output>
        impl(Input in, Output out, Predicate pred, T const & new_value)
        {
            auto in_r = ural::make_replace_if_sequence(std::move(in),
                                                       std::move(pred),
                                                       std::cref(new_value));
            auto r = ural::copy_fn{}(std::move(in_r), std::move(out));

            return ural::make_tuple(std::move(r[ural::_1]).bases()[ural::_1],
                                    std::move(r[ural::_2]));
        }
    };

    class replace_copy_fn
    {
    public:
        template <class Input, class Output, class T>
        auto operator()(Input && in, Output && out, T const & old_value,
                        T const & new_value) const
        -> ural::tuple<decltype(sequence_fwd<Input>(in)),
                       decltype(sequence_fwd<Output>(out))>
        {
            return (*this)(std::forward<Input>(in), std::forward<Output>(out),
                           old_value, new_value, ural::equal_to<>{});
        }

        template <class Input, class Output, class T, class BinaryPredicate>
        auto operator()(Input && in, Output && out, T const & old_value,
                        T const & new_value,
                        BinaryPredicate bin_pred) const
        -> ural::tuple<decltype(sequence_fwd<Input>(in)),
                       decltype(sequence_fwd<Output>(out))>
        {
            auto const pred
                = std::bind(ural::make_callable(std::move(bin_pred)),
                            std::placeholders::_1,
                            std::cref(old_value));
            return ural::replace_copy_if_fn{}(std::forward<Input>(in),
                                              std::forward<Output>(out),
                                              std::move(pred), new_value);
        }
    };

    // Тусовка
    class shuffle_fn
    {
    public:
        template <class RASequence, class URNG>
        void operator()(RASequence && s, URNG && g) const
        {
            return this->impl(sequence_fwd<RASequence>(s),
                              std::forward<URNG>(g));
        }

    private:
        template <class RASequence, class URNG>
        static void impl(RASequence s, URNG && g)
        {
            if(!s)
            {
                return;
            }

            for(; !!s; s.pop_back())
            {
                std::uniform_int_distribution<decltype(s.size())>
                    d(0, s.size() - 1);
                auto index = d(g);
                ::ural::details::do_swap(s[index], s.back());
            }
        }
    };

    class random_shuffle_fn
    {
    public:
        template <class RASequence>
        void operator()(RASequence && s) const
        {
            ural::c_rand_engine rnd;
            return ::ural::shuffle_fn{}(std::forward<RASequence>(s), rnd);
        }
    };

    // Разделение
    class is_partitioned_fn
    {
    public:
        template <class Input, class UnaryPredicate>
        bool operator()(Input && in, UnaryPredicate pred) const
        {
            return this->impl(sequence_fwd<Input>(in),
                              make_callable(std::move(pred)));
        }

    private:
        template <class Input, class UnaryPredicate>
        static bool impl(Input in, UnaryPredicate pred)
        {
            auto tail = find_if_not_fn{}(std::move(in), pred);
            return !find_if_fn{}(std::move(tail), std::move(pred));
        }
    };

    class partition_fn
    {
    public:
        template <class ForwardSequence, class UnaryPredicate>
        auto operator()(ForwardSequence && in, UnaryPredicate pred) const
        -> decltype(sequence_fwd<ForwardSequence>(in))
        {
            return this->impl(sequence_fwd<ForwardSequence>(in),
                              make_callable(std::move(pred)));
        }
    private:
        template <class ForwardSequence, class UnaryPredicate>
        static ForwardSequence
        impl(ForwardSequence in, UnaryPredicate pred)
        {
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

    class stable_partition_fn
    {
    public:
        template <class ForwardSequence, class UnaryPredicate>
        auto operator()(ForwardSequence && in, UnaryPredicate pred) const
        -> decltype(sequence_fwd<ForwardSequence>(in))
        {
            return this->impl(sequence_fwd<ForwardSequence>(in),
                              make_callable(std::move(pred)));
        }

    private:
        template <class ForwardSequence, class UnaryPredicate>
        ForwardSequence
        impl_inplace(ForwardSequence in, UnaryPredicate pred) const
        {
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

    class partition_copy_fn
    {
    public:
        template <class Input, class Output1, class Output2, class UnaryPredicate>
        auto operator()(Input && in, Output1 && out_true, Output2 && out_false,
                        UnaryPredicate pred) const
        -> ural::tuple<decltype(sequence_fwd<Input>(in)),
                       decltype(sequence_fwd<Output1>(out_true)),
                       decltype(sequence_fwd<Output2>(out_false))>
        {
            return this->impl(sequence_fwd<Input>(in),
                              sequence_fwd<Output1>(out_true),
                              sequence_fwd<Output2>(out_false),
                              make_callable(std::move(pred)));
        }

    private:
        template <class Input, class Output1, class Output2, class UnaryPredicate>
        static ural::tuple<Input, Output1, Output2>
        impl(Input in, Output1 out_true, Output2 out_false, UnaryPredicate pred)
        {
            auto out = ural::make_partition_sequence(std::move(out_true),
                                                     std::move(out_false),
                                                     std::move(pred));
            auto r = copy_fn{}(std::move(in), std::move(out));

            typedef ural::tuple<Input, Output1, Output2> Tuple;
            return Tuple(r[ural::_1], r[ural::_2].true_sequence(),
                         r[ural::_2].false_sequence());
        }
    };

    class partition_point_fn
    {
    public:
        template <class ForwardSequence, class Predicate>
        auto operator()(ForwardSequence && in, Predicate pred) const
        -> decltype(sequence_fwd<ForwardSequence>(in))
        {
            return this->impl(sequence_fwd<ForwardSequence>(in),
                              ural::make_callable(std::move(pred)));
        }

    private:
        template <class ForwardSequence, class Predicate>
        static ForwardSequence
        impl(ForwardSequence in, Predicate pred)
        {
            // @todo Нужен ли этот вызов?
            in.shrink_front();
            return find_if_not_fn{}(std::move(in), std::move(pred));
        }
    };

    // Бинарные кучи
    class is_heap_until_fn
    {
    public:
        template <class RASequence>
        auto operator()(RASequence && seq) const
        -> decltype(sequence_fwd<RASequence>(seq))
        {
            return (*this)(std::forward<RASequence>(seq), ural::less<>{});
        }

        template <class RASequence, class Compare>
        auto operator()(RASequence && seq, Compare cmp) const
        -> decltype(sequence_fwd<RASequence>(seq))
        {
            return this->impl(sequence_fwd<RASequence>(seq),
                              make_callable(std::move(cmp)));
        }

    private:
        template <class RandomAccessSequence, class Compare>
        static RandomAccessSequence
        impl(RandomAccessSequence seq, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequence<decltype(seq)>));
            BOOST_CONCEPT_ASSERT((concepts::Callable<Compare, bool(decltype(*seq), decltype(*seq))>));

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

    class is_heap_fn
    {
    public:
        template <class RASequence>
        bool operator()(RASequence && seq) const
        {
            return (*this)(std::forward<RASequence>(seq), ural::less<>{});
        }

        template <class RASequence, class Compare>
        bool operator()(RASequence && seq, Compare cmp) const
        {
            return this->impl(sequence_fwd<RASequence>(seq),
                              make_callable(std::move(cmp)));
        }

    private:
        template <class RASequence, class Compare>
        static bool
        impl(RASequence seq, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequence<RASequence>));
            BOOST_CONCEPT_ASSERT((concepts::Callable<Compare, bool(decltype(*seq), decltype(*seq))>));

            return !::ural::is_heap_until_fn{}(seq, cmp);
        }
    };

    class heap_sink_fn
    {
    public:
        template <class RASequence, class Size, class Compare>
        void operator()(RASequence && seq, Size first, Size last, Compare cmp) const
        {
            return this->impl(ural::sequence_fwd<RASequence>(seq), first, last,
                              ural::make_callable(std::move(cmp)));
        }

    private:
        template <class RASequence, class Size, class Compare>
        static void
        update_largest(RASequence seq, Size & largest, Size candidate, Size last, Compare cmp)
        {
            if(candidate < last && cmp(seq[largest], seq[candidate]))
            {
                largest = candidate;
            }
        }

        template <class RASequence, class Size, class Compare>
        void impl(RASequence seq, Size first, Size last, Compare cmp) const
        {
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

    class make_heap_fn
    {
    public:
        template <class RASequence>
        void operator()(RASequence && seq) const
        {
            return (*this)(std::forward<RASequence>(seq), ural::less<>{});
        }

        template <class RASequence, class Compare>
        void operator()(RASequence && seq, Compare cmp) const
        {
            return this->impl(sequence_fwd<RASequence>(seq),
                              ural::make_callable(std::move(cmp)));
        }

    private:
        template <class RandomAccessSequence, class Compare>
        static void
        impl(RandomAccessSequence seq, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((ural::concepts::RandomAccessSequence<decltype(seq)>));
            BOOST_CONCEPT_ASSERT((ural::concepts::Callable<Compare, bool(decltype(*seq), decltype(*seq))>));

            for(auto n = seq.size() / 2; n > 0; -- n)
            {
                heap_sink_fn{}(seq, n - 1, seq.size(), cmp);
            }

            assert(is_heap_fn{}(seq, cmp));
        }
    };

    class push_heap_fn
    {
    public:
        /** Добавление нового элемента в бинарную кучу
        @brief Оператор вызова функции
        @param seq последовательность произвольного доступа
        @pre @c seq не пуста
        @pre Первые <tt> seq.size() - 1 </tt> элементов @c seq образуют бинарную
        кучу
        @post <tt> is_heap(seq) </tt>
        */
        template <class RASequence>
        void operator()(RASequence && seq) const
        {
            return (*this)(std::forward<RASequence>(seq), ural::less<>{});
        }

        /** Добавление нового элемента в бинарную кучу
        @brief Оператор вызова функции
        @param seq последовательность произвольного доступа
        @param cmp функция сравнения
        @pre @c seq не пуста
        @pre Первые <tt> seq.size() - 1 </tt> элементов @c seq образуют бинарную
        кучу по отношению @c cmp
        @post <tt> is_heap(seq, cmp) </tt>
        */
        template <class RASequence, class Compare>
        void operator()(RASequence && seq, Compare cmp) const
        {
            return this->impl(sequence_fwd<RASequence>(seq),
                              make_callable(std::move(cmp)));
        }

    private:
        template <class RASequence, class Compare>
        static void
        impl(RASequence seq, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequence<RASequence>));
            BOOST_CONCEPT_ASSERT((concepts::Callable<Compare, bool(decltype(*seq), decltype(*seq))>));

            assert(::ural::is_heap_until_fn{}(seq, cmp).size() <= 1);

            if(seq.size() >= 1)
            {
                ::ural::details::heap_swim(seq, seq.size() - 1, cmp);
            }

            assert(is_heap_fn{}(seq, cmp));
        }
    };

    class pop_heap_fn
    {
    public:
        template <class RASequence>
        void operator()(RASequence && seq) const
        {
            return (*this)(std::forward<RASequence>(seq), ural::less<>{});
        }

        template <class RASequence, class Compare>
        void operator()(RASequence && seq, Compare cmp) const
        {
            return this->impl(sequence_fwd<RASequence>(seq),
                              make_callable(std::move(cmp)));
        }

    private:
        template <class RASequence, class Compare>
        static void
        impl(RASequence seq, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((ural::concepts::RandomAccessSequence<RASequence>));
            BOOST_CONCEPT_ASSERT((ural::concepts::Callable<Compare, bool(decltype(*seq), decltype(*seq))>));

            assert(is_heap_fn{}(seq, cmp));
            auto const N = seq.size();

            if(N <= 1)
            {
                return;
            }

            ::ural::details::do_swap(seq[0], seq[N-1]);
            heap_sink_fn{}(seq, 0*N, N-1, cmp);
        }
    };

    class sort_heap_fn
    {
    public:
        /** @brief Оператор вызова функции
        @param seq последовательность произвольного доступа
        @post <tt> is_sorted(seq) </tt>
        */
        template <class RASequence>
        void operator()(RASequence && seq) const
        {
            return (*this)(std::forward<RASequence>(seq),
                           ural::less<>{});
        }

        /** @brief Оператор вызова функции
        @param seq последовательность произвольного доступа
        @param cmp функция сравнения
        @post <tt> is_sorted(seq, cmp) </tt>
        */
        template <class RASequence, class Compare>
        void operator()(RASequence && seq, Compare cmp) const
        {
            return this->impl(sequence_fwd<RASequence>(seq),
                              make_callable(std::move(cmp)));
        }

    private:
        template <class RASequence, class Compare>
        static void
        impl(RASequence seq, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequence<RASequence>));
            BOOST_CONCEPT_ASSERT((concepts::Callable<Compare, bool(decltype(*seq), decltype(*seq))>));

            assert(is_heap_fn{}(seq, cmp));
            for(auto n = seq.size(); n > 0; --n)
            {
                pop_heap_fn{}(seq, cmp);
                seq.pop_back();
            }

            assert(::ural::is_sorted_fn{}(seq, cmp));
        }
    };

    class heap_select_fn
    {
    public:
        template <class RASequence, class Compare>
        void operator()(RASequence && s, Compare cmp) const
        {
            return this->impl(ural::sequence_fwd<RASequence>(s),
                              ural::make_callable(std::move(cmp)));
        }

    private:
        template <class RASequence, class Compare>
        static void
        impl(RASequence s, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::RandomAccessSequence<RASequence>));
            BOOST_CONCEPT_ASSERT((concepts::Callable<Compare, bool(decltype(*s), decltype(*s))>));

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

    // Сортировка
    class lower_bound_fn
    {
    public:
        template <class RASequence, class T>
        auto operator()(RASequence && in, T const & value) const
        -> decltype(sequence_fwd<RASequence>(in))
        {
            return (*this)(std::forward<RASequence>(in), value, ural::less<>{});
        }

        template <class RASequence, class T, class Compare>
        auto operator()(RASequence && in, T const & value, Compare cmp) const
        -> decltype(sequence_fwd<RASequence>(in))
        {
            return this->impl(sequence_fwd<RASequence>(in), value,
                              make_callable(std::move(cmp)));
        }
    private:
        template <class RASequence, class T, class Compare>
        static RASequence
        impl(RASequence in, T const & value, Compare cmp)
        {
            auto pred = std::bind(std::move(cmp), ural::_1, std::cref(value));
            return ::ural::partition_point_fn{}(std::move(in), std::move(pred));
        }
    };

    class upper_bound_fn
    {
    public:
        /** @brief Поиск верхней грани
        @param in последовательность
        @param value значение
        */
        template <class RASequence, class T>
        auto operator()(RASequence && in, T const & value) const
        -> decltype(sequence_fwd<RASequence>(in))
        {
            return (*this)(std::forward<RASequence>(in), value, ural::less<>{});
        }

        /** @brief Поиск верхней грани
        @param in последовательность
        @param value значение
        @param cmp функция сравнения
        */
        template <class RASequence, class T, class Compare>
        auto operator()(RASequence && in, T const & value, Compare cmp) const
        -> decltype(sequence_fwd<RASequence>(in))
        {
            return this->impl(sequence_fwd<RASequence>(in), value,
                              make_callable(std::move(cmp)));
        }
    private:
        template <class RASequence, class T, class Compare>
        static RASequence
        impl(RASequence in, T const & value, Compare cmp)
        {
            auto pred = ural::not_fn(std::bind(std::move(cmp), std::cref(value), ural::_1));
            return ::ural::partition_point_fn{}(std::move(in), std::move(pred));
        }
    };

    class binary_search_fn
    {
    public:
        template <class RASequence, class T>
        bool operator()(RASequence && in, T const & value) const
        {
            return (*this)(std::forward<RASequence>(in), value, ural::less<>{});
        }

        template <class RASequence, class T, class Compare>
        bool operator()(RASequence && in, T const & value, Compare cmp) const
        {
            return this->impl(sequence_fwd<RASequence>(in), value,
                              make_callable(std::move(cmp)));
        }

    private:
        template <class RASequence, class T, class Compare>
        static bool impl(RASequence in, T const & value, Compare cmp)
        {
            // @todo Добавить проверки концепций
            in = lower_bound_fn{}(std::move(in), value, cmp);

            return !!in && !cmp(value, *in);
        }
    };

    class equal_range_fn
    {
    public:
        template <class RASequence, class T>
        auto operator()(RASequence && in, T const & value) const
        -> decltype(sequence_fwd<RASequence>(in))
        {
            return (*this)(std::forward<RASequence>(in), value, ural::less<>{});
        }

        template <class RASequence, class T, class Compare>
        auto operator()(RASequence && in, T const & value, Compare cmp) const
        -> decltype(sequence_fwd<RASequence>(in))
        {
            return this->impl(sequence_fwd<RASequence>(in), value,
                              make_callable(std::move(cmp)));
        }

    private:
        template <class RASequence, class T, class Compare>
        static RASequence impl(RASequence in, T const & value, Compare cmp)
        {
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

    class partial_sort_fn
    {
    public:
        template <class RASequence, class Size>
        void operator()(RASequence && s, Size part) const
        {
            return (*this)(std::forward<RASequence>(s), part, ural::less<>{});
        }

        template <class RASequence, class Size, class Compare>
        void operator()(RASequence && s, Size part, Compare cmp) const
        {
            return this->impl(sequence_fwd<RASequence>(s), std::move(part),
                              ural::make_callable(std::move(cmp)));
        }

    private:
        template <class RASequence, class Size, class Compare>
        static void
        impl(RASequence s, Size const part, Compare cmp)
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

    class partial_sort_copy_fn
    {
    public:
        template <class Input, class RASequence>
        auto operator()(Input && in, RASequence && out) const
        -> decltype(sequence_fwd<RASequence>(out))
        {
            return (*this)(sequence_fwd<Input>(in),
                           sequence_fwd<RASequence>(out), ural::less<>{});
        }

        template <class Input, class RASequence, class Compare>
        auto operator()(Input && in, RASequence && out, Compare cmp) const
        -> decltype(sequence_fwd<RASequence>(out))
        {
            return this->impl(sequence_fwd<Input>(in),
                              sequence_fwd<RASequence>(out),
                              ural::make_callable(std::move(cmp)));
        }

    private:
        template <class Input, class RASequence, class Compare>
        static RASequence
        impl(Input in, RASequence out, Compare cmp)
        {
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

    class nth_element_fn
    {
    public:
        template <class RASequence, class Compare>
        void operator()(RASequence && s, Compare cmp) const
        {
            return this->impl(sequence_fwd<RASequence>(s),
                              ural::make_callable(std::move(cmp)));
        }

        template <class RASequence>
        void operator()(RASequence && s) const
        {
            return (*this)(std::forward<RASequence>(s), ural::less<>{});
        }

    private:
        template <class RASequence, class Compare>
        static void impl(RASequence s, Compare cmp)
        {
            return heap_select_fn{}(std::move(s), std::move(cmp));
        }
    };

    class merge_fn
    {
    public:
        template <class Input1, class Input2, class Output>
        auto operator()(Input1 && in1, Input2 && in2, Output && out) const
        -> tuple<decltype(::ural::sequence_fwd<Input1>(in1)),
                 decltype(::ural::sequence_fwd<Input2>(in2)),
                 decltype(::ural::sequence_fwd<Output>(out))>
        {
            return (*this)(std::forward<Input1>(in1), std::forward<Input2>(in2),
                           std::forward<Output>(out), ural::less<>{});
        }

        template <class Input1, class Input2, class Output, class Compare>
        auto operator()(Input1 && in1, Input2 && in2, Output && out,
                        Compare cmp) const
        -> tuple<decltype(::ural::sequence_fwd<Input1>(in1)),
                 decltype(::ural::sequence_fwd<Input2>(in2)),
                 decltype(::ural::sequence_fwd<Output>(out))>
        {
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

    class inplace_merge_fn
    {
    public:
        template <class BidirectionalSequence>
        void operator()(BidirectionalSequence && s) const
        {
            return (*this)(std::forward<BidirectionalSequence>(s),
                           ural::less<>{});
        }

        template <class BidirectionalSequence, class Compare>
        void operator()(BidirectionalSequence && s, Compare cmp) const
        {
            return this->impl(sequence_fwd<BidirectionalSequence>(s),
                              ural::make_callable(std::move(cmp)));
        }

    private:
        template <class BidirectionalSequence, class Compare>
        static void impl(BidirectionalSequence s, Compare cmp)
        {
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

    class lexicographical_compare_fn
    {
    public:
        template <class Input1, class  Input2>
        bool operator()(Input1 && in1, Input2 && in2) const
        {
            return (*this)(std::forward<Input1>(in1),
                           std::forward<Input2>(in2),
                           ural::less<>());
        }

        template <class Input1, class  Input2, class Compare>
        bool operator()(Input1 && in1, Input2 && in2, Compare cmp) const
        {
            return this->impl(sequence_fwd<Input1>(in1),
                              sequence_fwd<Input2>(in2),
                              ural::make_callable(std::move(cmp)));
        }

    private:
        template <class Input1, class  Input2, class Compare>
        static bool
        impl(Input1 in1, Input2 in2, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassSequence<Input1>));
            BOOST_CONCEPT_ASSERT((ural::concepts::ReadableSequence<Input1>));
            BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassSequence<Input2>));
            BOOST_CONCEPT_ASSERT((ural::concepts::ReadableSequence<Input2>));
            BOOST_CONCEPT_ASSERT((ural::concepts::Callable<Compare, bool(decltype(*in1), decltype(*in2))>));

            for(; !!in1 && !!in2; ++ in1, ++ in2)
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

    class is_permutation_fn
    {
    public:
        template <class Forward1, class Forward2>
        bool operator()(Forward1 && s1, Forward2 && s2) const
        {
            return (*this)(std::forward<Forward1>(s1),
                           std::forward<Forward2>(s2),
                           ural::equal_to<>{});
        }

        template <class Forward1, class Forward2, class BinaryPredicate>
        bool operator()(Forward1 && s1, Forward2 && s2,
                        BinaryPredicate pred) const
        {
            return this->impl(sequence_fwd<Forward1>(s1),
                              sequence_fwd<Forward2>(s2),
                              make_callable(std::move(pred)));
        }

    private:
        template <class Forward1, class Forward2, class BinaryPredicate>
        static bool
        impl(Forward1 s1, Forward2 s2, BinaryPredicate pred)
        {
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
    class includes_fn
    {
    public:
        template <class Input1, class  Input2>
        bool operator()(Input1 && in1, Input2 && in2) const
        {
            return (*this)(std::forward<Input1>(in1),
                           std::forward<Input2>(in2), ural::less<>());
        }

        template <class Input1, class  Input2, class Compare>
        bool operator()(Input1 && in1, Input2 && in2, Compare cmp) const
        {
            return this->impl(sequence_fwd<Input1>(in1),
                              sequence_fwd<Input2>(in2),
                              ural::make_callable(std::move(cmp)));
        }

    private:
        template <class Input1, class  Input2, class Compare>
        static bool impl(Input1 in1, Input2 in2, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassSequence<Input1>));
            BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassSequence<Input2>));
            BOOST_CONCEPT_ASSERT((ural::concepts::ReadableSequence<Input1>));
            BOOST_CONCEPT_ASSERT((ural::concepts::ReadableSequence<Input2>));
            BOOST_CONCEPT_ASSERT((ural::concepts::Callable<Compare, bool(decltype(*in1), decltype(*in2))>));

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

    // Поиск наибольшего и наименьшего
    class min_element_fn
    {
    private:
        template <class ForwardSequence, class Compare>
        static ForwardSequence
        impl(ForwardSequence in, Compare cmp)
        {
            if(!in)
            {
                return in;
            }

            auto cmp_s = ural::compare_by(ural::dereference<>{}, std::move(cmp));

            ::ural::min_element_accumulator<ForwardSequence, decltype(cmp_s)>
                acc(in++, cmp_s);

            auto seq = in | ural::outdirected;

            acc = ::ural::for_each_fn{}(std::move(seq), std::move(acc));

            return acc.result();
        }
    public:
        template <class ForwardSequence, class Compare>
        auto operator()(ForwardSequence && in, Compare cmp) const
        -> decltype(sequence_fwd<ForwardSequence>(in))
        {
            return this->impl(sequence_fwd<ForwardSequence>(in),
                              ural::make_callable(std::move(cmp)));
        }

        template <class ForwardSequence>
        auto operator()(ForwardSequence && in) const
        -> decltype(sequence_fwd<ForwardSequence>(in))
        {
            return (*this)(sequence_fwd<ForwardSequence>(in), ural::less<>{});
        }
    };

    class max_element_fn
    {
    private:
        template <class ForwardSequence, class Compare>
        static ForwardSequence
        impl(ForwardSequence in, Compare cmp)
        {
            auto transposed_cmp = ural::make_binary_reverse_args(std::move(cmp));

            return ::ural::min_element_fn{}(std::move(in),
                                            std::move(transposed_cmp));
        }

    public:
        template <class ForwardSequence, class Compare>
        auto operator()(ForwardSequence && in, Compare cmp) const
        -> decltype(sequence_fwd<ForwardSequence>(in))
        {
            return this->impl(sequence_fwd<ForwardSequence>(in),
                              ural::make_callable(std::move(cmp)));
        }

        template <class ForwardSequence>
        auto operator()(ForwardSequence && in) const
        -> decltype(sequence_fwd<ForwardSequence>(in))
        {
            return (*this)(sequence_fwd<ForwardSequence>(in), ural::less<>{});
        }
    };

    class minmax_element_fn
    {
    public:
        template <class ForwardSequence>
        auto operator()(ForwardSequence && in) const
        -> ural::tuple<decltype(sequence_fwd<ForwardSequence>(in)),
                       decltype(sequence_fwd<ForwardSequence>(in))>
        {
            return (*this)(sequence_fwd<ForwardSequence>(in), ural::less<>{});
        }

        template <class ForwardSequence, class Compare>
        auto operator()(ForwardSequence && in, Compare cmp) const
        -> ural::tuple<decltype(sequence_fwd<ForwardSequence>(in)),
                       decltype(sequence_fwd<ForwardSequence>(in))>
        {
            return this->impl(sequence_fwd<ForwardSequence>(in),
                              ural::make_callable(std::move(cmp)));
        }

    private:
        template <class ForwardSequence, class Compare>
        static tuple<ForwardSequence, ForwardSequence>
        impl(ForwardSequence in, Compare cmp)
        {
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
    class next_permutation_fn
    {
    public:
        template <class BiSequence>
        bool operator()(BiSequence && s) const
        {
            return (*this)(std::forward<BiSequence>(s), ural::less<>{});
        }

        template <class BiSequence, class Compare>
        bool operator()(BiSequence && s, Compare cmp) const
        {
            return this->impl(sequence_fwd<BiSequence>(s),
                              ural::make_callable(std::move(cmp)));

        }

    private:
        template <class BiSequence, class Compare>
        bool impl(BiSequence s, Compare cmp) const
        {
            typedef typename BiSequence::value_type Value;

            BOOST_CONCEPT_ASSERT((concepts::BidirectionalSequence<BiSequence>));
            BOOST_CONCEPT_ASSERT((concepts::ReadableSequence<BiSequence>));
            BOOST_CONCEPT_ASSERT((concepts::WritableSequence<BiSequence, Value>));
            BOOST_CONCEPT_ASSERT((concepts::Callable<Compare, bool(Value, Value)>));

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

    class prev_permutation_fn
    {
    public:
        template <class BiSequence>
        bool operator()(BiSequence && s) const
        {
            return (*this)(std::forward<BiSequence>(s), ural::less<>{});
        }

        template <class BiSequence, class Compare>
        bool operator()(BiSequence && s, Compare cmp) const
        {
            return this->impl(sequence_fwd<BiSequence>(s),
                              ural::make_callable(std::move(cmp)));

        }

    private:
        template <class BiSequence, class Compare>
        bool impl(BiSequence s, Compare cmp) const
        {
            auto constexpr f = next_permutation_fn{};
            return f(std::move(s), ::ural::not_fn(std::move(cmp)));
        }
    };

    class remove_if_fn
    {
    public:
        /** @brief Оператор вызова функции
        @param seq входная последовательность
        @param pred унарный предикат
        */
        template <class ForwardSequence, class Predicate>
        auto operator()(ForwardSequence && seq, Predicate pred) const
        -> decltype(sequence_fwd<ForwardSequence>(seq))
        {
            return this->impl(sequence_fwd<ForwardSequence>(seq),
                              ural::make_callable(std::move(pred)));
        }

    private:
        template <class ForwardSequence, class Predicate>
        ForwardSequence
        impl(ForwardSequence in, Predicate pred) const
        {
            typedef typename ForwardSequence::value_type Value;

            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<ForwardSequence>));
            BOOST_CONCEPT_ASSERT((concepts::ReadableSequence<ForwardSequence>));
            BOOST_CONCEPT_ASSERT((concepts::WritableSequence<ForwardSequence, Value>));
            BOOST_CONCEPT_ASSERT((concepts::Callable<Predicate, bool(Value)>));

            auto out = find_if_fn{}(std::move(in), pred);

            if(!out)
            {
                return out;
            }

            auto in_filtered = ural::next(out) | ural::removed_if(std::move(pred));

            return ural::move_fn{}(in_filtered, out)[ural::_2];
        }
    };

    class remove_fn
    {
    public:
        /** @brief Оператор вызова функции
        @param seq входная последовательность
        @param value значение
        */
        template <class ForwardSequence, class Value>
        auto operator()(ForwardSequence && seq, Value const & value) const
        -> decltype(sequence_fwd<ForwardSequence>(seq))
        {
            return (*this)(std::forward<ForwardSequence>(seq), value,
                           ural::equal_to<>{});
        }

        /** @brief Оператор вызова функции
        @param seq входная последовательность
        @param value значение
        @param pred бинарный предикат, определяющий эквивалентность элементов
        */
        template <class ForwardSequence, class Value, class BinaryPredicate>
        auto operator()(ForwardSequence && seq, Value const & value,
                        BinaryPredicate pred) const
        -> decltype(sequence_fwd<ForwardSequence>(seq))
        {
            return this->impl(sequence_fwd<ForwardSequence>(seq),
                              value, make_callable(std::move(pred)));
        }

    private:
        template <class ForwardSequence, class Value, class BinaryPredicate>
        ForwardSequence
        impl(ForwardSequence in, Value const & value,
             BinaryPredicate pred) const
        {
            typedef typename ForwardSequence::value_type value_type;

            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<ForwardSequence>));
            BOOST_CONCEPT_ASSERT((concepts::ReadableSequence<ForwardSequence>));
            BOOST_CONCEPT_ASSERT((concepts::WritableSequence<ForwardSequence, value_type>));
            BOOST_CONCEPT_ASSERT((concepts::Callable<BinaryPredicate, bool(value_type, Value)>));

            auto pred_1 = std::bind(std::move(pred), ural::_1, std::cref(value));
            return remove_if_fn{}(in, std::move(pred_1));
        }
    };

    class remove_copy_if_fn
    {
    public:
        template <class Input, class Output, class Predicate>
        auto operator()(Input && in, Output && out, Predicate pred) const
        -> tuple<decltype(ural::sequence_fwd<Input>(in)),
                 decltype(ural::sequence_fwd<Output>(out))>
        {
            return ural::copy_if_fn{}(std::forward<Input>(in),
                                      std::forward<Output>(out),
                                      ural::not_fn(std::move(pred)));
        }
    };

    class remove_copy_fn
    {
    public:
        template <class Input, class Output, class T>
        auto operator()(Input && in, Output && out, T const & value) const
        -> tuple<decltype(ural::sequence_fwd<Input>(in)),
                 decltype(ural::sequence_fwd<Output>(out))>
        {
            return (*this)(std::forward<Input>(in), std::forward<Output>(out),
                           value, ural::equal_to<>{});
        }

        template <class Input, class Output, class T, class BinaryPredicate>
        auto operator()(Input && in, Output && out, T const & value,
                        BinaryPredicate bin_pred) const
        -> tuple<decltype(ural::sequence_fwd<Input>(in)),
                 decltype(ural::sequence_fwd<Output>(out))>
        {
            auto pred = std::bind(ural::make_callable(std::move(bin_pred)),
                                  ural::_1, std::cref(value));
            return remove_copy_if_fn{}(std::forward<Input>(in),
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
        -> tuple<decltype(ural::sequence_fwd<Input>(in)),
                 decltype(ural::sequence_fwd<Output>(out))>
        {
            return this->impl(ural::sequence_fwd<Input>(in),
                              ural::sequence_fwd<Output>(out));
        }

        template <class T>
        constexpr
        typename std::conditional<!std::is_nothrow_move_constructible<T>::value
                                  && std::is_move_constructible<T>::value,
                                  T const &, T &&>::type
        operator()(T & x) const
        {
            return std::move(x);
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

    namespace
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
        constexpr auto const partition_point = partition_point_fn{};

        // 25.4 Сортировка и связанные с ним операции
        // 25.4.1 Сортировка
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
        constexpr auto const inplace_merge = inplace_merge_fn{};

        // 25.4.5 Операции с сортированными множествами
        constexpr auto const includes = includes_fn{};
        // @todo set_union
        // @todo set_intersection
        // @todo set_difference
        // @todo set_symmetric_difference

        // 25.4.6 Операции с бинарными кучами
        constexpr auto const push_heap = push_heap_fn{};
        constexpr auto const pop_heap = pop_heap_fn{};
        constexpr auto const make_heap = make_heap_fn{};
        constexpr auto const sort_heap = sort_heap_fn{};
        constexpr auto const is_heap = is_heap_fn{};
        constexpr auto const is_heap_until = is_heap_until_fn{};

        // 25.4.7 Наибольшее и наименьшее значение
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
