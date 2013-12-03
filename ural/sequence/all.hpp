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
    /** @brief Создание последовательности на основе контейнера
    @param c контейнер
    @return <tt> iterator_sequence<decltype(c.begin())>{c.begin(), c.end()}</tt>
    */
    template <class Container>
    auto sequence(Container && c)
    -> iterator_sequence<decltype(c.begin())>
    {
        return iterator_sequence<decltype(c.begin())>{c.begin(), c.end()};
    }

    template <class T, size_t N>
    iterator_sequence<T *>
    sequence(T (&x)[N])
    {
        return iterator_sequence<T *>{x, x + N};
    }

    /** @brief Создание последовательности на основе итератора вставки в конец
    контейнера
    @param i итератор-вставка
    */
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
