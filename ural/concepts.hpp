#ifndef Z_URAL_CONCEPTS_HPP_INCLUDED
#define Z_URAL_CONCEPTS_HPP_INCLUDED

/*  This file is part of Ural.

    Ural is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Ural is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Ural.  If not, see <http://www.gnu.org/licenses/>.
*/

/** @file ural/concepts.hpp
 @brief Классы для проверки концепций
*/

#include <boost/concept/usage.hpp>

#include <ural/defs.hpp>
#include <ural/sequence/base.hpp>

namespace ural
{
namespace concepts
{
    template <class T>
    class SemiRegular
    {
    public:
        /// @brief Проверка неявных интерфейсов
        BOOST_CONCEPT_USAGE(SemiRegular)
        {
            // @todo Какие ещё требования?
            T x = make();
            x = make();
        }

    private:
        static T make();
        static T value;
    };

    template <class T>
    class EqualityComparable
    {
    public:
        /// @brief Проверка неявных интерфейсов
        BOOST_CONCEPT_USAGE(EqualityComparable)
        {
            value_consumer<bool>() = (x == y);
            value_consumer<bool>() = (x != y);
        }

    private:
        static T const x;
        static T const y;
    };

    /** @brief Концепция регулярного типа
    @tparam T тип, для которого проверяется концепция
    */
    template <class T>
    class Regular
     : SemiRegular<T>
     , EqualityComparable<T>
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
            seq.pop_front();

            value_consumer<ural::single_pass_traversal_tag>() = traversal_tag{};
        }
    private:
        static Seq seq;
        typedef typename Seq::traversal_tag traversal_tag;
    };

    /** @brief Концепция прямой последовательности
    @tparam тип последовательности, для которого проверяется концепция
    */
    template <class Seq>
    class ForwardSequence
     : SinglePassSequence<Seq>
     , EqualityComparable<Seq>
    {
    public:
        /// @brief Проверка неявных интерфейсов
        BOOST_CONCEPT_USAGE(ForwardSequence)
        {
            ural::value_consumer<Seq>() = seq.traversed_front();
            seq.shrink_front();
        }

    private:
        static Seq seq;
    };

    /** @brief Концепция двустороннней последовательности
    @tparam тип последовательности, для которого проверяется концепция
    */
    template <class Seq>
    class BidirectionalSequence
     : ForwardSequence<Seq>
    {
    public:
        /// @brief Проверка неявных интерфейсов
        BOOST_CONCEPT_USAGE(BidirectionalSequence)
        {
            seq.pop_back();
            value_consumer<reference>() = seq.back();
            value_consumer<Seq>() = seq.traversed_back();
            seq.shrink_back();
        }

    private:
        typedef typename Seq::reference reference;
        static Seq seq;
    };

    /** @brief Концепция последовательности произвольного доступа
    @tparam Seq тип последовательности, для которого проверяется концепция
    */
    template <class Seq>
    class RandomAccessSequence
     : ForwardSequence<Seq>
    {
    public:
        /// @brief Проверка неявных интерфейсов
        BOOST_CONCEPT_USAGE(RandomAccessSequence)
        {
            value_consumer<reference>() = seq[distance_type{0}];
            value_consumer<Seq&>() = (seq += distance_type{0});
            value_consumer<distance_type>() = seq.size();

            seq.pop_back(distance_type{1});
        }

    private:
        static Seq seq;
        typedef typename Seq::distance_type distance_type;
        typedef typename Seq::reference reference;
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
            decltype(consume_ref(*seq));
            decltype(consume_ref(seq.front()));

            decltype(consume_value(*seq));
            decltype(consume_value(seq.front()));
        }

    private:
        static Seq seq;
        typedef typename Seq::reference reference;
        typedef typename Seq::value_type value_type;

        static void consume_ref(reference);
        static void consume_value(value_type);
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
        }

    private:
        static Seq seq;
        static T make_value();
    };

    /** @brief Концепция вызываемого объекта
    @tparam F тип объекта, для которого проверяется концепция
    @tparam Signature сигнатура
    */
    template <class F, class Signature>
    class Callable;

    /** @brief Специалиазация
    @tparam F тип объекта, для которого проверяется концепция
    @tparam R тип возвращаемого значения
    @tparam Args типы аргументов
    */
    template <class F, class R, class... Args>
    class Callable<F, R(Args...)>
    {
    public:
        /// @brief Примеры использования
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
