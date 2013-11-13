#ifndef Z_URAL_SEQUENCE_FUNCTION_OUTPUT_HPP_INCLUDED
#define Z_URAL_SEQUENCE_FUNCTION_OUTPUT_HPP_INCLUDED

#include <ural/sequence/base.hpp>

namespace ural
{
    /** @todo Оптимизация размера
    @todo Точное задание типов аргументов?
    */
    template <class UnaryFunction>
    class function_output_sequence
     : private ural::sequence_base<function_output_sequence<UnaryFunction>>
    {
    public:
        explicit function_output_sequence(UnaryFunction f)
         : f_{std::move(f)}
        {}

        UnaryFunction const & functor() const
        {
            return this->f_;
        }

        bool operator!() const
        {
            return false;
        }

        function_output_sequence & operator++()
        {
            return *this;
        }

        function_output_sequence & operator*()
        {
            return *this;
        }

        template <class Arg>
        function_output_sequence & operator=(Arg && arg)
        {
            f_(std::forward<Arg>(arg));
            return *this;
        }

    private:
        UnaryFunction f_;
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
