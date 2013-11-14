#ifndef Z_URAL_ALGO_BASE_HPP_INCLUDED
#define Z_URAL_ALGO_BASE_HPP_INCLUDED

#include <ural/sequence/function_output.hpp>
#include <ural/algorithm/details/copy.hpp>

namespace ural
{
namespace details
{
    template <class Input, class UnaryFunction>
    UnaryFunction for_each(Input in, UnaryFunction f)
    {
        auto r = ural::details::copy(in, ural::make_function_output_sequence(std::move(f)));
        return r[ural::_2].functor();
    }

    template <class Input1, class Input2, class BinaryPredicate>
    bool equal(Input1 in1, Input2 in2, BinaryPredicate pred)
    {
        for(; !!in1 && !!in2; ++ in1, ++ in2)
        {
            if(!pred(*in1, *in2))
            {
                return false;
            }
        }

        return !in1 && !in2;
    }
}
// namespace details
}
// namespace ural

#endif
// Z_URAL_ALGO_BASE_HPP_INCLUDED
