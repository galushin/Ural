#ifndef Z_URAL_ALGORITHM_HPP_INCLUDED
#define Z_URAL_ALGORITHM_HPP_INCLUDED

#include <ural/sequence/all.hpp>

#include <ural/algorithm/details/copy.hpp>

namespace ural
{
    template <class Input, class Output>
    auto copy(Input && in, Output && out)
    -> decltype(ural::details::copy(ural::sequence(std::forward<Input>(in)),
                                    ural::sequence(std::forward<Output>(out))))
    {
        return ural::details::copy(ural::sequence(std::forward<Input>(in)),
                                   ural::sequence(std::forward<Output>(out)));
    }
}
// namespace ural

#endif
// Z_URAL_ALGORITHM_HPP_INCLUDED
