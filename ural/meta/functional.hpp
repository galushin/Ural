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

    template <class F, class... BArgs>
    struct bind
    {
        template <class... Args>
        struct apply
         : meta::apply<F, typename meta::apply<BArgs, Args...>::type...>
        {};
    };

    /** @brief Каррирование (частичное применение) функции над типами
    @tparam F исходная функция над типами
    @tparam A1 тип первого аргумента
    */
    template <class F, class A1>
    struct curry
    {
        template <class... Args>
        struct apply
         : meta::apply<F, A1, Args...>
        {};
    };

    template <template <class...> class Trait, class... BArgs>
    struct template_bind
     : bind<template_to_applied<Trait>, BArgs...>
    {};

    /** @brief Функция над типами, возвращающая один из своих аргументов
    @tparam k номер аргумента, который должен быть возвращён
    */
    template <size_t k>
    struct arg
    {
        template <class... Args>
        struct apply
         : meta::at<ural::typelist<Args...>, k>
        {};
    };

    /// @brief Функция над типами, возвращающая свой аргумент
    typedef meta::bind<template_to_applied<declare_type>, meta::arg<0>>
        identity;

    /** @brief Функция над типами, возвращающая одно и то же тип
    @tparam T возвращаемый тип
    */
    template <class T>
    struct constant
     : curry<identity, T>
    {};

    /** @brief Функция над типами, аналогичная условному оператору C++
    @tparam If функция над типами, задающая условие
    @tparam Then функция над типами, вызываемая, если @c If вернёт @b true
    @tparam Else функция над типами, вызываемая, если @c If вернёт @b false
    */
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

    template <class UnaryFunction, class Function>
    class composed
    {
    public:
        template <class... Args>
        struct apply
        {
        private:
            typedef typename meta::apply<Function, Args...>::type R1;

        public:
            typedef typename meta::apply<UnaryFunction, R1>::type type;
        };
    };

    struct compose
    {
        template <class F1, class... F_others>
        struct apply
        {
        private:
            typedef typename meta::compose::template apply<F_others...>::type
                F2;
            typedef meta::composed<F1, F2> F12;
            typedef std::is_same<F1, ural::meta::identity> F1_is_Id;
            typedef std::is_same<F2, ural::meta::identity> F2_is_Id;

        public:
           typedef typename std::conditional<F1_is_Id::value,
                                             ural::declare_type<F2>,
                                             std::conditional<F2_is_Id::value, F1, F12>>::type::type type;
        };

        template <class F1>
        struct apply<F1>
         : declare_type<F1>
        {};
    };
}
// namespace meta
}
// namespace ural

#endif
// Z_URAL_META_FUNCTIONAL_HPP_INCLUDED
