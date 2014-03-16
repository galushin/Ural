#ifndef Z_URAL_TYPE_TRAITS_HPP_INCLUDED
#define Z_URAL_TYPE_TRAITS_HPP_INCLUDED

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

/** @file ural/type_traits.hpp
 @brief Характеристики типов
*/

#include <type_traits>

namespace ural
{
    template <class T>
    using decay_t = typename std::decay<T>::type;
}
// namespace ural

#endif
// Z_URAL_TYPE_TRAITS_HPP_INCLUDED
