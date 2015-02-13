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

#include <ural/sequence/filtered.hpp>
#include <ural/sequence/transform.hpp>
#include <ural/sequence/moved.hpp>
#include <ural/sequence/make.hpp>
#include <ural/sequence/uniqued.hpp>

#include <ural/functional.hpp>
#include <ural/random/c_rand_engine.hpp>
#include <ural/functional/make_functor.hpp>

#include <ural/algorithm/copy.hpp>
#include <ural/algorithm/details/algo_base.hpp>

namespace ural
{
namespace details
{
    // Модифицирующие алгоритмы
    class unique_functor_t
    {
    public:
        template <class ForwardSequence>
        auto operator()(ForwardSequence && seq) const
        -> decltype(ural::sequence_fwd<ForwardSequence>(seq))
        {
            return (*this)(std::forward<ForwardSequence>(seq), ural::equal_to<>{});
        }

        template <class ForwardSequence, class BinaryPredicate>
        auto operator()(ForwardSequence && seq, BinaryPredicate pred) const
        -> decltype(ural::sequence_fwd<ForwardSequence>(seq))
        {
            return this->impl(ural::sequence_fwd<ForwardSequence>(seq),
                              make_functor(std::move(pred)));
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
    /**
    @todo Принимать в качестве аргументов последовательность на основе
    константных итераторов, как этого требует стандарт C++11
    */
    class erase_functor_t
    {
    public:
        template <class Container>
        auto operator()(Container & c,
                        iterator_sequence<typename Container::iterator> seq) const
        -> typename Container::iterator
        {
            return c.erase(seq.begin(), seq.end());
        }
    };

    class unique_erase_t
    {
    public:
        template <class Container>
        Container & operator()(Container & c) const
        {
            return (*this)(c, ural::equal_to<>{});
        }

        template <class Container, class BinaryPredicate>
        Container &
        operator()(Container & c, BinaryPredicate bin_pred) const
        {
            auto to_erase = unique_functor_t{}(c, std::move(bin_pred));
            erase_functor_t{}(c, to_erase);
            return c;
        }
    };
}
// namespace details

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
            return r[ural::_2].functor();
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
        -> decltype(ural::make_functor(std::move(f)))
        {
            return for_each_fn::impl(sequence_fwd<Input>(in),
                                     ural::make_functor(std::move(f)));
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

    auto constexpr all_of = all_of_fn{};
    auto constexpr none_of = none_of_fn{};
    auto constexpr any_of = any_of_fn{};

    auto constexpr for_each = for_each_fn{};

    auto constexpr find = find_fn{};
    auto constexpr find_if = find_if_fn{};
    auto constexpr find_if_not = find_if_not_fn{};

    auto constexpr count = count_fn{};
    auto constexpr count_if = count_if_fn{};

    template <class Forward1, class Forward2, class BinaryPredicate>
    auto find_end(Forward1 && in, Forward2 && s, BinaryPredicate bin_pred)
    -> decltype(sequence_fwd<Forward1>(in))
    {
        return ::ural::details::find_end(sequence_fwd<Forward1>(in),
                                         sequence_fwd<Forward2>(s),
                                         make_functor(std::move(bin_pred)));
    }

    template <class Forward1, class Forward2>
    auto find_end(Forward1 && in, Forward2 && s)
    -> decltype(sequence_fwd<Forward1>(in))
    {
        return ::ural::find_end(std::forward<Forward1>(in),
                                std::forward<Forward2>(s),
                                ural::equal_to<>{});
    }

    template <class Input, class Forward, class BinaryPredicate>
    auto find_first_of(Input && in, Forward && s, BinaryPredicate bin_pred)
    -> decltype(sequence_fwd<Input>(in))
    {
        return ::ural::details::find_first_of(sequence_fwd<Input>(in),
                                              sequence_fwd<Forward>(s),
                                              make_functor(std::move(bin_pred)));
    }

    template <class Input, class Forward>
    auto find_first_of(Input && in, Forward && s)
    -> decltype(sequence_fwd<Input>(in))
    {
        return ::ural::find_first_of(std::forward<Input>(in),
                                     std::forward<Forward>(s),
                                     ural::equal_to<>{});
    }

    template <class Forward, class BinaryPredicate>
    auto adjacent_find(Forward && s, BinaryPredicate pred)
    -> decltype(sequence_fwd<Forward>(s))
    {
        return ::ural::details::adjacent_find(sequence_fwd<Forward>(s),
                                              ural::make_functor(std::move(pred)));
    }

    template <class Forward>
    auto adjacent_find(Forward && s)
    -> decltype(sequence_fwd<Forward>(s))
    {
        return ::ural::adjacent_find(std::forward<Forward>(s),
                                     ::ural::equal_to<>{});
    }

    template <class Forward1, class Forward2, class BinaryPredicate>
    auto search(Forward1 && in, Forward2 && s, BinaryPredicate bin_pred)
    -> decltype(sequence_fwd<Forward1>(in))
    {
        return ::ural::details::search(sequence_fwd<Forward1>(in),
                                       sequence_fwd<Forward2>(s),
                                       ural::make_functor(std::move(bin_pred)));
    }

    template <class Forward1, class Forward2>
    auto search(Forward1 && in, Forward2 && s)
    -> decltype(sequence_fwd<Forward1>(in))
    {
        return ::ural::search(std::forward<Forward1>(in),
                              std::forward<Forward2>(s),
                              ural::equal_to<>{});
    }

    template <class Forward, class Size, class T,  class BinaryPredicate>
    auto search_n(Forward && in, Size count, T const & value,
                  BinaryPredicate bin_pred)
    -> decltype(sequence_fwd<Forward>(in))
    {
        return ::ural::details::search_n(sequence_fwd<Forward>(in),
                                         std::move(count), value,
                                         ural::make_functor(std::move(bin_pred)));
    }

    template <class Forward, class Size, class T>
    auto search_n(Forward && in, Size count, T const & value)
    -> decltype(sequence_fwd<Forward>(in))
    {
        return ::ural::search_n(std::forward<Forward>(in), std::move(count),
                                value, ural::equal_to<>{});
    }

    template <class Input1, class Input2, class BinaryPredicate>
    bool equal(Input1 && in1, Input2 && in2, BinaryPredicate pred)
    {
        return ::ural::details::equal(sequence_fwd<Input1>(in1),
                                      sequence_fwd<Input2>(in2),
                                      ural::make_functor(std::move(pred)));
    }

    template <class Input1, class Input2>
    bool equal(Input1 && in1, Input2 && in2)
    {
        return ::ural::equal(std::forward<Input1>(in1),
                             std::forward<Input2>(in2), ural::equal_to<>());
    }

    template <class Input1, class Input2, class BinaryPredicate>
    auto mismatch(Input1 && in1, Input2 && in2, BinaryPredicate pred)
    -> tuple<decltype(sequence_fwd<Input1>(in1)),
             decltype(sequence_fwd<Input2>(in2))>
    {
        return ::ural::details::mismatch(sequence_fwd<Input1>(in1),
                                         sequence_fwd<Input2>(in2),
                                         make_functor(std::move(pred)));
    }

    template <class Input1, class Input2>
    auto mismatch(Input1 && in1, Input2 && in2)
    -> tuple<decltype(sequence_fwd<Input1>(in1)),
             decltype(sequence_fwd<Input2>(in2))>
    {
        return ::ural::mismatch(std::forward<Input1>(in1),
                                std::forward<Input2>(in2), ural::equal_to<>());
    }

    // Модифицирующие последовательность алгоритмы
    auto constexpr copy = copy_fn{};

    class copy_if_fn
    {
    public:
        template <class Input, class Output, class Predicate>
        auto operator()(Input && in, Output && out, Predicate pred) const
        -> tuple<decltype(sequence_fwd<Input>(in)),
                 decltype(sequence_fwd<Output>(out))>
        {
            auto in_f = sequence_fwd<Input>(in) | ural::filtered(pred);
            auto res = ural::copy(std::move(in_f), sequence_fwd<Output>(out));

            return ural::make_tuple(res[ural::_1].base(), res[ural::_2]);
        }

    private:
    };

    auto constexpr copy_if = copy_if_fn{};

    class move_fn
    {
    public:
        template <class Input, class Output>
        auto operator()(Input && in, Output && out) const
        -> tuple<decltype(sequence_fwd<Input>(in)),
                 decltype(sequence_fwd<Output>(out))>
        {
            auto in_moved = ural::sequence_fwd<Input>(in) | ural::moved;
            auto res = ural::copy(std::move(in_moved), sequence_fwd<Output>(out));
            return ural::make_tuple(res[ural::_1].base(), res[ural::_2]);
        }
    };

    auto constexpr move = move_fn{};

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
            auto res = ural::copy(std::move(in) | ural::reversed,
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
            auto res = ural::move(std::move(in) | ural::reversed | ural::moved,
                                  std::move(out) | ural::reversed);

            return ural::make_tuple(std::move(res[ural::_1].base().base()),
                                    std::move(res[ural::_2].base()));
        }
    };

    auto constexpr move_backward = move_backward_fn{};

    // 25.3.4
    class transform_f
    {
    public:
        template <class Input, class Output, class UnaryFunction>
        auto operator()(Input && in, Output && out, UnaryFunction f) const
        -> tuple<decltype(sequence_fwd<Input>(in)),
                 decltype(sequence_fwd<Output>(out))>
        {
            return this->impl(sequence_fwd<Input>(in),
                              sequence_fwd<Output>(out),
                              ural::make_functor(std::move(f)));
        }

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
                              ural::make_functor(std::move(f)));
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

        template <class Input1, class Input2, class Output, class UnaryFunction>
        tuple<Input1, Input2, Output>
        impl(Input1 in1, Input2 in2, Output out, UnaryFunction f) const
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

    auto constexpr transform = transform_f{};

    class generate_fn
    {
    public:
        template <class ForwardSequence, class Generator>
        auto operator()(ForwardSequence && seq, Generator gen) const
        -> decltype(ural::sequence_fwd<ForwardSequence>(seq))
        {
            return this->impl(ural::sequence_fwd<ForwardSequence>(seq),
                              ural::make_functor(std::move(gen)));
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

    auto constexpr fill = fill_fn{};
    auto constexpr generate = generate_fn{};

    template <class Forward1, class Forward2>
    auto swap_ranges(Forward1 && s1, Forward2 && s2)
    -> ural::tuple<decltype(sequence_fwd<Forward1>(s1)),
                   decltype(sequence_fwd<Forward2>(s2))>
    {
        return ::ural::details::swap_ranges(sequence_fwd<Forward1>(s1),
                                            sequence_fwd<Forward2>(s2));
    }

    template <class BidirectionalSequence>
    void reverse(BidirectionalSequence && seq)
    {
        return ::ural::details::reverse(sequence_fwd<BidirectionalSequence>(seq));
    }

    template <class ForwardSequence>
    auto rotate(ForwardSequence && seq)
    -> decltype(sequence_fwd<ForwardSequence>(seq))
    {
        return ::ural::details::rotate(sequence_fwd<ForwardSequence>(seq));
    }

    template <class Forward, class Output>
    auto rotate_copy(Forward && in, Output && out)
    -> ural::tuple<decltype(sequence_fwd<Forward>(in)),
                   decltype(sequence_fwd<Output>(out))>
    {
        return ::ural::details::rotate_copy(sequence_fwd<Forward>(in),
                                            sequence_fwd<Output>(out));
    }

    template <class ForwardSequence, class T, class BinaryPredicate>
    void replace(ForwardSequence && seq, T const & old_value, T const & new_value,
                 BinaryPredicate bin_pred)
    {
        return ::ural::details::replace(sequence_fwd<ForwardSequence>(seq),
                                        old_value, new_value,
                                        make_functor(std::move(bin_pred)));
    }

    template <class ForwardSequence, class T>
    void replace(ForwardSequence && seq, T const & old_value, T const & new_value)
    {
        return ::ural::replace(std::forward<ForwardSequence>(seq),
                               old_value, new_value, ural::equal_to<>{});
    }

    template <class ForwardSequence, class Predicate, class T>
    void replace_if(ForwardSequence && seq, Predicate pred, T const & new_value)
    {
        return ::ural::details::replace_if(sequence_fwd<ForwardSequence>(seq),
                                           make_functor(std::move(pred)),
                                           new_value);
    }

    // Тусовка
    template <class RASequence, class URNG>
    void shuffle(RASequence && s, URNG && g)
    {
        return ::ural::details::shuffle(sequence_fwd<RASequence>(s),
                                        std::forward<URNG>(g));
    }

    template <class RASequence>
    void random_shuffle(RASequence && s)
    {
        ural::c_rand_engine rnd;
        return ::ural::shuffle(std::forward<RASequence>(s), rnd);
    }

    // Разделение
    template <class Input, class UnaryPredicate>
    bool is_partitioned(Input && in, UnaryPredicate pred)
    {
        return ::ural::details::is_partitioned(sequence_fwd<Input>(in),
                                               make_functor(std::move(pred)));
    }

    template <class ForwardSequence, class UnaryPredicate>
    auto partition(ForwardSequence && in, UnaryPredicate pred)
    -> decltype(sequence_fwd<ForwardSequence>(in))
    {
        return ::ural::details::partition(sequence_fwd<ForwardSequence>(in),
                                          make_functor(std::move(pred)));
    }

    template <class ForwardSequence, class UnaryPredicate>
    auto stable_partition(ForwardSequence && in, UnaryPredicate pred)
    -> decltype(sequence_fwd<ForwardSequence>(in))
    {
        auto s = sequence_fwd<ForwardSequence>(in);
        return ::ural::details::stable_partition(std::move(s),
                                                 make_functor(std::move(pred)));
    }

    template <class Input, class Output1, class Output2, class UnaryPredicate>
    auto partition_copy(Input && in, Output1 && out_true, Output2 && out_false,
                        UnaryPredicate pred)
    -> ural::tuple<decltype(sequence_fwd<Input>(in)),
                   decltype(sequence_fwd<Output1>(out_true)),
                   decltype(sequence_fwd<Output2>(out_false))>
    {
        return ::ural::details::partition_copy(sequence_fwd<Input>(in),
                                               sequence_fwd<Output1>(out_true),
                                               sequence_fwd<Output2>(out_false),
                                               make_functor(std::move(pred)));
    }

    template <class ForwardSequence, class Predicate>
    auto partition_point(ForwardSequence && in, Predicate pred)
    -> decltype(sequence_fwd<ForwardSequence>(in))
    {
        return ::ural::details::partition_point(sequence_fwd<ForwardSequence>(in),
                                                ural::make_functor(std::move(pred)));
    }

    // Бинарные кучи
    template <class RandomAccessSequence, class Compare>
    bool is_heap(RandomAccessSequence && seq, Compare cmp)
    {
        typedef RandomAccessSequence Seq;
        return ::ural::details::is_heap(sequence_fwd<Seq>(seq),
                                        make_functor(std::move(cmp)));
    }

    template <class RandomAccessSequence>
    bool is_heap(RandomAccessSequence && seq)
    {
        return ::ural::is_heap(std::forward<RandomAccessSequence>(seq),
                               ural::less<>{});
    }

    template <class RandomAccessSequence, class Compare>
    void make_heap(RandomAccessSequence && seq, Compare cmp)
    {
        typedef RandomAccessSequence Seq;
        return ::ural::details::make_heap(sequence_fwd<Seq>(seq),
                                          make_functor(std::move(cmp)));
    }

    template <class RandomAccessSequence>
    void make_heap(RandomAccessSequence && seq)
    {
        return ::ural::make_heap(std::forward<RandomAccessSequence>(seq),
                                 ural::less<>{});
    }

    template <class RandomAccessSequence, class Compare>
    void push_heap(RandomAccessSequence && seq, Compare cmp)
    {
        typedef RandomAccessSequence Seq;
        return ::ural::details::push_heap(sequence_fwd<Seq>(seq),
                                          make_functor(std::move(cmp)));
    }

    template <class RandomAccessSequence>
    void push_heap(RandomAccessSequence && seq)
    {
        return ::ural::push_heap(std::forward<RandomAccessSequence>(seq),
                                 ural::less<>{});
    }

    template <class RandomAccessSequence, class Compare>
    void pop_heap(RandomAccessSequence && seq, Compare cmp)
    {
        typedef RandomAccessSequence Seq;
        return ::ural::details::pop_heap(sequence_fwd<Seq>(seq),
                                          make_functor(std::move(cmp)));
    }

    template <class RandomAccessSequence>
    void pop_heap(RandomAccessSequence && seq)
    {
        return ::ural::pop_heap(std::forward<RandomAccessSequence>(seq),
                                ural::less<>{});
    }

    template <class RandomAccessSequence, class Compare>
    void sort_heap(RandomAccessSequence && seq, Compare cmp)
    {
        typedef RandomAccessSequence Seq;
        return ::ural::details::sort_heap(sequence_fwd<Seq>(seq),
                                          make_functor(std::move(cmp)));
    }

    template <class RandomAccessSequence>
    void sort_heap(RandomAccessSequence && seq)
    {
        return ::ural::sort_heap(std::forward<RandomAccessSequence>(seq),
                                 ural::less<>{});
    }

    // Сортировка
    template <class RASequence, class T, class Compare>
    auto lower_bound(RASequence && in, T const & value, Compare cmp)
    -> decltype(sequence_fwd<RASequence>(in))
    {
        return ::ural::details::lower_bound(sequence_fwd<RASequence>(in),
                                            value, make_functor(std::move(cmp)));
    }

    template <class RASequence, class T>
    auto lower_bound(RASequence && in, T const & value)
    -> decltype(sequence_fwd<RASequence>(in))
    {
        return ::ural::lower_bound(std::forward<RASequence>(in), value,
                                   ural::less<>{});
    }

    template <class RASequence, class T, class Compare>
    auto upper_bound(RASequence && in, T const & value, Compare cmp)
    -> decltype(sequence_fwd<RASequence>(in))
    {
        return ::ural::details::upper_bound(sequence_fwd<RASequence>(in),
                                            value, make_functor(std::move(cmp)));
    }

    template <class RASequence, class T>
    auto upper_bound(RASequence && in, T const & value)
    -> decltype(sequence_fwd<RASequence>(in))
    {
        return ::ural::upper_bound(std::forward<RASequence>(in), value,
                                   ural::less<>{});
    }

    template <class RASequence, class T, class Compare>
    bool binary_search(RASequence && in, T const & value, Compare cmp)
    {
        return ::ural::details::binary_search(sequence_fwd<RASequence>(in),
                                              value, make_functor(std::move(cmp)));
    }

    template <class RASequence, class T>
    bool binary_search(RASequence && in, T const & value)
    {
        return ::ural::binary_search(std::forward<RASequence>(in), value, ural::less<>{});
    }

    template <class RASequence, class T, class Compare>
    auto equal_range(RASequence && in, T const & value, Compare cmp)
    -> decltype(sequence_fwd<RASequence>(in))
    {
        return ::ural::details::equal_range(sequence_fwd<RASequence>(in),
                                            value, make_functor(std::move(cmp)));
    }

    template <class RASequence, class T>
    auto equal_range(RASequence && in, T const & value)
    -> decltype(sequence_fwd<RASequence>(in))
    {
        return ::ural::equal_range(std::forward<RASequence>(in), value,
                                   ural::less<>{});
    }

    auto constexpr is_sorted_until = is_sorted_until_fn{};
    auto constexpr is_sorted = is_sorted_fn{};

    template <class RASequence, class Compare>
    void sort(RASequence && s, Compare cmp)
    {
        return ::ural::details::sort(sequence_fwd<RASequence>(s),
                                     ural::make_functor(std::move(cmp)));
    }

    template <class RASequence>
    void sort(RASequence && s)
    {
        return ::ural::sort(std::forward<RASequence>(s), ural::less<>{});
    }

    template <class RASequence, class Compare>
    void stable_sort(RASequence && s, Compare cmp)
    {
        return ::ural::details::stable_sort(sequence_fwd<RASequence>(s),
                                            ural::make_functor(std::move(cmp)));
    }

    template <class RASequence>
    void stable_sort(RASequence && s)
    {
        return ::ural::stable_sort(std::forward<RASequence>(s), ural::less<>{});
    }

    template <class RASequence, class Size, class Compare>
    void partial_sort(RASequence && s, Size part, Compare cmp)
    {
        return ::ural::details::partial_sort(sequence_fwd<RASequence>(s),
                                             std::move(part),
                                             ural::make_functor(std::move(cmp)));
    }

    template <class RASequence, class Size>
    void partial_sort(RASequence && s, Size part)
    {
        return ::ural::partial_sort(std::forward<RASequence>(s), part,
                                    ural::less<>{});
    }

    template <class Input, class RASequence, class Compare>
    auto partial_sort_copy(Input && in, RASequence && out, Compare cmp)
    -> decltype(sequence_fwd<RASequence>(out))
    {
        return ::ural::details::partial_sort_copy(sequence_fwd<Input>(in),
                                                  sequence_fwd<RASequence>(out),
                                                  ural::make_functor(std::move(cmp)));
    }

    template <class Input, class RASequence>
    auto partial_sort_copy(Input && in, RASequence && out)
    -> decltype(sequence_fwd<RASequence>(out))
    {
        return ::ural::partial_sort_copy(sequence_fwd<Input>(in),
                                         sequence_fwd<RASequence>(out),
                                         ural::less<>{});
    }

    template <class RASequence, class Compare>
    void nth_element(RASequence && s, Compare cmp)
    {
        return ::ural::details::nth_element(sequence_fwd<RASequence>(s),
                                            ural::make_functor(std::move(cmp)));
    }

    template <class RASequence>
    void nth_element(RASequence && s)
    {
        return ::ural::nth_element(std::forward<RASequence>(s), ural::less<>{});
    }

    template <class BidirectionalSequence, class Compare>
    void inplace_merge(BidirectionalSequence && s, Compare cmp)
    {
        return ::ural::details::inplace_merge(sequence_fwd<BidirectionalSequence>(s),
                                              ural::make_functor(std::move(cmp)));
    }

    template <class BidirectionalSequence>
    void inplace_merge(BidirectionalSequence && s)
    {
        return ::ural::inplace_merge(std::forward<BidirectionalSequence>(s),
                                     ural::less<>{});
    }

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
                              ural::make_functor(std::move(cmp)));
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

    auto constexpr lexicographical_compare = lexicographical_compare_fn{};

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
                              make_functor(std::move(pred)));
        }

    private:
        template <class Forward1, class Forward2, class BinaryPredicate>
        static bool
        impl(Forward1 s1, Forward2 s2, BinaryPredicate pred)
        {
            std::tie(s1, s2) = ural::details::mismatch(std::move(s1), std::move(s2),
                                                       pred);

            s1.shrink_front();
            s2.shrink_front();

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
    auto constexpr is_permutation = is_permutation_fn{};

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
                              ural::make_functor(std::move(cmp)));
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

    auto constexpr includes = includes_fn{};

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

            acc = ::ural::for_each(std::move(seq), std::move(acc));

            return acc.result();
        }
    public:
        template <class ForwardSequence, class Compare>
        auto operator()(ForwardSequence && in, Compare cmp) const
        -> decltype(sequence_fwd<ForwardSequence>(in))
        {
            return this->impl(sequence_fwd<ForwardSequence>(in),
                              ural::make_functor(std::move(cmp)));
        }

        template <class ForwardSequence>
        auto operator()(ForwardSequence && in) const
        -> decltype(sequence_fwd<ForwardSequence>(in))
        {
            return (*this)(sequence_fwd<ForwardSequence>(in), ural::less<>{});
        }
    };

    auto constexpr min_element = min_element_fn{};

    class max_element_fn
    {
    private:
        template <class ForwardSequence, class Compare>
        static ForwardSequence
        impl(ForwardSequence in, Compare cmp)
        {
            auto transposed_cmp = ural::make_binary_reverse_args(std::move(cmp));

            return ::ural::min_element(std::move(in),
                                       std::move(transposed_cmp));
        }

    public:
        template <class ForwardSequence, class Compare>
        auto operator()(ForwardSequence && in, Compare cmp) const
        -> decltype(sequence_fwd<ForwardSequence>(in))
        {
            return this->impl(sequence_fwd<ForwardSequence>(in),
                              ural::make_functor(std::move(cmp)));
        }

        template <class ForwardSequence>
        auto operator()(ForwardSequence && in) const
        -> decltype(sequence_fwd<ForwardSequence>(in))
        {
            return (*this)(sequence_fwd<ForwardSequence>(in), ural::less<>{});
        }
    };

    auto constexpr max_element = max_element_fn{};

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
                              ural::make_functor(std::move(cmp)));
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

    auto constexpr minmax_element = minmax_element_fn{};

namespace details
{
    // Перестановки
    class next_permutation_functor
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
                              ural::make_functor(std::move(cmp)));

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
                ::ural::details::reverse(std::move(s));
                return false;
            }
            else
            {
                auto r1 = r;
                auto r2 = s | ural::reversed;

                for(; cmp(*r2, *r1); ++r2)
                {}

                ::ural::details::do_swap(*r1, *r2);
                ural::details::reverse(r1.traversed_front().base());

                return true;
            }
        }
    };

    class prev_permutation_functor
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
                              ural::make_functor(std::move(cmp)));

        }

    private:
        template <class BiSequence, class Compare>
        bool impl(BiSequence s, Compare cmp) const
        {
            auto constexpr f = ::ural::details::next_permutation_functor{};
            return f(std::move(s), ::ural::not_fn(std::move(cmp)));
        }
    };

    // Алгоритмы над контейнерами
    class remove_functor_t
    {
    public:
        template <class ForwardSequence, class Value>
        auto operator()(ForwardSequence && seq, Value const & value) const
        -> decltype(sequence_fwd<ForwardSequence>(seq))
        {
            return (*this)(std::forward<ForwardSequence>(seq), value,
                           ural::equal_to<>{});
        }

        template <class ForwardSequence, class Value, class BinaryPredicate>
        auto operator()(ForwardSequence && seq, Value const & value,
                        BinaryPredicate pred) const
        -> decltype(sequence_fwd<ForwardSequence>(seq))
        {
            return this->impl(sequence_fwd<ForwardSequence>(seq),
                              value, make_functor(std::move(pred)));
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

            // @todo устранить дублирование, выделить алгоритмы
            auto out = ural::find(in, value, pred);

            if(!out)
            {
                return out;
            }

            in = out;
            ++ in;

            for(; !!in; ++ in)
            {
                if(pred(*in, value) == false)
                {
                    *out = std::move(*in);
                    ++ out;
                }
            }

            return out;
        }
    };

    class remove_if_functor_t
    {
    public:
        template <class ForwardSequence, class Predicate>
        auto operator()(ForwardSequence && seq, Predicate pred) const
        -> decltype(sequence_fwd<ForwardSequence>(seq))
        {
            return this->impl(sequence_fwd<ForwardSequence>(seq),
                              ural::make_functor(std::move(pred)));
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

            // @todo устранить дублирование, выделить алгоритмы
            auto out = find_if_fn{}(std::move(in), pred);

            if(!out)
            {
                return out;
            }

            in = out;
            ++ in;

            for(; !!in; ++ in)
            {
                if(pred(*in) == false)
                {
                    *out = std::move(*in);
                    ++ out;
                }
            }

            return out;
        }
    };

    class remove_if_erase_functor_t
    {
    public:
        template <class Container, class Predicate>
        Container & operator()(Container & c, Predicate pred) const
        {
            auto to_erase = remove_if_functor_t{}(c, pred);
            erase_functor_t{}(c, to_erase);
            return c;
        }

    private:
    };

    class remove_erase_functor_t
    {
    public:
        template <class Container, class Value>
        Container & operator()(Container & target, Value const & value) const
        {
            auto to_erase = remove_functor_t{}(target, value);

            erase_functor_t{}(target, to_erase);

            return target;
        }
    };
}
// namespace details

    auto constexpr const erase = ::ural::details::erase_functor_t{};

    auto constexpr const remove = ::ural::details::remove_functor_t{};
    auto constexpr const remove_erase = ::ural::details::remove_erase_functor_t{};

    auto constexpr const remove_if = ::ural::details::remove_if_functor_t{};
    auto constexpr const remove_if_erase = ::ural::details::remove_if_erase_functor_t{};

    auto constexpr const unique = ::ural::details::unique_functor_t{};
    auto constexpr const unique_erase = ::ural::details::unique_erase_t{};

    auto constexpr const next_permutation = ::ural::details::next_permutation_functor{};
    auto constexpr const prev_permutation = ::ural::details::prev_permutation_functor{};
}
// namespace ural

#endif
// Z_URAL_ALGORITHM_HPP_INCLUDED
