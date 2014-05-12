#ifndef Z_URAL_FUNCTIONAL_COMPOSE_HPP_INCLUDED
#define Z_URAL_FUNCTIONAL_COMPOSE_HPP_INCLUDED

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

/** @file ural/functional/compose.hpp
 @brief Функциональный объект, реализующий композицию двух функциональных
 объектов
*/

#include <boost/compressed_pair.hpp>

namespace ural
{
    // Композиция функциональных объектов
    /** @brief Композиция функциональных объектов
    @tparam UnaryFunctor тип унарного функционального объекта, вычисляющего
    окончательное значение
    @tparam Functor тип функционального объекта, применяемого непосредственно к
    аргументам
    */
    template <class UnaryFunctor, class Functor>
    class compose_functor
     : boost::compressed_pair<decltype(ural::make_functor(std::declval<UnaryFunctor>())),
                              decltype(ural::make_functor(std::declval<Functor>()))>
    {
    public:
        /** @brief тип унарного функционального объекта, вычисляющего
        окончательное значение
        */
        typedef decltype(ural::make_functor(std::declval<UnaryFunctor>()))
            first_functor_type;

        /** @brief тип функционального объекта, применяемого непосредственно к
        аргументам
        */
        typedef decltype(ural::make_functor(std::declval<Functor>()))
            second_functor_type;

    private:
        typedef boost::compressed_pair<first_functor_type, second_functor_type>
            Base;

    public:
        /** @brief Конструктор
        @param f1 функциональный объект, вычисляющий окончательный результат
        @param f2 функциональный объект, применяемый к аргументам
        @post <tt> this->first_functor() == f1 </tt>
        @post <tt> this->second_functor() == f2 </tt>
        */
        explicit compose_functor(UnaryFunctor f1, Functor f2)
         : Base{std::move(f1), std::move(f2)}
        {}

        /** @brief Первый функциональный объект
        @return Первый функциональный объект
        */
        first_functor_type const & first_functor() const
        {
            return Base::first();
        }

        /** @brief Второй функциональный объект
        @return Второй функциональный объект
        */
        second_functor_type const & second_functor() const
        {
            return Base::second();
        }

        /** @brief Применение функционального объекта
        @param args аргументы
        */
        template <class... Args>
        constexpr auto operator()(Args && ... args) const
        -> decltype(std::declval<first_functor_type>()(std::declval<second_functor_type>()(std::forward<Args>(args)...)))
        {
            return this->first_functor()(this->second_functor()(std::forward<Args>(args)...));
        }
    };
}
// namespace ural

#endif
// Z_URAL_FUNCTIONAL_COMPOSE_HPP_INCLUDED
