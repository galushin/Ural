#ifndef Z_URAL_SEQUENCE_REPLACE_HPP_INCLUDED
#define Z_URAL_SEQUENCE_REPLACE_HPP_INCLUDED

/** @brief ural/sequence/replace.hpp
 @brief Создание последовательностей, в которых элементы, удовлетворяющие
 определённым условиям, заменены заданным значением.
*/

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

    template <class Sequence, class Predicate, class T>
    auto replace_if(Sequence && seq, Predicate pred, T const & new_value)
    -> decltype(::ural::transform(std::forward<Sequence>(seq),
                                  make_replace_if_functor(std::move(pred), new_value)))
    {
        return ::ural::transform(std::forward<Sequence>(seq),
                                  make_replace_if_functor(std::move(pred), new_value));
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_REPLACE_HPP_INCLUDED
