#ifndef Z_URAL_ARCHETYPES_HPP_INCLUDED
#define Z_URAL_ARCHETYPES_HPP_INCLUDED

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

/** @file ural/archetypes.hpp
 @brief Архетипы
*/

#include <ural/sequence/base.hpp>

namespace ural
{

/** @namespace archetypes
 @brief Архетипы -- минималистичные реализации концепций
*/
namespace archetypes
{
    /** @brief Архетип функционального объекта (остаётся неопределённым)
    @tparam Signature сигнатура
    @todo переименовать в @c Callable
    */
    template <class Signature>
    class functor;

    /** @brief Архетип функционального объекта (общее определение)
    @tparam R типы аргументов
    @tparam Args типы аргументов
    */
    template <class R, class... Args>
    class functor<R(Args...)>
    {
    public:
        /** @brief Оператор применения функционального объекта
        @return <tt> R{} </tt>
        */
        R operator()(Args...) const
        {
            return instance();
        }

    private:
        static R instance()
        {
            static R r;
            return r;
        }
    };

    /** @brief Архетип функционального объекта без возвращаемого значения
    @tparam Args типы аргументов
    */
    template <class... Args>
    class functor<void(Args...)>
    {
    public:
        /// @brief Оператор применения функционального объекта
        void operator()(Args...) const
        {}
    };

    /** @brief Архетип последовательности ввода
    @tparam Value тип значения
    */
    template <class Value>
    class input_sequence
     : public ural::sequence_base<input_sequence<Value>>
    {
    public:
        /// @brief Тип значения
        typedef Value value_type;

        /// @brief Тип ссылки
        typedef value_type & reference;

        /// @brief Тип указателя
        typedef value_type * pointer;

        /// @brief Тип для представления расстояния
        typedef size_t distance_type;

        /// @brief Тэг категории обхода
        typedef single_pass_traversal_tag traversal_tag;

        /** @brief Проверка исчерпания последовательности
        @return @b true
        */
        bool operator!() const
        {
            return true;
        }

        /** @brief Первый элемент последовательности
        @return Ссылка на первый элемент последовательности
        */
        reference front() const
        {
            return this->value();
        }

        /// @brief Переход к следующему элементу
        void pop_front()
        {}

    private:
        static reference value()
        {
            static value_type x;
            return x;
        }
    };

    /// @brief Архетип генератора равномерно распределённых случайных чисел
    class URNG_archetype
    {
    public:
        /// @brief Тип результата
        typedef size_t result_type;

        /** @brief Оператор вызова функции
        @return <tt> this->min() </tt>
        */
        result_type operator()() const
        {
            return this->min();
        }

        /** @brief Наименьшее возможное значение
        @return Наименьшее возможное значение
        */
        constexpr static result_type min URAL_PREVENT_MACRO_SUBSTITUTION ()
        {
            return 0;
        }

        /** @brief Наибольшее возможное значение
        @return Наибольшее возможное значение
        */
        constexpr static result_type max URAL_PREVENT_MACRO_SUBSTITUTION  ()
        {
            return 1;
        }

    private:
    };
}
// namespace archetypes
}
// namespace ural

#endif
// Z_URAL_ARCHETYPES_HPP_INCLUDED
