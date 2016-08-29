#ifndef Z_URAL_SEQUENCE_GENERATOR_HPP_INCLUDED
#define Z_URAL_SEQUENCE_GENERATOR_HPP_INCLUDED

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

/** @file ural/sequence/generator.hpp
 @brief Последовательность значений генератора (функции без аргументов).
*/

#include <ural/functional.hpp>
#include <ural/sequence/base.hpp>

namespace ural
{
namespace experimental
{
    /** @brief Курсор последовательности на основе функции без аргументов
    @tparam Generator функция без аргументов
    @tparam D тип расстояния, по умолчанию используется <tt>std::ptrdiff_t</tt>
    */
    template <class Generator,
              class D = use_default>
    class generator_cursor
     : public cursor_base<generator_cursor<Generator, D>,
                            decltype(make_callable(std::declval<Generator>()))>
    {
    public:
        typedef decltype(make_callable(std::declval<Generator>()))
            function_type;

    private:
        typedef cursor_base<generator_cursor, function_type>
            Base_class;

        template <class T>
        static T make_value(T);

    public:
        // Типы
        /// @brief Тип ссылки
        typedef decltype(std::declval<function_type>()()) reference;

        /// @brief Тип значения
        typedef decltype(make_value(std::declval<reference>())) value_type;

        /// @brief Тип расстояния
        using distance_type = experimental::defaulted_type_t<D, std::ptrdiff_t>;

        /// @brief Категория курсора
        using cursor_tag = input_cursor_tag;

        /// @brief Тип указателя
        typedef typename std::remove_reference<reference>::type * pointer;

        // Конструктор
        /** @brief Конструктор
        @post <tt> this->function() == Generator{} </tt>
        */
        explicit generator_cursor()
         : Base_class()
        {}

        /** @brief Конструктор
        @param gen функция без аргументов
        @post <tt> this->function() == gen </tt>
        */
        explicit generator_cursor(Generator gen)
         : Base_class{std::move(gen)}
        {}

        generator_cursor(generator_cursor const &) = delete;
        generator_cursor(generator_cursor &&) = default;

        generator_cursor & operator=(generator_cursor const &) = delete;
        generator_cursor & operator=(generator_cursor &&) = default;

        // Однопроходный курсор
        /** @brief Провекра исчерпания последовательности
        @return @b false.
        */
        constexpr bool operator!() const
        {
            return false;
        }

        /** @brief Текущий элемент
        @return Текущий элемент
        */
        reference front() const
        {
            return this->function()();
        }

        /// @brief Переход к следующему элементу. Ничего не делает.
        void pop_front()
        {}

        // Свойства
        //@{
        /** @brief Используемый функциональный объект
        @return Используемый функциональный объект
        */
        function_type const & function() const &
        {
            return this->payload();
        }

        function_type && function() &&;
        //@}
    };

    /** @brief Создание курсора на основе генератора
    @param g функция без аргументов
    @return <tt> generator_cursor<Generator>{std::move(g)} </tt>
    */
    template <class Generator>
    generator_cursor<Generator>
    make_generator_cursor(Generator g)
    {
        return generator_cursor<Generator>{std::move(g)};
    }
}
// namespace experimental
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_GENERATOR_HPP_INCLUDED
