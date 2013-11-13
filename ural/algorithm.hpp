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
    template <class Input, class Output>
    auto copy(Input && in, Output && out)
    -> decltype(ural::details::copy(sequence(std::forward<Input>(in)),
                                    sequence(std::forward<Output>(out))))
    {
        return ural::details::copy(sequence(std::forward<Input>(in)),
                                   sequence(std::forward<Output>(out)));
    }

    template <class Input, class UnaryFunction>
    auto for_each(Input && in, UnaryFunction f)
    -> decltype(ural::make_functor(std::move(f)))
    {
        return ural::details::for_each(ural::sequence(std::forward<Input>(in)),
                                       ural::make_functor(std::move(f)));
    }
}
// namespace ural

#endif
// Z_URAL_ALGORITHM_HPP_INCLUDED
