#ifndef Z_URAL_SEQUENCE_FUNCTION_OUTPUT_HPP_INCLUDED
#define Z_URAL_SEQUENCE_FUNCTION_OUTPUT_HPP_INCLUDED

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

/** @file ural/sequence/function_output.hpp
 @brief Выходная последовательность, передающая записываемое значение как
 аргумент заданной функции.
*/

#include <ural/functional/make_callable.hpp>
#include <ural/sequence/base.hpp>

namespace ural
{
    /** @brief Выходная последовательность, выполняющая вызов функции для
    каждого "записываемого элемента"
    @tparam UnaryFunction тип унарного функционального объекта
    @tparam D тип расстояния, по умолчанию используется <tt>std::ptrdiff_t</tt>
    */
    template <class UnaryFunction, class D = use_default>
    class function_output_sequence
     : public ural::sequence_base<function_output_sequence<UnaryFunction>,
                                   UnaryFunction>
    {
        typedef ural::sequence_base<function_output_sequence<UnaryFunction>,
                                    UnaryFunction> Base_class;
    public:
        // Типы
        /// @brief Категория обхода
        typedef single_pass_traversal_tag traversal_tag;

        /// @brief Категория итератора
        typedef std::output_iterator_tag iterator_category;

        /// @brief Тип значения
        typedef void value_type;

        /// @brief Тип расстояния
        typedef typename default_helper<D, std::ptrdiff_t>::type difference_type;

        /// @brief Тип указателя
        typedef void pointer;

        /// @brief Тип ссылки
        typedef void reference;

        // Конструкторы
        /** @brief Конструктор без аргументов
        @post <tt> this->function() == UnaryFunction{} </tt>
        */
        function_output_sequence()
         : Base_class()
        {}

        /** @brief Конструктор
        @param f функциональный объект
        @post <tt> this->function() == f </tt>
        */
        explicit function_output_sequence(UnaryFunction f)
         : Base_class{std::move(f)}
        {}

        // Свойства
        /** @brief Используемый функциональный объект
        @return Константная ссылка на используемый функциональный объект
        */
        UnaryFunction const & function() const
        {
            return this->payload();
        }

        // Однопроходная последовательность
        /** @brief Провекра исчерпания последовательности
        @return @b false.
        */
        bool operator!() const
        {
            return false;
        }

        /** @brief Текущий элемент
        @return <tt> *this </tt>
        */
        function_output_sequence & operator*()
        {
            return *this;
        }

        /// @brief Переход к следующему элементу. Ничего не делает.
        void pop_front()
        {}

        //@{
        /** @brief Оператор присваивания
        @return *this
        */
        function_output_sequence &
        operator=(function_output_sequence const &) = default;

        function_output_sequence &
        operator=(function_output_sequence &) = default;
        //@}

        /** @brief Передача значения функциональному объекту
        @param arg аргумент
        @return *this
        */
        template <class Arg>
        function_output_sequence & operator=(Arg && arg)
        {
            this->function_ref()(std::forward<Arg>(arg));
            return *this;
        }

    private:
        UnaryFunction & function_ref()
        {
            return this->payload();
        }
    };

    /** @brief Создание @c make_function_output_sequence
    @param f функциональный объект
    @return <tt> function_output_sequence<F>(make_callable(std::move(f))) </tt>,
    где @c F есть decltype(ural::make_callable(std::move(f))).
    */
    template <class UnaryFunction>
    auto make_function_output_sequence(UnaryFunction f)
    -> function_output_sequence<decltype(ural::make_callable(std::move(f)))>
    {
        typedef decltype(ural::make_callable(std::move(f))) Function;
        typedef function_output_sequence<Function> Result;
        return Result(ural::make_callable(std::move(f)));
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_FUNCTION_OUTPUT_HPP_INCLUDED
