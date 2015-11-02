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

#include <ural/sequence/base.hpp>

namespace ural
{
    /**
    @brief Последовательность чисел Фибоначчи
    @tparam Integer целочисленный тип
    @todo Обобщить: категория обхода, операция, начальные значения
    @todo Проверка концепций
    */
    template <class Integer>
    class fibonacci_sequence
     : public ural::sequence_base<fibonacci_sequence<Integer>>
    {
    public:
        /// @brief Категория обхода
        using traversal_tag = ural::single_pass_traversal_tag;

        /// @brief Тип значения
        using value_type = Integer;

        /// @brief Тип ссылки
        using reference = value_type const &;

        /// @brief Тип указателя
        using pointer = value_type const *;

        /// @brief Тип расстояния
        using distance_type = Integer;

        // Конструкторы и свойства
        /** @brief Конструктор
        @post <tt> this->front() == Integer(1) </tt>
        @post <tt> next(*this).front() == Integer(1) </tt>
        */
        fibonacci_sequence()
         : cur_(1)
         , next_(1)
        {}

        // Прямая последовательность
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
        reference front() const
        {
            return this->cur_;
        }

        /// @brief Переход к следующему значению
        void pop_front()
        {
            auto new_value = cur_ + next_;
            cur_ = std::move(next_);
            next_ = std::move(new_value);
        }

    private:
        Integer cur_;
        Integer next_;
    };
}
// namespace ural

#endif
// Z_URAL_MATH_FIBONACCI_HPP_INCLUDED
