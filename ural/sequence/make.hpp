#ifndef Z_URAL_SEQUENCE_MAKE_HPP_INCLUDED
#define Z_URAL_SEQUENCE_MAKE_HPP_INCLUDED

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

/** @file ural/sequence/make.hpp
 @brief Функции создания последовательностей
*/

#include <iterator>
#include <valarray>

#include <ural/sequence/cargo.hpp>
#include <ural/sequence/iostream.hpp>
#include <ural/sequence/iterator_sequence.hpp>
#include <ural/sequence/insertion.hpp>

#include <ural/concepts.hpp>

namespace ural
{
inline namespace v0
{
    /** @brief Создание последовательности на основе контейнера
    @param c контейнер
    @return <tt> iterator_cursor<decltype(c.begin())>(c.begin(), c.end())</tt>
    */
    template <class Container>
    auto cursor(Container && c)
    -> typename std::enable_if<std::is_lvalue_reference<Container>::value,
                               ::ural::iterator_cursor<decltype(c.begin())>>::type
    {
        return iterator_cursor<decltype(c.begin())>(c.begin(), c.end());
    }

    template <class Container>
    auto cursor(Container && c)
    -> typename std::enable_if<!std::is_lvalue_reference<Container>::value,
                               experimental::cargo_sequence<iterator_cursor<decltype(c.begin())>, Container>>::type
    {
        typedef ::ural::experimental::cargo_sequence<::ural::iterator_cursor<decltype(c.begin())>, Container>
            Result;
        auto seq = ::ural::iterator_cursor<decltype(c.begin())>(c.begin(), c.end());
        return Result(std::move(seq), std::move(c));
    }

    /** @brief Создание курсора на основе массива фиксированной длины
    @param x массив
    @return <tt> iterator_cursor<T *>{x, x + N} </tt>
    */
    template <class T, size_t N>
    iterator_cursor<T *>
    cursor(T (&x)[N])
    {
        return iterator_cursor<T *>{x, x + N};
    }

    /** @brief Создание курсора на основе итератора вставки в конец контейнера
    @param i итератор-вставка
    @return <tt> weak_output_iterator_cursor<Iterator, Diff>(std::move(i)) </tt>
    */
    template <class Container>
    auto cursor(std::back_insert_iterator<Container> i)
    {
        typedef std::back_insert_iterator<Container> Iterator;
        typedef DifferenceType<Container> Diff;
        return experimental::weak_output_iterator_cursor<Iterator, Diff>(std::move(i));
    }

    /** @brief Создание курсора на основе итератора вставки в начало контейнера
    @param i итератор-вставка
    @return <tt> weak_output_iterator_cursor<Iterator, Diff>(std::move(i)) </tt>
    */
    template <class Container>
    auto cursor(std::front_insert_iterator<Container> i)
    {
        typedef std::front_insert_iterator<Container> Iterator;
        typedef DifferenceType<Container> Diff;
        return experimental::weak_output_iterator_cursor<Iterator, Diff>(std::move(i));
    }

    /** @brief Создание курсора на основе итератора вставки в заданную точку
    контейнера
    @param i итератор-вставка
    @return <tt> weak_output_iterator_cursor<decltype(i), Diff>(std::move(i)) </tt>,
    где @c Diff --- <tt> DifferenceType<Container> </tt>
    */
    template <class Container>
    auto cursor(std::insert_iterator<Container> i)
    {
        typedef std::insert_iterator<Container> Iterator;
        typedef DifferenceType<Container> Diff;
        return experimental::weak_output_iterator_cursor<Iterator, Diff>(std::move(i));
    }

    //@{
    /** @brief Создание курсора на основе <tt> std::valarray </tt>
    @param c вектор для которого создаётся курсор
    @return <tt> iterator_cursor<T*>(begin(c), end(c)) </tt>
    */
    template <class T>
    iterator_cursor<T*>
    cursor(std::valarray<T> & c)
    {
        if(c.size() == 0)
        {
            return iterator_cursor<T*>(nullptr, nullptr);
        }
        else
        {
            auto first = std::addressof(c[0]);
            return iterator_cursor<T*>(first, first + c.size());
        }
    }

    template <class T>
    iterator_cursor<T const*>
    cursor(std::valarray<T> const & c)
    {
        if(c.size() == 0)
        {
            return iterator_cursor<const T*>(nullptr, nullptr);
        }
        else
        {
            auto first = std::addressof(c[0]);
            return iterator_cursor<const T*>(first, first + c.size());
        }
    }
    //@}

    /// @cond false
    namespace details
    {
        template <class T,
                  class Char = typename T::char_type,
                  class Traits = typename T::traits_type>
        struct is_derived_from_basic_istream
         : std::is_base_of<std::basic_istream<Char, Traits>, T>
        {};

        template <class T,
                  class Char = typename T::char_type,
                  class Traits = typename T::traits_type>
        struct is_derived_from_basic_ostream
         : std::is_base_of<std::basic_ostream<Char, Traits>, T>
        {};
    }
    /// @endcond

    //@{
    /** @brief Преобразование потока ввода в курсор последовательности символов
    @param is поток ввода
    */
    template <class Char, class Traits>
    auto cursor(std::basic_istream<Char, Traits> & is)
    {
        using Product = experimental::istream_cursor<std::basic_istream<Char, Traits> &,
                                                     Char, experimental::istream_get_reader>;
        return Product(is);
    }

    template <class IStream>
    auto cursor(IStream && is)
    -> typename std::enable_if<details::is_derived_from_basic_istream<IStream>::value,
                               experimental::istream_cursor<IStream, typename IStream::char_type, experimental::istream_get_reader>>::type
    {
        using Product = experimental::istream_cursor<IStream, typename IStream::char_type, experimental::istream_get_reader>;
        return Product(std::forward<IStream>(is));
    }
    //@}

    //@{
    /** @brief Преобразование потока вывода в курсор
    @param os поток вывода
    @return <tt> ::ural::make_ostream_cursor(os) </tt>
    */
    template <class Char, class Traits>
    auto cursor(std::basic_ostream<Char, Traits> & os)
    {
        return ::ural::experimental::make_ostream_cursor(os);
    }

    template <class OStream>
    auto cursor(OStream && os)
    -> typename std::enable_if<details::is_derived_from_basic_ostream<OStream>::value,
                               decltype(::ural::experimental::make_ostream_cursor(std::move(os)))>::type
    {
        return ::ural::experimental::make_ostream_cursor(std::move(os));
    }
    //@}

    //@{
    /** @brief Функция, комбинирующая @c cursor и <tt> std::forward </tt>
    @param t аргумент
    @return <tt> cursor(std::forward<Sequence>(t)) </tt>
    @todo Преобразовать в функциональный объект/шаблон переменной
    */
    template <class Sequence>
    auto cursor_fwd(typename std::remove_reference<Sequence>::type & t)
    -> decltype(cursor(std::forward<Sequence>(t)))
    {
        return cursor(std::forward<Sequence>(t));
    }

    template <class Sequence>
    auto cursor_fwd(typename std::remove_reference<Sequence>::type && t)
    -> decltype(cursor(std::forward<Sequence>(t)))
    {
        return cursor(std::forward<Sequence>(t));
    }
    //@}

    /** @brief Класс-характеристика для определения типа курсора
    @tparam S тип последовательности
    */
    template <class S>
    struct cursor_type
     : declare_type<typename std::decay<decltype(cursor(std::declval<S>()))>::type>
    {};

    /** @brief Синоним для типа курсора, определяемого классом-характеристикой
    @c cursor_type
    @tparam S тип, на основе которого создаётся курсора.
    */
    template <class S>
    using cursor_type_t = typename cursor_type<S>::type;

    /** @brief Класс характеристика для определения типа передней пройденной
    части курсора
    @tparam Cur тип курсора
    */
    template <class Cur>
    struct traversed_front_type
    {
    private:
        template <class U>
        static decltype(std::declval<U>().traversed_front())
        declare(declare_type<decltype(std::declval<U>().traversed_front())> *);

        template <class U>
        static void declare (...);

    public:
        using type = decltype(declare<Cur>(nullptr));
    };

    /** @brief Тип синонима для типа передней пройденной части
    последовательности, определяемой классом-характеристикой
    @c traversed_front_type
    @tparam S тип
    */
    template <class S>
    using TraversedFrontType = typename traversed_front_type<S>::type;

    namespace concepts
    {
        /** @brief Концепция однопроходной последовательности
        @tparam тип последовательности, для которого проверяется концепция
        */
        template <class Cur>
        class SinglePassCursor
        {
        public:
            /// @brief Примеры использования
            BOOST_CONCEPT_USAGE(SinglePassCursor)
            {
                using ::ural::experimental::value_consumer;

                !cur;
                value_consumer<Cur&>() = ++ cur;
                cur.pop_front();

                // Постфиксный инкремент требует создания копий

                value_consumer<single_pass_cursor_tag>() = cursor_tag{};
            }
        private:
            static Cur cur;
            using cursor_tag = typename Cur::cursor_tag;

            using difference_type = DifferenceType<Cur>;
        };

        template <class Cur>
        struct InputCursor
        {
            // @todo Как в Range extensions
        public:
            /// @brief Использование
            BOOST_CONCEPT_USAGE(InputCursor)
            {
                BOOST_CONCEPT_ASSERT((concepts::SinglePassCursor<Cur>));
                BOOST_CONCEPT_ASSERT((concepts::Readable<Cur>));
            }
        };

        template <class Cur, class T>
        struct OutputCursor
        {
        public:
            /// @brief Использование
            BOOST_CONCEPT_USAGE(OutputCursor)
            {
                BOOST_CONCEPT_ASSERT((concepts::SinglePassCursor<Cur>));
                BOOST_CONCEPT_ASSERT((concepts::Writable<Cur, T>));
            }
        };

        /** @brief Концепция прямой последовательности
        @tparam тип последовательности, для которого проверяется концепция
        */
        template <class Cur>
        class ForwardCursor
         : InputCursor<Cur>
         , Incrementable<Cur>
        {
        public:
            /// @brief Проверка неявных интерфейсов
            BOOST_CONCEPT_USAGE(ForwardCursor)
            {
                using ::ural::experimental::value_consumer;

                value_consumer<forward_cursor_tag>() = cursor_tag{};

                BOOST_CONCEPT_ASSERT((concepts::EqualityComparable<Cur>));

                value_consumer<Cur>() = cur++;

                cur.shrink_front();
                cur.traversed_front();

                value_consumer<Cur>() = cur.original();

                // @todo Проверить, что тип traversed_front совпадает с Seq или
                // tf - прямая последовательность, а также совместимость
                // типов ссылки, значения, указателя и разности для
                // самой последовательности и её передней пройденной части

                static_assert(std::is_same<decltype(cur.front()), decltype(*cur)>::value, "");
            }

        private:
            using cursor_tag = typename Cur::cursor_tag;
            static Cur cur;
        };

        template <class Cur>
        class FiniteForwardCursor
         : ForwardCursor<Cur>
        {
        public:
            /// @brief Проверка неявных интерфейсов
            BOOST_CONCEPT_USAGE(FiniteForwardCursor)
            {
                static_assert(&FiniteForwardCursor::reguire != 0, "");
            }

        private:
            void reguire(Cur)
            {}
        };

        /** @brief Концепция двустороннней последовательности
        @tparam тип последовательности, для которого проверяется концепция
        */
        template <class Cur>
        class BidirectionalCursor
         : FiniteForwardCursor<Cur>
        {
        public:
            /// @brief Проверка неявных интерфейсов
            BOOST_CONCEPT_USAGE(BidirectionalCursor)
            {
                using ::ural::experimental::value_consumer;

                cur.pop_back();
                value_consumer<reference>() = cur.back();
                cur.shrink_back();

                value_consumer<Cur>() = cur.traversed_front();
                value_consumer<Cur>() = cur.traversed_back();

                cur.exhaust_back();
                cur.exhaust_front();
            }

        private:
            typedef typename Cur::reference reference;
            static Cur cur;
        };

        /** @brief Концепция последовательности произвольного доступа
        @tparam Seq тип последовательности, для которого проверяется концепция
        */
        template <class Cur>
        class RandomAccessCursor
         : ForwardCursor<Cur>
        {
        public:
            /// @brief Проверка неявных интерфейсов
            BOOST_CONCEPT_USAGE(RandomAccessCursor)
            {
                using ::ural::experimental::value_consumer;

                value_consumer<reference>() = cur[distance_type{0}];
                value_consumer<Cur&>() = (cur += distance_type{0});
                value_consumer<distance_type>() = cur.size();

                cur.pop_back(distance_type{1});
            }

        private:
            static Cur cur;
            typedef DifferenceType<Cur> distance_type;
            typedef typename Cur::reference reference;
        };

        template <class Cur, class Out>
        struct IndirectlyMovable
        {
            /// @brief Использование
            BOOST_CONCEPT_USAGE(IndirectlyMovable)
            {
                // @todo нужно ли это static_assert(concepts::Semiregular<Out>(), "");?
                BOOST_CONCEPT_ASSERT((concepts::Readable<Cur>));
                BOOST_CONCEPT_ASSERT((concepts::MoveWritable<Out, ReferenceType<Cur>>));
            }
        };

        template <class Cur, class Out>
        struct IndirectlyCopyable
        {
            /// @brief Использование
            BOOST_CONCEPT_USAGE(IndirectlyCopyable)
            {
                // @todo нужно ли это static_assert(concepts::Semiregular<Seq>(), "");?

                BOOST_CONCEPT_ASSERT((concepts::Readable<Cur>));
                // @todo нужно ли это BOOST_CONCEPT_ASSERT((concepts::IndirectlyMovable<Seq, Out>))?;
                BOOST_CONCEPT_ASSERT((concepts::Writable<Out, ReferenceType<Cur>>));
            }
        };

        template <class Cur1, class Cur2>
        class IndirectlySwappable
        {
        public:
            /// @brief Использование
            BOOST_CONCEPT_USAGE(IndirectlySwappable)
            {
                BOOST_CONCEPT_ASSERT((concepts::Readable<Cur1>));
                BOOST_CONCEPT_ASSERT((concepts::Readable<Cur2>));
                BOOST_CONCEPT_ASSERT((concepts::Swappable<ReferenceType<Cur1>,
                                                          ReferenceType<Cur2>>));
                BOOST_CONCEPT_ASSERT((concepts::Swappable<ReferenceType<Cur1>>));
                BOOST_CONCEPT_ASSERT((concepts::Swappable<ReferenceType<Cur2>>));
            }
        };

        template <class Cur>
        struct Permutable
         : concepts::ForwardCursor<Cur>
         // @todo Уточнить второй шаблонный параметр
         , concepts::IndirectlyMovable<Cur, Cur>
        {
        public:
            /// @brief Использование
            BOOST_CONCEPT_USAGE(Permutable)
            {
                BOOST_CONCEPT_ASSERT((concepts::Semiregular<ValueType<Cur>>));
            }
        };

        template <class I1, class I2, class O, class R = ural::less<>>
        struct Mergeable
        {
        public:
            /// @brief Использование
            BOOST_CONCEPT_USAGE(Mergeable)
            {
                BOOST_CONCEPT_ASSERT((concepts::InputCursor<I1>));
                BOOST_CONCEPT_ASSERT((concepts::InputCursor<I2>));
                BOOST_CONCEPT_ASSERT((concepts::SinglePassCursor<O>));
                BOOST_CONCEPT_ASSERT((concepts::IndirectlyCopyable<I1, O>));
                BOOST_CONCEPT_ASSERT((concepts::IndirectlyCopyable<I2, O>));
                BOOST_CONCEPT_ASSERT((concepts::IndirectlyComparable<I1, I2, R>));

                // @todo R должно быть строгим слабым упорядочением
                // @todo Если R == less<>, то значения I1 и I2 должны быть TotallyOrdered
            }
        };

        template <class Cur, class R = ::ural::less<>>
        struct Sortable
        {
        public:
            /// @brief Использование
            BOOST_CONCEPT_USAGE(Sortable)
            {
                BOOST_CONCEPT_ASSERT((concepts::ForwardCursor<Cur>));
                BOOST_CONCEPT_ASSERT((concepts::Permutable<Cur>));
                BOOST_CONCEPT_ASSERT((concepts::IndirectRelation<R, Cur>));
            }
        };

        template <class S>
        struct Sequence
        {
            // В Range extensions требуется, чтобы begin и end имели константную
            // сложность. Нужно ли нам потребовать этого от sequence?
            /// @brief Использование
            BOOST_CONCEPT_USAGE(Sequence)
            {
                using Cursor =  typename std::decay<decltype(cursor(std::declval<S>()))>::type;
                static_assert(std::is_same<Cursor, cursor_type>::value, "");
            }
        private:
            using cursor_type = cursor_type_t<S>;
        };

        template <class S>
        struct SinglePassSequence
         : Sequence<S>
         , SinglePassCursor<cursor_type_t<S>>
        {};

        template <class S>
        struct InputSequence
         : concepts::SinglePassSequence<S>
         , concepts::InputCursor<cursor_type_t<S>>
        {};

        template <class S>
        struct ForwardSequence
         : concepts::InputSequence<S>
         , concepts::ForwardCursor<cursor_type_t<S>>
        {};

        template <class S>
        struct BidirectionalSequence
         : concepts::ForwardSequence<S>
         , concepts::BidirectionalCursor<cursor_type_t<S>>
        {};

        template <class S>
        struct RandomAccessSequence
         : concepts::BidirectionalSequence<S>
         , concepts::RandomAccessCursor<cursor_type_t<S>>
        {};
    }
    // namespace concepts
}
// namespace v0
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_MAKE_HPP_INCLUDED
