#ifndef Z_URAL_PLACEHOLDERS_HPP_INCLUDED
#define Z_URAL_PLACEHOLDERS_HPP_INCLUDED

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

/** @file ural/placeholders.hpp
 @brief Статические индексы
*/

namespace ural
{
    template <size_t N>
    struct placeholder{};

    /// @cond false
    constexpr placeholder<0> _1 {};
    constexpr placeholder<1> _2 {};
    constexpr placeholder<2> _3 {};
    constexpr placeholder<3> _4 {};
    constexpr placeholder<4> _5 {};
    /// @endcond
}
// namespace ural

#endif
// Z_URAL_PLACEHOLDERS_HPP_INCLUDED
