#ifndef Z_URAL_ALGORITHM_DETAILS_COPY_HPP_INCLUDED
#define Z_URAL_ALGORITHM_DETAILS_COPY_HPP_INCLUDED

#include <ural/tuple.hpp>

namespace ural
{
namespace details
{
    template <class Input, class Output>
    ural::tuple<Input, Output>
    copy(Input in, Output out)
    {
        for(; !!in && !!out; ++ in, ++ out)
        {
            *out = *in;
        }

        return {in, out};
    }
}
// namespace details
}
// namespace ural

#endif
// Z_URAL_ALGORITHM_DETAILS_COPY_HPP_INCLUDED
