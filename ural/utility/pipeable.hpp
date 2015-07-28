#ifndef Z_URAL_UTILITY_PIPEABLE_HPP_INCLUDED
#define Z_URAL_UTILITY_PIPEABLE_HPP_INCLUDED

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

/** @file ural/utility/pipeable.hpp
 @brief Вспомогательные возможности для создания последовательностей в
 конвейерном стиле
*/

#include <ural/functional.hpp>
#include <ural/concepts.hpp>
#include <ural/utility.hpp>

#include <utility>
#include <tuple>

namespace ural
{
    /** @brief Элемент "конвейера"
    @param Factory тип функции создания
    @param Args список аргументов функции
    */
    template <class Factory, class... Args>
    struct pipeable
    {
    public:
        /** @brief Оператор создания адаптора
        @param seq исходная последовательность
        @param pipe объект, содержащий аргументы для создания адаптора
        @return <tt> ::ural::apply(f, std::move(pipe.args)) </tt>, где
        @c f --- <tt> ::ural::curry(Factory{}, std::forward<Sequence>(seq))</tt>
        */
        template <class Sequence>
        friend auto operator|(Sequence && seq, pipeable<Factory, Args...> pipe)
        -> decltype(Factory{}(std::forward<Sequence>(seq), std::declval<Args>()...))
        {
            auto f = ::ural::curry(Factory{}, std::forward<Sequence>(seq));
            return ::ural::apply(std::move(f), std::move(pipe.args));
        }

        static_assert(std::is_empty<Factory>::value, "Factory must be empty");

        /// @brief Кортеж аргументов для создания адаптора
        std::tuple<Args...> args;
    };

    /** @brief Вспомогательный класс для создания функциональных объектов,
    результаты которых можно использовать как этапы "конвейерного" создания
    последовательностей.
    @tparam Factory тип функционального объекта без состояния
    */
    template <class Factory>
    class pipeable_maker
    {
        static_assert(std::is_empty<Factory>::value, "Factory must be empty");

    public:
        /** @brief Оператор создания адаптора
        @param seq исходная последовательность
        @return <tt> ::ural::apply(f, std::move(pipe.args)) </tt>, где
        @c f --- <tt> ::ural::curry(Factory{}, std::forward<Sequence>(seq))</tt>
        */
        template <class Sequence>
        friend auto operator|(Sequence && seq, pipeable_maker<Factory>)
        -> decltype(Factory{}(std::forward<Sequence>(seq)))
        {
            return Factory{}(std::forward<Sequence>(seq));
        }

        /** @brief Оператор вызова функции
        @param args список аргументов
        */
        template <class... Args>
        pipeable<Factory, Args&&...>
        operator()(Args &&... args) const
        {
            return {std::forward_as_tuple(std::forward<Args>(args)...)};
        }
    };
}
// namespace ural

#endif
// Z_URAL_UTILITY_PIPEABLE_HPP_INCLUDED
