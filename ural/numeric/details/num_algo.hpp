#ifndef Z_URAL_NUMERIC_NUM_ALGO_HPP_INCLUDED
#define Z_URAL_NUMERIC_NUM_ALGO_HPP_INCLUDED

/*  This file is part of Ural.

    Ural is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Ural is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Ural.  If not, see <http://www.gnu.org/licenses/>.
*/

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
