#ifndef Z_URAL_ABI_HPP_INCLUDED
#define Z_URAL_ABI_HPP_INCLUDED

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

/** @file ural/abi.hpp
 @brief Бинарный интерфейс компилятора
*/

#include <cxxabi.h>

namespace ural
{
namespace abi
{
    /** @brief Функция, возвращающая исходное ("неисковерканное" компилятором)
    имя, если это возможно
    @param name "исковерканное" компилятором имя
    @return исходное имя, из которого было получено @c name
    @note Работает только для GCC
    */
    inline std::string demangle_name(char const * name)
    {
        int status = 0;
        auto realname = ::__cxxabiv1::__cxa_demangle(name, nullptr, nullptr, &status);
        std::string result(realname);
        std::free(realname);
        return result;
    }
}
// namespace abi
}
// namespace ural

#endif
// Z_URAL_ABI_HPP_INCLUDED
