#ifndef Z_URAL_SEQUENCE_ALL_HPP_INCLUDED
#define Z_URAL_SEQUENCE_ALL_HPP_INCLUDED

#include <ural/sequence/iterator_sequence.hpp>

namespace ural
{
    template <class Container>
    auto sequence(Container && c)
    -> iterator_sequence<decltype(c.begin())>
    {
        return iterator_sequence<decltype(c.begin())>{c.begin(), c.end()};
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_ALL_HPP_INCLUDED
