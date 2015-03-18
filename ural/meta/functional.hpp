#ifndef Z_URAL_META_FUNCTIONAL_HPP_INCLUDED
#define Z_URAL_META_FUNCTIONAL_HPP_INCLUDED

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

/** @file ural/meta/functional.hpp
 @brief Функции над типами
*/

#include <ural/meta/list.hpp>

namespace ural
{
namespace meta
{
    /** @brief Вспомогательный шаблон, позволяющий получать доступ к вложенному
    в класс @c F шаблону @c apply без использования дополнительного ключегового
    слова @c template
    @tparam F функция над типами
    @tparam Args типы-аргументы
    */
    template <class F, class... Args>
    struct apply
     : public F::template apply<Args...>
    {};

    /** @brief Преобразование шаблона класса в функцию над типами
    @tparam F шаблон класса
    */
    template <template <class...> class F>
    struct template_to_applied
    {
        /** @brief Вычисление функции над типами
        @tparam Args типы-аргументы
        */
        template <class... Args>
        struct apply
         : F<Args...>
        {};
    };

    template <class T>
    struct constant
    {
        template <class... Args>
        struct apply
         : declare_type<T>
        {};
    };

    template <size_t k>
    struct arg
    {
        template <class... Args>
        struct apply
         : meta::at<ural::typelist<Args...>, k>
        {};
    };

    template <class F, class... BArgs>
    struct bind
    {
        template <class... Args>
        struct apply
         : meta::apply<F, typename meta::apply<BArgs, Args...>::type...>
        {};
    };

    template <class If, class Then, class Else>
    struct if_then_else
    {
        template <class... Args>
        struct apply
        {
        private:
            using if_value = meta::apply<If, Args...>;
            using F = typename std::conditional<if_value::value, Then, Else>::type;

        public:
            using type = typename meta::apply<F, Args...>::type;
        };
    };

    /** @brief Отрицание предиката над типами
    @tparam F предикат над типами
    */
    template <class F>
    struct not_fn
    {
        /** @brief Вычисление функции над типами
        @tparam Ts типы-аргументы
        */
        template <class... Ts>
        struct apply
         : std::integral_constant<bool, !::ural::meta::apply<F, Ts...>::value>
        {};
    };

    /// @brief Предикат совпадения двух типов
    typedef template_to_applied<std::is_same> is_same;

    /// @brief Предикат несовпадения двух типов
    typedef not_fn<is_same> is_not_same;
}
// namespace meta
}
// namespace ural

#endif
// Z_URAL_META_FUNCTIONAL_HPP_INCLUDED
