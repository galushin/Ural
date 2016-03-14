#ifndef Z_URAL_RANDOM_C_RAND_ENGINE_HPP_INCLUDED
#define Z_URAL_RANDOM_C_RAND_ENGINE_HPP_INCLUDED

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

/** @file ural/random/c_rand_engine.hpp
 @brief Генератор случайных чисел, основанный на использовании функции
    <tt> std::rand </tt>
*/

#include <ural/defs.hpp>
#include <cstdlib>

namespace ural
{
namespace experimental
{
    /** @brief Генератор случайных чисел, основанный на использовании функции
    <tt> std::rand </tt>.
    */
    class c_rand_engine
    {
    public:
        /// @brief Тип возвращаемого значения
        typedef typename std::make_unsigned<decltype(std::rand())>::type result_type;

        /** @brief Генерация значения
        @return <tt> std::rand() </tt>
        */
        result_type operator()() const
        {
            return std::rand();
        }

        /** @brief Наименьшее возвращаемое значение
        @return Наименьшее возвращаемое значение
        */
        static constexpr result_type min URAL_PREVENT_MACRO_SUBSTITUTION ()
        {
            return 0;
        }

        /** @brief Наибольшее возвращаемое значение
        @return Наибольшее возвращаемое значение
        */
        static constexpr result_type max URAL_PREVENT_MACRO_SUBSTITUTION ()
        {
            return RAND_MAX;
        }
    };
}
// namespace experimental
}
// namespace ural
#endif
// Z_URAL_RANDOM_C_RAND_ENGINE_HPP_INCLUDED
