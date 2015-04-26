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
    /** @brief Последовательность на основе функции без аргументов
    @tparam Generator функция без аргументов
    */
    template <class Generator>
    class generator_sequence
     : public sequence_base<generator_sequence<Generator>,
                            decltype(make_callable(std::declval<Generator>()))>
    {
    public:
        typedef decltype(make_callable(std::declval<Generator>()))
            functor_type;

    private:
        typedef sequence_base<generator_sequence, functor_type>
            Base_class;

        template <class T>
        static T make_value(T);

    public:
        // Типы
        /// @brief Тип ссылки
        typedef decltype(std::declval<functor_type>()()) reference;

        /// @brief Тип значения
        typedef decltype(make_value(std::declval<reference>())) value_type;

        /// @brief Тип расстояния
        typedef size_t distance_type;

        /// @brief Категория обхода
        typedef single_pass_traversal_tag traversal_tag;

        /// @brief Тип указателя
        typedef typename std::remove_reference<reference>::type * pointer;

        // Конструктор
        /** @brief Конструктор
        @param gen функция без аргументо
        @post <tt> this->functor() == gen </tt>
        */
        explicit generator_sequence(Generator gen)
         : Base_class{std::move(gen)}
        {}

        generator_sequence(generator_sequence const &) = delete;
        generator_sequence(generator_sequence &&) = default;

        generator_sequence & operator=(generator_sequence const &) = delete;
        generator_sequence & operator=(generator_sequence &&) = default;

        // Однопроходная последовательность
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
            return this->functor()();
        }

        /// @brief Переход к следующему элементу. Ничего не делает.
        void pop_front()
        {}

        // Свойства
        /** @brief Используемый функциональный объект
        @return Используемый функциональный объект
        */
        functor_type const & functor() const
        {
            return *this;
        }
    };

    /** @brief Создание последоательности на основе генератора
    @param g функция без аргументов
    @return <tt> generator_sequence<Generator>{std::move(g)} </tt>
    */
    template <class Generator>
    generator_sequence<Generator>
    make_generator_sequence(Generator g)
    {
        return generator_sequence<Generator>{std::move(g)};
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_GENERATOR_HPP_INCLUDED
