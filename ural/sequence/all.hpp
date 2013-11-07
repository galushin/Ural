#ifndef Z_URAL_SEQUENCE_ALL_HPP_INCLUDED
#define Z_URAL_SEQUENCE_ALL_HPP_INCLUDED

/** @file ural/sequence/all.hpp
 @brief Все последовательности
*/

#include <iterator>

#include <ural/sequence/base.hpp>
#include <ural/sequence/iterator_sequence.hpp>
#include <ural/sequence/insertion.hpp>

namespace ural
{
    template <class Container>
    auto sequence(Container && c)
    -> iterator_sequence<decltype(c.begin())>
    {
        return iterator_sequence<decltype(c.begin())>{c.begin(), c.end()};
    }

    template <class Container>
    ural::output_iterator_sequence<std::back_insert_iterator<Container>>
    sequence(std::back_insert_iterator<Container> i)
    {
        typedef std::back_insert_iterator<Container> Iterator;
        return ural::output_iterator_sequence<Iterator>(std::move(i));
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_ALL_HPP_INCLUDED
