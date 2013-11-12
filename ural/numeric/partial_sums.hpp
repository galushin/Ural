#ifndef Z_URAL_NUMERIC_PARTIAL_SUMS_HPP_INCLUDED
#define Z_URAL_NUMERIC_PARTIAL_SUMS_HPP_INCLUDED

/** @file ural/numeric/partial_sums.hpp
 @brief Последовательность частных сумм
*/

#include <ural/functional.hpp>
#include <ural/optional.hpp>

#include <ural/sequence/base.hpp>

namespace ural
{
    /** @todo Оптимизация размера
    @todo Специализированный алгоритм @c copy
    */
    template <class Input, class BinaryOperation>
    class partial_sums_sequence
     : public sequence_base<partial_sums_sequence<Input, BinaryOperation>>
    {
    public:
        typedef typename Input::value_type value_type;

        typedef value_type const & reference;
        typedef BinaryOperation operation_type;

        explicit partial_sums_sequence(Input in, BinaryOperation add)
         : members_{std::move(in), std::move(add), {}}
        {
            if(!!this->base())
            {
                members_[ural::_3] = *this->base();
            }
        }

        bool operator!() const
        {
            return !this->base();
        }

        reference operator*() const
        {
            // @note Проверка, что последовательность не пуста - через стратегию
            return *(members_[ural::_3]);
        }

        partial_sums_sequence & operator++()
        {
            ++ members_[ural::_1];

            if(!!this->base())
            {
                members_[ural::_3] = this->operation()(*(*this), *this->base());
            }

            return *this;
        }

        Input const & base() const
        {
            return members_[ural::_1];
        }

        operation_type const & operation() const
        {
            return members_[ural::_2];
        }

    private:
        typedef ural::optional<value_type> Optional_value;

        ural::tuple<Input, BinaryOperation, Optional_value> members_;
    };

    template <class Input, class BinaryOperation>
    auto partial_sums(Input && s, BinaryOperation add)
    -> partial_sums_sequence<decltype(sequence(std::forward<Input>(s))),
                             decltype(make_functor(std::move(add)))>
    {
        typedef partial_sums_sequence<decltype(sequence(std::forward<Input>(s))),
                             decltype(make_functor(std::move(add)))> Result;
        return Result{sequence(std::forward<Input>(s)),
                      make_functor(std::move(add))};
    }

    template <class Input>
    auto partial_sums(Input && s)
    -> partial_sums_sequence<decltype(sequence(std::forward<Input>(s))), ural::plus<>>
    {
        return partial_sums(std::forward<Input>(s), ural::plus<>{});
    }
}
// namespace ural

#endif
// Z_URAL_NUMERIC_PARTIAL_SUMS_HPP_INCLUDED
