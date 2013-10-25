#ifndef Z_URAL_NUMERIC_NUM_ALGO_HPP_INCLUDED
#define Z_URAL_NUMERIC_NUM_ALGO_HPP_INCLUDED

namespace ural
{
namespace details
{
    template <class ForwardSequence, class Incrementable>
    Incrementable
    iota(ForwardSequence seq, Incrementable init_value)
    {
        for(; !!seq; ++ seq, ++ init_value)
        {
            *seq = init_value;
        }

        return init_value;
    }
}
// namespace details
}
// namespace ural

#endif
// Z_URAL_NUMERIC_NUM_ALGO_HPP_INCLUDED
