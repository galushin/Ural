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

#include <ural/sequence/make.hpp>
#include <ural/sequence/uniqued.hpp>

#include <ural/functional.hpp>
#include <ural/random/c_rand_engine.hpp>
#include <ural/functional/make_functor.hpp>

#include <ural/algorithm/details/copy.hpp>
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
        -> decltype(sequence(std::forward<ForwardSequence>(seq)))
        {
            return (*this)(std::forward<ForwardSequence>(seq), ural::equal_to<>{});
        }

        template <class ForwardSequence, class BinaryPredicate>
        auto operator()(ForwardSequence && seq, BinaryPredicate pred) const
        -> decltype(sequence(std::forward<ForwardSequence>(seq)))
        {
            return this->impl(sequence(std::forward<ForwardSequence>(seq)),
                              make_functor(std::move(pred)));
        }

    private:
        template <class ForwardSequence, class BinaryPredicate>
        ForwardSequence
        impl(ForwardSequence seq, BinaryPredicate pred) const
        {
            // @todo Оптимизация
            auto us = ural::make_unique_sequence(seq, std::move(pred));

            auto result = ural::details::copy(us, seq);

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
}
// namespace details

    /** @brief Применяет функциональный объект к каждому элементу
    последовательности
    @param in входная последовательность
    @param f функциональный объект
    @return @c f
    */
    template <class Input, class UnaryFunction>
    auto for_each(Input && in, UnaryFunction f)
    -> decltype(ural::make_functor(std::move(f)))
    {
        return ural::details::for_each(sequence(std::forward<Input>(in)),
                                       ural::make_functor(std::move(f)));
    }

    template <class Input, class Predicate>
    auto find_if(Input && in, Predicate pred)
    -> decltype(sequence(std::forward<Input>(in)))
    {
        return ::ural::details::find_if(sequence(std::forward<Input>(in)),
                                       ural::make_functor(std::move(pred)));
    }

    template <class Input, class Predicate>
    auto find_if_not(Input && in, Predicate pred)
    -> decltype(sequence(std::forward<Input>(in)))
    {
        return ::ural::details::find_if(sequence(std::forward<Input>(in)),
                                        ural::not_fn(std::move(pred)));
    }

    template <class Input, class T, class BinaryPredicate>
    auto find(Input && in, T const & value, BinaryPredicate pred)
    -> decltype(sequence(std::forward<Input>(in)))
    {
        return ::ural::find_if(std::forward<Input>(in),
                               std::bind(ural::make_functor(std::move(pred)),
                                         std::placeholders::_1,
                                         std::ref(value)));
    }

    template <class Input, class T>
    auto find(Input && in, T const & value)
    -> decltype(sequence(std::forward<Input>(in)))
    {
        return ::ural::find(std::forward<Input>(in), value,
                            ural::equal_to<T>{});
    }

    /** @brief Подсчитывает количество элементов последовательности,
    удовлетворяющих предикату.
    @param in входная последовтельность
    @param pred предикат
    @return Количество элементов @c x последовательности @c in, таких, что
    <tt> pred(x) != false </tt>.
    */
    template <class Input, class UnaryPredicate>
    auto count_if(Input && in, UnaryPredicate pred)
    -> typename decltype(sequence(std::forward<Input>(in)))::distance_type
    {
        return ::ural::details::count_if(sequence(std::forward<Input>(in)),
                                         ural::make_functor(std::move(pred)));
    }

    template <class Input, class T, class BinaryPredicate>
    auto count(Input && in, T const & value, BinaryPredicate pred)
    -> typename decltype(sequence(std::forward<Input>(in)))::distance_type
    {
         return ::ural::count_if(std::forward<Input>(in),
                                 std::bind(ural::make_functor(std::move(pred)),
                                           std::placeholders::_1,
                                           std::ref(value)));
    }

    /** @brief Подсчитывает количество элементов последовательности, равных
    заданному значению.
    @param in входная последовтельность
    @param value значение
    @return Количество элементов @c x последовательности @c in, таких, что
    <tt> x == value </tt>.
    */
    template <class Input, class T>
    auto count(Input && in, T const & value)
    -> typename decltype(sequence(std::forward<Input>(in)))::distance_type
    {
        return ::ural::count(std::forward<Input>(in), value,
                             ural::equal_to<T>{});
    }

    /** @brief Проверяет, что все элементы последовательности удовлетворяют
    заданному предикату
    @param in входная последовтельность
    @param pred предикат
    @return @b true, если для всех элементов @c x последовательности @c in
    выполняется <tt> pred(x) != false </tt>
    */
    template <class Input, class UnaryPredicate>
    bool all_of(Input && in, UnaryPredicate pred)
    {
        return !::ural::find_if_not(std::forward<Input>(in), std::move(pred));
    }

    template <class Input, class UnaryPredicate>
    bool none_of(Input && in, UnaryPredicate pred)
    {
        return !::ural::find_if(std::forward<Input>(in), std::move(pred));
    }

    /** @brief Проверяет, что хотя бы один элемент последовательности
    удовлетворяет заданному предикату.
    @param in входная последовтельность
    @param pred предикат
    @return @b true, если для хотя бы одного элемента @c x последовательности
    @c in выполняется <tt> pred(x) != false </tt>
    */
    template <class Input, class UnaryPredicate>
    bool any_of(Input && in, UnaryPredicate pred)
    {
        return !none_of(std::forward<Input>(in), std::move(pred));
    }

    template <class Forward1, class Forward2, class BinaryPredicate>
    auto find_end(Forward1 && in, Forward2 && s, BinaryPredicate bin_pred)
    -> decltype(sequence(std::forward<Forward1>(in)))
    {
        return ::ural::details::find_end(sequence(std::forward<Forward1>(in)),
                                         sequence(std::forward<Forward2>(s)),
                                         make_functor(std::move(bin_pred)));
    }

    template <class Forward1, class Forward2>
    auto find_end(Forward1 && in, Forward2 && s)
    -> decltype(sequence(std::forward<Forward1>(in)))
    {
        return ::ural::find_end(std::forward<Forward1>(in),
                                std::forward<Forward2>(s),
                                ural::equal_to<>{});
    }

    template <class Input, class Forward, class BinaryPredicate>
    auto find_first_of(Input && in, Forward && s, BinaryPredicate bin_pred)
    -> decltype(sequence(std::forward<Input>(in)))
    {
        return ::ural::details::find_first_of(sequence(std::forward<Input>(in)),
                                              sequence(std::forward<Forward>(s)),
                                              make_functor(std::move(bin_pred)));
    }

    template <class Input, class Forward>
    auto find_first_of(Input && in, Forward && s)
    -> decltype(sequence(std::forward<Input>(in)))
    {
        return ::ural::find_first_of(std::forward<Input>(in),
                                     std::forward<Forward>(s),
                                     ural::equal_to<>{});
    }

    template <class Forward, class BinaryPredicate>
    auto adjacent_find(Forward && s, BinaryPredicate pred)
    -> decltype(sequence(std::forward<Forward>(s)))
    {
        return ::ural::details::adjacent_find(sequence(std::forward<Forward>(s)),
                                              ural::make_functor(std::move(pred)));
    }

    template <class Forward>
    auto adjacent_find(Forward && s)
    -> decltype(sequence(std::forward<Forward>(s)))
    {
        return ::ural::adjacent_find(std::forward<Forward>(s),
                                     ::ural::equal_to<>{});
    }

    template <class Forward1, class Forward2, class BinaryPredicate>
    auto search(Forward1 && in, Forward2 && s, BinaryPredicate bin_pred)
    -> decltype(sequence(std::forward<Forward1>(in)))
    {
        return ::ural::details::search(sequence(std::forward<Forward1>(in)),
                                       sequence(std::forward<Forward2>(s)),
                                       ural::make_functor(std::move(bin_pred)));
    }

    template <class Forward1, class Forward2>
    auto search(Forward1 && in, Forward2 && s)
    -> decltype(sequence(std::forward<Forward1>(in)))
    {
        return ::ural::search(std::forward<Forward1>(in),
                              std::forward<Forward2>(s),
                              ural::equal_to<>{});
    }

    template <class Forward, class Size, class T,  class BinaryPredicate>
    auto search_n(Forward && in, Size count, T const & value,
                  BinaryPredicate bin_pred)
    -> decltype(sequence(std::forward<Forward>(in)))
    {
        return ::ural::details::search_n(sequence(std::forward<Forward>(in)),
                                         std::move(count), value,
                                         ural::make_functor(std::move(bin_pred)));
    }

    template <class Forward, class Size, class T>
    auto search_n(Forward && in, Size count, T const & value)
    -> decltype(sequence(std::forward<Forward>(in)))
    {
        return ::ural::search_n(std::forward<Forward>(in), std::move(count),
                                value, ural::equal_to<>{});
    }

    template <class Input1, class Input2, class BinaryPredicate>
    bool equal(Input1 && in1, Input2 && in2, BinaryPredicate pred)
    {
        return ::ural::details::equal(sequence(std::forward<Input1>(in1)),
                                      sequence(std::forward<Input2>(in2)),
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
    -> tuple<decltype(sequence(std::forward<Input1>(in1))),
             decltype(sequence(std::forward<Input2>(in2)))>
    {
        return ::ural::details::mismatch(sequence(std::forward<Input1>(in1)),
                                         sequence(std::forward<Input2>(in2)),
                                         make_functor(std::move(pred)));
    }

    template <class Input1, class Input2>
    auto mismatch(Input1 && in1, Input2 && in2)
    -> tuple<decltype(sequence(std::forward<Input1>(in1))),
             decltype(sequence(std::forward<Input2>(in2)))>
    {
        return ::ural::mismatch(std::forward<Input1>(in1),
                                std::forward<Input2>(in2), ural::equal_to<>());
    }

    // Модифицирующие последовательность алгоритмы
    template <class Input, class Output>
    auto copy(Input && in, Output && out)
    -> decltype(ural::details::copy(sequence(std::forward<Input>(in)),
                                    sequence(std::forward<Output>(out))))
    {
        return ural::details::copy(sequence(std::forward<Input>(in)),
                                   sequence(std::forward<Output>(out)));
    }

    template <class ForwardSequence, class T>
    auto fill(ForwardSequence && seq, T const & value)
    -> decltype(sequence(std::forward<ForwardSequence>(seq)))
    {
        return ural::details::fill(sequence(std::forward<ForwardSequence>(seq)),
                                   value);
    }

    template <class ForwardSequence, class Generator>
    void generate(ForwardSequence && seq, Generator gen)
    {
        ural::copy(ural::make_generator_sequence(gen),
                   std::forward<ForwardSequence>(seq));
    }

    template <class Forward1, class Forward2>
    auto swap_ranges(Forward1 && s1, Forward2 && s2)
    -> ural::tuple<decltype(sequence(std::forward<Forward1>(s1))),
                   decltype(sequence(std::forward<Forward2>(s2)))>
    {
        return ::ural::details::swap_ranges(sequence(std::forward<Forward1>(s1)),
                                            sequence(std::forward<Forward2>(s2)));
    }

    template <class BidirectionalSequence>
    void reverse(BidirectionalSequence && seq)
    {
        return ::ural::details::reverse(sequence(std::forward<BidirectionalSequence>(seq)));
    }

    template <class ForwardSequence>
    auto rotate(ForwardSequence && seq)
    -> decltype(sequence(std::forward<ForwardSequence>(seq)))
    {
        return ::ural::details::rotate(sequence(std::forward<ForwardSequence>(seq)));
    }

    template <class Forward, class Output>
    auto rotate_copy(Forward && in, Output && out)
    -> ural::tuple<decltype(sequence(std::forward<Forward>(in))),
                   decltype(sequence(std::forward<Output>(out)))>
    {
        return ::ural::details::rotate_copy(sequence(std::forward<Forward>(in)),
                                            sequence(std::forward<Output>(out)));
    }

    template <class ForwardSequence, class T, class BinaryPredicate>
    void replace(ForwardSequence && seq, T const & old_value, T const & new_value,
                 BinaryPredicate bin_pred)
    {
        return ::ural::details::replace(sequence(std::forward<ForwardSequence>(seq)),
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
        return ::ural::details::replace_if(sequence(std::forward<ForwardSequence>(seq)),
                                           make_functor(std::move(pred)),
                                           new_value);
    }

    constexpr auto unique = ::ural::details::unique_functor_t{};

    // Тусовка
    template <class RASequence, class URNG>
    void shuffle(RASequence && s, URNG && g)
    {
        return ::ural::details::shuffle(sequence(std::forward<RASequence>(s)),
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
        return ::ural::details::is_partitioned(sequence(std::forward<Input>(in)),
                                               make_functor(std::move(pred)));
    }

    template <class ForwardSequence, class UnaryPredicate>
    auto partition(ForwardSequence && in, UnaryPredicate pred)
    -> decltype(sequence(std::forward<ForwardSequence>(in)))
    {
        return ::ural::details::partition(sequence(std::forward<ForwardSequence>(in)),
                                          make_functor(std::move(pred)));
    }

    template <class ForwardSequence, class UnaryPredicate>
    auto stable_partition(ForwardSequence && in, UnaryPredicate pred)
    -> decltype(sequence(std::forward<ForwardSequence>(in)))
    {
        auto s = sequence(std::forward<ForwardSequence>(in));
        return ::ural::details::stable_partition(std::move(s),
                                                 make_functor(std::move(pred)));
    }

    template <class Input, class Output1, class Output2, class UnaryPredicate>
    auto partition_copy(Input && in, Output1 && out_true, Output2 && out_false,
                        UnaryPredicate pred)
    -> ural::tuple<decltype(sequence(std::forward<Input>(in))),
                   decltype(sequence(std::forward<Output1>(out_true))),
                   decltype(sequence(std::forward<Output2>(out_false)))>
    {
        return ::ural::details::partition_copy(sequence(std::forward<Input>(in)),
                                               sequence(std::forward<Output1>(out_true)),
                                               sequence(std::forward<Output2>(out_false)),
                                               make_functor(std::move(pred)));
    }

    template <class ForwardSequence, class Predicate>
    auto partition_point(ForwardSequence && in, Predicate pred)
    -> decltype(sequence(std::forward<ForwardSequence>(in)))
    {
        return ::ural::details::partition_point(sequence(std::forward<ForwardSequence>(in)),
                                                ural::make_functor(std::move(pred)));
    }

    // Бинарные кучи
    template <class RandomAccessSequence, class Compare>
    bool is_heap(RandomAccessSequence && seq, Compare cmp)
    {
        typedef RandomAccessSequence Seq;
        return ::ural::details::is_heap(sequence(std::forward<Seq>(seq)),
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
        return ::ural::details::make_heap(sequence(std::forward<Seq>(seq)),
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
        return ::ural::details::push_heap(sequence(std::forward<Seq>(seq)),
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
        return ::ural::details::pop_heap(sequence(std::forward<Seq>(seq)),
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
        return ::ural::details::sort_heap(sequence(std::forward<Seq>(seq)),
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
    -> decltype(sequence(std::forward<RASequence>(in)))
    {
        return ::ural::details::lower_bound(sequence(std::forward<RASequence>(in)),
                                            value, make_functor(std::move(cmp)));
    }

    template <class RASequence, class T>
    auto lower_bound(RASequence && in, T const & value)
    -> decltype(sequence(std::forward<RASequence>(in)))
    {
        return ::ural::lower_bound(std::forward<RASequence>(in), value,
                                   ural::less<>{});
    }

    template <class RASequence, class T, class Compare>
    auto upper_bound(RASequence && in, T const & value, Compare cmp)
    -> decltype(sequence(std::forward<RASequence>(in)))
    {
        return ::ural::details::upper_bound(sequence(std::forward<RASequence>(in)),
                                            value, make_functor(std::move(cmp)));
    }

    template <class RASequence, class T>
    auto upper_bound(RASequence && in, T const & value)
    -> decltype(sequence(std::forward<RASequence>(in)))
    {
        return ::ural::upper_bound(std::forward<RASequence>(in), value,
                                   ural::less<>{});
    }

    template <class RASequence, class T, class Compare>
    bool binary_search(RASequence && in, T const & value, Compare cmp)
    {
        return ::ural::details::binary_search(sequence(std::forward<RASequence>(in)),
                                              value, make_functor(std::move(cmp)));
    }

    template <class RASequence, class T>
    bool binary_search(RASequence && in, T const & value)
    {
        return ::ural::binary_search(std::forward<RASequence>(in), value, ural::less<>{});
    }

    template <class RASequence, class T, class Compare>
    auto equal_range(RASequence && in, T const & value, Compare cmp)
    -> decltype(sequence(std::forward<RASequence>(in)))
    {
        return ::ural::details::equal_range(sequence(std::forward<RASequence>(in)),
                                            value, make_functor(std::move(cmp)));
    }

    template <class RASequence, class T>
    auto equal_range(RASequence && in, T const & value)
    -> decltype(sequence(std::forward<RASequence>(in)))
    {
        return ::ural::equal_range(std::forward<RASequence>(in), value,
                                   ural::less<>{});
    }

    template <class ForwardSequence, class Compare>
    bool is_sorted(ForwardSequence && in, Compare cmp)
    {
        return ::ural::details::is_sorted(sequence(std::forward<ForwardSequence>(in)),
                                          ural::make_functor(std::move(cmp)));
    }

    template <class ForwardSequence>
    bool is_sorted(ForwardSequence && in)
    {
        return ::ural::is_sorted(sequence(std::forward<ForwardSequence>(in)),
                                 ural::less<>{});
    }

    template <class ForwardSequence, class Compare>
    auto is_sorted_until(ForwardSequence && in, Compare cmp)
    -> decltype(sequence(std::forward<ForwardSequence>(in)))
    {
        return ::ural::details::is_sorted_until(sequence(std::forward<ForwardSequence>(in)),
                                                ural::make_functor(std::move(cmp)));
    }

    template <class ForwardSequence>
    auto is_sorted_until(ForwardSequence && in)
    -> decltype(sequence(std::forward<ForwardSequence>(in)))
    {
        return ::ural::is_sorted_until(sequence(std::forward<ForwardSequence>(in)),
                                       ural::less<>{});
    }

    template <class RASequence, class Compare>
    void sort(RASequence && s, Compare cmp)
    {
        return ::ural::details::sort(sequence(std::forward<RASequence>(s)),
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
        return ::ural::details::stable_sort(sequence(std::forward<RASequence>(s)),
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
        return ::ural::details::partial_sort(sequence(std::forward<RASequence>(s)),
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
    -> decltype(sequence(std::forward<RASequence>(out)))
    {
        return ::ural::details::partial_sort_copy(sequence(std::forward<Input>(in)),
                                                  sequence(std::forward<RASequence>(out)),
                                                  ural::make_functor(std::move(cmp)));
    }

    template <class Input, class RASequence>
    auto partial_sort_copy(Input && in, RASequence && out)
    -> decltype(sequence(std::forward<RASequence>(out)))
    {
        return ::ural::partial_sort_copy(sequence(std::forward<Input>(in)),
                                         sequence(std::forward<RASequence>(out)),
                                         ural::less<>{});
    }

    template <class RASequence, class Compare>
    void nth_element(RASequence && s, Compare cmp)
    {
        return ::ural::details::nth_element(sequence(std::forward<RASequence>(s)),
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
        return ::ural::details::inplace_merge(sequence(std::forward<BidirectionalSequence>(s)),
                                              ural::make_functor(std::move(cmp)));
    }

    template <class BidirectionalSequence>
    void inplace_merge(BidirectionalSequence && s)
    {
        return ::ural::inplace_merge(std::forward<BidirectionalSequence>(s),
                                     ural::less<>{});
    }

    template <class Input1, class  Input2, class Compare>
    bool lexicographical_compare(Input1 && in1, Input2 && in2, Compare cmp)
    {
        return ::ural::details::lexicographical_compare(
                                         sequence(std::forward<Input1>(in1)),
                                         sequence(std::forward<Input2>(in2)),
                                         ural::make_functor(std::move(cmp)));
    }

    template <class Input1, class  Input2>
    bool lexicographical_compare(Input1 && in1, Input2 && in2)
    {
        return ::ural::lexicographical_compare(std::forward<Input1>(in1),
                                               std::forward<Input2>(in2),
                                               ural::less<>());
    }

    template <class Forward1, class Forward2, class BinaryPredicate>
    bool is_permutation(Forward1 && s1, Forward2 && s2, BinaryPredicate pred)
    {
        return ::ural::details::is_permutation(sequence(std::forward<Forward1>(s1)),
                                               sequence(std::forward<Forward2>(s2)),
                                               make_functor(std::move(pred)));
    }

    template <class Forward1, class Forward2>
    bool is_permutation(Forward1 && s1, Forward2 && s2)
    {
        return ::ural::is_permutation(std::forward<Forward1>(s1),
                                      std::forward<Forward2>(s2),
                                      ural::equal_to<>{});
    }


    // Операции с множествами
    template <class Input1, class  Input2, class Compare>
    bool includes(Input1 && in1, Input2 && in2, Compare cmp)
    {
        return ::ural::details::includes(sequence(std::forward<Input1>(in1)),
                                         sequence(std::forward<Input2>(in2)),
                                         ural::make_functor(std::move(cmp)));
    }

    template <class Input1, class  Input2>
    bool includes(Input1 && in1, Input2 && in2)
    {
        return ::ural::includes(std::forward<Input1>(in1),
                                std::forward<Input2>(in2), ural::less<>());
    }

    // Поиск наибольшего и наименьшего
    template <class ForwardSequence, class Compare>
    auto min_element(ForwardSequence && in, Compare cmp)
    -> decltype(sequence(std::forward<ForwardSequence>(in)))
    {
        return ::ural::details::min_element(sequence(std::forward<ForwardSequence>(in)),
                                            ural::make_functor(std::move(cmp)));
    }

    template <class ForwardSequence>
    auto min_element(ForwardSequence && in)
    -> decltype(sequence(std::forward<ForwardSequence>(in)))
    {
        return ::ural::min_element(sequence(std::forward<ForwardSequence>(in)),
                                   ural::less<>{});
    }

    template <class ForwardSequence, class Compare>
    auto max_element(ForwardSequence && in, Compare cmp)
    -> decltype(sequence(std::forward<ForwardSequence>(in)))
    {
        return ::ural::details::max_element(sequence(std::forward<ForwardSequence>(in)),
                                            ural::make_functor(std::move(cmp)));
    }

    template <class ForwardSequence>
    auto max_element(ForwardSequence && in)
    -> decltype(sequence(std::forward<ForwardSequence>(in)))
    {
        return ::ural::max_element(sequence(std::forward<ForwardSequence>(in)),
                                   ural::less<>{});
    }

    template <class ForwardSequence, class Compare>
    auto minmax_element(ForwardSequence && in, Compare cmp)
    -> ural::tuple<decltype(sequence(std::forward<ForwardSequence>(in))),
                   decltype(sequence(std::forward<ForwardSequence>(in)))>
    {
        return ::ural::details::minmax_element(sequence(std::forward<ForwardSequence>(in)),
                                               ural::make_functor(std::move(cmp)));
    }

    template <class ForwardSequence>
    auto minmax_element(ForwardSequence && in)
    -> ural::tuple<decltype(sequence(std::forward<ForwardSequence>(in))),
                   decltype(sequence(std::forward<ForwardSequence>(in)))>
    {
        return ::ural::minmax_element(sequence(std::forward<ForwardSequence>(in)),
                                      ural::less<>{});
    }

    template <class BiSequence, class Compare>
    bool next_permutation(BiSequence && s, Compare cmp)
    {
        return ::ural::details::next_permutation(sequence(std::forward<BiSequence>(s)),
                                                 ural::make_functor(std::move(cmp)));

    }

    template <class BiSequence>
    bool next_permutation(BiSequence && s)
    {
        return ural::next_permutation(std::forward<BiSequence>(s), ural::less<>{});
    }

    template <class BiSequence, class Compare>
    bool prev_permutation(BiSequence && s, Compare cmp)
    {
        return ::ural::details::prev_permutation(sequence(std::forward<BiSequence>(s)),
                                                 ural::make_functor(std::move(cmp)));

    }

    template <class BiSequence>
    bool prev_permutation(BiSequence && s)
    {
        return ural::prev_permutation(std::forward<BiSequence>(s), ural::less<>{});
    }

    // Алгоритмы над контейнерами
    auto constexpr erase = ::ural::details::erase_functor_t{};
}
// namespace ural

#endif
// Z_URAL_ALGORITHM_HPP_INCLUDED
