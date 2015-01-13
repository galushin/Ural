#ifndef Z_URAL_FORMAT_STREAM_TRAITS_HPP_INCLUDED
#define Z_URAL_FORMAT_STREAM_TRAITS_HPP_INCLUDED

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

/** @file ural/format/stream_traits.hpp
 @brief Класс-характеристика для потоков ввода и вывода
*/

#include <string>

namespace ural
{
    /** @brief Класс-характеристика для потоков ввода и вывода
    @tparam Stream поток ввода или вывода
    */
    template <class Stream>
    struct stream_traits
    {
        /// @brief Тип символов
        typedef typename Stream::char_type char_type;

        /// @brief Класс-характеристика
        typedef typename Stream::traits_type traits_type;

        /// @brief Соответствующий класс строк
        typedef typename std::basic_string<char_type, traits_type> string_type;
    };
}
// namespace ural

#endif
// Z_URAL_FORMAT_STREAM_TRAITS_HPP_INCLUDED
