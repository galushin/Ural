#ifndef Z_URAL_DEFS_HPP_INCLUDED
#define Z_URAL_DEFS_HPP_INCLUDED

/** @file ural/defs.hpp
 @brief Определения основных типов, используемых библиотекой
*/

#define URAL_PREVENT_MACRO_SUBSTITUTION

namespace ural
{
    struct empty_type{};
    class null_type{};
    struct use_default{};
    struct auto_tag{};

    template <class T>
    struct declare_type
    {
        typedef T type;
    };

    template <class T, class Default>
    struct default_helper
     : std::conditional<std::is_same<T, use_default>::value, Default, T>
    {};

    template <class T>
    constexpr typename std::enable_if<std::is_empty<T>::value, bool>::type
    operator==(T const &, T const &)
    {
        return true;
    }

    template <class T1, class T2>
    constexpr bool operator!=(T1 const & x, T2 const & y)
    {
        return !(x == y);
    }

    template <class T1, class T2>
    constexpr bool operator>(T1 const & x, T2 const & y)
    {
        return y < x;
    }

    template <class T1, class T2>
    constexpr bool operator<=(T1 const & x, T2 const & y)
    {
        return !(x > y);
    }

    template <class T1, class T2>
    constexpr bool operator>=(T1 const & x, T2 const & y)
    {
        return !(x < y);
    }

    template <class T>
    struct value_consumer
    {
        void operator=(T const &)
        {}
    };
}
// namespace ural

#endif
// Z_URAL_DEFS_HPP_INCLUDED
