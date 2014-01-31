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

    template <class T, class Default>
    struct default_helper
     : std::conditional<std::is_same<T, use_default>::value, Default, T>
    {};
}
// namespace ural

#endif
// Z_URAL_DEFS_HPP_INCLUDED
