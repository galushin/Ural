#ifndef Z_URAL_NUMERIC_HPP_INCLUDED
#define Z_URAL_NUMERIC_HPP_INCLUDED

#include <ural/functional.hpp>
#include <ural/sequence/all.hpp>

#include <ural/numeric/details/num_algo.hpp>

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
}
// namespace ural

#endif
// Z_URAL_NUMERIC_HPP_INCLUDED
