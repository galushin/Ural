#ifndef Z_URAL_TYPE_TRAITS_HPP_INCLUDED
#define Z_URAL_TYPE_TRAITS_HPP_INCLUDED

#include <type_traits>

namespace ural
{
    template <class T>
    using decay_t = typename std::decay<T>::type;
}
// namespace ural

#endif
// Z_URAL_TYPE_TRAITS_HPP_INCLUDED
