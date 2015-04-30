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

#include <ural/utility.hpp>
#include <boost/compressed_pair.hpp>

namespace ural
{
    // Композиция функциональных объектов
    /** @brief Композиция функциональных объектов
    @tparam UnaryFunction тип унарного функционального объекта, вычисляющего
    окончательное значение
    @tparam Function тип функционального объекта, применяемого непосредственно к
    аргументам
    @todo Проверка, что класс пустой, если пусты оба шаблонных параметра
    */
    template <class UnaryFunction, class Function>
    class compose_function
    {
    friend bool operator==(compose_function const & x, compose_function const & y)
    {
        return x.first_function() == y.first_function()
                && x.second_function() == y.second_function();
    }
    public:
        /** @brief тип унарного функционального объекта, вычисляющего
        окончательное значение
        */
        typedef decltype(ural::make_callable(std::declval<UnaryFunction>()))
            first_function_type;

        /** @brief тип функционального объекта, применяемого непосредственно к
        аргументам
        */
        typedef decltype(ural::make_callable(std::declval<Function>()))
            second_function_type;

    private:
        typedef boost::compressed_pair<first_function_type, second_function_type>
            Base;

    public:
        /** @brief Конструктор
        @param f1 функциональный объект, вычисляющий окончательный результат
        @param f2 функциональный объект, применяемый к аргументам
        @post <tt> this->first_function() == f1 </tt>
        @post <tt> this->second_function() == f2 </tt>
        */
        explicit compose_function(UnaryFunction f1, Function f2)
         : base_{std::move(f1), std::move(f2)}
        {}

        /** @brief Первый функциональный объект
        @return Первый функциональный объект
        */
        first_function_type const & first_function() const
        {
            return ::ural::get(base_, ural::_1);
        }

        /** @brief Второй функциональный объект
        @return Второй функциональный объект
        */
        second_function_type const & second_function() const
        {
            return ::ural::get(base_, ural::_2);
        }

        /** @brief Применение функционального объекта
        @param args аргументы
        */
        template <class... Args>
        constexpr auto operator()(Args && ... args) const
        -> decltype(std::declval<first_function_type>()(std::declval<second_function_type>()(std::forward<Args>(args)...)))
        {
            return this->first_function()(this->second_function()(std::forward<Args>(args)...));
        }

    private:
        Base base_;
    };
}
// namespace ural

#endif
// Z_URAL_FUNCTIONAL_COMPOSE_HPP_INCLUDED
