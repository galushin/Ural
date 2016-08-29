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
inline namespace v1
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

    // Трюк из N3911
    template <class... Args>
    struct declare_void
     : declare_type<void>
    {};

    template <class... Args>
    using void_t = typename declare_void<Args...>::type;

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

    template <class T>
    std::false_type
    has_pre_increment_helper(...);

    template <class T>
    std::true_type
    has_pre_increment_helper(ural::declare_type<decltype(++std::declval<T&>())> *);

    template <class T>
    std::false_type
    has_pre_decrement_helper(...);

    template <class T>
    std::true_type
    has_pre_decrement_helper(ural::declare_type<decltype(--std::declval<T&>())> *);
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

    /** @brief Класс-характеристика для определения типа объекта, из которого
    будет приозводится перемещение.
    @tparam T тип
    Если @c T -- тип ссылки, то результатом будет
    <tt> typename std::remove_reference<T>::type && </tt>, иначе -- сам тип T
    */
    template <class T>
    struct moved_type
     : std::conditional<std::is_reference<T>::value,
                        typename std::remove_reference<T>::type &&,
                        T>
    {};

    /** @brief Класс-характеристика "поддерживает пре-инкрмент"
    @tparam T тип, для которого проверяется свойство
    */
    template <class T>
    struct has_pre_increment
     : decltype(::ural::details::has_pre_increment_helper<T>(nullptr))
    {};

    /** @brief Класс-характеристика "поддерживает пре-декремент"
    @tparam T тип, для которого проверяется свойство
    */
    template <class T>
    struct has_pre_decrement
     : decltype(::ural::details::has_pre_decrement_helper<T>(nullptr))
    {};

    //@{
    /** @brief Класс-характеристика, преобразующая
    <tt> std::reference_wrapper<T> </tt> в <tt> T & </tt>, а остальные типы
    оставляющая без изменений
    @tparam T преобразуемый тип
    */
    template <class T>
    struct reference_wrapper_to_reference
     : declare_type<T>
    {};

    template <class T>
    struct reference_wrapper_to_reference<std::reference_wrapper<T>>
     : declare_type<T &>
    {};

    template <class T>
    struct reference_wrapper_to_reference<const std::reference_wrapper<T>>
     : declare_type<T &>
    {};
    //@}
}
// namespace v1

namespace experimental
{
    template <class T, class = void>
    struct is_range
     : std::false_type
    {};

    template <class T>
    struct is_range<T, void_t<decltype(std::declval<T&>().begin())>>
     : std::true_type
    {};

    template <class T, class = void>
    struct is_cv_invariant_range
     : std::false_type
    {};

    template <class T>
    struct is_cv_invariant_range<T, void_t<decltype(std::declval<T&>().begin()), decltype(std::declval<T const&>().begin())>>
     : std::is_same<decltype(std::declval<T&>().begin()), decltype(std::declval<T const&>().begin())>
    {};

    template <class T, class = void>
    struct has_allocator_type
     : std::false_type
    {};

    template <class T>
    struct has_allocator_type<T, ural::void_t<typename T::allocator_type>>
     : std::true_type
    {};

    template <class T>
    struct is_container
     : std::integral_constant<bool, is_range<T>::value && (has_allocator_type<T>::value || !is_cv_invariant_range<T>::value)>
    {};

    template <class T>
    constexpr bool const is_container_v = is_container<T>::value;
}
// namespace experimental
}
// namespace ural

#endif
// Z_URAL_TYPE_TRAITS_HPP_INCLUDED
