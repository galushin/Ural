#ifndef Z_URAL_CONCEPTS_HPP_INCLUDED
#define Z_URAL_CONCEPTS_HPP_INCLUDED

/** @file ural/concepts.hpp
 @brief Классы для проверки концепций
 @todo Реализовать
*/

namespace ural
{
namespace concepts
{
    template <class T>
    class Regular
    {};

    template <class Seq>
    class SinglePassSequence
     : Regular<Seq>
    {};

    template <class Seq>
    class ReadableSequence
    {};

    template <class Seq, class T>
    class WritableSequence
    {};
}
// namespace concepts
}
// namespace ural

#endif
// Z_URAL_CONCEPTS_HPP_INCLUDED
