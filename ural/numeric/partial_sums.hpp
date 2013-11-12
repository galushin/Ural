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
        /// @brief Тип значения
        typedef typename Input::value_type value_type;

        /// @brief Тип ссылки
        typedef value_type const & reference;

        /// @brief Тип операции, используемой для вычисления суммы
        typedef BinaryOperation operation_type;

        /** @brief Конструктор
        @param in исходная последовательность
        @param add операция, используемая для вычисления суммы
        @post <tt> this->base() == in </tt>
        @post <tt> this->operation() == add </tt>
        */
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

        /** @brief Текущий элемент
        @pre <tt> !*this == false </tt>
        */
        reference operator*() const
        {
            // @note Проверка, что последовательность не пуста - через стратегию
            return *(members_[ural::_3]);
        }

        /** @brief Переход к следующему элементу
        @pre <tt> bool(*this) != false </tt>
        @return <tt> *this </tt>
        */
        partial_sums_sequence & operator++()
        {
            ++ members_[ural::_1];

            if(!!this->base())
            {
                members_[ural::_3] = this->operation()(*(*this), *this->base());
            }

            return *this;
        }

        /** @brief Исходная последовательность
        @return Текущее состояние входной последовательности
        */
        Input const & base() const
        {
            return members_[ural::_1];
        }

        /** @brief Операция, используемая для вычисления суммы
        @return Операция, используемая для вычисления суммы
        */
        operation_type const & operation() const
        {
            return members_[ural::_2];
        }

    private:
        typedef ural::optional<value_type> Optional_value;

        /// @brief Кортеж, содержащий все переменные-члены
        ural::tuple<Input, BinaryOperation, Optional_value> members_;
    };

    /** @brief Создание последовательности частных сумм
    @param s базовая последовательность
    @param add операция, определяющая сложение
    */
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

    /** @brief Создание последовательности частных сумм
    @param s базовая последовательность
    @return <tt> partial_sums(std::forward<Input>(s), ural::plus<>{}) </tt>
    */
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
