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

#include <ural/archetypes.hpp>
#include <ural/defs.hpp>
#include <ural/sequence/base.hpp>

#include <boost/concept_check.hpp>
#include <boost/concept/usage.hpp>
#include <boost/type_traits.hpp>

#include <type_traits>

#define URAL_CONCEPT_ERROR_MSG(T, Concept) #T " is not " #Concept

#define URAL_CONCEPT_ASSERT(T, Concept)\
    static_assert(Concept<T>(), URAL_CONCEPT_ERROR_MSG(T, Concept) )

namespace ural
{
namespace concepts
{
    /** @brief Концепция "Полурегулярный тип"
    */
    template <class T>
    constexpr bool SemiRegular()
    {
        return std::is_copy_constructible<T>::value
            && std::is_copy_assignable<T>::value;
    }

    template <class T>
    constexpr bool CopyAssignable()
    {
        return std::is_copy_assignable<T>::value;
    }

    template <class T>
    constexpr bool EqualityComparable()
    {
        return boost::has_equal_to<T>::value;
    }

    /** @brief Концепция регулярного типа
    @tparam T тип, для которого проверяется концепция
    */
    template <class T>
    constexpr bool Regular()
    {
        return SemiRegular<T>() && EqualityComparable<T>();
    }

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
    {
    public:
        /// @brief Проверка неявных интерфейсов
        BOOST_CONCEPT_USAGE(ForwardSequence)
        {
            URAL_CONCEPT_ASSERT(Seq, EqualityComparable);
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
        typedef typename Seq::distance_type distance_type;
        typedef typename Seq::pointer pointer;

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
            // @todo Убрать static_cast, чтобы требовать неявное преобразование
            typedef decltype(static_cast<R>(f_(std::declval<Args>()...))) result_type;

            // Подавляем предупреждение компилятора
            static_assert(std::is_convertible<result_type, R>::value, "");
        }

    private:
        static F f_;
    };

    /** @brief Концепция генератора равномерно распределённых случайных чисел

    Основан на Working draft standard n3485, пункт 26.5.1.3.
    */
    template <class G>
    class Uniform_random_number_generator
    {
    public:
        BOOST_CONCEPT_USAGE(Uniform_random_number_generator)
        {
            typedef typename G::result_type result_type;

            static_assert(std::is_unsigned<result_type>::value,
                          "result_type must be unsigned integer type");

            BOOST_CONCEPT_ASSERT((Callable<G, result_type()>));

            constexpr result_type x_min = G::min();
            constexpr result_type x_max = G::max();

            static_assert(x_min < x_max, "Min must be lesser then max");
        }
    };

    template <class D>
    class RandomDistribution
     : boost::CopyConstructible<D>
    {
    public:
        BOOST_CONCEPT_USAGE(RandomDistribution)
        {
            URAL_CONCEPT_ASSERT(D, CopyAssignable);

            D d0;

            static_assert(std::is_same<decltype(d0.reset()), void>::value, "");
            P p0 = d0.param();

            BOOST_CONCEPT_ASSERT((boost::CopyConstructible<P>));
            URAL_CONCEPT_ASSERT(P, CopyAssignable);
            URAL_CONCEPT_ASSERT(P, EqualityComparable);

            static_assert(std::is_same<D, PD>::value, "");

            D d1(p0);
            d0.param(p0);

            ural::archetypes::URNG_archetype g;

            value_consumer<T>() = d0(g);
            value_consumer<T>() = d0(g, p0);

            value_consumer<T>() = d0.min();
            value_consumer<T>() = d0.max();

            value_consumer<bool>() = (d0 == d1);
            value_consumer<bool>() = (d0 != d1);

            // @todo Ввод/вывод
            // @todo Конструкторы
        }

    private:
        typedef typename D::result_type T;
        typedef typename D::param_type P;
        typedef typename P::distribution_type PD;
    };
}
// namespace concepts
}
// namespace ural

#endif
// Z_URAL_CONCEPTS_HPP_INCLUDED
