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
namespace details
{
    class square_functor
    {
    public:
        /** @brief Функция вычисления квадрата
        @param x аргумент
        @return <tt> x * x</tt>
        */
        template <class T>
        constexpr T operator()(T const & x) const
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
        constexpr T operator()(T const & x, BinOp op) const
        {
            return op(x, x);
        }
    };
}
// namespace details

    auto constexpr square = ::ural::details::square_functor{};

    template <class T, class BinOp>
    constexpr T cube(T const & x, BinOp op)
    {
        return op(ural::square(x, op), x);
    }

    template <class T>
    constexpr T cube(T const & x)
    {
        return ural::cube(x, ural::multiplies<>{});
    }

    /** @brief Класс-характеристика единичного элемента относительно операции
    @tparam T тип элемента
    @tparam F тип функционального объекта
    */
    template <class T, class F>
    struct unit_element_traits;

    template <class T>
    struct unit_element_traits<T, ural::multiplies<>>
    {
    public:
        constexpr static T make(ural::multiplies<> const &)
        {
            return T{1};
        }
    };

    template <class T>
    struct unit_element_traits<T, ural::plus<>>
    {
    public:
        constexpr static T make(ural::plus<> const &)
        {
            return T{0};
        }
    };

    template <class T, class F>
    constexpr T make_unit_element(F const & f)
    {
        return unit_element_traits<T, F>::make(f);
    }

    class natural_power_f
    {
    public:
        /** @brief Возведение числа в натуральную степень
        @param x число
        @param n степень
        @pre <tt> n >= 0 </tt>
        @return @c x в степени @c n
        */
        template <class T>
        constexpr T operator()(T const & x, size_t n) const
        {
            return (*this)(x, n, ural::multiplies<>{});
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
            return (*this)(x, n, std::move(op), make_unit_element<T>(op));
        }

        /** @brief Возведение числа в натуральную степень
        @param x число
        @param n степень
        @param op ассоциативная бинарная операция, используемая в качестве
        умножения
        @pre <tt> n >= 0 </tt>
        @return Если <tt> n > 0 </tt> @c x в степени @c n, иначе --- @c unit
        */
        template <class T, class AssocBinOp>
        constexpr T operator()(T const & x, size_t n, AssocBinOp op, T const & unit) const
        {
            return this->compute(x, n, op, unit);
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
                    : adjust(ural::square(this->compute(x, n / 2, op), op), x, n % 2 != 0, op);
        }

        template <class T, class AssocBinOp>
        constexpr T compute(T const & x, size_t n, AssocBinOp op, T const & unit) const
        {
            return (n == 0) ? unit : compute(x, enforce_positive(n), op);
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
