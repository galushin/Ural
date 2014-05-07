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

#include <cassert>
#include <cstddef>

namespace ural
{
    /** @brief Функция вычисления квадрата
    @param x аргумент
    @return <tt> x * x</tt>
    */
    template <class T>
    T square(T const & x)
    {
        return x * x;
    }

    template <class T>
    T natural_power(T const & x, std::size_t n)
    {
        assert(n != 0);

        if(n == 1)
        {
            return x;
        }

        auto r = natural_power(x, n / 2);
        r *= r;
        if(n % 2 != 0)
        {
            r *= x;
        }
        return r;
    }
}
// namespace ural

#endif
// Z_URAL_MATH_HPP_INCLUDED
