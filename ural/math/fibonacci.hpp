#ifndef Z_URAL_MATH_FIBONACCI_HPP_INCLUDED
#define Z_URAL_MATH_FIBONACCI_HPP_INCLUDED

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

/** @file ural/math/fibonacci.hpp
 @brief Последовательность чисел Фибоначчи
*/

#include <ural/sequence/base.hpp>
#include <ural/sequence/adaptors/delimit.hpp>

namespace ural
{
    /** @brief Последовательность чисел (типа) Фибоначчи
    @tparam Integer целочисленный тип
    @tparam BinaryOperation бинарная операция, используемая в качестве сложения
    @tparam Traversal категория обхода
    */
    template <class Integer, class BinaryOperation = use_default,
              class Traversal = use_default>
    class fibonacci_sequence
     : public sequence_base<fibonacci_sequence<Integer, BinaryOperation, Traversal>>
    {
        using Inherited = sequence_base<fibonacci_sequence<Integer, BinaryOperation, Traversal>>;
    public:
        /// @brief Категория обхода
        using traversal_tag = DefaultedType<Traversal, single_pass_traversal_tag>;

        /// @brief Тип значения
        using value_type = Integer;

        /// @brief Тип ссылки
        using reference = value_type const &;

        /// @brief Тип указателя
        using pointer = value_type const *;

        /// @brief Тип расстояния
        using distance_type = Integer;

        /// @brief Тип операции
        using operation_type = DefaultedType<FunctionType<BinaryOperation>,
                                             ural::plus<Integer>>;

        // Конструкторы и свойства
        /** @brief Конструктор
        @post <tt> this->front() == Integer(1) </tt>
        @post <tt> next(*this).front() == Integer(1) </tt>
        */
        constexpr fibonacci_sequence()
         : fibonacci_sequence(Integer{1}, Integer{1}, operation_type{})
        {}

        /** @brief Конструктор
        @param first первое значение
        @param second второе значение
        @post <tt> this->front() == first </tt>
        @post <tt> next(*this).front() == second </tt>
        */
        constexpr fibonacci_sequence(value_type first, value_type second)
         : data_(std::move(first), std::move(second), operation_type{})
        {}

        /** @brief Конструктор
        @param first первое значение
        @param second второе значение
        @param op операция
        @post <tt> this->front() == first </tt>
        @post <tt> this->operation() == op </tt>
        @post <tt> next(*this).front() == second </tt>
        */
        constexpr fibonacci_sequence(value_type first, value_type second,
                                     operation_type op)
         : data_(Value(std::move(first)), Value(std::move(second)),
                 std::move(op))
        {}

        /** @brief Используемая операция
        @return Используемая операция
        */
        constexpr operation_type const & operation() const
        {
            return this->data_[ural::_3];
        }

        /** @brief Оператор "равно"
        @param x, y аргументы
        @return @b true, если @c x и @c y равны, иначе --- @b false.
        */
        friend bool
        operator==(fibonacci_sequence const & x, fibonacci_sequence const & y)
        {
            return x.data_ == y.data_;
        }

        // Однопроходная последовательность
        /** @brief Проверка исчерпания последовательности
        @return @b false
        */
        bool operator!() const
        {
            return false;
        }

        /** @brief Текущий элемент
        @return Ссылка на текущий элемент
        */
        constexpr reference front() const
        {
            return ural::get(this->data_[ural::_1]);
        }

        /// @brief Переход к следующему значению
        void pop_front()
        {
            auto new_value = this->operation()(std::move(ural::get(data_[ural::_1])),
                                               ural::get(data_[ural::_2]));
            data_[ural::_1] = std::move(ural::get(data_[ural::_2]));
            data_[ural::_2]= std::move(new_value);
        }

        // Прямая последовательность
        /** @brief Передняя пройденная часть последовательности
        @return Передняя пройденная часть последовательности
        */
        delimit_sequence<fibonacci_sequence>
        traversed_front() const
        {
            return this->original() | ural::delimited(this->front());
        }

        /** @brief Исходная последовательность
        @return Исходная последовательность
        */
        fibonacci_sequence original() const
        {
            return fibonacci_sequence(data_[ural::_1].old_value(),
                                      data_[ural::_2].old_value(),
                                      this->operation());
        }

        void shrink_front()
        {
            data_[ural::_1].commit();
            data_[ural::_2].commit();
        }

    private:
        static auto constexpr is_forward
            = std::is_convertible<traversal_tag, forward_traversal_tag>::value;

        using Value = typename std::conditional<is_forward,
                                                with_old_value<value_type>,
                                                value_type>::type;

        tuple<Value, Value, operation_type> data_;
    };

    /** @brief Тип Функционального объекта для создания последовательности чисел
    (типа) Фибоначчи.
    */
    struct make_fibonacci_sequence_fn
    {
    public:
        /** @brief Конструктор
        @param first первое значение
        @param second второе значение
        */
        template <class Integer>
        constexpr fibonacci_sequence<Integer>
        operator()(Integer first, Integer second) const
        {
            using Result = fibonacci_sequence<Integer>;
            return Result{std::move(first), std::move(second)};
        }

        /** @brief Конструктор
        @param first первое значение
        @param second второе значение
        @param op бинарная операция
        */
        template <class Integer, class BinaryOperation>
        constexpr fibonacci_sequence<Integer, FunctionType<BinaryOperation>>
        operator()(Integer first, Integer second, BinaryOperation op) const
        {
            using F = FunctionType<BinaryOperation>;
            using Result = fibonacci_sequence<Integer, F>;
            return Result{std::move(first), std::move(second), std::move(op)};
        }
    };

    namespace
    {
        /** @brief Функциональный объект для создания последовательности чисел
        (типа) Фибоначчи.
        */
        constexpr auto const & make_fibonacci_sequence
            = odr_const<make_fibonacci_sequence_fn>;
    }
}
// namespace ural

#endif
// Z_URAL_MATH_FIBONACCI_HPP_INCLUDED
