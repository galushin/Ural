#ifndef Z_URAL_SEQUENCE_FUNCTION_OUTPUT_HPP_INCLUDED
#define Z_URAL_SEQUENCE_FUNCTION_OUTPUT_HPP_INCLUDED

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

/** @file ural/sequence/function_output.hpp
 @brief Выходная последовательность, передающая записываемое значение как
 аргумент заданной функции.
*/

#include <ural/sequence/base.hpp>

namespace ural
{
    /** @brief Выходная последовательность, выполняющая вызов функции для
    каждого "записываемого элемента"
    */
    template <class UnaryFunction>
    class function_output_sequence
     : public ural::sequence_base<function_output_sequence<UnaryFunction>,
                                   UnaryFunction>
    {
        typedef ural::sequence_base<function_output_sequence<UnaryFunction>,
                                    UnaryFunction> Base_class;
    public:
        typedef single_pass_traversal_tag traversal_tag;

        function_output_sequence()
         : Base_class()
        {}

        explicit function_output_sequence(UnaryFunction f)
         : Base_class{std::move(f)}
        {}

        UnaryFunction const & functor() const
        {
            return *this;
        }

        bool operator!() const
        {
            return false;
        }

        function_output_sequence & operator*()
        {
            return *this;
        }

        void pop_front()
        {}

        template <class Arg>
        function_output_sequence & operator=(Arg && arg)
        {
            this->functor_ref()(std::forward<Arg>(arg));
            return *this;
        }

    private:
        UnaryFunction & functor_ref()
        {
            return static_cast<UnaryFunction &>(*this);
        }
    };

    template <class UnaryFunction>
    auto make_function_output_sequence(UnaryFunction f)
    -> function_output_sequence<decltype(ural::make_functor(std::move(f)))>
    {
        typedef decltype(ural::make_functor(std::move(f))) Functor;
        typedef function_output_sequence<Functor> Result;
        return Result(ural::make_functor(std::move(f)));
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_FUNCTION_OUTPUT_HPP_INCLUDED
