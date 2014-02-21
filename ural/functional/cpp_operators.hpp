#ifndef Z_URAL_FUNCTIONAL_CPP_OPERATORS_HPP_INCLUDED
#define Z_URAL_FUNCTIONAL_CPP_OPERATORS_HPP_INCLUDED

/** @file ural/functional/cpp_operators.hpp
 @brief Функциональные объекты, аналогичные определённым в @< functional @>
 @todo Оптимальные типы параметров
*/

#include <ural/functional/make_functor.hpp>
#include <ural/functional/compose.hpp>

namespace ural
{
// Функциональные объекты для операторов
    template <class T1 = void, class T2 = T1>
    class plus;

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

    template <class T1 = void, class T2 = T1>
    class minus;

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

    template <class T1 = void, class T2 = T1>
    class multiplies;

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

    // @todo divides
    // @todo modulus

    template <class T = void>
    class negate
    {
    public:
        constexpr auto operator()(T const & x) const
        -> decltype(-x)
        {
            return -x;
        }
    };

    template <>
    class negate<>
    {
    public:
        template <class T>
        constexpr auto operator()(T const & x) const
        -> decltype(-x)
        {
            return -x;
        }
    };

    template <class T1 = void, class T2 = T1>
    class equal_to
    {
    public:
        constexpr auto
        operator()(typename boost::call_traits<T1>::param_type x,
                   typename boost::call_traits<T1>::param_type y) const
        -> decltype(x == y)
        {
            return x == y;
        }
    };

    template <>
    class equal_to<void, void>
    {
    public:
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
    {
    public:
        constexpr auto
        operator()(typename boost::call_traits<T1>::param_type x,
                   typename boost::call_traits<T2>::param_type y) const
        -> decltype(x != y)
        {
            return x != y;
        }
    };

    template <class T1>
    class not_equal_to<T1, void>
    {
    public:
        template <class T2>
        constexpr auto
        operator()(typename boost::call_traits<T1>::param_type x,
                   T2 const & y) const
        -> decltype(x != y)
        {
            return x != y;
        }
    };

    template <class T2>
    class not_equal_to<void, T2>
    {
    public:
        template <class T1>
        constexpr auto
        operator()(T1 const & x,
                   typename boost::call_traits<T2>::param_type y) const
        -> decltype(x != y)
        {
            return x != y;
        }
    };

    template <>
    class not_equal_to<void, void>
    {
    public:
        template <class T1, class T2>
        constexpr auto operator()(T1 const & x, T2 const & y) const
        -> decltype(x != y)
        {
            return x != y;
        }
    };

    // @todo Специализации для указателей

    template <class T1 = void, class T2 = T1>
    class less
    {
    public:
        constexpr auto operator()(T1 const & x, T2 const & y) const
        -> decltype(x < y)
        {
            return x < y;
        }
    };

    template <>
    class less<void, void>
    {
    public:
        template <class T1, class T2>
        constexpr auto operator()(T1 const & x, T2 const & y) const
        -> decltype(x < y)
        {
            return x < y;
        }
    };

    template <class T1 = void, class T2 = T1>
    class greater;

    template <>
    class greater<void, void>
    {
    public:
        template <class T1, class T2>
        constexpr auto operator()(T1 const & x, T2 const & y) const
        -> decltype(x > y)
        {
            return x > y;
        }
    };

    // @todo greater_equal
    // @todo less_equal

    template <class T1 = void, class T2 = T1>
    class logical_and
    {
    public:
        constexpr auto operator()(T1 const & x, T2 const & y) const
        -> decltype(x && y)
        {
            return x && y;
        }
    };

    template <>
    class logical_and<void, void>
    {
    public:
        template <class T1, class T2>
        constexpr auto operator()(T1 const & x, T2 const & y) const
        -> decltype(x && y)
        {
            return x && y;
        }
    };

    template <class T1>
    class logical_and<T1, void>
    {
    public:
        template <class T2>
        constexpr auto operator()(T1 const & x, T2 const & y) const
        -> decltype(x && y)
        {
            return x && y;
        }
    };

    template <class T2>
    class logical_and<void, T2>
    {
    public:
        template <class T1>
        constexpr auto operator()(T1 const & x, T2 const & y) const
        -> decltype(x && y)
        {
            return x && y;
        }
    };

    template <class T1 = void, class T2 = T1>
    class logical_or
    {
    public:
        constexpr auto operator()(T1 const & x, T2 const & y) const
        -> decltype(x || y)
        {
            return x || y;
        }
    };

    template <>
    class logical_or<void, void>
    {
    public:
        template <class T1, class T2>
        constexpr auto operator()(T1 const & x, T2 const & y) const
        -> decltype(x || y)
        {
            return x || y;
        }
    };

    /** @brief Специаилизация с выводом типа второго аргумента
    @tparam T1 тип первого аргумента
    */
    template <class T1>
    class logical_or<T1, void>
    {
    public:
        template <class T2>
        constexpr auto operator()(T1 const & x, T2 const & y) const
        -> decltype(x || y)
        {
            return x || y;
        }
    };

    /** @brief Специаилизация с выводом типа первого аргумента
    @tparam T1 тип второго аргумента
    */
    template <class T2>
    class logical_or<void, T2>
    {
    public:
        template <class T1>
        constexpr auto operator()(T1 const & x, T2 const & y) const
        -> decltype(x || y)
        {
            return x || y;
        }
    };

    template <class T = void>
    class logical_not
    {
    public:
        constexpr auto operator()(T const & x) const
        -> decltype(!x)
        {
            return !x;
        }
    };

    template <>
    class logical_not<void>
    {
    public:
        template <class T>
        constexpr auto operator()(T const & x) const
        -> decltype(!x)
        {
            return !x;
        }
    };

    // @todo bit_and
    // @todo bit_or
    // @todo bit_xor
    // @todo bit_not

    // Негатор
    template <class Predicate>
    class not_functor
     : private compose_functor<ural::logical_not<>, Predicate>
    {
    friend constexpr bool
    operator==(not_functor const & x, not_functor const & y)
    {
        return static_cast<Base const &>(x) ==
                static_cast<Base const &>(y);
    }

        typedef compose_functor<ural::logical_not<>, Predicate> Base;

    public:
        typedef decltype(make_functor(std::declval<Predicate>())) target_type;

        constexpr not_functor()
         : Base{}
        {}

        explicit not_functor(Predicate pred)
         : Base(ural::logical_not<>{}, std::move(pred))
        {}

        constexpr target_type const & target() const
        {
            return Base::second_functor();
        }

        template <class... Args>
        constexpr auto operator()(Args && ... args) const
        -> decltype(std::declval<Base>()(std::forward<Args>(args)...))
        {
            return Base::operator()(std::forward<Args>(args)...);
        }
    };

    template <class Predicate>
    auto not_fn(Predicate pred)
    -> not_functor<decltype(make_functor(std::move(pred)))>
    {
        typedef not_functor<decltype(make_functor(std::move(pred)))> Functor;
        return Functor{make_functor(std::move(pred))};
    }
}
// namespace ural

#endif
// Z_URAL_FUNCTIONAL_CPP_OPERATORS_HPP_INCLUDED
