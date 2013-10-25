#ifndef Z_URAL_ALGORITHM_HPP_INCLUDED
#define Z_URAL_ALGORITHM_HPP_INCLUDED

#include <ural/sequence/all.hpp>

#include <ural/algorithm/details/copy.hpp>

namespace ural
{
    template <class Input, class Output>
    void copy(Input && in, Output && out)
    {
        // @todo forward
        ural::details::copy(ural::sequence(std::forward<Input>(in)),
                            ural::sequence(std::forward<Output>(out)));
    }
}
// namespace ural

#endif
// Z_URAL_ALGORITHM_HPP_INCLUDED
