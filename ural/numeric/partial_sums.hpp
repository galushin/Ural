#ifndef Z_URAL_NUMERIC_PARTIAL_SUMS_HPP_INCLUDED
#define Z_URAL_NUMERIC_PARTIAL_SUMS_HPP_INCLUDED

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

/** @file ural/numeric/partial_sums.hpp
 @brief Последовательность частных сумм
*/

#include <ural/functional.hpp>
#include <ural/optional.hpp>

#include <ural/sequence/base.hpp>

namespace ural
{
    /** @brief Последовательность частных сумм
    */
    template <class Input, class BinaryOperation>
    class partial_sums_sequence
     : public sequence_base<partial_sums_sequence<Input, BinaryOperation>,
                            BinaryOperation>
    {
        typedef sequence_base<partial_sums_sequence, BinaryOperation>
            Base_class;
    public:
        /** @brief Оператор "равно"
        @param x, y аргументы
        @return <tt> x.base() == y.base() && x.operation() == y.operation() </tt>
        */
        friend bool operator==(partial_sums_sequence const & x,
                               partial_sums_sequence const & y)
        {
            return x.base() == y.base() && x.operation() == y.operation();
        }

        /// @brief Тип значения
        typedef ValueType<Input> value_type;

        /// @brief Тип ссылки
        typedef value_type const & reference;

        /// @brief Категория курсора
        using cursor_tag = common_tag_t<typename Input::cursor_tag, finite_forward_cursor_tag>;

        /// @brief Тип указателя
        typedef value_type const & pointer;

        /// @brief Тип расстояния
        typedef DifferenceType<Input> distance_type;

        /// @brief Тип операции, используемой для вычисления суммы
        typedef BinaryOperation operation_type;

        // Создание и свойства
        /** @brief Конструктор
        @param in исходная последовательность
        @param add операция, используемая для вычисления суммы
        @post <tt> this->base() == in </tt>
        @post <tt> this->operation() == add </tt>
        */
        explicit partial_sums_sequence(Input in, BinaryOperation add)
         : Base_class{std::move(add)}
         , members_{std::move(in), {}}
        {
            if(!!this->base())
            {
                current_value_ref() = *this->base();
            }
        }

        //@{
        /** @brief Исходная последовательность
        @return Текущее состояние входной последовательности
        */
        Input const & base() const &
        {
            return ::ural::get(this->members_, ural::_1);
        }

        Input && base() &&
        {
            return ::ural::get(std::move(this->members_), ural::_1);
        }
        //@}

        /** @brief Операция, используемая для вычисления суммы
        @return Операция, используемая для вычисления суммы
        */
        operation_type const & operation() const
        {
            return this->payload();
        }

        // Однопроходная последовательность
        bool operator!() const
        {
            return !this->base();
        }

        /** @brief Текущий элемент
        @pre <tt> !*this == false </tt>
        */
        reference front() const
        {
            // @note Проверка, что последовательность не пуста - через стратегию
            return *::ural::get(members_, ural::_2);
        }

        /** @brief Переход к следующему элементу
        @pre <tt> bool(*this) != false </tt>
        @return <tt> *this </tt>
        */
        void pop_front()
        {
            ++ input_ref();

            if(!!this->base())
            {
                current_value_ref() = this->operation()(*current_value_ref(),
                                                        *this->base());
            }
        }

        // Прямая последовательность
        /** @brief Исходная последовательность
        @return Исходная последовательность
        */
        partial_sums_sequence original() const;

        /** @brief Передняя пройденная часть последовательности
        @return Передняя пройденная часть последовательности
        */
        partial_sums_sequence<TraversedFrontType<Input>, BinaryOperation>
        traversed_front() const
        {
            using Result = partial_sums_sequence<TraversedFrontType<Input>, BinaryOperation>;
            return Result(this->base().traversed_front(), this->operation());
        }

        /** @brief Отбрасывание передней пройденной части последовательности
        @post <tt> !this->traversed_front() </tt>
        */
        void shrink_front();

    private:
        typedef ural::optional<value_type> Optional_value;

        Optional_value & current_value_ref()
        {
            return ::ural::get(members_, ural::_2);
        }

        Input & input_ref()
        {
            return ::ural::get(members_, ural::_1);
        }

    private:
        ural::tuple<Input, Optional_value> members_;
    };

    /** @brief Создание последовательности частных сумм
    @param s базовая последовательность
    @param add операция, определяющая сложение
    */
    template <class Input, class BinaryOperation>
    auto partial_sums(Input && s, BinaryOperation add)
    -> partial_sums_sequence<decltype(::ural::sequence_fwd<Input>(s)),
                             decltype(make_callable(std::move(add)))>
    {
        typedef partial_sums_sequence<decltype(::ural::sequence_fwd<Input>(s)),
                             decltype(make_callable(std::move(add)))> Result;
        return Result(::ural::sequence_fwd<Input>(s),
                      make_callable(std::move(add)));
    }

    /** @brief Создание последовательности частных сумм
    @param s базовая последовательность
    @return <tt> partial_sums(std::forward<Input>(s), ural::plus<>{}) </tt>
    */
    template <class Input>
    auto partial_sums(Input && s)
    -> partial_sums_sequence<decltype(::ural::sequence_fwd<Input>(s)), ural::plus<>>
    {
        return partial_sums(std::forward<Input>(s), ural::plus<>{});
    }
}
// namespace ural

#endif
// Z_URAL_NUMERIC_PARTIAL_SUMS_HPP_INCLUDED
