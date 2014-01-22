#ifndef Z_URAL_FUNCTIONAL_CPP_OPERATORS_HPP_INCLUDED
#define Z_URAL_FUNCTIONAL_CPP_OPERATORS_HPP_INCLUDED

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
}
// namespace ural

#endif
// Z_URAL_FUNCTIONAL_CPP_OPERATORS_HPP_INCLUDED
