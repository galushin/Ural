#ifndef Z_URAL_NUMERIC_HPP_INCLUDED
#define Z_URAL_NUMERIC_HPP_INCLUDED

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

/** @file ural/numeric.hpp
 @brief Обобщённые численные операции
*/

#include <ural/functional.hpp>
#include <ural/sequence/all.hpp>

#include <ural/numeric/partial_sums.hpp>
#include <ural/numeric/adjacent_differences.hpp>

namespace ural
{
    class
    {
    public:
        template <class ForwardSequence, class Incrementable>
        Incrementable
        operator()(ForwardSequence && seq, Incrementable init_value) const
        {
            return impl(ural::sequence(std::forward<ForwardSequence>(seq)),
                        std::move(init_value));
        }

    private:
        template <class ForwardSequence, class Incrementable>
        Incrementable
        impl(ForwardSequence seq, Incrementable init_value) const
        {
            for(; !!seq; ++ seq, ++ init_value)
            {
                *seq = init_value;
            }

            return init_value;
        }
    }
    constexpr iota{};

    class
    {
    public:
        template <class Input, class T, class BinaryOperation>
        T operator()(Input && in, T init_value, BinaryOperation op) const
        {
            return impl(sequence(std::forward<Input>(in)),
                        std::move(init_value),
                        ural::make_functor(std::move(op)));
        }

        template <class Input, class T>
        T operator()(Input && in, T init_value) const
        {
            return (*this)(std::forward<Input>(in), std::move(init_value),
                           ural::plus<>());
        }

    private:
        template <class InputSequence, class T, class BinaryOperation>
        T impl(InputSequence in, T init_value, BinaryOperation op) const
        {
            for(; !!in; ++ in)
            {
                init_value = op(std::move(init_value), *in);
            }

            return init_value;
        }
    }
    constexpr accumulate {};

    class
    {
    public:
        template <class Input1, class Input2, class T,
                class BinaryOperation1, class BinaryOperation2>
        T operator()(Input1 && in1, Input2 && in2, T init_value,
                     BinaryOperation1 add, BinaryOperation2 mult) const
        {
            return impl(ural::sequence(std::forward<Input1>(in1)),
                        ural::sequence(std::forward<Input2>(in2)),
                        std::move(init_value),
                        ural::make_functor(std::move(add)),
                        ural::make_functor(std::move(mult)));
        }

        template <class Input1, class Input2, class T>
        T operator()(Input1 && in1, Input2 && in2, T init_value) const
        {
            return (*this)(std::forward<Input1>(in1), std::forward<Input2>(in2),
                           std::move(init_value),
                           ural::plus<>{}, ural::multiplies<>{});
        }

    private:
        template <class Input1, class Input2, class T,
                  class BinaryOperation1, class BinaryOperation2>
        T impl(Input1 in1, Input2 in2, T value,
               BinaryOperation1 add, BinaryOperation2 mult) const
        {
            for(; !!in1 && !!in2; ++ in1, ++in2)
            {
                value = add(std::move(value), mult(*in1, *in2));
            }

            return value;
        }
    }
    constexpr inner_product{};
}
// namespace ural

#endif
// Z_URAL_NUMERIC_HPP_INCLUDED
