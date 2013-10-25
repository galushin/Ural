#ifndef Z_URAL_NUMERIC_HPP_INCLUDED
#define Z_URAL_NUMERIC_HPP_INCLUDED

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
}
// namespace ural

#endif
// Z_URAL_NUMERIC_HPP_INCLUDED
