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

#include <functional>
#include <utility>
#include <tuple>

namespace ural
{
    /** @brief Элемент "конвейера"
    @param Factory тип функции создания
    */
    template <class Factory>
    class pipeable
     : private FunctionType<Factory>
    {
        using Base = FunctionType<Factory>;
    public:
        /** @brief Оператор создания адаптора
        @param seq исходная последовательность
        @param pipe объект, содержащий аргументы для создания адаптора
        @return <tt> ::ural::apply(f, std::move(pipe.args)) </tt>, где
        @c f --- <tt> ::ural::curry(Factory{}, std::forward<Sequence>(seq))</tt>
        */
        template <class Sequence>
        friend auto operator|(Sequence && seq, pipeable<Factory> pipe)
        -> decltype(std::declval<Factory>()(std::forward<Sequence>(seq)))
        {
            return pipe.function()(std::forward<Sequence>(seq));
        }

        constexpr explicit pipeable() = default;

        constexpr explicit pipeable(Factory f)
         : Base(std::move(f))
        {}

    private:
        FunctionType<Factory> const & function() const
        {
            return *this;
        }
    };

    /** @brief Вспомогательный класс для создания функциональных объектов,
    результаты которых можно использовать как этапы "конвейерного" создания
    последовательностей.
    @tparam Factory тип функционального объекта без состояния
    */
    template <class Factory>
    class pipeable_maker
    {
        static_assert(std::is_empty<Factory>::value, "must be empty!");

        template <class... Args>
        struct pipeable_binder
        {
        public:
            explicit pipeable_binder(Args && ... args)
             : args_(std::forward_as_tuple(std::forward<Args>(args)...))
            {}

            template <class Sequence>
            auto operator()(Sequence && seq) const
            {
                auto f = ural::curry(Factory{}, std::forward<Sequence>(seq));
                return ural::apply(std::move(f), args_);
            }

        private:
            std::tuple<Args &&...> args_;
        };

        template <class... Args>
        static
        pipeable_binder<Args...>
        pipeable_bind(Args &&... args)
        {
            return pipeable_binder<Args...>(std::forward<Args>(args)...);
        }

    public:
        /** @brief Оператор вызова функции
        @param args список аргументов
        */
        template <class... Args>
        auto operator()(Args &&... args) const
        {
            // todo обезопасить за счёт запрета копирования возвращаемого значения
            auto f = this->pipeable_bind(std::forward<Args>(args)...);
            return pipeable<decltype(f)>(std::move(f));
        }
    };
}
// namespace ural

#endif
// Z_URAL_UTILITY_PIPEABLE_HPP_INCLUDED
