#ifndef Z_URAL_FUNCTIONAL_CPP_OPERATORS_HPP_INCLUDED
#define Z_URAL_FUNCTIONAL_CPP_OPERATORS_HPP_INCLUDED

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

/** @file ural/functional/cpp_operators.hpp
 @brief Функциональные объекты, аналогичные определённым в @< functional @>
*/

#include <ural/functional/make_functor.hpp>
#include <ural/functional/compose.hpp>

namespace ural
{
    /** @brief Вспомогательный класс для функциональных объектов для бинарных
    операторов
    @tparam T1 тип первого аргумента
    @tparam T2 тип второго аргумента
    @tparam F тип "прозрачного" функционального объекта, реализующего оператор
    */
    template <class T1, class T2, class F>
    class binary_operator_helper
    {
    public:
        static_assert(std::is_empty<F>::value, "must be empty class");

        constexpr auto
        operator()(typename boost::call_traits<T1>::param_type x,
                   typename boost::call_traits<T2>::param_type y) const
        -> decltype(std::declval<F const>()(x, y))
        {
            typedef typename F::is_transparent F_is_transparent;

            return F{}(x, y);
        }
    };

    template <class T1, class F>
    class binary_operator_helper<T1, void, F>
    {
    public:
        static_assert(std::is_empty<F>::value, "must be empty class");

        template <class T2>
        constexpr auto
        operator()(typename boost::call_traits<T1>::param_type x,
                   T2 && y) const
        -> decltype(std::declval<F const>()(x, std::forward<T2>(y)))
        {
            typedef typename F::is_transparent F_is_transparent;

            return F{}(x, std::forward<T2>(y));
        }
    };

    template <class T2, class F>
    class binary_operator_helper<void, T2, F>
    {
    public:
        static_assert(std::is_empty<F>::value, "must be empty class");

        template <class T1>
        constexpr auto
        operator()(T1 && x,
                   typename boost::call_traits<T2>::param_type y) const
        -> decltype(std::declval<F const>()(std::forward<T1>(x), y))
        {
            typedef typename F::is_transparent F_is_transparent;

            return F{}(std::forward<T1>(x), y);
        }
    };

    template <class T1, class T2, class F>
    class compound_assignment_helper
    {
    public:
        static_assert(std::is_empty<F>::value, "Must be empty!");

        T1 & operator()(T1 & x, T2 const & y) const
        {
            typedef typename F::is_transparent F_is_transparent;

            return F{}(x, y);
        }
    };

    template <class T1, class F>
    class compound_assignment_helper<T1, void, F>
    {
    public:
        static_assert(std::is_empty<F>::value, "Must be empty!");

        template <class T2>
        T1 & operator()(T1 & x, T2 && y) const
        {
            typedef typename F::is_transparent F_is_transparent;

            return F{}(x, std::forward<T2>(y));
        }
    };

    template <class T2, class F>
    class compound_assignment_helper<void, T2, F>
    {
    public:
        static_assert(std::is_empty<F>::value, "Must be empty!");

        template <class T1>
        T1 & operator()(T1 & x, T2 const & y) const
        {
            typedef typename F::is_transparent F_is_transparent;

            return F{}(x, y);
        }
    };

    /** @brief Вспомогательный класс для определения функциональных объектов
    для унарных операторов
    @tparam T тип аргумента
    @tparam F тип "прозрачного" функционального объекта
    */
    template <class T, class F>
    class unary_operator_helper
    {
    public:
        static_assert(std::is_empty<F>::value, "must be empty class");

        typedef T argument_type;
        typedef decltype(std::declval<F const>()(std::declval<T>())) result_type;

        constexpr result_type
        operator()(typename boost::call_traits<T>::param_type x) const
        {
            typedef typename F::is_transparent F_is_transparent;

            return F{}(x);
        }
    };

// Функциональные объекты для операторов
    /** @brief Функциональный объект для оператора "бинарный плюс"
    @tparam T1 тип первого аргумента, если он совпадает с @b void, то тип будет
    выведен по фактическому аргументу
    @tparam T2 тип второго аргумента, если он совпадает с @b void, то тип будет
    выведен по фактическому аргументу
    */
    template <class T1 = void, class T2 = T1>
    class plus
     : public binary_operator_helper<T1, T2, plus<>>
    {};

    /// @brief Специализация с выводом типов обоих аргументов
    template <>
    class plus<void, void>
    {
    public:
        /** @brief Оператор вычисления значения
        @param x левый операнд
        @param y правый операнд
        @return <tt> std::forward<T1>(x) + std::forward<T2>(y) </tt>
        */
        template <class T1, class T2>
        constexpr auto operator()(T1 && x, T2 && y) const
        -> decltype(std::forward<T1>(x) + std::forward<T2>(y))
        {
            return std::forward<T1>(x) + std::forward<T2>(y);
        }
    };

    /** @brief Функциональный объект для оператора "бинарный минус"
    @tparam T1 тип первого аргумента, если он совпадает с @b void, то тип будет
    выведен по фактическому аргументу
    @tparam T2 тип второго аргумента, если он совпадает с @b void, то тип будет
    выведен по фактическому аргументу
    */
    template <class T1 = void, class T2 = T1>
    class minus
     : public binary_operator_helper<T1, T2, minus<>>
    {};

    /// @brief Специализация с выводом типов обоих аргументов
    template <>
    class minus<void, void>
    {
    public:
        /** @brief Оператор вычисления значения
        @param x левый операнд
        @param y правый операнд
        @return <tt> std::forward<T1>(x) - std::forward<T2>(y) </tt>
        */
        template <class T1, class T2>
        constexpr auto operator()(T1 && x, T2 && y) const
        -> decltype(std::forward<T1>(x) - std::forward<T2>(y))
        {
            return std::forward<T1>(x) - std::forward<T2>(y);
        }
    };

    /** @brief Функциональный объект для оператора "умножить"
    @tparam T1 тип первого аргумента, если он совпадает с @b void, то тип будет
    выведен по фактическому аргументу
    @tparam T2 тип второго аргумента, если он совпадает с @b void, то тип будет
    выведен по фактическому аргументу
    */
    template <class T1 = void, class T2 = T1>
    class multiplies
     : public binary_operator_helper<T1, T2, multiplies<>>
    {};

    /// @brief Специализация с выводом типов обоих аргументов
    template <>
    class multiplies<void, void>
    {
    public:
        /** @brief Оператор вычисления значения
        @param x левый операнд
        @param y правый операнд
        @return <tt> std::forward<T1>(x) * std::forward<T2>(y) </tt>
        */
        template <class T1, class T2>
        constexpr auto operator()(T1 && x, T2 && y) const
        -> decltype(std::forward<T1>(x) * std::forward<T2>(y))
        {
            return std::forward<T1>(x) * std::forward<T2>(y);
        }
    };

    /** @brief Функциональный объект для оператора "разделить"
    @tparam T1 тип первого аргумента, если он совпадает с @b void, то тип будет
    выведен по фактическому аргументу
    @tparam T2 тип второго аргумента, если он совпадает с @b void, то тип будет
    выведен по фактическому аргументу
    */
    template <class T1 = void, class T2 = T1>
    class divides
      : public binary_operator_helper<T1, T2, divides<>>
    {};

    template <>
    class divides<void, void>
    {
    public:
        typedef std::true_type is_transparent;

        template <class T1, class T2>
        constexpr auto operator()(T1 && x, T2 && y) const
        -> decltype(std::forward<T1>(x) / std::forward<T2>(y))
        {
            return std::forward<T1>(x) / std::forward<T2>(y);
        }
    };

    template <class T1 = void, class T2 = T1>
    class modulus
     : public binary_operator_helper<T1, T2, modulus<>>
    {};

    /** @brief Функциональный объект для оператора "остаток от деления"
    @tparam T1 тип первого аргумента, если он совпадает с @b void, то тип будет
    выведен по фактическому аргументу
    @tparam T2 тип второго аргумента, если он совпадает с @b void, то тип будет
    выведен по фактическому аргументу
    */
    template <>
    class modulus<void, void>
    {
    public:
        typedef std::true_type is_transparent;

        template <class T1, class T2>
        constexpr auto operator()(T1 && x, T2 && y) const
        -> decltype(std::forward<T1>(x) % std::forward<T2>(y))
        {
            return std::forward<T1>(x) % std::forward<T2>(y);
        }
    };

    /** @brief Функциональный объект для оператора "унарный минус"
    @tparam T тип аргумента, если он совпадает с @b void, то тип будет выведен
    по фактическому аргументу.
    */
    template <class T = void>
    class negate
     : public unary_operator_helper<T, negate<>>
    {};

    template <>
    class negate<>
    {
    public:
        typedef std::true_type is_transparent;

        template <class T>
        constexpr auto operator()(T && x) const
        -> decltype(-std::forward<T>(x))
        {
            return -std::forward<T>(x);
        }
    };

    /** @brief Функциональный объект для оператора "равно"
    @tparam T1 тип первого аргумента, если он совпадает с @b void, то тип будет
    выведен по фактическому аргументу
    @tparam T2 тип второго аргумента, если он совпадает с @b void, то тип будет
    выведен по фактическому аргументу
    */
    template <class T1 = void, class T2 = T1>
    class equal_to
     : public binary_operator_helper<T1, T2, equal_to<>>
    {};

    template <>
    class equal_to<void, void>
    {
    public:
        typedef std::true_type is_transparent;

        template <class T1, class T2>
        constexpr auto operator()(T1 const & x, T2 const & y) const
        -> decltype(x == y)
        {
            return x == y;
        }
    };

    /** @brief Функциональный объект для оператора "не равно"
    @tparam T1 тип первого аргумента
    @tparam T2 тип второго аргумента
    Если один из типов аргумента равен @b void, то его тип будет выводится
    по типу фактического параметра
    */
    template <class T1 = void, class T2 = T1>
    class not_equal_to
     : public binary_operator_helper<T1, T2, not_equal_to<>>
    {};

    template <>
    class not_equal_to<void, void>
    {
    public:
        typedef std::true_type is_transparent;

        template <class T1, class T2>
        constexpr auto operator()(T1 && x, T2 && y) const
        -> decltype (std::forward<T1>(x) != std::forward<T2>(y))
        {
            return std::forward<T1>(x) != std::forward<T2>(y);
        }
    };

    /** @brief Функциональный объект для оператора "меньше"
    @tparam T1 тип первого аргумента
    @tparam T2 тип второго аргумента
    Если один из типов аргумента равен @b void, то его тип будет выводится
    по типу фактического параметра
    */
    template <class T1 = void, class T2 = T1>
    class less
     : public binary_operator_helper<T1, T2, less<>>
    {};

    template <>
    class less<void, void>
    {
    public:
        typedef std::true_type is_transparent;

        template <class T1, class T2>
        constexpr auto operator()(T1 && x, T2 && y) const
        -> decltype (std::forward<T1>(x) < std::forward<T2>(y))
        {
            return std::forward<T1>(x) < std::forward<T2>(y);
        }
    };

    /** @brief Функциональный объект для оператора "больше"
    @tparam T1 тип первого аргумента
    @tparam T2 тип второго аргумента
    Если один из типов аргумента равен @b void, то его тип будет выводится
    по типу фактического параметра
    */
    template <class T1 = void, class T2 = T1>
    class greater
     : public binary_operator_helper<T1, T2, greater<>>
    {};

    template <>
    class greater<void, void>
    {
    public:
        template <class T1, class T2>
        constexpr auto operator()(T1 && x, T2 && y) const
        -> decltype (std::forward<T1>(x) > std::forward<T2>(y))
        {
            return std::forward<T1>(x) > std::forward<T2>(y);
        }
    };

    /** @brief Функциональный объект для оператора "меньше или равно"
    @tparam T1 тип первого аргумента
    @tparam T2 тип второго аргумента
    Если один из типов аргумента равен @b void, то его тип будет выводится
    по типу фактического параметра
    */
    template <class T1 = void, class T2 = T1>
    class less_equal
      : public binary_operator_helper<T1, T2, less_equal<>>
    {};

    template <>
    class less_equal<void, void>
    {
    public:
        template <class T1, class T2>
        constexpr auto operator()(T1 && x, T2 && y) const
        -> decltype (std::forward<T1>(x) <= std::forward<T2>(y))
        {
            return std::forward<T1>(x) <= std::forward<T2>(y);
        }
    };

    /** @brief Функциональный объект для оператора "больше или равно"
    @tparam T1 тип первого аргумента
    @tparam T2 тип второго аргумента
    Если один из типов аргумента равен @b void, то его тип будет выводится
    по типу фактического параметра
    */
    template <class T1 = void, class T2 = T1>
    class greater_equal
     : public binary_operator_helper<T1, T2, greater_equal<>>
    {};

    template <>
    class greater_equal<void, void>
    {
    public:
        template <class T1, class T2>
        constexpr auto operator()(T1 && x, T2 && y) const
        -> decltype (std::forward<T1>(x) >= std::forward<T2>(y))
        {
            return std::forward<T1>(x) >= std::forward<T2>(y);
        }
    };

    // Логические операции
    /** @brief Функциональный объект для оператора "логическое И"
    @tparam T1 тип первого аргумента
    @tparam T2 тип второго аргумента
    Если один из типов аргумента равен @b void, то его тип будет выводится
    по типу фактического параметра
    */
    template <class T1 = void, class T2 = T1>
    class logical_and
     : public binary_operator_helper<T1, T2, logical_and<>>
    {};

    template <>
    class logical_and<void, void>
    {
    public:
        typedef std::true_type is_transparent;

        template <class T1, class T2>
        constexpr auto operator()(T1 && x, T2 && y) const
        -> decltype(std::forward<T1>(x) && std::forward<T2>(y))
        {
            return std::forward<T1>(x) && std::forward<T2>(y);
        }
    };

    /** @brief Функциональный объект для оператора "логическое ИЛИ"
    @tparam T1 тип первого аргумента
    @tparam T2 тип второго аргумента
    Если один из типов аргумента равен @b void, то его тип будет выводится
    по типу фактического параметра
    */
    template <class T1 = void, class T2 = T1>
    class logical_or
     : public binary_operator_helper<T1, T2, logical_or<>>
    {};

    template <>
    class logical_or<void, void>
    {
    public:
        typedef std::true_type is_transparent;

        template <class T1, class T2>
        constexpr auto operator()(T1 && x, T2 && y) const
        -> decltype(std::forward<T1>(x) || std::forward<T2>(y))
        {
            return std::forward<T1>(x) || std::forward<T2>(y);
        }
    };

    /** @brief Функциональный объект для оператора "логическое НЕ"
    @tparam T тип аргумента, если этот тип совпадает с @b void, то тип будет
    выводится по фактическому параметру.
    */
    template <class T = void>
    class logical_not
     : public unary_operator_helper<T, logical_not<>>
    {};

    template <>
    class logical_not<void>
    {
    public:
        typedef std::true_type is_transparent;

        template <class T>
        constexpr auto operator()(T && x) const
        -> decltype(!std::forward<T>(x))
        {
            return !std::forward<T>(x);
        }
    };

    template <class T1 = void, class T2 = T1>
    class logical_implication
     : public binary_operator_helper<T1, T2, logical_implication<>>
    {};

    template <>
    class logical_implication<void, void>
    {
    public:
        typedef std::true_type is_transparent;

        template <class T1, class T2>
        constexpr auto operator()(T1 && x, T2 && y) const
        -> decltype(!std::forward<T1>(x) || std::forward<T2>(y))
        {
            return !std::forward<T1>(x) || std::forward<T2>(y);
        }
    };

    // Побитовые операции
    /** @brief Функциональный объект для оператора "побитовое НЕ"
    @tparam T тип аргумента, если этот тип совпадает с @b void, то тип будет
    выводится по фактическому параметру.
    */
    template <class T = void>
    class bit_not
     : public unary_operator_helper<T, bit_not<>>
    {};

    template <>
    class bit_not<void>
    {
    public:
        typedef std::true_type is_transparent;

        template <class T>
        constexpr auto operator()(T && arg) const
        -> decltype(~std::forward<T>(arg))
        {
            return ~std::forward<T>(arg);
        }
    };

    /** @brief Функциональный объект для оператора "побитовое И"
    @tparam T1 тип первого аргумента
    @tparam T2 тип второго аргумента
    Если один из типов аргумента равен @b void, то его тип будет выводится
    по типу фактического параметра
    */
    template <class T1 = void, class T2 = T1>
    class bit_and
     : public binary_operator_helper<T1, T2, bit_and<void, void>>
    {};

    template <>
    class bit_and<void, void>
    {
    public:
        typedef std::true_type is_transparent;

        template <class T1, class T2>
        constexpr auto operator()(T1 && x, T2 && y) const
        -> decltype(std::forward<T1>(x) & std::forward<T2>(y))
        {
            return std::forward<T1>(x) & std::forward<T2>(y);
        }
    };

    /** @brief Функциональный объект для оператора "побитовое ИЛИ"
    @tparam T1 тип первого аргумента
    @tparam T2 тип второго аргумента
    Если один из типов аргумента равен @b void, то тип будет выводится по
    фактическому параметру
    */
    template <class T1 = void, class T2 = T1>
    class bit_or
     : public binary_operator_helper<T1, T2, bit_or<void, void>>
    {};

    /// @brief Специализация с выводом типов аргументов
    template <>
    class bit_or<void, void>
    {
    public:
        typedef std::true_type is_transparent;

        template <class T1, class T2>
        constexpr auto operator()(T1 && x, T2 && y) const
        -> decltype(std::forward<T1>(x) | std::forward<T2>(y))
        {
            return std::forward<T1>(x) | std::forward<T2>(y);
        }
    };

    /** @brief Функциональный объект для оператора "побитовое исключающее ИЛИ"
    @tparam T1 тип первого аргумента
    @tparam T2 тип второго аргумента
    Если один из типов аргумента равен @b void, то тип будет выводится по
    фактическому параметру
    */
    template <class T1 = void, class T2 = T1>
    class bit_xor
     : public binary_operator_helper<T1, T2, bit_xor<void, void>>
    {};

    /// @brief Специализация с выводом типов аргументов
    template <>
    class bit_xor<void, void>
    {
    public:
        typedef std::true_type is_transparent;

        template <class T1, class T2>
        constexpr auto operator()(T1 && x, T2 && y) const
        -> decltype(std::forward<T1>(x) ^ std::forward<T2>(y))
        {
            return std::forward<T1>(x) ^ std::forward<T2>(y);
        }
    };

    // Негатор
    template <class Predicate>
    class not_functor
     : private compose_functor<ural::logical_not<>, Predicate>
    {
        typedef compose_functor<ural::logical_not<>, Predicate> Base;

        friend constexpr bool
        operator==(not_functor const & x, not_functor const & y)
        {
            return static_cast<Base const &>(x) ==
                    static_cast<Base const &>(y);
        }

    public:
        /// @brief Тип базового функционального объекта
        typedef decltype(make_functor(std::declval<Predicate>())) target_type;

        // Конструкторы
        /** @brief Конструктор
        @post <tt> this->target() == target_type{} </tt>
        */
        constexpr not_functor()
         : Base{}
        {}

        /** @brief Конструктор
        @param pred предикат
        @post <tt> this->target() == pred </tt>
        */
        explicit not_functor(Predicate pred)
         : Base(ural::logical_not<>{}, std::move(pred))
        {}

        /** @brief Базовый функциональный объект
        @return Базовый функциональный объект
        */
        constexpr target_type const & target() const
        {
            return Base::second_functor();
        }

        /** @brief Применение функционального объекта
        @param args параметры вызова
        @return <tt> !(this->target())(args...) </tt>
        */
        template <class... Args>
        constexpr auto operator()(Args && ... args) const
        -> decltype(std::declval<Base>()(std::forward<Args>(args)...))
        {
            return Base::operator()(std::forward<Args>(args)...);
        }
    };

    /** @brief Создание негатора
    @param pred предикат
    */
    template <class Predicate>
    auto not_fn(Predicate pred)
    -> not_functor<decltype(make_functor(std::move(pred)))>
    {
        typedef not_functor<decltype(make_functor(std::move(pred)))> Functor;
        return Functor{make_functor(std::move(pred))};
    }

    // Составные операторы присваивания
    /** @brief Функциональный объект для оператора "плюс-равно"
    @tparam T1 тип первого аргумента
    @tparam T2 тип второго аргумента
    Если один из типов аргумента равен @b void, то тип будет выводится по
    фактическому параметру
    */
    template <class T1 = void, class T2 = T1>
    class plus_assign
     : public compound_assignment_helper<T1, T2, plus_assign<>>
    {};

    template <>
    class plus_assign<>
    {
    public:
        typedef std::true_type is_transparent;

        template <class T1, class T2>
        constexpr T1 & operator()(T1 & x, T2 && y) const
        {
            return x += std::forward<T2>(y);
        }
    };

    /** @brief Функциональный объект для оператора "минус-равно"
    @tparam T1 тип первого аргумента
    @tparam T2 тип второго аргумента
    Если один из типов аргумента равен @b void, то тип будет выводится по
    фактическому параметру
    */
    template <class T1 = void, class T2 = T1>
    class minus_assign
     : public compound_assignment_helper<T1, T2, minus_assign<>>
    {};

    template <>
    class minus_assign<>
    {
    public:
        typedef std::true_type is_transparent;

        template <class T1, class T2>
        constexpr T1 & operator()(T1 & x, T2 && y) const
        {
            return x -= std::forward<T2>(y);
        }
    };

    template <class T1 = void, class T2 = T1>
    class multiplies_assign
     : public compound_assignment_helper<T1, T2, multiplies_assign<>>
    {};

    template <>
    class multiplies_assign<void, void>
    {
    public:
        typedef std::true_type is_transparent;

        template <class T1, class T2>
        constexpr T1 & operator()(T1 & x, T2 && y) const
        {
            return x *= std::forward<T2>(y);
        }
    };

    template <class T1 = void, class T2 = T1>
    class divides_assign
     : public compound_assignment_helper<T1, T2, divides_assign<>>
    {};

    template <>
    class divides_assign<void, void>
    {
    public:
        typedef std::true_type is_transparent;

        template <class T1, class T2>
        constexpr T1 & operator()(T1 & x, T2 && y) const
        {
            return x /= std::forward<T2>(y);
        }
    };

    template <class T1 = void, class T2 = T1>
    class modulus_assign
     : public compound_assignment_helper<T1, T2, modulus_assign<>>
    {};

    template <>
    class modulus_assign<void, void>
    {
    public:
        typedef std::true_type is_transparent;

        template <class T1, class T2>
        constexpr T1 & operator()(T1 & x, T2 && y) const
        {
            return x %= std::forward<T2>(y);
        }
    };

    /** @brief Функциональный объект для унарного оператора * (разыменования)
    @tparam T тип аргумента, если этот тип совпадает с @b void, то тип аргумента
    будет выводится по фактическим параметрам
    */
    template <class T = void>
    class dereference
     : public unary_operator_helper<T, dereference<>>
    {};

    /// @brief Специализация с выводом типа аргумента
    template <>
    class dereference<void>
    {
    public:
        template <class T>
        constexpr auto operator()(T && x) const
        -> decltype(*std::forward<T>(x))
        {
            return *std::forward<T>(x);
        }
    };
}
// namespace ural

#endif
// Z_URAL_FUNCTIONAL_CPP_OPERATORS_HPP_INCLUDED
