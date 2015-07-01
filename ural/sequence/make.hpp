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
    /** @brief Создание последовательности на основе контейнера
    @param c контейнер
    @return <tt> iterator_sequence<decltype(c.begin())>(c.begin(), c.end())</tt>
    */
    template <class Container>
    auto sequence(Container && c)
    -> typename std::enable_if<std::is_reference<Container>::value,
                               ::ural::iterator_sequence<decltype(c.begin())>>::type
    {
        return iterator_sequence<decltype(c.begin())>(c.begin(), c.end());
    }

    template <class Container>
    auto sequence(Container && c)
    -> typename std::enable_if<!std::is_reference<Container>::value,
                               ::ural::cargo_sequence<::ural::iterator_sequence<decltype(c.begin())>,
                                                      Container>>::type
    {
        typedef ::ural::cargo_sequence<::ural::iterator_sequence<decltype(c.begin())>, Container>
            Result;
        auto seq = ::ural::iterator_sequence<decltype(c.begin())>(c.begin(), c.end());
        return Result(std::move(seq), std::move(c));
    }

    /** @brief Создание последовательности на основе массива фиксированной длины
    @param x массив
    @return <tt> iterator_sequence<T *>{x, x + N} </tt>
    */
    template <class T, size_t N>
    iterator_sequence<T *>
    sequence(T (&x)[N])
    {
        return iterator_sequence<T *>{x, x + N};
    }

    /** @brief Создание последовательности на основе итератора вставки в конец
    контейнера
    @param i итератор-вставка
    @return <tt> output_iterator_sequence<decltype(i)>(std::move(i)) </tt>
    */
    template <class Container>
    weak_output_iterator_sequence<std::back_insert_iterator<Container>,
                             typename Container::difference_type>
    sequence(std::back_insert_iterator<Container> i)
    {
        typedef std::back_insert_iterator<Container> Iterator;
        typedef typename Container::difference_type Diff;
        return weak_output_iterator_sequence<Iterator, Diff>(std::move(i));
    }

    /** @brief Создание последовательности на основе итератора вставки в начало
    контейнера
    @param i итератор-вставка
    @return <tt> output_iterator_sequence<decltype(i)>(std::move(i)) </tt>
    */
    template <class Container>
    weak_output_iterator_sequence<std::front_insert_iterator<Container>,
                             typename Container::difference_type>
    sequence(std::front_insert_iterator<Container> i)
    {
        typedef std::front_insert_iterator<Container> Iterator;
        typedef typename Container::difference_type Diff;
        return weak_output_iterator_sequence<Iterator, Diff>(std::move(i));
    }

    //@{
    /** @brief Создание последовательности на основе <tt> std::valarray </tt>
    @param c вектор для которого создаётся последовательность
    @return <tt> iterator_sequence<T*>(begin(c), end(c)) </tt>
    */
    template <class T>
    iterator_sequence<T*>
    sequence(std::valarray<T> & c)
    {
        if(c.size() == 0)
        {
            return iterator_sequence<T*>(nullptr, nullptr);
        }
        else
        {
            auto first = std::addressof(c[0]);
            return iterator_sequence<T*>(first, first + c.size());
        }
    }

    template <class T>
    iterator_sequence<T const*>
    sequence(std::valarray<T> const & c)
    {
        if(c.size() == 0)
        {
            return iterator_sequence<const T*>(nullptr, nullptr);
        }
        else
        {
            auto first = std::addressof(c[0]);
            return iterator_sequence<const T*>(first, first + c.size());
        }
    }
    //@}

    /** @brief Преобразование потока ввода в последовательность символов
    @param is поток ввода
    @return <tt> ::ural::make_istream_sequence<Char>(is) </tt>
    */
    template <class Char, class Traits>
    auto sequence(std::basic_istream<Char, Traits> & is)
    -> decltype(::ural::make_istream_sequence<Char>(is))
    {
        return ::ural::make_istream_sequence<Char>(is);
    }

    /** @brief Преобразование потока вывода в последовательность символов
    @param is поток ввода
    @return <tt> ::ural::make_ostream_sequence(os) </tt>
    */
    template <class Char, class Traits>
    auto sequence(std::basic_ostream<Char, Traits> & os)
    -> decltype(::ural::make_ostream_sequence(os))
    {
        return ::ural::make_ostream_sequence(os);
    }

    //@{
    /** @brief Функция, комбинирующая @c sequence и <tt> std::forward </tt>
    @param t аргумент
    @return <tt> sequence(std::forward<Traversable>(t)) </tt>
    @todo Преобразовать в функциональный объект/шаблон переменной
    */
    template <class Traversable>
    auto sequence_fwd(typename std::remove_reference<Traversable>::type & t)
    -> decltype(sequence(std::forward<Traversable>(t)))
    {
        return sequence(std::forward<Traversable>(t));
    }

    template <class Traversable>
    auto sequence_fwd(typename std::remove_reference<Traversable>::type && t)
    -> decltype(sequence(std::forward<Traversable>(t)))
    {
        return sequence(std::forward<Traversable>(t));
    }
    //@}

    template <class S>
    struct sequence_type
     : declare_type<decltype(sequence(std::declval<S>()))>
    {};

    template <class S>
    using SequenceType = typename sequence_type<S>::type;

    namespace concepts
    {
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
            static Seq seq;
            typedef typename Seq::traversal_tag traversal_tag;

            typedef DifferenceType<Seq> difference_type;
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
         , Incrementable<Seq>
        {
        public:
            /// @brief Проверка неявных интерфейсов
            BOOST_CONCEPT_USAGE(ForwardSequence)
            {
                value_consumer<ural::forward_traversal_tag>() = traversal_tag{};

                BOOST_CONCEPT_ASSERT((concepts::EqualityComparable<Seq>));

                seq.shrink_front();
                seq.traversed_front();

                // @todo Проверить, что тип traversed_front совпадает с Seq или
                // tf - прямая последовательность

                static_assert(std::is_same<decltype(seq.front()), decltype(*seq)>::value, "");
            }

        private:
            using traversal_tag = typename Seq::traversal_tag;
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
            typedef DifferenceType<Seq> distance_type;
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
            // В Range extensions требуется, чтобы begin и end имели константную
            // сложность. Нужно ли нам потребовать этого от sequence?
            BOOST_CONCEPT_USAGE(Sequenced)
            {
                typedef decltype(sequence(std::declval<S>())) Seq;
                static_assert(std::is_same<Seq, sequence_type>::value, "");
            }
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
    }
    // namespace concepts
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_MAKE_HPP_INCLUDED
