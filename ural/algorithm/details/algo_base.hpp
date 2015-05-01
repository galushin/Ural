#ifndef Z_URAL_ALGO_BASE_HPP_INCLUDED
#define Z_URAL_ALGO_BASE_HPP_INCLUDED

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

#include <ural/sequence/base.hpp>
#include <ural/sequence/function_output.hpp>
#include <ural/sequence/generator.hpp>
#include <ural/sequence/outdirected.hpp>
#include <ural/sequence/reversed.hpp>
#include <ural/sequence/partition.hpp>

#include <ural/algorithm/copy.hpp>
#include <ural/functional.hpp>

#include <cassert>

namespace ural
{
    class is_sorted_until_fn
    {
    private:
        template <class ForwardSequence, class Compare>
        static ForwardSequence
        impl(ForwardSequence in, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<decltype(in)>));
            BOOST_CONCEPT_ASSERT((concepts::ReadableSequence<decltype(in)>));
            BOOST_CONCEPT_ASSERT((concepts::Callable<Compare, bool(decltype(*in), decltype(*in))>));

            if(!in)
            {
                return in;
            }

            auto in_next = ural::next(in);

            for(; !!in_next; ++in_next, ++ in)
            {
                if(cmp(*in_next, *in))
                {
                    break;
                }
            }

            return in_next;
        }

    public:
        template <class ForwardSequence, class Compare = ::ural::less<>>
        auto operator()(ForwardSequence && in, Compare cmp = Compare()) const
        -> decltype(sequence_fwd<ForwardSequence>(in))
        {
            return this->impl(sequence_fwd<ForwardSequence>(in),
                              ural::make_callable(std::move(cmp)));
        }
    };

    class is_sorted_fn
    {
    private:
        template <class ForwardSequence, class Compare>
        static bool
        impl(ForwardSequence in, Compare cmp)
        {
            BOOST_CONCEPT_ASSERT((ural::concepts::ForwardSequence<decltype(in)>));
            BOOST_CONCEPT_ASSERT((ural::concepts::ReadableSequence<decltype(in)>));
            BOOST_CONCEPT_ASSERT((ural::concepts::Callable<Compare, bool(decltype(*in), decltype(*in))>));

            return !is_sorted_until_fn{}(std::move(in), std::move(cmp));
        }

    public:
        template <class ForwardSequence, class Compare = ural::less<>>
        bool operator()(ForwardSequence && in, Compare cmp = Compare()) const
        {
            return this->impl(sequence_fwd<ForwardSequence>(in),
                              ural::make_callable(std::move(cmp)));
        }
    };

    class count_if_fn
    {
    private:
        template <class Input, class UnaryPredicate>
        static typename Input::distance_type
        impl(Input in, UnaryPredicate pred)
        {
            BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassSequence<Input>));
            BOOST_CONCEPT_ASSERT((ural::concepts::ReadableSequence<Input>));
            BOOST_CONCEPT_ASSERT((ural::concepts::Callable<UnaryPredicate, bool(decltype(*in))>));

            typename Input::distance_type result{0};

            for(; !!in; ++ in)
            {
                if (pred(*in))
                {
                    ++ result;
                }
            }
            return result;
        }

    public:
        /** @brief Подсчитывает количество элементов последовательности,
        удовлетворяющих предикату.
        @param in входная последовтельность
        @param pred предикат
        @return Количество элементов @c x последовательности @c in, таких, что
        <tt> pred(x) != false </tt>.
        */
        template <class Input, class UnaryPredicate>
        auto operator()(Input && in, UnaryPredicate pred) const
        -> typename decltype(sequence_fwd<Input>(in))::distance_type
        {
            return this->impl(sequence_fwd<Input>(in),
                              ural::make_callable(std::move(pred)));
        }
    };

    class count_fn
    {
    private:
        template <class Input, class T, class BinaryPredicate>
        static typename Input::distance_type
        impl(Input in, T const & value, BinaryPredicate pred)
        {
            BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassSequence<Input>));
            BOOST_CONCEPT_ASSERT((ural::concepts::ReadableSequence<Input>));
            BOOST_CONCEPT_ASSERT((ural::concepts::Callable<BinaryPredicate,
                                                           bool(decltype(*in), T const &)>));

            return count_if_fn{}(std::move(in),
                                 std::bind(std::move(pred), ural::_1,
                                           std::ref(value)));
        }

    public:
        /** @brief Подсчитывает количество элементов последовательности, равных
        заданному значению.
        @param in входная последовтельность
        @param value значение
        @return Количество элементов @c x последовательности @c in, таких, что
        <tt> x == value </tt>.
        */
        template <class Input, class T,
                  class BinaryPredicate = ::ural::equal_to<>>
        auto operator()(Input && in, T const & value,
                        BinaryPredicate pred = BinaryPredicate()) const
        -> typename decltype(sequence_fwd<Input>(in))::distance_type
        {
            return this->impl(sequence_fwd<Input>(in), value,
                              ural::make_callable(std::move(pred)));
        }
    };

    /** @brief Класс функционального объекта, выполняющего поиск элемента
    последовательности, удовлетворяющего заданному предикату.
    */
    class find_if_fn
    {
    private:
        template <class Input, class Predicate>
        static Input
        impl(Input in, Predicate pred)
        {
            BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassSequence<Input>));
            BOOST_CONCEPT_ASSERT((ural::concepts::ReadableSequence<Input>));
            BOOST_CONCEPT_ASSERT((ural::concepts::Callable<Predicate, bool(decltype(*in))>));

            for(; !!in; ++ in)
            {
                if(pred(*in))
                {
                    return in;
                }
            }
            return in;
        }

    public:
        /** @brief Оператор вызова функции
        @param in входная последовательность
        @param pred унарный предикат
        @return Последовательность @c r, полученная из
        <tt> ::ural::sequence_fwd<Input>(in) </tt> продвижением до тех пор,
        пока не выполнится условие <tt> pred(r.front()) != false </tt>.
        */
        template <class Input, class Predicate>
        auto operator()(Input && in, Predicate pred) const
        -> decltype(::ural::sequence_fwd<Input>(in))
        {
            return this->impl(::ural::sequence_fwd<Input>(in),
                              ::ural::make_callable(std::move(pred)));
        }
    };

    /** @brief Класс функционального объекта, выполняющего поиск заданного
    значения в последовательности.
    */
    class find_fn
    {
    private:
        template <class Input, class T, class BinaryPredicate>
        static Input
        impl(Input in, T const & value, BinaryPredicate bin_pred)
        {
            BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassSequence<Input>));
            BOOST_CONCEPT_ASSERT((ural::concepts::ReadableSequence<Input>));
            BOOST_CONCEPT_ASSERT((ural::concepts::Callable<BinaryPredicate,
                                                           bool(decltype(*in), T const &)>));

            auto pred = std::bind(std::move(bin_pred), ural::_1, std::cref(value));

            return find_if_fn{}(std::move(in), std::move(pred));
        }

    public:
        /** @brief Оператор вызова функции
        @param in входная последовательность
        @param value значение, которое нужно найти
        @param pred бинарный предикат, используемый для сравнения элементов
        последовательности и заданного значения. Если этот параметр не указан,
        то используется <tt> equal_to<>() </tt>, то есть оператор "равно".
        @return Последовательность @c r, полученная из
        <tt> ::ural::sequence_fwd<Input>(in) </tt> продвижением до тех пор, пока
        не встретится элемент @c x такой, что <tt> pred(r.front(), value) </tt>.
        */
        template <class Input, class T,
                  class BinaryPredicate = ural::equal_to<>>
        auto operator()(Input && in, T const & value,
                        BinaryPredicate pred = BinaryPredicate()) const
        -> decltype(::ural::sequence_fwd<Input>(in))
        {
            return this->impl(::ural::sequence_fwd<Input>(in), value,
                              ural::make_callable(std::move(pred)));
        }
    };

    /** @brief Класс функционального объекта, выполняющего поиск элемента
    последовательности, не удовлетворяющего заданному предикату.
    */
    class find_if_not_fn
    {
    private:
        template <class Input, class Predicate>
        static Input impl(Input in, Predicate pred)
        {
            BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassSequence<Input>));
            BOOST_CONCEPT_ASSERT((ural::concepts::ReadableSequence<Input>));
            BOOST_CONCEPT_ASSERT((ural::concepts::Callable<Predicate, bool(decltype(*in))>));

            return find_if_fn{}(std::move(in), ural::not_fn(std::move(pred)));
        }

    public:
        /** @brief Оператор вызова функции
        @param in входная последовательность
        @param pred унарный предикат
        @return Последовательность @c r, полученная из
        <tt> ::ural::sequence_fwd<Input>(in) </tt> продвижением до тех пор, пока
        не выполнится условие <tt> pred(r.front()) == false </tt>.
        */
        template <class Input, class Predicate>
        auto operator()(Input && in, Predicate pred) const
        -> decltype(sequence_fwd<Input>(in))
        {
            return this->impl(sequence_fwd<Input>(in), std::move(pred));
        }
    };

    template <class T1, class T2>
    void swap(T1 & x, T2 & y);

namespace details
{
    class swap_fn
    {
    public:
        template <class T>
        void operator()(T & x, T & y) const
        {
            using std::swap;
            using ural::swap;
            using boost::swap;
            return swap(x, y);
        }
    };
    auto constexpr do_swap = swap_fn{};
}

    class swap_ranges_fn
    {
    public:
        template <class Forward1, class Forward2>
        auto operator()(Forward1 && s1, Forward2 && s2) const
        -> ural::tuple<decltype(sequence_fwd<Forward1>(s1)),
                       decltype(sequence_fwd<Forward2>(s2))>
        {
            return this->impl(sequence_fwd<Forward1>(s1),
                              sequence_fwd<Forward2>(s2));
        }
    private:
        template <class Forward1, class Forward2>
        static ural::tuple<Forward1, Forward2>
        impl(Forward1 in1, Forward2 in2)
        {
            for(; !!in1 && !!in2; ++ in1, ++ in2)
            {
                ::ural::details::do_swap(*in1, *in2);
            }
            return ural::tuple<Forward1, Forward2>{in1, in2};
        }
    };

    class rotate_fn
    {
    public:
        template <class ForwardSequence>
        auto operator()(ForwardSequence && seq) const
        -> decltype(sequence_fwd<ForwardSequence>(seq))
        {
            return this->impl(sequence_fwd<ForwardSequence>(seq));
        }

        template <class Forward1, class Forward2>
        auto operator()(Forward1 && in1, Forward2 && in2) const
        -> ural::tuple<decltype(sequence_fwd<Forward1>(in1)),
                       decltype(sequence_fwd<Forward2>(in2))>
        {
            return this->impl(sequence_fwd<Forward1>(in1),
                              sequence_fwd<Forward2>(in2));
        }

    private:
        template <class Forward1, class Forward2>
        ural::tuple<Forward1, Forward2>
        impl(Forward1 in1, Forward2 in2) const
        {
            in1.shrink_front();
            in2.shrink_front();

            if(!in1 || !in2)
            {
                return ural::tuple<Forward1, Forward2>{std::move(in1),
                                                       std::move(in2)};
            }

            auto r = ::ural::swap_ranges_fn{}(in1, in2);

            if(!r[ural::_1] && !r[ural::_2])
            {
                return r;
            }
            else if(!r[ural::_1])
            {
                assert(!r[ural::_1]);
                return this->impl(r[ural::_2].traversed_front(),
                                  ::ural::shrink_front(r[ural::_2]));
            }
            else
            {
                assert(!r[ural::_2]);
                return this->impl(::ural::shrink_front(r[ural::_1]), in2);
            }
        }

        template <class ForwardSequence>
        ForwardSequence impl(ForwardSequence seq) const
        {
            auto seq_old = seq.original();

            this->impl(seq.traversed_front(), ural::shrink_front(seq));

            ural::advance(seq_old, seq.size());
            return seq_old;
        }
    };

namespace details
{
    template <class RASequence, class Compare>
    void insertion_sort(RASequence s, Compare cmp)
    {
        BOOST_CONCEPT_ASSERT((ural::concepts::RandomAccessSequence<decltype(s)>));
        BOOST_CONCEPT_ASSERT((ural::concepts::ReadableSequence<decltype(s)>));
        BOOST_CONCEPT_ASSERT((ural::concepts::WritableSequence<decltype(s), decltype(*s)>));
        BOOST_CONCEPT_ASSERT((ural::concepts::Callable<Compare, bool(decltype(*s), decltype(*s))>));

        if(!s)
        {
            return;
        }

        typedef decltype(s.size()) Index;

        for(Index i = 1; i != s.size(); ++ i)
        for(Index j = i; j > 0; -- j)
        {
            if(cmp(s[j], s[j-1]))
            {
                ::ural::details::do_swap(s[j], s[j-1]);
            }
            else
            {
                break;
            }
        }
    }

    // Алгоритмы, модифицирующие последовательность

    // Разделение

    // Бинарные кучи
    template <class Size>
    Size heap_parent(Size pos)
    {
        return (pos - 1) / 2;
    }

    template <class Size>
    Size heap_child_1(Size pos)
    {
        return 2 * pos + 1;
    }

    template <class Size>
    Size heap_child_2(Size pos)
    {
        return 2 * pos + 2;
    }

    template <class RandomAccessSequence, class Size, class Compare>
    void heap_swim(RandomAccessSequence seq, Size index, Compare cmp)
    {
        for(; index > 0;)
        {
            auto const parent = heap_parent(index);

            if(cmp(seq[parent], seq[index]))
            {
                ::ural::details::do_swap(seq[parent], seq[index]);
            }

            index = parent;
        }
    }
}
// namespace details

    class sort_fn
    {
    public:
        template <class RASequence, class Compare = ::ural::less<>>
        void operator()(RASequence && s, Compare cmp = Compare()) const
        {
            return this->impl(sequence_fwd<RASequence>(s),
                              ural::make_callable(std::move(cmp)));
        }
    private:
        template <class RASequence, class Compare>
        static void impl(RASequence s, Compare cmp)
        {
            return ::ural::details::insertion_sort(std::move(s), std::move(cmp));
        }
    };

    class stable_sort_fn
    {
    public:
        template <class RASequence, class Compare = ::ural::less<>>
        void operator()(RASequence && s, Compare cmp = Compare()) const
        {
            return this->impl(sequence_fwd<RASequence>(s),
                              ural::make_callable(std::move(cmp)));
        }

    private:
        template <class RASequence, class Compare>
        static void impl(RASequence s, Compare cmp)
        {
            return ::ural::details::insertion_sort(std::move(s), std::move(cmp));
        }
    };
}
// namespace ural
#endif
// Z_URAL_ALGO_BASE_HPP_INCLUDED
