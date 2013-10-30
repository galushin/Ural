#ifndef Z_URAL_NUMERIC_PARTIAL_SUMS_HPP_INCLUDED
#define Z_URAL_NUMERIC_PARTIAL_SUMS_HPP_INCLUDED

#include <ural/functional.hpp>
#include <ural/optional.hpp>

namespace ural
{
    /** @todo Реализация для прямых последовательностей
    @todo реализация для последовательностей, у элементов которых нет
    конструктора без аргументов
    @todo Оптимизация размера
    @todo Специализированный алгоритм @c copy
    */
    template <class Input, class BinaryOperation>
    class partial_sums_sequence
    {
    public:
        /// @todo через класс-характеристику?
        typedef typename Input::reference reference;
        typedef typename Input::value_type value_type;
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
            // @note Проверка того, что последовательность не пуста
            return members_[ural::_3].value_unsafe();
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

    /** @todo Автоматизировать создание таких функций
    */
    template <class Input, class BinaryOperation>
    partial_sums_sequence<Input, BinaryOperation>
    sequence(partial_sums_sequence<Input, BinaryOperation> s)
    {
        return s;
    }

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
