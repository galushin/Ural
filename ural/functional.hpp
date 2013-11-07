#ifndef Z_URAL_FUNCTIONAL_HPP_INCLUDED
#define Z_URAL_FUNCTIONAL_HPP_INCLUDED

/** @file ural/functional.hpp
 @brief Функциональные объекты и средства для работы с ними
*/

namespace ural
{
    // Преобразование в функциональный объект
    template <class F>
    F make_functor(F f)
    {
        return f;
    }

    // Функциональные объекты для операторов
    template <class T1 = void, class T2 = T1>
    class plus;

    template <>
    class plus<void, void>
    {
    public:
        template <class T1, class T2>
        constexpr auto operator()(T1 && x, T2 && y) const
        -> decltype(std::forward<T1>(x) + std::forward<T2>(y))
        {
            return std::forward<T1>(x) + std::forward<T2>(y);
        }
    };

    template <class T1 = void, class T2 = T1>
    class minus;

    template <>
    class minus<void, void>
    {
    public:
        template <class T1, class T2>
        constexpr auto operator()(T1 && x, T2 && y) const
        -> decltype(std::forward<T1>(x) - std::forward<T2>(y))
        {
            return std::forward<T1>(x) - std::forward<T2>(y);
        }
    };

    template <class T1 = void, class T2 = T1>
    class multiplies;

    template <>
    class multiplies<void, void>
    {
    public:
        template <class T1, class T2>
        constexpr auto operator()(T1 && x, T2 && y) const
        -> decltype(std::forward<T1>(x) * std::forward<T2>(y))
        {
            return std::forward<T1>(x) * std::forward<T2>(y);
        }
    };
}
// namespace ural

#endif
// Z_URAL_FUNCTIONAL_HPP_INCLUDED
