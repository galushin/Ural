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
