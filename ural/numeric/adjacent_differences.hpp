#ifndef Z_URAL_NUMERIC_ADJACENT_DIFFERENCES_HPP_INCLUDED
#define Z_URAL_NUMERIC_ADJACENT_DIFFERENCES_HPP_INCLUDED

#include <ural/functional.hpp>

namespace ural
{
    template <class Input, class BinaryOperation>
    class adjacent_differences_sequence
    {
    public:
        explicit adjacent_differences_sequence(Input in, BinaryOperation op)
         : members_{std::move(in), std::move(op), {}, {}}
        {
            if(!!*this)
            {
                members_[ural::_3] = *in;
            }
        }

        typedef typename Input::value_type value_type;
        typedef value_type const & reference;

        bool operator!() const
        {
            return !this->base();
        }

        reference operator*() const
        {
            // @todo проверка через стратегию?
            return members_[ural::_3].value();
        }

        adjacent_differences_sequence & operator++()
        {
            ++ members_[ural::_1];

            return *this;
        }

        Input const & base() const
        {
            return members_[ural::_1];
        }

    private:
    private:
        typedef ural::optional<value_type> Optional_value;

        ural::tuple<Input, BinaryOperation, Optional_value, Optional_value> members_;
    };

    /** @todo Автоматизировать создание таких функций
    */
    template <class Input, class BinaryOperation>
    adjacent_differences_sequence<Input, BinaryOperation>
    sequence(adjacent_differences_sequence<Input, BinaryOperation> s)
    {
        return s;
    }

    template <class Input, class BinaryOperation>
    auto adjacent_differences(Input && in, BinaryOperation sub)
    -> adjacent_differences_sequence<decltype(sequence(std::forward<Input>(in))),
                                     decltype(make_functor(std::move(sub)))>
    {
        typedef adjacent_differences_sequence<decltype(sequence(std::forward<Input>(in))),
                                              decltype(make_functor(std::move(sub)))> Result;
        return Result{sequence(std::forward<Input>(in)),
                      make_functor(std::move(sub))};
    }

    template <class Input>
    auto adjacent_differences(Input && in)
    -> adjacent_differences_sequence<decltype(sequence(std::forward<Input>(in))),
                                     ural::minus<>>
    {
        return adjacent_differences(std::forward<Input>(in), ural::minus<>{});
    }
}
// namespace ural

#endif
// Z_URAL_NUMERIC_ADJACENT_DIFFERENCES_HPP_INCLUDED
