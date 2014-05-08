#ifndef Z_URAL_FUNCTIONAL_ADJOIN_HPP_INCLUDED
#define Z_URAL_FUNCTIONAL_ADJOIN_HPP_INCLUDED

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

/** @file ural/functional/adjoin.hpp
 @brief Объединение нескольких функциональных объектов в один, возвращающий
 кортеж
*/

#include <ural/tuple.hpp>
#include <ural/utility.hpp>
#include <ural/functional/make_functor.hpp>

namespace ural
{
    /** @brief Функциональный объект, объединяющий несколько функциональных
    объектов и возвращающий кортеж их результатов
    @tparam Fs типы базовых функциональных объектов
    */
    template <class... Fs>
    class adjoin_functor_type
    {
    public:
        /// @brief Конструктор без параметров
        constexpr adjoin_functor_type() = default;

        /** @brief Конструктор
        @param fs базовые функциональные объекты
        */
        constexpr explicit adjoin_functor_type(Fs... fs)
         : functors_{std::move(fs)...}
        {}

        /** @brief Оператор применения функционального объекта
        @param args аргументы
        */
        template <class... Args>
        constexpr auto operator()(Args && ... args) const
        -> ural::tuple<decltype(std::declval<Fs>()(args...))...>
        {
            typedef ural::tuple<decltype(std::declval<Fs>()(args...))...> R;
            return call_impl(declare_type<R>{},
                             ural::forward_as_tuple(std::forward<Args>(args)...),
                             ural::index_sequence_for<Fs...>{});
        }

    private:
        template <class R, class Tuple, size_t... Is>
        constexpr
        R call_impl(declare_type<R>, Tuple && t,
                    integer_sequence<size_t, Is...>) const
        {
            return R(apply(std::get<Is>(functors_), std::forward<Tuple>(t))...);
        }

    private:
        ural::tuple<Fs...> functors_;
    };

    /** @brief Создание функционального объекта, формирующего кортеж результатов
    нескольких функциональных объектов
    @param fs базовые функциональные объекты
    */
    template <class... Fs>
    constexpr auto adjoin_functors(Fs... fs)
    -> adjoin_functor_type<decltype(ural::make_functor(std::move(fs)))...>
    {
        typedef adjoin_functor_type<decltype(ural::make_functor(std::move(fs)))...>
            Functor;
        return Functor{ural::make_functor(std::move(fs))...};
    }
}
// namespace ural

#endif
// Z_URAL_FUNCTIONAL_ADJOIN_HPP_INCLUDED
