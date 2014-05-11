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
    /** @brief "Отрицание" к <tt> std::enable_if </tt>
    @tparam Flag булево значение
    @tparam T тип
    */
    template <bool Flag, class T = void>
    struct disable_if
     : std::enable_if<!Flag, T>
    {};

    /** @brief Класс, содержащий @b typedef-объявление типа
    @tparam T тип
    */
    template <class T>
    struct declare_type
    {
        /// @brief Объявляемый тип
        typedef T type;
    };

    /** @brief Синоним для <tt> typename std::decay<T>::type </tt>
    @tparam T тип-аргумент
    */
    template <class T>
    using decay_t = typename std::decay<T>::type;

    /// @cond false
    namespace details
    {
        template <class T, class U>
        std::integral_constant<bool, false>
        is_assignable_helper(...);

        template <class T, class U>
        std::integral_constant<bool, true>
        is_assignable_helper(ural::declare_type<decltype(std::declval<T>() = std::declval<U>())> *);
    }
    // namespace details
    /// @endcond

    /** @brief Класс-характеристика, проверяющая, можно ли присвоить объекту
    типа @c T значение типа @c U.
    @tparam T тип левого операнда
    @tparam U тип правого операнда
    */
    template <class T, class U>
    struct is_assignable
     : decltype(::ural::details::is_assignable_helper<T, U>(nullptr))
    {
        /// @brief Тип-результат
        typedef decltype(::ural::details::is_assignable_helper<T, U>(nullptr)) type;

        /// @brief Значение-результат
        constexpr static auto value = type::value;
    };
}
// namespace ural

#endif
// Z_URAL_TYPE_TRAITS_HPP_INCLUDED
