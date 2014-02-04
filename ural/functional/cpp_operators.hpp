#ifndef Z_URAL_FUNCTIONAL_CPP_OPERATORS_HPP_INCLUDED
#define Z_URAL_FUNCTIONAL_CPP_OPERATORS_HPP_INCLUDED

/** @file ural/functional/cpp_operators.hpp
 @brief Функциональные объекты, аналогичные определённым в <functional>
 @todo Оптимальные типы параметров
*/

#include <ural/functional/make_functor.hpp>

namespace ural
{
    // Негатор
    /**
    @todo Выразить через logical_not и pipe (compose)
    */
    template <class Predicate>
    class not_functor
     : decltype(make_functor(std::declval<Predicate>()))
    {
    friend constexpr bool
    operator==(not_functor const & x, not_functor const & y)
    {
        return x.target() == y.target();
    }

    public:
        typedef decltype(make_functor(std::declval<Predicate>())) target_type;

        constexpr not_functor()
         : target_type{}
        {}

        explicit not_functor(Predicate pred)
         : target_type(std::move(pred))
        {}

        constexpr target_type const & target() const
        {
            return *this;
        }

        template <class... Args>
        constexpr auto operator()(Args && ... args) const
        -> decltype(!this->target()(std::forward<Args>(args)...))
        {
            return !this->target()(std::forward<Args>(args)...);
        }
    };

    template <class Predicate>
    auto not_fn(Predicate pred)
    -> not_functor<decltype(make_functor(std::move(pred)))>
    {
        typedef not_functor<decltype(make_functor(std::move(pred)))> Functor;
        return Functor{make_functor(std::move(pred))};
    }

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
    // @todo negate

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

    template <class T1 = void, class T2 = T1>
    class not_equal_to
    {
    public:
        constexpr auto
        operator()(typename boost::call_traits<T1>::param_type x,
                   typename boost::call_traits<T1>::param_type y) const
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

    // @todo Может быть проблема с указателями
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

    // @todo выразить через less и некоторые другие примитивы
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

    // @todo logical_and
    // @todo logical_or

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
}
// namespace ural

#endif
// Z_URAL_FUNCTIONAL_CPP_OPERATORS_HPP_INCLUDED
