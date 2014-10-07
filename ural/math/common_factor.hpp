#ifndef Z_URAL_MATH_COMMON_FACTOR_HPP_INCLUDED
#define Z_URAL_MATH_COMMON_FACTOR_HPP_INCLUDED

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

/** @file ural/math/common_factor.hpp
 @brief Наибольший общий делитель и наименьшее общее кратное.
*/

#include <cmath>
#include <utility>

namespace ural
{
    /** @brief Абсолютное значение
    @param x аргумент
    @return <tt> (x < IntegerType(0)) ? -std::move(x) : x </tt>
    */
    template <class IntegerType>
    constexpr IntegerType absolute_value(IntegerType x)
    {
        return (x < IntegerType(0)) ? -std::move(x) : x;
    }

    /** @brief Функциональный объект, вычисляющий наибольший общий делитель
    @tparam IntegerType целочисленный тип
    */
    template <typename IntegerType>
    class gcd_evaluator
    {
    private:
        constexpr IntegerType euclidean(IntegerType x, IntegerType y) const
        {
            return y == IntegerType(0) ? x : euclidean(y, x % y);
        }

    public:
        /** @brief Вычисление НОД
        @param x первый аргумент
        @param y второй аргумент
        @return НОД чисел @c x и @c y
        */
        constexpr IntegerType operator()(IntegerType x, IntegerType y) const
        {
            return absolute_value(euclidean(x, y));
        }
    };

    /** @brief Функциональный объект, вычисляющий наименьшее общее кратное
    @tparam IntegerType целочисленный тип
    */
    template <typename IntegerType>
    class lcm_evaluator
    {
    private:
        constexpr IntegerType impl(IntegerType x, IntegerType y) const
        {
            return (x == y) ? x : x / gcd_evaluator<IntegerType>{}(x, y) * y;
        }

    public:
        /** @brief Вычисление НОК
        @param x первый аргумент
        @param y второй аргумент
        @return НОК чисел @c x и @c y
        */
        constexpr IntegerType operator()(IntegerType x, IntegerType y) const
        {
            return absolute_value(impl(x, y));
        }
    };

    /** @brief Наибольший общий делитель
    @param a первый аргумент
    @param b второй аргумент
    @return Наибольший общий делитель чисел @c a и @c b.
    */
    template <typename IntegerType>
    constexpr IntegerType gcd(IntegerType const &a, IntegerType const &b)
    {
        return gcd_evaluator<IntegerType>{}(a, b);
    }

    /** @brief Наименьшее общее кратное
    @param a первый аргумент
    @param b второй аргумент
    @return Наименьшее общее кратное чисел @c a и @c b.
    */
    template <typename IntegerType>
    constexpr IntegerType lcm(IntegerType const &a, IntegerType const &b)
    {
        return lcm_evaluator<IntegerType>{}(a, b);
    }

    /// @brief Целочисленный тип, используемый в статических НОД и НОК
    typedef int static_gcd_type;

    /** @brief Класс-характеристика для вычисления НОД
    @tparam Value1 первый операнд
    @tparam Value2 второй операнд
    */
    template <static_gcd_type Value1, static_gcd_type Value2>
    struct static_gcd
     : std::integral_constant<static_gcd_type, gcd(Value1, Value2)>
    {};

    /** @brief Класс-характеристика для вычисления НОК
    @tparam Value1 первый операнд
    @tparam Value2 второй операнд
    */
    template <static_gcd_type Value1, static_gcd_type Value2>
    struct static_lcm
     : std::integral_constant<static_gcd_type, lcm(Value1, Value2)>
    {};
}
// namespace ural

#endif
// Z_URAL_MATH_COMMON_FACTOR_HPP_INCLUDED
