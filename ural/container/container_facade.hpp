#ifndef Z_URAL_CONTAINER_FACADE_HPP_INCLUDED
#define Z_URAL_CONTAINER_FACADE_HPP_INCLUDED

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

/** @file ural/container/container_facade.hpp
 @brief Определение операций, неспецифичных для конкретных конейнеров
*/

#include <ural/algorithm.hpp>

namespace ural
{
namespace experimental
{
    /** @brief Базовый класс (CRTP), определяющий операции, не зависящие от
    реализации конкретных контейнеров
    @tparam Container тип контейнера
    */
    template <class Container>
    struct container_facade
    {
    protected:
        ~container_facade() = default;
        container_facade() = default;

    public:
        /** @brief Оператор "равно"
        @param x левый операнд
        @param y правый операнд
        @return <tt> ural::equal(x, y) </tt>
        */
        friend bool operator==(Container const & x, Container const & y)
        {
            return ural::equal(x, y);
        }

        /** @brief Оператор "меньше"
        @param x левый операнд
        @param y правый операнд
        @return <tt> ural::lexicographical_compare(x, y) </tt>
        */
        friend bool operator<(Container const & x, Container const & y)
        {
            return ural::lexicographical_compare(x, y);
        }

        friend void swap(Container & x, Container & y)
        {
            return x.swap(y);
        }
    };
}
// namespace experimental
}
// namespace ural

#endif
// Z_URAL_CONTAINER_FACADE_HPP_INCLUDED
