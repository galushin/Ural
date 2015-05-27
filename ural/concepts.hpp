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
 @todo избавиться от лямбда-функций в требованиях
*/

#include <ural/type_traits.hpp>
#include <ural/archetypes.hpp>
#include <ural/defs.hpp>
#include <ural/sequence/base.hpp>

#include <boost/concept_check.hpp>
#include <boost/concept/usage.hpp>
#include <boost/type_traits.hpp>

#include <type_traits>

/// @brief Макрос для вывода сообщения о несоответствии концепции
#define URAL_CONCEPT_ERROR_MSG(T, Concept) #T " is not " #Concept

/// @brief Макрос для проверки соответствия концепции
#define URAL_CONCEPT_ASSERT(T, Concept)\
    static_assert(Concept<T>(), URAL_CONCEPT_ERROR_MSG(T, Concept) )

namespace ural
{
    template <class T, class U>
    using CommonType = typename std::common_type<T, U>::type;

    /**
    @todo Покрыть тестами: массивы известной и неизвестной длины,
    типы c T::element_type
    */
    template <class T, class = void>
    struct value_type
    {};

    template <class T>
    struct value_type<T*, void>
     : std::enable_if<!std::is_void<T>::value,
                      typename std::remove_cv<T>::type>
    {};

    template <class T>
    struct value_type<T, void_t<typename T::value_type>>
     : std::enable_if<!std::is_void<typename T::value_type>::value,
                      typename T::value_type>
    {};

    template <class T>
    using ValueType = typename value_type<T>::type;

    // @todo улучшить реализацию
    template <class Sequence>
    using DifferenceType = typename Sequence::distance_type;

    template <class Readable>
    using ReferenceType = decltype(*std::declval<Readable>());

    template <class F, class... Args>
    using ResultType = decltype(std::declval<F>()(std::declval<Args>()...));

    template <class F, class... Ins>
    using IndirectCallableResultType
        = ResultType<FunctionType<F>, ValueType<Ins>...>;

    // @todo найти место для этого
    template <class S>
    using SequenceType = decltype(sequence(std::declval<S>()));

/** @namespace concepts
 @brief Концепции --- коллекции требований к типам
*/
namespace concepts
{
    template <class T, class U>
    struct Convertible
    {
    public:
        BOOST_CONCEPT_USAGE(Convertible)
        {
            static_assert(std::is_convertible<T, U>::value, "");
        }
    };

    template <class T, class U>
    struct Common
    {
    public:
        BOOST_CONCEPT_USAGE(Common)
        {
            [](T t) -> CT { return std::forward<T>(t); };
            [](U u) -> CT { return std::forward<U>(u); };
        }

    private:
        typedef CommonType<T, U> CT;
    };

    template <class T>
    struct MoveConstructible
    {
        static_assert(std::is_move_constructible<T>::value, "");
    };

    template <class T>
    struct CopyConstructible
     : MoveConstructible<T>
    {
        static_assert(std::is_copy_constructible<T>::value, "");
    };

    template <class T>
    struct Destructible
    {
        static_assert(std::is_destructible<T>::value, "");
    };

    template <class T, class U = T>
    struct Swappable
    {
    public:
        BOOST_CONCEPT_USAGE(Swappable)
        {
            using std::swap;
            [](T t, U u) { swap(std::forward<T>(t), std::forward<U>(u)); };
            [](T t, U u) { swap(std::forward<U>(u), std::forward<T>(t)); };
        }
    };

    /** @brief Концепция-функция "допускающий копирующее присваивание"
    @tparam T тип, проверяемый на соответствие концепции
    */
    template <class T>
    constexpr bool CopyAssignable()
    {
        return std::is_copy_assignable<T>::value;
    }

    /** @brief Концепция "Полурегулярный тип"
    @tparam T тип, проверяемый на соответствие концепции
    */
    template <class T>
    class Semiregular
    {
        // @todo В соответствии с range extensions
        static_assert(concepts::CopyAssignable<T>(), "");
    };

    /** @brief Концепция-функция "допускающий проверку на равенство"
    @tparam T тип, проверяемый на соответствие концепции
    */
    template <class T, class U = T>
    class EqualityComparable
    {
    public:
        BOOST_CONCEPT_USAGE(EqualityComparable)
        {
            BOOST_CONCEPT_ASSERT((concepts::Common<T, U>));
            BOOST_CONCEPT_ASSERT((concepts::EqualityComparable<T>));
            BOOST_CONCEPT_ASSERT((concepts::EqualityComparable<U>));
            BOOST_CONCEPT_ASSERT((concepts::EqualityComparable<CommonType<T, U>>));

            ural::value_consumer<bool>() = (a == b);
            ural::value_consumer<bool>() = (a != b);
            ural::value_consumer<bool>() = (b == a);
            ural::value_consumer<bool>() = (b != a);

            // @todo семантические требования
        }

    private:
        static T a;
        static U b;
    };

    template <class T>
    class EqualityComparable<T>
    {
    public:
        BOOST_CONCEPT_USAGE(EqualityComparable)
        {
            ural::value_consumer<bool>() = (a == b);
            ural::value_consumer<bool>() = (a != b);
            ural::value_consumer<bool>() = (b == a);
            ural::value_consumer<bool>() = (b != a);

            // @todo семантические требования
        }

    private:
        static T a;
        static T b;
    };

    /** @brief Концепция регулярного типа
    @tparam T тип, для которого проверяется концепция
    */
    template <class T>
    class Regular
     : Semiregular<T>
     , EqualityComparable<T>
    {};

    template <class T, class U = T>
    struct TotallyOrdered
    {
    public:
        BOOST_CONCEPT_USAGE(TotallyOrdered)
        {
            BOOST_CONCEPT_ASSERT((concepts::Common<T, U>));
            BOOST_CONCEPT_ASSERT((concepts::TotallyOrdered<T>));
            BOOST_CONCEPT_ASSERT((concepts::TotallyOrdered<U>));
            BOOST_CONCEPT_ASSERT((concepts::TotallyOrdered<CommonType<T, U>>));
            BOOST_CONCEPT_ASSERT((concepts::EqualityComparable<T, U>));

            ural::value_consumer<bool>() = a < b;
            ural::value_consumer<bool>() = a > b;
            ural::value_consumer<bool>() = a <= b;
            ural::value_consumer<bool>() = a >= b;

            ural::value_consumer<bool>() = b < a;
            ural::value_consumer<bool>() = b > a;
            ural::value_consumer<bool>() = b <= a;
            ural::value_consumer<bool>() = b >= a;

            // @todo Семантические требования
        }

    private:
        static T a;
        static U b;
    };

    template <class T>
    struct TotallyOrdered<T>
    {
    public:
        BOOST_CONCEPT_USAGE(TotallyOrdered)
        {
            BOOST_CONCEPT_ASSERT((concepts::EqualityComparable<T>));

            ural::value_consumer<bool>() = a < b;
            ural::value_consumer<bool>() = a > b;
            ural::value_consumer<bool>() = a <= b;
            ural::value_consumer<bool>() = a >= b;

            // @todo Семантические требования
        }

    private:
        static T a;
        static T b;
    };

    /** @brief Концепция последовательности, допускающей чтение
    @tparam Seq тип последовательности, для которого проверяется концепция
    */
    template <class Seq>
    class Readable
    {
    public:
        /// @brief Примеры использования
        BOOST_CONCEPT_USAGE(Readable)
        {
            // @todo нужно ли это требование ?
            // static_assert(concepts::Semiregular<Seq>(), "");

            typedef decltype(*seq) Result;

            static_assert(std::is_convertible<Result, Value const &>::value, "");
        }

    private:
        typedef typename ::ural::ValueType<Seq> Value;
        static Seq seq;
    };

    template <class Seq>
    using ReadableSequence = Readable<Seq>;

    template <class Out, class T>
    struct MoveWritable
    {
    public:
        BOOST_CONCEPT_USAGE(MoveWritable)
        {
            [](Out out, T && v){ *out = std::move(v); };
        }
    };

    /** @brief Конпцепция последовательности, допускающей запись
    @tparam Seq тип последовательности, для которого проверяется концепция
    @tparam T тип записываемого значения
    */
    template <class Seq, class T>
    class Writable
    {
    public:
        /// @brief Примеры использования
        BOOST_CONCEPT_USAGE(Writable)
        {
            typedef decltype(*seq = make_value()) AssignResult;

            // @todo begin?
            /* @todo OutputIterator
            Проблема в том, что концепция boost::OutputIterator<I, T> объявляет
            переменную типа T, что приводит к проблемам, когда T -- ссылка
            */
        }

    private:
        static Seq seq;
        static T make_value();
    };

    template <class Seq, class T>
    using WritableSequence = Writable<Seq, T>;

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
            seq.pop_front();
            // Постфиксный инкремент требует создания копий

            value_consumer<ural::single_pass_traversal_tag>() = traversal_tag{};
        }
    private:
        // @todo потребовать наличия типа расстояния (раз есть ++)
        static Seq seq;
        typedef typename Seq::traversal_tag traversal_tag;
    };

    template <class Seq>
    struct InputSequence
    {
        // @todo Как в Range extensions
    public:
        BOOST_CONCEPT_USAGE(InputSequence)
        {
            BOOST_CONCEPT_ASSERT((concepts::SinglePassSequence<Seq>));
            BOOST_CONCEPT_ASSERT((concepts::Readable<Seq>));
        }
    };

    template <class Seq, class T>
    struct OutputSequence
    {
    public:
        BOOST_CONCEPT_USAGE(OutputSequence)
        {
            BOOST_CONCEPT_ASSERT((concepts::SinglePassSequence<Seq>));
            BOOST_CONCEPT_ASSERT((concepts::Writable<Seq, T>));
        }
    };

    /** @brief Концепция прямой последовательности
    @tparam тип последовательности, для которого проверяется концепция
    @todo Разделить на конечные и бесконечные, в конечные добавить exhaust_front
    @todo Добавить функцию original
    */
    template <class Seq>
    class ForwardSequence
     : InputSequence<Seq>
    {
    public:
        /// @brief Проверка неявных интерфейсов
        BOOST_CONCEPT_USAGE(ForwardSequence)
        {
            BOOST_CONCEPT_ASSERT((concepts::EqualityComparable<Seq>));

            seq.shrink_front();
            seq ++;
            seq.traversed_front();
            // @todo Проверить, что тип traversed_front совпадает с Seq или
            // tf - прямая последовательность
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
            seq.shrink_back();
            seq.traversed_back();

            // @todo Проверить, что traversed_back либо BidirectionalSequence,
            // либо совпадает с Seq

            seq.exhaust_back();
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

    template <class Seq, class Out>
    struct IndirectlyMovable
    {
        BOOST_CONCEPT_USAGE(IndirectlyMovable)
        {
            // @todo нужно ли это static_assert(concepts::Semiregular<Out>(), "");?
            BOOST_CONCEPT_ASSERT((concepts::Readable<Seq>));
            BOOST_CONCEPT_ASSERT((concepts::MoveWritable<Out, ReferenceType<Seq>>));
        }
    };

    template <class Seq, class Out>
    struct IndirectlyCopyable
    {
        BOOST_CONCEPT_USAGE(IndirectlyCopyable)
        {
            // @todo нужно ли это static_assert(concepts::Semiregular<Seq>(), "");?

            BOOST_CONCEPT_ASSERT((concepts::Readable<Seq>));
            // @todo нужно ли это BOOST_CONCEPT_ASSERT((concepts::IndirectlyMovable<Seq, Out>))?;
            BOOST_CONCEPT_ASSERT((concepts::Writable<Out, ReferenceType<Seq>>));
        }
    };

    template <class S1, class S2>
    class IndirectlySwappable
    {
    public:
        BOOST_CONCEPT_USAGE(IndirectlySwappable)
        {
            BOOST_CONCEPT_ASSERT((concepts::Readable<S1>));
            BOOST_CONCEPT_ASSERT((concepts::Readable<S2>));
            BOOST_CONCEPT_ASSERT((concepts::Swappable<ReferenceType<S1>,
                                                      ReferenceType<S2>>));
            BOOST_CONCEPT_ASSERT((concepts::Swappable<ReferenceType<S1>>));
            BOOST_CONCEPT_ASSERT((concepts::Swappable<ReferenceType<S2>>));
        }
    };

    template <class F, class... Args>
    struct Function
     : concepts::Destructible<F>
     , concepts::CopyConstructible<F>
    {
    public:
        BOOST_CONCEPT_USAGE(Function)
        {
            static_assert(std::is_same<F *, decltype(&f)>::value, "");
            static_assert(noexcept(f.~F()), "");


            typedef decltype(f(std::declval<Args>()...)) Result;
            static_assert(std::is_same<ResultType<F, Args...>, Result>::value, "");
        }
        // @todo Нужно ли проверять new и delete?
        // @todo Как проверить, что &f == std::adderssof(f)?

    private:
        using Result = ResultType<F, Args...>;
        static F f;
    };

    template <class F, class... Args>
    struct RegularFunction
     : concepts::Function<F, Args...>
    {};

    template <class F, class... Args>
    struct Predicate
     : concepts::RegularFunction<F, Args...>
     , concepts::Convertible<ResultType<F, Args...>, bool>
    {};

    template <class R, class... Args>
    struct Relation;

    template <class R, class T>
    struct Relation<R, T>
     : concepts::Predicate<R, T, T>
    {};

    template <class R, class T, class U>
    struct Relation<R, T, U>
    {
    public:
        BOOST_CONCEPT_USAGE(Relation)
        {
            BOOST_CONCEPT_ASSERT((concepts::Relation<R, T>));
            BOOST_CONCEPT_ASSERT((concepts::Relation<R, U>));
            BOOST_CONCEPT_ASSERT((concepts::Common<T, U>));
            BOOST_CONCEPT_ASSERT((concepts::Relation<R, CommonType<T, U>>));

            BOOST_CONCEPT_ASSERT((Convertible<decltype(r(a, b)), bool>));
            BOOST_CONCEPT_ASSERT((Convertible<decltype(r(b, a)), bool>));
        }

    private:
        static R r;
        static T a;
        static T b;
    };

    template <class F, class... Seqs>
    struct IndirectCallable
     : Function<FunctionType<F>, ReferenceType<Seqs>...>
    {
        // @todo Проверить, что Seqs - Readable
    };

    template <class F, class... Seqs>
    struct IndirectPredicate
     : Predicate<FunctionType<F>, ValueType<Seqs>...>
    {
        // @todo Проверить, что Seqs - Readable
    };

    template <class F, class S1, class S2 = S1>
    struct IndirectRelation
     : concepts::Relation<FunctionType<F>, ValueType<S1>, ValueType<S2>>
    {
    public:
        BOOST_CONCEPT_USAGE(IndirectRelation)
        {
            BOOST_CONCEPT_ASSERT((concepts::Readable<S1>));
            BOOST_CONCEPT_ASSERT((concepts::Readable<S2>));
        }
    };

    template <class Seq1, class Seq2, class R>
    struct IndirectlyComparable
     : IndirectRelation<R, Seq1, Seq2>
    {};

    template <class Seq>
    struct Permutable
     : concepts::ForwardSequence<Seq>
     // @todo Уточнить второй шаблонный параметр
     , concepts::IndirectlyMovable<Seq, Seq>
    {
    public:
        BOOST_CONCEPT_USAGE(Permutable)
        {
            BOOST_CONCEPT_ASSERT((concepts::Semiregular<ValueType<Seq>>));
        }
    };

    template <class I1, class I2, class O, class R = ural::less<>>
    struct Mergeable
    {
    public:
        BOOST_CONCEPT_USAGE(Mergeable)
        {
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<I1>));
            BOOST_CONCEPT_ASSERT((concepts::InputSequence<I2>));
            BOOST_CONCEPT_ASSERT((concepts::SinglePassSequence<O>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyCopyable<I1, O>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyCopyable<I2, O>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectlyComparable<I1, I2, R>));

            // @todo R должно быть строгим слабым упорядочением
            // @todo Если R == less<>, то значения I1 и I2 должны быть TotallyOrdered
        }
    };

    template <class S, class R = ::ural::less<>>
    struct Sortable
    {
    public:
        BOOST_CONCEPT_USAGE(Sortable)
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<S>));
            BOOST_CONCEPT_ASSERT((concepts::Permutable<S>));
            BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<R, S>));
        }
    };

    template <class S>
    struct Sequenced
    {
        // @todo требования
    private:
        typedef SequenceType<S> sequence_type;
    };

    template <class S>
    struct SinglePassSequenced
     : Sequenced<S>
     , SinglePassSequence<SequenceType<S>>
    {};

    template <class S>
    struct InputSequenced
     : concepts::SinglePassSequenced<S>
     , concepts::InputSequence<::ural::SequenceType<S>>
    {};

    template <class S>
    struct ForwardSequenced
     : concepts::InputSequenced<S>
     , concepts::ForwardSequence<::ural::SequenceType<S>>
    {};

    template <class S>
    struct BidirectionalSequenced
     : concepts::ForwardSequenced<S>
     , concepts::BidirectionalSequence<::ural::SequenceType<S>>
    {};

    template <class S>
    struct RandomAccessSequenced
     : concepts::BidirectionalSequenced<S>
     , concepts::RandomAccessSequence<::ural::SequenceType<S>>
    {};

    /** @brief Концепция вызываемого объекта
    @tparam F тип объекта, для которого проверяется концепция
    @tparam Signature сигнатура
    */
    template <class F, class Signature>
    struct Callable;

    /** @brief Специалиазация
    @tparam F тип объекта, для которого проверяется концепция
    @tparam R тип возвращаемого значения
    @tparam Args типы аргументов
    @note Если @c R совпадает с @c void, то тип возвращаемого значения
    не проверяется.
    */
    template <class F, class R, class... Args>
    struct Callable<F, R(Args...)>
    {
    public:
        /// @brief Примеры использования
        BOOST_CONCEPT_USAGE(Callable)
        {
            typedef decltype(f_(std::declval<Args>()...)) result_type;

            // Подавляем предупреждение компилятора
            static_assert(std::is_same<R, void>::value
                          || std::is_convertible<result_type, R>::value, "");
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
        /// @brief Примеры использования
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

    /** @brief Концепция "Распределение вероятностей" (Стандарт C++ 26.5.1.6)
    @tparam D тип, проверяемый на соответствие концепции
    */
    template <class D>
    class RandomDistribution
     : boost::CopyConstructible<D>
    {
    public:
        /// @brief Примеры использования
        BOOST_CONCEPT_USAGE(RandomDistribution)
        {
            URAL_CONCEPT_ASSERT(D, CopyAssignable);

            D d0;

            static_assert(std::is_same<decltype(d0.reset()), void>::value, "");
            P p0 = d0.param();

            BOOST_CONCEPT_ASSERT((boost::CopyConstructible<P>));
            URAL_CONCEPT_ASSERT(P, concepts::CopyAssignable);
            BOOST_CONCEPT_ASSERT((concepts::EqualityComparable<P>));

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

            // Ввод/вывод
            auto const d0_c = d0;
            value_consumer<std::ostream&>() = (get_ostream() << d0_c);

            value_consumer<std::istream&>() = (get_istream() >> d0);

            // @todo Конструкторы
            // @todo Соответствие концепции распределения стандарта C++11
        }

    private:
        typedef typename D::result_type T;
        typedef typename D::param_type P;
        typedef typename P::distribution_type PD;

        static std::istream & get_istream();
        static std::ostream & get_ostream();
    };
}
// namespace concepts
}
// namespace ural

#endif
// Z_URAL_CONCEPTS_HPP_INCLUDED
