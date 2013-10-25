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

    template <class InputSequence, class T, class BinaryOperation>
    T accumulate(InputSequence in, T init_value, BinaryOperation op)
    {
        for(; !!in; ++ in)
        {
            init_value = op(std::move(init_value), *in);
        }

        return init_value;
    }

    template <class Input1, class Input2, class T,
              class BinaryOperation1, class BinaryOperation2>
    T inner_product(Input1 in1, Input2 in2, T value,
                    BinaryOperation1 add, BinaryOperation2 mult)
    {
        for(; !!in1 && !!in2; ++ in1, ++in2)
        {
            value = add(std::move(value), mult(*in1, *in2));
        }

        return value;
    }
}
// namespace details
}
// namespace ural

#endif
// Z_URAL_NUMERIC_NUM_ALGO_HPP_INCLUDED
