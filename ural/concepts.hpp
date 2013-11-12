#ifndef Z_URAL_CONCEPTS_HPP_INCLUDED
#define Z_URAL_CONCEPTS_HPP_INCLUDED

/** @file ural/concepts.hpp
 @brief Классы для проверки концепций
 @todo Реализовать
*/

#include <boost/concept/usage.hpp>

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
    {
    public:
        BOOST_CONCEPT_USAGE(SinglePassSequence)
        {
            !seq;
            ++ seq;
            seq ++;
        }
    private:
        static Seq seq;
    };

    template <class Seq>
    class ReadableSequence
    {
    public:
        BOOST_CONCEPT_USAGE(ReadableSequence)
        {
            consume(*seq);
        }

    private:
        static Seq seq;
        typedef typename Seq::reference reference;
        static void consume(reference ref);
    };

    template <class Seq, class T>
    class WritableSequence
    {
    public:
        BOOST_CONCEPT_USAGE(WritableSequence)
        {
            *seq = value;
        }

    private:
        static Seq seq;
        static T value;
    };
}
// namespace concepts
}
// namespace ural

#endif
// Z_URAL_CONCEPTS_HPP_INCLUDED
