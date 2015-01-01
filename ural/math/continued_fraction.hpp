#ifndef Z_URAL_MATH_CONTINUED_FRACTION_H_INCLUDED
#define Z_URAL_MATH_CONTINUED_FRACTION_H_INCLUDED

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

/** @file ural/math/continued_fraction.hpp
 @brief Классы и функции, связанные с цепными дробями.
*/

#include <ural/math/rational.hpp>
#include <cassert>

namespace ural
{
    template <class IntType>
    class convergent
    {
    public:
        typedef ural::rational<IntType> result_type;

        convergent()
         : h_1{1}
         , k_1{0}
         , h_2{0}
         , k_2{1}
        {}

        result_type value() const
        {
            assert(k_1 != 0);
            return result_type{h_1, k_1};
        }

        void update(IntType const & a)
        {
            auto h = a * h_1 + h_2;
            auto k = a * k_1 + k_2;

            h_2 = std::move(h_1);
            k_2 = std::move(k_1);
            h_1 = std::move(h);
            k_1 = std::move(k);
        }

    private:
        IntType h_1;
        IntType k_1;
        IntType h_2;
        IntType k_2;
    };
}
// namespace ural

#endif // Z_URAL_MATH_CONTINUED_FRACTION_H_INCLUDED