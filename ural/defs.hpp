#ifndef Z_URAL_DEFS_HPP_INCLUDED
#define Z_URAL_DEFS_HPP_INCLUDED

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

/** @file ural/defs.hpp
 @brief Определения основных типов, используемых библиотекой
*/

#include <ural/type_traits.hpp>

/// @brief Макрос для предотвращения макро-подстановки при объявлении функции
#define URAL_PREVENT_MACRO_SUBSTITUTION

namespace ural
{
    /** Пустой базовый класс. Используется, когда формально требуется указать
    базовый класс, но никакой базовый класс на самом деле не нужен.
    @brief Пустой базовый класс.
    */
    struct empty_type{};

    /** Класс-тэг обозначающий отсутствие типа. Используется, например, как
    пустой список типов
    @brief Класс-тэг обозначающий отсутствие типа. Исполь
    */
    class null_type{};

    /** @brief Тип-тэг, обозначающий, что нужно использовать значение по
    умолчанию
    */
    struct use_default{};

    /** @brief Тип-тэг, обозначающий, что тип параметров должен быть выведен
    по фактическим аргументам
    */
    struct auto_tag{};

    /** Если @c T совпадает с @c use_default, то результат @c --- Default,
    иначе --- @c T.
    @brief Класс-характеристика для определения значения шаблонного
    параметра
    @tparam T тип
    @tparam Default тип, используемый по-умолчанию
    */
    template <class T, class Default>
    struct default_helper
     : std::conditional<std::is_same<T, use_default>::value, Default, T>
    {};

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

    /** @brief Вспомогательный класс, которому можно присвоить значение
    заданного типа
    @tparam T тип присваемоего значения
    */
    template <class T>
    struct value_consumer
    {
        /// @brief Оператор присваивания
        void operator=(T const &)
        {}
    };
}
// namespace ural

#endif
// Z_URAL_DEFS_HPP_INCLUDED
