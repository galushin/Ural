#ifndef Z_URAL_MATH_HPP_INCLUDED
#define Z_URAL_MATH_HPP_INCLUDED

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

/** @file ural/math.hpp
 @brief Математические функции и типы данных
*/

#include <ural/meta/hierarchy.hpp>
#include <ural/meta/list.hpp>

#include <ural/functional/cpp_operators.hpp>

#include <cassert>
#include <cstddef>
#include <cmath>
#include <cstdlib>

#include <utility>
#include <stdexcept>

namespace ural
{
    /** @brief Тип функционального объекта для вычисления квадрата
    (второй степени)
    */
    class square_fn
    {
    public:
        /** @brief Функция вычисления квадрата
        @param x аргумент
        @return <tt> x * x</tt>
        */
        template <class T>
        constexpr T operator()(T const & x) const
        {
            return x * x;
        }

        /** @brief Функция вычисления квадрата
        @param x аргумент
        @param op ассоциативная бинарная операция, используемая в качестве
            умножения
        @return <tt> op(x, x) </tt>
        */
        template <class T, class BinOp>
        constexpr T operator()(T const & x, BinOp op) const
        {
            return op(x, x);
        }
    };

    auto constexpr square = square_fn{};

    /// @brief Тип функционального объекта для вычисления куба (третьей степени)
    class cube_fn
    {
    public:
        /** @brief Функция вычисления куба числа
        @param x значение, для которого вычисляется куб
        @return <tt> ural::cube(x, ural::multiplies<>{}) </tt>
        */
        template <class T>
        constexpr T operator()(T const & x) const
        {
            return (*this)(x, ural::multiplies<>{});
        }

        /** @brief Функция вычисления обобщённого куба числа
        @param x значение, для которого вычисляется куб
        @param op операция, используемая в качестве умножения
        @return <tt> op(ural::square(x, op), x) </tt>
        */
        template <class T, class BinOp>
        constexpr T operator()(T const & x, BinOp op) const
        {
            return op(ural::square_fn{}(x, op), x);
        }
    };

    auto constexpr cube = cube_fn{};

    /** @brief Класс-характеристика единичного элемента относительно операции
    @tparam T тип элемента
    @tparam BinaryOperation тип функционального объекта
    */
    template <class T, class BinaryOperation>
    struct unit_element_traits;

    /** @brief Нейтральный элемент относительно умножения
    @tparam T тип значений
    */
    template <class T>
    struct unit_element_traits<T, ural::multiplies<>>
    {
    public:
        /** @brief Функция создания
        @return <tt> T{1} </tt>
        */
        constexpr static T make(ural::multiplies<> const &)
        {
            return T{1};
        }
    };

    /** @brief Нейтральный элемент относительно сложения
    @tparam T тип значений
    */
    template <class T>
    struct unit_element_traits<T, ural::plus<>>
    {
    public:
        /** @brief Функция создания
        @return <tt> T{0} </tt>
        */
        constexpr static T make(ural::plus<> const &)
        {
            return T{0};
        }
    };

    /** @brief Функция создания единичного элемента операции
    @tparam T тип элементов
    @param f функциональный объект
    */
    template <class T, class BinaryOperation>
    constexpr T make_unit_element(BinaryOperation const & f)
    {
        return unit_element_traits<T, BinaryOperation>::make(f);
    }

    /** @brief Тип бинарного функционального объекта для вычисления
    неотрицательных степеней
    */
    class natural_power_f
    {
    public:
        /** @brief Возведение числа в натуральную степень
        @param x число
        @param n степень
        @pre <tt> n >= 0 </tt>
        @return @c x в степени @c n
        */
        template <class T>
        constexpr T operator()(T const & x, size_t n) const
        {
            return (*this)(x, n, ural::multiplies<>{});
        }

        /** @brief Возведение числа в натуральную степень
        @param x число
        @param n степень
        @param op ассоциативная бинарная операция, используемая в качестве
        умножения
        @pre <tt> n >= 0 </tt>
        @return @c x в степени @c n
        */
        template <class T, class AssocBinOp>
        constexpr T operator()(T const & x, size_t n, AssocBinOp op) const
        {
            return (*this)(x, n, std::move(op), make_unit_element<T>(op));
        }

        /** @brief Возведение числа в натуральную степень
        @param x число
        @param n степень
        @param op ассоциативная бинарная операция, используемая в качестве
        умножения
        @param unit единичный элемент
        @pre <tt> n >= 0 </tt>
        @return Если <tt> n > 0 </tt> @c x в степени @c n, иначе --- @c unit
        */
        template <class T, class AssocBinOp>
        constexpr T operator()(T const & x, size_t n, AssocBinOp op, T const & unit) const
        {
            return this->compute(x, n, op, unit);
        }

    private:
        template <class T, class AssocBinOp>
        constexpr T adjust(T value, T const & x, bool is_odd, AssocBinOp op) const
        {
            return is_odd ? op(std::move(value), x) : value;
        }

        template <class T, class AssocBinOp>
        constexpr T compute(T const & x, size_t n, AssocBinOp op) const
        {
            return (n == 1)
                    ? x
                    : adjust(ural::square(this->compute(x, n / 2, op), op), x, n % 2 != 0, op);
        }

        template <class T, class AssocBinOp>
        constexpr T compute(T const & x, size_t n, AssocBinOp op, T const & unit) const
        {
            return (n == 0) ? unit : compute(x, enforce_positive(n), op);
        }

        static constexpr size_t enforce_positive(size_t n)
        {
            return (n > 0) ? n : throw std::logic_error{"zero power"};
        }
    };

    auto constexpr natural_power = natural_power_f{};

    // Абсолютное значение
    namespace details
    {
        using std::abs;

        /// @brief Тип функционального объекта для вычисления модуля
        class abs_fn
        {
        public:
            /** @brief Оператор вызова функции
            @param x аргумент
            @return <tt> abs(x) </tt>
            */
            template <class T>
            auto operator()(T const & x) const
            -> decltype(abs(x))
            {
                return abs(x);
            }
        };
    }

    using details::abs_fn;

    /** @brief Создание функционального объекта для вычисления модуля
    @return <tt> details::abs_fn{} </tt>
    */
    constexpr abs_fn abs()
    {
        return abs_fn{};
    }

    /** @brief Класс-характеристика, определяющая, являются ли все типы пачки
    целочисленными.
    @tparam Ts пачка типов
    */
    template <class... Ts>
    struct are_integral
     : meta::all_of<typename meta::make_list<Ts...>::type, std::is_integral>
    {};

    /** @brief Класс-характеристика для определения типа среднего значения
    @tparam T тип элементов выборки
    @tparam N тип для представления количества элементов
    @tparam Enabler вспомогательный тип для специализации на основе
    <tt> std::enable_if </tt>
    */
    template <class T, class N, class Enabler = void>
    struct average_type
     : declare_type<decltype(std::declval<T>() / std::declval<N>())>
    {};

    /** @brief Специализация для целочисленных выборок
    @tparam T тип элементов выборки
    @tparam N тип для представления количества элементов
    */
    template <class T, class N>
    struct average_type<T, N, typename std::enable_if<are_integral<T, N>::value>::type>
     : declare_type<double>
    {};

    /** @brief Преобразование в беззнаковое число
    @param x преобразуемое число
    @pre @c x должно быть представимо в виде соответствующего знакового типа
    @return <tt> typename std::make_signed<T>::type(std::move(x)) </tt>
    @todo Настройка способа проверки корректности - стратегии
    */
    template <class T>
    typename std::make_signed<T>::type
    to_signed(T x)
    {
        // @todo Проверка корректности
        return typename std::make_signed<T>::type(std::move(x));
    }

    /** @brief Преобразование в беззнаковое число
    @param x преобразуемое число
    @pre <tt> x >= 0 </tt>
    @return <tt> typename std::make_unsigned<T>::type(std::move(x)) </tt>
    @todo Настройка способа проверки корректности - стратегии
    */
    template <class T>
    typename std::make_unsigned<T>::type
    to_unsigned(T x)
    {
        assert(x >= 0);
        return typename std::make_unsigned<T>::type(std::move(x));
    }
}
// namespace ural

#endif
// Z_URAL_MATH_HPP_INCLUDED
