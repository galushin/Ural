#ifndef Z_URAL_SEQUENCE_REPLACE_HPP_INCLUDED
#define Z_URAL_SEQUENCE_REPLACE_HPP_INCLUDED

#include <ural/sequence/transform.hpp>
#include <ural/functional/replace.hpp>

namespace ural
{
    template <class Sequence, class T>
    auto replace(Sequence && seq, T const & old_value, T const & new_value)
    -> decltype(::ural::transform(std::forward<Sequence>(seq),
                                  make_replace_functor(old_value, new_value)))
    {
        return ::ural::transform(std::forward<Sequence>(seq),
                                  make_replace_functor(old_value, new_value));
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_REPLACE_HPP_INCLUDED
