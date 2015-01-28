#ifndef Z_URAL_OPERATORS_HPP_INCLUDED
#define Z_URAL_OPERATORS_HPP_INCLUDED

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

/** @file ural/operators.hpp
 @brief Автоматизация определения операторов
*/

#include <ural/defs.hpp>

namespace ural
{
    /** @brief Оператор "равно" для пустых типов
    @return true
    */
    template <class T>
    constexpr typename std::enable_if<std::is_empty<T>::value, bool>::type
    operator==(T const &, T const &)
    {
        return true;
    }

    /** @brief Естественное определение оператора "не равно"
    @param x левый операнд
    @param y правый операнд
    @return <tt> !(x == y) </tt>
    */
    template <class T1, class T2>
    constexpr bool operator!=(T1 const & x, T2 const & y)
    {
        return !(x == y);
    }

    /** @brief Естественное определение оператора "больше"
    @param x левый операнд
    @param y правый операнд
    @return <tt> y < x </tt>
    */
    template <class T1, class T2>
    constexpr bool operator>(T1 const & x, T2 const & y)
    {
        return y < x;
    }

    /** @brief Естественное определение оператора "меньше либо равно"
    @param x левый операнд
    @param y правый операнд
    @return <tt> !(x > y) </tt>
    */
    template <class T1, class T2>
    constexpr bool operator<=(T1 const & x, T2 const & y)
    {
        return !(y < x);
    }

    /** @brief Естественное определение оператора "больше либо равно"
    @param x левый операнд
    @param y правый операнд
    @return <tt> !(x < y) </tt>
    */
    template <class T1, class T2>
    constexpr bool operator>=(T1 const & x, T2 const & y)
    {
        return !(x < y);
    }

    /** Обобщённая реализация пост-инкремента. Применяет оператор пре-инкремента
    и возвращает значение, которое объект имел до этого.
    @brief Обобщённая реализация пост-инкремента
    @param x аргмент
    @return Значение, которое @c x имел до вызова
    @todo Добавить требование наличия пре-инкремента в enable_if
    */
    template <class Incrementable>
    typename std::enable_if<std::is_copy_constructible<Incrementable>::value, Incrementable>::type
    operator++(Incrementable & x, int)
    {
        auto tmp = x;
        ++ x;
        return tmp;
    }
}
// namespace ural

#endif
// Z_URAL_OPERATORS_HPP_INCLUDED
