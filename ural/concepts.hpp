#ifndef Z_URAL_CONCEPTS_HPP_INCLUDED
#define Z_URAL_CONCEPTS_HPP_INCLUDED

/** @file ural/concepts.hpp
 @brief Классы для проверки концепций
 @todo Реализовать
*/

#include <boost/concept/usage.hpp>

#include <ural/defs.hpp>

namespace ural
{
namespace concepts
{
    template <class T>
    class SemiRegular
    {
    public:
        BOOST_CONCEPT_USAGE(SemiRegular)
        {}

    private:
        static T make();
        static T value;
    };

    /** @brief Концепция регулярного типа
    @tparam T тип, для которого проверяется концепция
    */
    template <class T>
    class Regular
     : SemiRegular<T>
    {};

    template <class T>
    class EqualityComparable
    {
    public:
        BOOST_CONCEPT_USAGE(EqualityComparable)
        {
            value_consumer<bool>() = (x == y);
            value_consumer<bool>() = (x != y);
        }

    private:
        static T const & make();
        static T const x;
        static T const y;
    };

    /** @brief Концепция однопроходной последовательности
    @tparam тип последовательности, для которого проверяется концепция
    */
    template <class Seq>
    class SinglePassSequence
    // @todo : EqualityComparable<Seq>
    {
    public:
        /// @brief Примеры использования
        BOOST_CONCEPT_USAGE(SinglePassSequence)
        {
            !seq;
            // @todo +seq <=> !!seq ?
            // @todo seq.has_more() как в Java ?
            ++ seq;
            seq ++;
            seq.pop_front();
        }
    private:
        static Seq seq;
        // @todo Категория обхода
    };

    template <class Seq>
    class ForwardSequence
     : SinglePassSequence<Seq>
    {
    public:
        BOOST_CONCEPT_USAGE(ForwardSequence)
        {
            // @todo traversed_front
        }
    private:
    };

    template <class Seq>
    class BidirectionalSequence
     : ForwardSequence<Seq>
    {
    public:
        BOOST_CONCEPT_USAGE(BidirectionalSequence)
        {
            seq.pop_back();
            // @todo Что с back
        }

    private:
        static Seq seq;
    };

    template <class Seq>
    class RandomAccessSequence
     : ForwardSequence<Seq>
    {
    public:
        BOOST_CONCEPT_USAGE(RandomAccessSequence)
        {}

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
            decltype(consume(*seq));
            decltype(consume(seq.front()));

            // @todo Проверки для value_type
        }

    private:
        static Seq seq;
        typedef typename Seq::reference reference;
        typedef typename Seq::value_type value_type;

        static void consume(reference);
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
            decltype(*seq = make_value());
            // @todo seq << make_value() ?
        }

    private:
        static Seq seq;
        static T make_value();
    };

    template <class F, class Signature>
    class Callable;

    template <class F, class R, class... Args>
    class Callable<F, R(Args...)>
    {
    public:
        BOOST_CONCEPT_USAGE(Callable)
        {
            typedef decltype(static_cast<R>(f_(std::declval<Args>()...)))
                result_type;
        }

    private:
        static F f_;
    };
}
// namespace concepts
}
// namespace ural

#endif
// Z_URAL_CONCEPTS_HPP_INCLUDED
