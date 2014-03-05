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

#include <cmath>

namespace ural
{
    template <class IntegerType>
    constexpr IntegerType absolute_value(IntegerType x)
    {
        return (x < IntegerType(0)) ? -std::move(x) : x;
    }


    template <typename IntegerType>
    class gcd_evaluator
    {
    private:
        constexpr IntegerType euclidean(IntegerType x, IntegerType y) const
        {
            return y == IntegerType(0) ? x : (*this)(y, x % y);
        }

    public:
        constexpr IntegerType operator()(IntegerType x, IntegerType y) const
        {
            return absolute_value(euclidean(x, y));
        }
    };

    template <typename IntegerType>
    class lcm_evaluator
    {
    private:
        constexpr IntegerType impl(IntegerType x, IntegerType y) const
        {
            return (x == y) ? x : x / gcd_evaluator<IntegerType>{}(x, y) * y;
        }

    public:
        constexpr IntegerType operator()(IntegerType x, IntegerType y) const
        {
            return absolute_value(impl(x, y));
        }
    };

    template <typename IntegerType>
    constexpr IntegerType gcd(IntegerType const &a, IntegerType const &b)
    {
        return gcd_evaluator<IntegerType>{}(a, b);
    }

    template <typename IntegerType>
    constexpr IntegerType lcm(IntegerType const &a, IntegerType const &b)
    {
        return lcm_evaluator<IntegerType>{}(a, b);
    }

    typedef int static_gcd_type;

    template <static_gcd_type Value1, static_gcd_type Value2>
    struct static_gcd
     : std::integral_constant<static_gcd_type, gcd(Value1, Value2)>
    {};

    template <static_gcd_type Value1, static_gcd_type Value2>
    struct static_lcm
     : std::integral_constant<static_gcd_type, lcm(Value1, Value2)>
    {};
}
// namespace ural

#endif
// Z_URAL_MATH_COMMON_FACTOR_HPP_INCLUDED
