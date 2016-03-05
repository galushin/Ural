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

#include <ural/defs.hpp>
#include <ural/math/common_factor.hpp>
#include <ural/meta/hierarchy.hpp>
#include <ural/meta/algo.hpp>

#include <ural/functional/cpp_operators.hpp>

#include <cassert>
#include <cstddef>
#include <cmath>
#include <cstdlib>

#include <utility>
#include <stdexcept>

/** @page function_objects_for_math_functions Функциональные объекты для математических функций

Функциональные объекты для математических функций

1. В специальном пространстве имён @c details импортируем функцию (из @c std),
которая не может быть найдена с помощью ADL, так как её аргументы могут быть
встроенными типами.
2. В пространстве имён @c details создаём класс функционального объекта,
выполняющий неквалифицированный вызов, чтобы использовать ADL.
3. В основное пространство имён библиотеки импортируем созданный класс
функциональных объектов.
4. Создаём функцию без аргументов с тем же именем, что "моделируемая" функция,
которая не получает параметров и возвращает нужный функциональный объект.

Например, функция для модуля может быть определена следующим образом:

@code
namespace ural
{
    namespace details
    {
        using std::abs;

        class abs_fn
        {
        public:
            template <class T>
            auto operator()(T const & x) const
            -> decltype(abs(x))
            {
                return abs(x);
            }
        };
    }

    using details::abs_fn;

    constexpr abs_fn abs()
    {
        return abs_fn{};
    }
}
@endcode

Теперь функциональный объект для модуля может быть создан и использован
следующим образом:

@code
auto seq = ural::make_transform_sequence(xs, ural::abs());
@endcode
*/

namespace ural
{
inline namespace v0
{
    /** @brief Тип функционального объекта для вычисления квадрата
    (второй степени)
    */
    class square_fn
    {
    public:
        square_fn() = default;

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

    /** @brief Класс функционального объекта для определения, является ли число
    чётным.
    */
    class is_even_fn
    {
    public:
        /** @brief Оператор вычисления значения функции
        @param x целое число
        @return <tt> x % Integer(2) == 0 </tt>
        */
        template <class Integer>
        constexpr bool operator()(Integer const & x) const
        {
            return x % Integer(2) == 0;
        }
    };

    namespace
    {
        constexpr auto const & is_even = odr_const<::ural::is_even_fn>;
        constexpr auto const & is_odd  = odr_const<::ural::not_function<is_even_fn>>;
    }

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
    class natural_power_fn
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
                    : adjust(ural::square_fn{}(this->compute(x, n / 2, op), op), x, n % 2 != 0, op);
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

    /** @brief Тип функционального объекта для вычисления произведения
    одного числа на неотрицательную целочисленную степень другого числа
    */
    struct power_accumulate_semigroup_fn
    {
    public:
        template <class A, class N>
        constexpr
        A operator()(A a, A x, N n, ural::plus<>) const
        {
            return a + (x * n);
        }

        /** @brief Оператор вычисления значения функции
        @param a множитель
        @param x число, возводимое в степень
        @param n показатель степени
        @param op операция возведения в степень
        @pre <tt> n >= 0 </tt>
        @return Если <tt> n == 0 </tt>, возвращает @c а,
        иначе --- <tt> a*natural_power(x, n, op) </tt>
        */
        template <class A, class N, class Op>
        constexpr
        A operator()(A a, A x, N n, Op op) const
        {
            assert(n >= 0);

            if(n == N(0))
            {
                return a;
            }

            for(;;)
            {
                if(is_odd(n))
                {
                    a = op(a, x);

                    if(n == N(1))
                    {
                        return a;
                    }
                }

                n = n / N(2);
                x = op(x, x);
            }
        }
    };

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
            @todo constexpr?
            */
            template <class T>
            constexpr auto operator()(T const & x) const
            -> decltype(abs(x))
            {
                return abs(x);
            }
        };

        using ::ural::experimental::abs_constexpr;

        class abs_constexpr_fn
        {
        public:
            /** @brief Оператор вычисления значения функции
            @param x число
            @return Модуль @c x
            */
            template <class T>
            constexpr auto operator()(T const & x) const
            -> decltype(abs_constexpr(x))
            {
                return abs_constexpr(x);
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

    constexpr details::abs_constexpr_fn abs_constexpr()
    {
        return details::abs_constexpr_fn{};
    }

    /** @brief Класс-характеристика, определяющая, являются ли все типы пачки
    целочисленными.
    @tparam Ts пачка типов
    */
    template <class... Ts>
    struct are_integral
     : ::ural::experimental::meta::all_of<typelist<Ts...>,
                                          ::ural::experimental::meta::template_to_applied<std::is_integral>>
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

    namespace
    {
        constexpr auto const & square = odr_const_holder<square_fn>::value;
        constexpr auto const & cube = odr_const<cube_fn>;

        constexpr auto const & natural_power = odr_const<natural_power_fn>;
        constexpr auto const & power_accumulate_semigroup
            = odr_const<power_accumulate_semigroup_fn>;
    }
}
// namespace v0
}
// namespace ural

#endif
// Z_URAL_MATH_HPP_INCLUDED
