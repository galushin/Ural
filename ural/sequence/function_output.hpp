#ifndef Z_URAL_SEQUENCE_FUNCTION_OUTPUT_HPP_INCLUDED
#define Z_URAL_SEQUENCE_FUNCTION_OUTPUT_HPP_INCLUDED

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
