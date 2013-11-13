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
    /** @brief Концепция регулярного типа
    @tparam T тип, для которого проверяется концепция
    */
    template <class T>
    class Regular
    {};

    /** @brief Концепция однопроходной последовательности
    @tparam тип последовательности, для которого проверяется концепция
    */
    template <class Seq>
    class SinglePassSequence
    {
    public:
        /// @brief Примеры использования
        BOOST_CONCEPT_USAGE(SinglePassSequence)
        {
            !seq;
            ++ seq;
            seq ++;
        }
    private:
        static Seq seq;
    };

    /** @brief Концепция последовательности, допускающей чтение
    @tparam Seq тип последовательности, для которого проверяется концепция
    */
    template <class Seq>
    class ReadableSequence
    {
    public:
        /// @brief Примеры использования
        BOOST_CONCEPT_USAGE(ReadableSequence)
        {
            consume(*seq);
        }

    private:
        static Seq seq;
        typedef typename Seq::reference reference;
        static void consume(reference ref);
    };

    /** @brief Конпцепция последовательности, допускающей запись
    @tparam Seq тип последовательности, для которого проверяется концепция
    @tparam T тип записываемого значения
    */
    template <class Seq, class T>
    class WritableSequence
    {
    public:
        /// @brief Примеры использования
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
