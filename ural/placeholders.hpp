#ifndef Z_URAL_PLACEHOLDERS_HPP_INCLUDED
#define Z_URAL_PLACEHOLDERS_HPP_INCLUDED

/** @file ural/placeholders.hpp
 @brief Статические индексы
*/

namespace ural
{
    template <size_t N>
    struct placeholder{};

    /// @cond false
    constexpr placeholder<0> _1 {};
    constexpr placeholder<1> _2 {};
    constexpr placeholder<2> _3 {};
    constexpr placeholder<3> _4 {};
    constexpr placeholder<4> _5 {};
    /// @endcond
}
// namespace ural

#endif
// Z_URAL_PLACEHOLDERS_HPP_INCLUDED
