#ifndef Z_URAL_ALGORITHM_HPP_INCLUDED
#define Z_URAL_ALGORITHM_HPP_INCLUDED

/** @file ural/algorithm.hpp
 @brief Обобщённые алгоритмы
*/

#include <ural/functional.hpp>
#include <ural/sequence/all.hpp>

#include <ural/algorithm/details/copy.hpp>
#include <ural/algorithm/details/algo_base.hpp>

namespace ural
{
    template <class Input, class UnaryFunction>
    auto for_each(Input && in, UnaryFunction f)
    -> decltype(ural::make_functor(std::move(f)))
    {
        return ural::details::for_each(ural::sequence(std::forward<Input>(in)),
                                       ural::make_functor(std::move(f)));
    }

    template <class Input, class Predicate>
    auto find_if(Input && in, Predicate pred)
    -> decltype(ural::sequence(std::forward<Input>(in)))
    {
        return ::ural::details::find_if(ural::sequence(std::forward<Input>(in)),
                                       ural::make_functor(std::move(pred)));
    }

    template <class Input, class Predicate>
    auto find_if_not(Input && in, Predicate pred)
    -> decltype(ural::sequence(std::forward<Input>(in)))
    {
        return ::ural::details::find_if(ural::sequence(std::forward<Input>(in)),
                                        ural::not_fn(std::move(pred)));
    }

    template <class Input, class T, class BinaryPredicate>
    auto find(Input && in, T const & value, BinaryPredicate pred)
    -> decltype(ural::sequence(std::forward<Input>(in)))
    {
        return ::ural::find_if(std::forward<Input>(in),
                               std::bind(std::move(pred), std::placeholders::_1, std::ref(value)));
    }

    template <class Input, class T>
    auto find(Input && in, T const & value)
    -> decltype(ural::sequence(std::forward<Input>(in)))
    {
        return ::ural::find(std::forward<Input>(in), value,
                            ural::equal_to<T>{});
    }

    template <class Input, class UnaryPredicate>
    bool all_of(Input && in, UnaryPredicate pred)
    {
        return !::ural::find_if_not(std::forward<Input>(in), pred);
    }

    template <class Input, class UnaryPredicate>
    bool any_of(Input && x, UnaryPredicate pred);

    template <class Input, class UnaryPredicate>
    bool none_of(Input && x, UnaryPredicate pred);

    template <class Input1, class Input2, class BinaryPredicate>
    bool equal(Input1 && in1, Input2 && in2, BinaryPredicate pred)
    {
        return ::ural::details::equal(ural::sequence(std::forward<Input1>(in1)),
                                      ural::sequence(std::forward<Input2>(in2)),
                                      ural::make_functor(std::move(pred)));
    }

    template <class Input1, class Input2>
    bool equal(Input1 && in1, Input2 && in2)
    {
        return ::ural::equal(std::forward<Input1>(in1),
                             std::forward<Input2>(in2), ural::equal_to<>());
    }

    template <class Input, class Output>
    auto copy(Input && in, Output && out)
    -> decltype(ural::details::copy(sequence(std::forward<Input>(in)),
                                    sequence(std::forward<Output>(out))))
    {
        return ural::details::copy(sequence(std::forward<Input>(in)),
                                   sequence(std::forward<Output>(out)));
    }

    template <class ForwardSequence, class T>
    void fill(ForwardSequence && seq, T const & value)
    {
        return ural::details::fill(sequence(std::forward<ForwardSequence>(seq)),
                                   value);
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

    // Операции с множествами
    template <class Input1, class  Input2, class Compare>
    bool includes(Input1 && in1, Input2 && in2, Compare cmp)
    {
        return ::ural::details::includes(sequence(std::forward<Input1>(in1)),
                                         sequence(std::forward<Input1>(in2)),
                                         ural::make_functor(std::move(cmp)));
    }

    template <class Input1, class  Input2>
    bool includes(Input1 && in1, Input2 && in2)
    {
        return ::ural::includes(std::forward<Input1>(in1),
                                std::forward<Input1>(in2), ural::less<>());
    }
}
// namespace ural

#endif
// Z_URAL_ALGORITHM_HPP_INCLUDED
