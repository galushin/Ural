#ifndef Z_URAL_MATH_HPP_INCLUDED
#define Z_URAL_MATH_HPP_INCLUDED

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

/** @file ural/math.hpp
 @brief Математические функции и типы данных
*/

#include <ural/functional/cpp_operators.hpp>

#include <cassert>
#include <cstddef>

#include <utility>
#include <stdexcept>

namespace ural
{
    /** @brief Функция вычисления квадрата
    @param x аргумент
    @return <tt> x * x</tt>
    */
    template <class T>
    constexpr T square(T const & x)
    {
        return x * x;
    }

    /** @brief Функция вычисления квадрата
    @param x аргумент
    @param op ассоциативная бинарная операция, используемая в качестве
        умножения
    @return <tt> op(x, x) </tt>
    */
    template <class T, class BinOp>
    constexpr T square(T const & x, BinOp op)
    {
        return op(x, x);
    }

    class natural_power_f
    {
    public:
        /** @brief Возведение числа в натуральную степень
        @param x число
        @param n степень
        @return @c x в степени @c n
        */
        template <class T>
        constexpr T operator()(T const & x, size_t n) const
        {
            return n == 0
                   ? T{1}
                   : (*this)(x, enforce_positive(n), ural::multiplies<>{});
        }

        /** @brief Возведение числа в натуральную степень
        @param x число
        @param n степень
        @param op ассоциативная бинарная операция, используемая в качестве
        умножения
        @pre <tt> n > 0 </tt>
        @return @c x в степени @c n
        */
        template <class T, class AssocBinOp>
        constexpr T operator()(T const & x, size_t n, AssocBinOp op) const
        {
            return this->compute(x, enforce_positive(n), std::move(op));
        }

    private:
        template <class T, class AssocBinOp>
        constexpr T adjust(T value, T const & x, bool is_odd, AssocBinOp op) const
        {
            return is_odd ? op(std::move(value), x) : value;
        }

        template <class T, class AssocBinOp>
        constexpr T compute(T const & x, size_t n, AssocBinOp op) const
        {
            return (n == 1)
                    ? x
                    : adjust(ural::square((*this)(x, n / 2), op), x, n % 2 != 0, op);
        }

        static constexpr size_t enforce_positive(size_t n)
        {
            return (n > 0) ? n : throw std::logic_error{"zero power"};
        }
    };

    auto constexpr natural_power = natural_power_f{};
}
// namespace ural

#endif
// Z_URAL_MATH_HPP_INCLUDED
