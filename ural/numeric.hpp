#ifndef Z_URAL_NUMERIC_HPP_INCLUDED
#define Z_URAL_NUMERIC_HPP_INCLUDED

/** @file ural/numeric.hpp
 @brief Обобщённые численные операции
*/

#include <ural/functional.hpp>
#include <ural/sequence/all.hpp>

#include <ural/numeric/details/num_algo.hpp>
#include <ural/numeric/partial_sums.hpp>
#include <ural/numeric/adjacent_differences.hpp>

namespace ural
{
    template <class ForwardSequence, class Incrementable>
    Incrementable
    iota(ForwardSequence && seq, Incrementable init_value)
    {
        return ::ural::details::iota(ural::sequence(std::forward<ForwardSequence>(seq)),
                                     std::move(init_value));
    }

    template <class Input, class T, class BinaryOperation>
    T accumulate(Input && in, T init_value, BinaryOperation op)
    {
        return ::ural::details::accumulate(ural::sequence(std::forward<Input>(in)),
                                           std::move(init_value),
                                           ural::make_functor(std::move(op)));
    }

    template <class Input, class T>
    T accumulate(Input && in, T init_value)
    {
        return ::ural::accumulate(std::forward<Input>(in),
                                  std::move(init_value),
                                  ural::plus<>());
    }

    template <class Input1, class Input2, class T,
                class BinaryOperation1, class BinaryOperation2>
    T inner_product(Input1 && in1, Input2 && in2, T init_value,
                    BinaryOperation1 add, BinaryOperation2 mult)
    {
        return ::ural::details::inner_product(ural::sequence(std::forward<Input1>(in1)),
                                              ural::sequence(std::forward<Input2>(in2)),
                                              std::move(init_value),
                                              ural::make_functor(std::move(add)),
                                              ural::make_functor(std::move(mult)));
    }

    template <class Input1, class Input2, class T>
    T inner_product(Input1 && in1, Input2 && in2, T init_value)
    {
        return ::ural::inner_product(std::forward<Input1>(in1),
                                     std::forward<Input2>(in2),
                                     std::move(init_value),
                                     ural::plus<>{}, ural::multiplies<>{});
    }
}
// namespace ural

#endif
// Z_URAL_NUMERIC_HPP_INCLUDED
