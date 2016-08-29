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
namespace experimental
{
    /** @brief Курсор последовательности чисел (типа) Фибоначчи
    @tparam Integer целочисленный тип
    @tparam BinaryOperation бинарная операция, используемая в качестве сложения
    @tparam CursorTag желаемая категория курсора
    */
    template <class Integer, class BinaryOperation = use_default,
              class CursorTag = use_default>
    class fibonacci_cursor
     : public cursor_base<fibonacci_cursor<Integer, BinaryOperation, CursorTag>>
    {
        using Inherited = cursor_base<fibonacci_cursor>;
    public:
        /// @brief Категория курсора
        using cursor_tag = defaulted_type_t<CursorTag, input_cursor_tag>;

        /// @brief Тип значения
        using value_type = Integer;

        /// @brief Тип ссылки
        using reference = value_type const &;

        /// @brief Тип указателя
        using pointer = value_type const *;

        /// @brief Тип расстояния
        using distance_type = Integer;

        /// @brief Тип операции
        using operation_type = defaulted_type_t<function_type_t<BinaryOperation>,
                                             ural::plus<Integer>>;

        // Конструкторы и свойства
        /** @brief Конструктор
        @post <tt> this->front() == Integer(1) </tt>
        @post <tt> next(*this).front() == Integer(1) </tt>
        */
        constexpr fibonacci_cursor()
         : fibonacci_cursor(Integer{1}, Integer{1}, operation_type{})
        {}

        /** @brief Конструктор
        @param first первое значение
        @param second второе значение
        @post <tt> this->front() == first </tt>
        @post <tt> next(*this).front() == second </tt>
        */
        constexpr fibonacci_cursor(value_type first, value_type second)
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
        constexpr fibonacci_cursor(value_type first, value_type second,
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
        operator==(fibonacci_cursor const & x, fibonacci_cursor const & y)
        {
            return x.data_ == y.data_;
        }

        // Однопроходый курсор
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
            return ural::experimental::get(this->data_[ural::_1]);
        }

        /// @brief Переход к следующему значению
        void pop_front()
        {
            auto new_value = this->operation()(std::move(ural::experimental::get(data_[ural::_1])),
                                               ural::experimental::get(data_[ural::_2]));
            data_[ural::_1] = std::move(ural::experimental::get(data_[ural::_2]));
            data_[ural::_2]= std::move(new_value);
        }

        // Прямой курсор
        /** @brief Передняя пройденная часть последовательности
        @return Передняя пройденная часть последовательности
        */
        delimit_cursor<fibonacci_cursor, value_type>
        traversed_front() const
        {
            return this->original() | ::ural::experimental::delimited(this->front());
        }

        /** @brief Исходная последовательность
        @return Исходная последовательность
        */
        fibonacci_cursor original() const
        {
            return fibonacci_cursor(data_[ural::_1].old_value(),
                                    data_[ural::_2].old_value(),
                                    this->operation());
        }

        void shrink_front()
        {
            data_[ural::_1].commit();
            data_[ural::_2].commit();
        }

    private:
        using Value = wrap_with_old_value_if_forward_t<cursor_tag, value_type>;

        tuple<Value, Value, operation_type> data_;
    };

    /** @brief Тип Функционального объекта для создания последовательности чисел
    (типа) Фибоначчи.
    */
    struct make_fibonacci_cursor_fn
    {
    public:
        /** @brief Конструктор
        @param first первое значение
        @param second второе значение
        */
        template <class Integer>
        constexpr fibonacci_cursor<Integer>
        operator()(Integer first, Integer second) const
        {
            using Result = fibonacci_cursor<Integer>;
            return Result{std::move(first), std::move(second)};
        }

        /** @brief Конструктор
        @param first первое значение
        @param second второе значение
        @param op бинарная операция
        */
        template <class Integer, class BinaryOperation>
        constexpr fibonacci_cursor<Integer, function_type_t<BinaryOperation>>
        operator()(Integer first, Integer second, BinaryOperation op) const
        {
            using F = function_type_t<BinaryOperation>;
            using Result = fibonacci_cursor<Integer, F>;
            return Result{std::move(first), std::move(second), std::move(op)};
        }
    };

    namespace
    {
        /** @brief Функциональный объект для создания курсора последовательности
        чисел (типа) Фибоначчи.
        */
        constexpr auto const & make_fibonacci_cursor
            = odr_const<make_fibonacci_cursor_fn>;
    }
}
// namespace experimental
}
// namespace ural

#endif
// Z_URAL_MATH_FIBONACCI_HPP_INCLUDED
