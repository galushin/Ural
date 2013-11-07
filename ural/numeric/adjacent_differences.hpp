#ifndef Z_URAL_NUMERIC_ADJACENT_DIFFERENCES_HPP_INCLUDED
#define Z_URAL_NUMERIC_ADJACENT_DIFFERENCES_HPP_INCLUDED

/** @file ural/numeric/adjacent_differences.hpp
 @brief Последовательность разностей соседних элементов
*/

#include <ural/functional.hpp>
#include <ural/sequence/base.hpp>

namespace ural
{
    /** @brief Последовательность разностей соседних элементов базовой
    последовательности
    @tparam Input тип базовая последовательнсоть
    @tparam BinaryOperation тип бинарной операции, определяющей разность между
    элементами
    */
    template <class Input, class BinaryOperation>
    class adjacent_differences_sequence
     : public sequence_base<adjacent_differences_sequence<Input, BinaryOperation>>
    {
    public:
        explicit adjacent_differences_sequence(Input in, BinaryOperation op)
         : members_{std::move(in), std::move(op), {}, {}}
        {
            if(!!*this)
            {
                members_[ural::_3] = *in;
                members_[ural::_4] = members_[ural::_3];
            }
        }

        typedef typename Input::value_type value_type;
        typedef value_type const & reference;

        /** @brief Проверка исчерпания последовательности
        @return <tt> !this->base() </tt>.
        */
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
            auto old_value = std::move(*members_[ural::_4]);
            ++ members_[ural::_1];

            if(!!*this)
            {
                members_[ural::_4] = *this->base();
                members_[ural::_3] = this->operation()(*members_[ural::_4],
                                                       std::move(old_value));
            }

            return *this;
        }

        Input const & base() const
        {
            return members_[ural::_1];
        }

        BinaryOperation const & operation() const
        {
            return members_[ural::_2];
        }

    private:
        typedef ural::optional<value_type> Optional_value;

        ural::tuple<Input, BinaryOperation, Optional_value, Optional_value> members_;
    };

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
