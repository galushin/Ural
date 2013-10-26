#ifndef Z_URAL_PLACEHOLDERS_HPP_INCLUDED
#define Z_URAL_PLACEHOLDERS_HPP_INCLUDED

namespace ural
{
    template <size_t N>
    struct placeholder{};

    constexpr placeholder<0> _1 {};
    constexpr placeholder<1> _2 {};
    constexpr placeholder<2> _3 {};
    constexpr placeholder<3> _4 {};
}
// namespace ural

#endif
// Z_URAL_PLACEHOLDERS_HPP_INCLUDED
