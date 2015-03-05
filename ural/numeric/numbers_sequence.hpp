#ifndef Z_URAL_NUMERIC_NUMBERS_RANGE_HPP_INCLUDED
#define Z_URAL_NUMERIC_NUMBERS_RANGE_HPP_INCLUDED

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

/** @file ural/numeric/numbers_sequence.hpp
 @brief Последовательность чисел
*/

#include <ural/tuple.hpp>
#include <ural/sequence/base.hpp>

namespace ural
{
    /** @brief Последовательность чисел, заданная наименьшим и наибольшим
    значением
    @tparam Number тип числа
    @tparam D тип приращения
    @todo Возможность задавать разные типы наибольшего и наименьшего значений?
    */
    template <class Number, class D = use_default>
    class numbers_sequence
     : public sequence_base<numbers_sequence<Number, D>>
    {
    public:
        /// @brief Тип значения
        typedef Number value_type;

        /// @brief Тип ссылки
        typedef value_type const & reference;

        /// @brief Тип указателя
        typedef value_type const & pointer;

        /// @brief Тип расстояния
        typedef typename default_helper<D, Number>::type distance_type;

        /** @brief Категория обхода
        @todo Усилить категорию обхода?
        */
        typedef single_pass_traversal_tag traversal_tag;

        // Создание, уничтожение, копирование
        /** @brief Конструктор
        @param x_min наименьшее значение
        @param x_max наибольшее значение
        @post <tt> this->front() == x_min </tt>
        */
        numbers_sequence(Number x_min, Number x_max)
         : data_(std::move(x_min), std::move(x_max), distance_type{1})
        {}

        /** @brief Конструктор
        @param x_min наименьшее значение
        @param x_max наибольшее значение
        @param step шаг
        @post <tt> this->front() == x_min </tt>
        */
        numbers_sequence(Number x_min, Number x_max, distance_type step)
         : data_{std::move(x_min), std::move(x_max), std::move(step)}
        {}

        // Однопроходная последовательность
        /** @brief Проверка исчерпания последовательности
        @return @b true, если последовательность исчерпана, иначе -- @b false
        */
        bool operator!() const
        {
            return data_[ural::_1] >= data_[ural::_2];
        }

        /** @brief Текущий элемент
        @return Ссылка на текущий элемент
        */
        reference front() const
        {
            return data_[ural::_1];
        }

        /// @brief Переход к следующему элементу
        void pop_front()
        {
            data_[ural::_1] += data_[ural::_3];
        }

    private:
        ural::tuple<Number, Number, distance_type> data_;
    };

    /** @brief Создание последовательности чисел
    @param x_min наименьшее значение
    @param x_max наибольшее значение
    @return <tt> numbers_sequence<T>(std::move(x_min), std::move(x_max)) </tt>,
    где @c T --- <tt> typename std::common_type<T1, T2>::type </tt>
    */
    template <class T1, class T2>
    numbers_sequence<typename std::common_type<T1, T2>::type>
    numbers(T1 x_min, T2 x_max)
    {
        typedef typename std::common_type<T1, T2>::type Number;

        return numbers_sequence<Number>(std::move(x_min), std::move(x_max));
    }

    /** @brief Создание последовательности чисел
    @param x_min наименьшее значение
    @param x_max наибольшее значение
    @param step шаг
    @return <tt> numbers_sequence<T, D>(x_min, x_max, step) </tt>,
    где @c T --- <tt> typename std::common_type<T1, T2>::type </tt>
    */
    template <class T1, class T2, class D>
    numbers_sequence<typename std::common_type<T1, T2>::type, D>
    numbers(T1 x_min, T2 x_max, D step)
    {
        typedef typename std::common_type<T1, T2>::type Number;
        typedef numbers_sequence<Number, D> Seq;

        return Seq{std::move(x_min), std::move(x_max), std::move(step)};
    }

    /** @brief Создание последовательности индексов массива
    @param v массив
    @return <tt> ural::numbers(0, v.size()) </tt>
    */
    template <class Vector>
    auto indices_of(Vector const & v)
    -> decltype(ural::numbers(0, v.size()))
    {
        return ural::numbers(0, v.size());
    }
}
// namespace ural

#endif
// Z_URAL_NUMERIC_NUMBERS_RANGE_HPP_INCLUDED
