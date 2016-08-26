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

#include <ural/functional/make_callable.hpp>
#include <ural/type_traits.hpp>
#include <ural/defs.hpp>

#include <boost/concept_check.hpp>
#include <boost/concept/usage.hpp>

#include <type_traits>

/// @brief Макрос для вывода сообщения о несоответствии концепции
#define URAL_CONCEPT_ERROR_MSG(T, Concept) #T " is not " #Concept

/// @brief Макрос для проверки соответствия концепции
#define URAL_CONCEPT_ASSERT(T, Concept)\
    static_assert(Concept<T>(), URAL_CONCEPT_ERROR_MSG(T, Concept) )

namespace ural
{
inline namespace v1
{
    template <class... Types>
    using common_type_t = typename std::common_type<Types...>::type;

    /** @brief Класс-характеристика для определения типа элемента или
    указываемого объекта
    @tparam T тип
    */
    template <class T, class = void>
    struct value_type
    {};

    /** @brief Специализация для встроенных указателей
    @tparam T тип указываемого объекта
    */
    template <class T>
    struct value_type<T*, void>
     : std::enable_if<!std::is_void<T>::value,
                      typename std::remove_cv<T>::type>
    {};

    /** @brief Специализация для встроенных массивов известной длины
    @tparam T тип элемента
    @tparam N количество элементов
    */
    template <class T, std::size_t N>
    struct value_type<T[N]>
     : declare_type<T>
    {};

    /** @brief Специализация для встроенных массивов неизвестной длины
    @tparam T тип элемента
    */
    template <class T>
    struct value_type<T[]>
     : declare_type<T>
    {};

    /** @brief Специализация для классов, определяющих @c value_type
    @tparam T тип
    */
    template <class T>
    struct value_type<T, void_t<typename T::value_type>>
     : std::enable_if<!std::is_void<typename T::value_type>::value,
                      typename T::value_type>
    {};

    /** @brief Специализация для классов, определяющих @c element_type
    @tparam T тип
    */
    template <class T>
    struct value_type<T, void_t<typename T::element_type>>
     : std::enable_if<!std::is_void<typename T::element_type>::value,
                      typename T::element_type>
    {};

    /** @brief Синоним для типа элементов составного объекта и указываемого
    объекта (умных) указателей.
    @details Для типов, имеющих typedef-объявление @c value_type или
    @c element_type, результатом будет этот тип, если он не совпадает с @b void.
    Для встроенных указателей результатом будет тип указываемого объекта, если
    он не совпадает с @b void. Пользователи могут адаптироват свои классы,
    специализируя класс-характеристику @c value_type.
    @tparam T тип составного объекта или (умного) указателя
    */
    template <class T>
    using value_type_t = typename value_type<T>::type;

    template <class T, class = void>
    struct difference_type;

    template <class T>
    struct difference_type<T, void_t<typename T::difference_type>>
     : std::enable_if<!std::is_void<typename T::difference_type>::value,
                      typename T::difference_type>
    {};

    template <class T>
    struct difference_type<T, void_t<typename T::distance_type>>
    : std::enable_if<!std::is_void<typename T::distance_type>::value,
                     typename T::distance_type>
    {};

    template <class T>
    using difference_type_t = typename difference_type<T>::type;

    template <class Readable>
    using reference_type_t = decltype(*std::declval<Readable>());

    template <class F, class... Args>
    using result_type_t = decltype(std::declval<F>()(std::declval<Args>()...));

    template <class F, class... Ins>
    using indirect_callable_result_type_t
        = result_type_t<function_type_t<F>, value_type_t<Ins>...>;

/** @namespace concepts
 @brief Концепции --- коллекции требований к типам
*/
namespace concepts
{
    template <class T, class U>
    struct Same
    {
        static_assert(std::is_same<T, U>::value, "Concept violation: Same");
    };

    template <class T, class U>
    struct Convertible
    {
    public:
        /// @brief Использование
        BOOST_CONCEPT_USAGE(Convertible)
        {
            static_assert(std::is_convertible<T, U>::value, "");
        }
    };

    template <class T, class U>
    struct Common
    {
    public:
        /// @brief Использование
        BOOST_CONCEPT_USAGE(Common)
        {
            [](T t) -> CT { return std::forward<T>(t); };
            [](U u) -> CT { return std::forward<U>(u); };
        }

    private:
        using CT = common_type_t<T, U>;
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
        /// @brief Использование
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
        /// @brief Использование
        BOOST_CONCEPT_USAGE(EqualityComparable)
        {
            BOOST_CONCEPT_ASSERT((concepts::Common<T, U>));
            BOOST_CONCEPT_ASSERT((concepts::EqualityComparable<T>));
            BOOST_CONCEPT_ASSERT((concepts::EqualityComparable<U>));
            BOOST_CONCEPT_ASSERT((concepts::EqualityComparable<common_type_t<T, U>>));

            using ::ural::experimental::value_consumer;
            value_consumer<bool>() = (a == b);
            value_consumer<bool>() = (a != b);
            value_consumer<bool>() = (b == a);
            value_consumer<bool>() = (b != a);

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
        /// @brief Использование
        BOOST_CONCEPT_USAGE(EqualityComparable)
        {
            using ::ural::experimental::value_consumer;

            value_consumer<bool>() = (a == b);
            value_consumer<bool>() = (a != b);
            value_consumer<bool>() = (b == a);
            value_consumer<bool>() = (b != a);

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
        /// @brief Использование
        BOOST_CONCEPT_USAGE(TotallyOrdered)
        {
            BOOST_CONCEPT_ASSERT((concepts::Common<T, U>));
            BOOST_CONCEPT_ASSERT((concepts::TotallyOrdered<T>));
            BOOST_CONCEPT_ASSERT((concepts::TotallyOrdered<U>));
            BOOST_CONCEPT_ASSERT((concepts::TotallyOrdered<common_type_t<T, U>>));
            BOOST_CONCEPT_ASSERT((concepts::EqualityComparable<T, U>));

            using ::ural::experimental::value_consumer;
            value_consumer<bool>() = a < b;
            value_consumer<bool>() = a > b;
            value_consumer<bool>() = a <= b;
            value_consumer<bool>() = a >= b;

            value_consumer<bool>() = b < a;
            value_consumer<bool>() = b > a;
            value_consumer<bool>() = b <= a;
            value_consumer<bool>() = b >= a;

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
        /// @brief Использование
        BOOST_CONCEPT_USAGE(TotallyOrdered)
        {
            BOOST_CONCEPT_ASSERT((concepts::EqualityComparable<T>));

            using ::ural::experimental::value_consumer;
            value_consumer<bool>() = a < b;
            value_consumer<bool>() = a > b;
            value_consumer<bool>() = a <= b;
            value_consumer<bool>() = a >= b;

            // @todo Семантические требования
        }

    private:
        static T a;
        static T b;
    };

    /** @brief Концепция курсора, допускающего чтение
    @tparam Cursor тип курсора, для которого проверяется концепция
    */
    template <class Cursor>
    class Readable
    {
    public:
        /// @brief Примеры использования
        BOOST_CONCEPT_USAGE(Readable)
        {
            // @todo нужно ли это требование ?
            // static_assert(concepts::Semiregular<Cursor>(), "");

            typedef decltype(*cur) Result;

            static_assert(std::is_convertible<Result, Value const &>::value, "");
        }

    private:
        using Value = ::ural::value_type_t<Cursor>;
        static Cursor cur;
    };

    template <class Cur>
    using ReadableCursor = Readable<Cur>;

    template <class Out, class T>
    struct MoveWritable
    {
    public:
        /// @brief Использование
        BOOST_CONCEPT_USAGE(MoveWritable)
        {
            [](Out out, T && v){ *out = std::move(v); };
        }
    };

    /** @brief Конпцепция курсора, допускающего запись
    @tparam Seq тип курсора, для которого проверяется концепция
    @tparam T тип записываемого значения
    */
    template <class Seq, class T>
    class Writable
    {
    public:
        /// @brief Примеры использования
        BOOST_CONCEPT_USAGE(Writable)
        {
            /* decltype нужен, так как make_value не определена, а static_assert
            нужен для того, чтобы подавить предупреждение о том, что тип
            AssignResult объявлен локально, но не используется.
            */
            using AssignResult = decltype(*seq = make_value());
            static_assert(std::is_same<AssignResult, AssignResult>::value, "");

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
    using WritableCursor = Writable<Seq, T>;

    template <class T>
    struct WeakIncrementable
     : private Semiregular<T>
     , private Same<T &, decltype(++ std::declval<T&>())>
    {
    public:
        /// @brief Примеры использования
        BOOST_CONCEPT_USAGE(WeakIncrementable)
        {
            // @todo как в Range Extensions?
        }
    };

    template <class T>
    struct Incrementable
     : private Regular<T>
     , private concepts::WeakIncrementable<T>
     , private concepts::Same<T, decltype(std::declval<T&>()++)>
    {};

    template <class F, class... Args>
    struct Function
     : concepts::Destructible<F>
     , concepts::CopyConstructible<F>
    {
    public:
        /// @brief Использование
        BOOST_CONCEPT_USAGE(Function)
        {
            static_assert(std::is_same<F *, decltype(&f)>::value, "");
            static_assert(noexcept(f.~F()), "");


            typedef decltype(f(std::declval<Args>()...)) Result;
            static_assert(std::is_same<result_type_t<F, Args...>, Result>::value, "");
        }
        // @todo Нужно ли проверять new и delete?
        // @todo Как проверить, что &f == std::adderssof(f)?

    private:
        using Result = result_type_t<F, Args...>;
        static F f;
    };

    template <class F, class... Args>
    struct RegularFunction
     : concepts::Function<F, Args...>
    {};

    template <class F, class... Args>
    struct Predicate
     : concepts::RegularFunction<F, Args...>
     , concepts::Convertible<result_type_t<F, Args...>, bool>
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
        /// @brief Использование
        BOOST_CONCEPT_USAGE(Relation)
        {
            BOOST_CONCEPT_ASSERT((concepts::Relation<R, T>));
            BOOST_CONCEPT_ASSERT((concepts::Relation<R, U>));
            BOOST_CONCEPT_ASSERT((concepts::Common<T, U>));
            BOOST_CONCEPT_ASSERT((concepts::Relation<R, common_type_t<T, U>>));

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
     : Function<function_type_t<F>, reference_type_t<Seqs>...>
    {
        // @todo Проверить, что Seqs - Readable
    };

    template <class F, class... Seqs>
    struct IndirectPredicate
     : Predicate<function_type_t<F>, value_type_t<Seqs>...>
    {
        // @todo Проверить, что Seqs - Readable
    };

    template <class F, class S1, class S2 = S1>
    struct IndirectRelation
     : concepts::Relation<function_type_t<F>, value_type_t<S1>, value_type_t<S2>>
    {
    public:
        /// @brief Использование
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
}
// namespace concepts

namespace archetypes
{
    /// @brief Архетип генератора равномерно распределённых случайных чисел
    class URNG_archetype
    {
    public:
        /// @brief Тип результата
        typedef size_t result_type;

        /** @brief Оператор вызова функции
        @return <tt> this->min() </tt>
        */
        result_type operator()() const
        {
            return this->min();
        }

        /** @brief Наименьшее возможное значение
        @return Наименьшее возможное значение
        */
        constexpr static result_type min URAL_PREVENT_MACRO_SUBSTITUTION ()
        {
            return 0;
        }

        /** @brief Наибольшее возможное значение
        @return Наибольшее возможное значение
        */
        constexpr static result_type max URAL_PREVENT_MACRO_SUBSTITUTION  ()
        {
            return 1;
        }

    private:
    };
}
// namespace archetypes

namespace concepts
{
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

            ::ural::archetypes::URNG_archetype g;

            using ::ural::experimental::value_consumer;
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
// namespace v1
}
// namespace ural

#endif
// Z_URAL_CONCEPTS_HPP_INCLUDED
