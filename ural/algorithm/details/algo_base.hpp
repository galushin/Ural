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
        template <class ForwardSequence, class Compare>
        auto operator()(ForwardSequence && in, Compare cmp) const
        -> decltype(sequence_fwd<ForwardSequence>(in))
        {
            return this->impl(sequence_fwd<ForwardSequence>(in),
                              ural::make_functor(std::move(cmp)));
        }

        template <class ForwardSequence>
        auto operator()(ForwardSequence && in) const
        -> decltype(sequence_fwd<ForwardSequence>(in))
        {
            return (*this)(sequence_fwd<ForwardSequence>(in), ural::less<>{});
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
        template <class ForwardSequence, class Compare>
        bool operator()(ForwardSequence && in, Compare cmp) const
        {
            return this->impl(sequence_fwd<ForwardSequence>(in),
                              ural::make_functor(std::move(cmp)));
        }

        template <class ForwardSequence>
        bool operator()(ForwardSequence && in) const
        {
            return (*this)(sequence_fwd<ForwardSequence>(in), ural::less<>{});
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
                              ural::make_functor(std::move(pred)));
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
        template <class Input, class T>
        auto operator()(Input && in, T const & value) const
        -> typename decltype(sequence_fwd<Input>(in))::distance_type
        {
            return (*this)(std::forward<Input>(in), value, ural::equal_to<T>{});
        }

        template <class Input, class T, class BinaryPredicate>
        auto operator()(Input && in, T const & value, BinaryPredicate pred) const
        -> typename decltype(sequence_fwd<Input>(in))::distance_type
        {
            return this->impl(sequence_fwd<Input>(in), value,
                              ural::make_functor(std::move(pred)));
        }
    };

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
        template <class Input, class Predicate>
        auto operator()(Input && in, Predicate pred) const
        -> decltype(ural::sequence_fwd<Input>(in))
        {
            return this->impl(sequence_fwd<Input>(in),
                              ural::make_functor(std::move(pred)));
        }
    };

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
        template <class Input, class T>
        auto operator()(Input && in, T const & value) const
        -> decltype(sequence_fwd<Input>(in))
        {
            return (*this)(std::forward<Input>(in), value, ural::equal_to<T>{});
        }

        template <class Input, class T, class BinaryPredicate>
        auto operator()(Input && in, T const & value, BinaryPredicate pred) const
        -> decltype(sequence_fwd<Input>(in))
        {
            return this->impl(sequence_fwd<Input>(in), value,
                              ural::make_functor(std::move(pred)));
        }
    };

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
        template <class Input, class Predicate>
        auto operator()(Input && in, Predicate pred) const
        -> decltype(sequence_fwd<Input>(in))
        {
            return this->impl(sequence_fwd<Input>(in), std::move(pred));
        }
    };

    class none_of_fn
    {
    public:
        template <class Input, class UnaryPredicate>
        bool operator()(Input && in, UnaryPredicate pred) const
        {
            return !find_if_fn{}(std::forward<Input>(in), std::move(pred));
        }
    };

    class any_of_fn
    {
    public:
        /** @brief Проверяет, что хотя бы один элемент последовательности
        удовлетворяет заданному предикату.
        @param in входная последовтельность
        @param pred предикат
        @return @b true, если для хотя бы одного элемента @c x последовательности
        @c in выполняется <tt> pred(x) != false </tt>
        */
        template <class Input, class UnaryPredicate>
        bool operator()(Input && in, UnaryPredicate pred) const
        {
            return !none_of_fn{}(std::forward<Input>(in), std::move(pred));
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

    template <class RASequence, class Compare>
    void sort(RASequence s, Compare cmp)
    {
        // @todo Реализовать быструю сортировку
        return ::ural::details::insertion_sort(std::move(s), std::move(cmp));
    }

    template <class RASequence, class Compare>
    void stable_sort(RASequence s, Compare cmp)
    {
        return ::ural::details::insertion_sort(std::move(s), std::move(cmp));
    }

    template<class Forward1, class Forward2, class BinaryPredicate>
    Forward1 search(Forward1 in, Forward2 s, BinaryPredicate p)
    {
        BOOST_CONCEPT_ASSERT((ural::concepts::ForwardSequence<Forward1>));
        BOOST_CONCEPT_ASSERT((ural::concepts::ReadableSequence<Forward1>));
        BOOST_CONCEPT_ASSERT((ural::concepts::ForwardSequence<Forward2>));
        BOOST_CONCEPT_ASSERT((ural::concepts::ReadableSequence<Forward2>));

        BOOST_CONCEPT_ASSERT((ural::concepts::Callable<BinaryPredicate,
                                                       bool(decltype(*in), decltype(*s))>));

        for(;; ++ in)
        {
            auto i = in;
            auto i_s = s;
            for(;; ++ i, ++ i_s)
            {
                if(!i_s)
                {
                    return in;
                }
                if(!i)
                {
                    return i;
                }
                if(!p(*i, *i_s))
                {
                    break;
                }
            }
        }
        assert(false);
    }

    template <class Forward, class Size, class T,  class BinaryPredicate>
    Forward search_n(Forward in, Size const n, T const & value,
                     BinaryPredicate bin_pred)
    {
        BOOST_CONCEPT_ASSERT((ural::concepts::ForwardSequence<Forward>));
        BOOST_CONCEPT_ASSERT((ural::concepts::ReadableSequence<Forward>));
        BOOST_CONCEPT_ASSERT((ural::concepts::Callable<BinaryPredicate, bool(decltype(*in), T)>));

        if(n == 0)
        {
            return in;
        }

        for(; !!in; ++ in)
        {
            if(!bin_pred(*in, value))
            {
                continue;
            }

            auto candidate = in;
            Size cur_count = 0;

            while(true)
            {
                ++ cur_count;
                if(cur_count == n)
                {
                    return candidate;
                }
                ++ in;
                if(!in)
                {
                    return in;
                }
                if(!bin_pred(*in, value))
                {
                    break;
                }
            }
        }
        return in;
    }

    template <class Forward1, class Forward2, class BinaryPredicate>
    Forward1 find_end(Forward1 in, Forward2 s, BinaryPredicate bin_pred)
    {
        BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Forward1>));
        BOOST_CONCEPT_ASSERT((concepts::ReadableSequence<Forward1>));
        BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<Forward2>));
        BOOST_CONCEPT_ASSERT((concepts::ReadableSequence<Forward2>));

        BOOST_CONCEPT_ASSERT((ural::concepts::Callable<BinaryPredicate,
                                                       bool(decltype(*in), decltype(*s))>));
        if(!s)
        {
            return in;
        }

        auto result = ::ural::details::search(in, s, bin_pred);;
        auto new_result = result;

        for(;;)
        {
            if(!new_result)
            {
                return result;
            }
            else
            {
                result = std::move(new_result);
                in = result;
                ++ in;
                new_result = ::ural::details::search(in, s, bin_pred);
            }
        }
        return result;
    }

    template <class Input, class Forward, class BinaryPredicate>
    Input find_first_of(Input in, Forward s, BinaryPredicate bin_pred)
    {
        for(; !!in; ++ in)
        {
            auto r = find_fn{}(s, *in, bin_pred);

            if(!!r)
            {
                return in;
            }
        }
        return in;
    }

    template <class Input1, class Input2, class BinaryPredicate>
    tuple<Input1, Input2>
    mismatch(Input1 in1, Input2 in2, BinaryPredicate pred)
    {
        BOOST_CONCEPT_ASSERT((concepts::SinglePassSequence<Input1>));
        BOOST_CONCEPT_ASSERT((concepts::ReadableSequence<Input1>));
        BOOST_CONCEPT_ASSERT((concepts::SinglePassSequence<Input2>));
        BOOST_CONCEPT_ASSERT((concepts::ReadableSequence<Input2>));
        BOOST_CONCEPT_ASSERT((concepts::Callable<BinaryPredicate,
                                                 bool(decltype(*in1), decltype(*in2))>));

        typedef tuple<Input1, Input2> Tuple;
        for(; !!in1 && !!in2; ++ in1, ++ in2)
        {
            if(!pred(*in1, *in2))
            {
                break;
            }
        }
        return Tuple{std::move(in1), std::move(in2)};
    }

    template <class Input1, class Input2, class BinaryPredicate>
    bool equal(Input1 in1, Input2 in2, BinaryPredicate pred)
    {
        BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassSequence<Input1>));
        BOOST_CONCEPT_ASSERT((ural::concepts::ReadableSequence<Input1>));
        BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassSequence<Input2>));
        BOOST_CONCEPT_ASSERT((ural::concepts::ReadableSequence<Input2>));
        BOOST_CONCEPT_ASSERT((ural::concepts::Callable<BinaryPredicate,
                                                       bool(decltype(*in1), decltype(*in2))>));

        auto const r = ural::details::mismatch(std::move(in1), std::move(in2),
                                               std::move(pred));
        return !r[ural::_1] && !r[ural::_2];
    }

    template <class Forward, class BinaryPredicate>
    Forward adjacent_find(Forward s, BinaryPredicate pred)
    {
        if(!s)
        {
            return s;
        }

        auto s_next = ural::next(s);

        for(; !!s_next; ++ s_next)
        {
            if(pred(*s, *s_next))
            {
                return s;
            }
            s = s_next;
        }
        return s_next;
    }

    // Алгоритмы, модифицирующие последовательность
    template <class Forward1, class Forward2>
    ural::tuple<Forward1, Forward2>
    swap_ranges(Forward1 in1, Forward2 in2)
    {
        for(; !!in1 && !!in2; ++ in1, ++ in2)
        {
            ::ural::details::do_swap(*in1, *in2);
        }
        return ural::tuple<Forward1, Forward2>{in1, in2};
    }

    template <class ForwardSequence, class Predicate, class T>
    void replace_if(ForwardSequence seq, Predicate pred, T const & new_value)
    {
        BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<ForwardSequence>));
        BOOST_CONCEPT_ASSERT((concepts::ReadableSequence<ForwardSequence>));
        BOOST_CONCEPT_ASSERT((concepts::WritableSequence<ForwardSequence, T>));
        BOOST_CONCEPT_ASSERT((concepts::Callable<Predicate, bool(decltype(*seq))>));

        for(; !!seq; ++ seq)
        {
            if(pred(*seq))
            {
                *seq = new_value;
            }
        }
    }

    template <class ForwardSequence, class T, class BinaryPredicate>
    void replace(ForwardSequence seq, T const & old_value, T const & new_value,
                 BinaryPredicate bin_pred)
    {
        BOOST_CONCEPT_ASSERT((concepts::ForwardSequence<ForwardSequence>));
        BOOST_CONCEPT_ASSERT((concepts::ReadableSequence<ForwardSequence>));
        BOOST_CONCEPT_ASSERT((concepts::WritableSequence<ForwardSequence, T>));

        BOOST_CONCEPT_ASSERT((concepts::Callable<BinaryPredicate, bool(decltype(*seq), T)>));

        auto const pred = std::bind(std::move(bin_pred), ural::_1, std::cref(old_value));

        return ::ural::details::replace_if(std::move(seq), std::move(pred),
                                           new_value);
    }

    template <class BidirectionalSequence>
    void reverse(BidirectionalSequence seq)
    {
        for(; !!seq; ++seq)
        {
            auto seq_next = seq;
            seq_next.pop_back();

            if(!seq_next)
            {
                break;
            }
            else
            {
               ::ural::details::do_swap(*seq, seq.back());
            }
            seq = seq_next;
        }
    }

    template <class Forward1, class Forward2>
    ural::tuple<Forward1, Forward2>
    rotate(Forward1 in1, Forward2 in2)
    {
        in1.shrink_front();
        in2.shrink_front();
        if(!in1 || !in2)
        {
            return ural::tuple<Forward1, Forward2>{std::move(in1),
                                                   std::move(in2)};
        }

        auto r = ::ural::details::swap_ranges(in1, in2);

        if(!r[ural::_1] && !r[ural::_2])
        {
            return r;
        }
        else if(!r[ural::_1])
        {
            assert(!r[ural::_1]);
            return ::ural::details::rotate(r[ural::_2].traversed_front(),
                                           ::ural::shrink_front(r[ural::_2]));
        }
        else
        {
            assert(!r[ural::_2]);
            return ::ural::details::rotate(::ural::shrink_front(r[ural::_1]),
                                           in2);
        }
    }

    template <class ForwardSequence>
    ForwardSequence rotate(ForwardSequence seq)
    {
        auto seq_old = seq.original();

        ::ural::details::rotate(seq.traversed_front(), ural::shrink_front(seq));

        ural::advance(seq_old, seq.size());
        return seq_old;
    }

    template <class Forward, class Output>
    ural::tuple<Forward, Output>
    rotate_copy(Forward in, Output out)
    {
        auto const n = ural::size(in);
        auto in_orig = ural::next(in.original(), n);

        auto in_1 = in.traversed_front();
        auto r1 = copy_fn{}(std::move(in), std::move(out));
        auto r2 = copy_fn{}(in_1, std::move(r1[ural::_2]));

        return ural::tuple<Forward, Output>{std::move(in_orig),
                                            std::move(r2[ural::_2])};
    }

    template <class RASequence, class URNG>
    void shuffle(RASequence s, URNG && g)
    {
        if(!s)
        {
            return;
        }

        for(; !!s; s.pop_back())
        {
            std::uniform_int_distribution<decltype(s.size())>
                d(0, s.size() - 1);
            auto index = d(g);
            ::ural::details::do_swap(s[index], s.back());
        }
    }

    // Разделение
    template <class Input, class UnaryPredicate>
    bool is_partitioned(Input in, UnaryPredicate pred)
    {
        auto tail = find_if_not_fn{}(std::move(in), pred);
        return !find_if_fn{}(std::move(tail), std::move(pred));
    }

    template <class ForwardSequence, class UnaryPredicate>
    ForwardSequence
    partition(ForwardSequence in, UnaryPredicate pred)
    {
        // пропускаем ведущие "хорошие" элеменнов
        auto sink = find_if_not_fn{}(std::move(in), pred);

        in = sink;
        ++ in;
        in = find_if_fn{}(std::move(in), pred);

        for(; !!in; ++ in)
        {
            if(pred(*in))
            {
                ::ural::details::do_swap(*sink, *in);
                ++ sink;
            }
        }
        return sink;
    }

    template <class ForwardSequence, class UnaryPredicate>
    ForwardSequence
    inplace_stable_partition(ForwardSequence in, UnaryPredicate pred)
    {
        auto const n = ural::size(in);

        assert(!!in);
        assert(n > 0);
        assert(!pred(*in));
        assert(!in.traversed_front());

        auto const s_orig = ural::shrink_front(in);

        if(n == 1)
        {
            return s_orig;
        }

        // Разделяем первую половину
        auto const n_left = n/2;
        auto s = ural::next(s_orig, n_left);

        auto r_left = ::ural::details::inplace_stable_partition(s.traversed_front(), pred);

        // Разделяем вторую половину
        auto s_right = find_if_not_fn{}(ural::shrink_front(s), pred);

        if(!!s_right)
        {
            auto r_right = ::ural::details::inplace_stable_partition(ural::shrink_front(s_right), pred);
            ural::advance(s_right, ural::size(r_right.traversed_front()));
        }

        // Поворачиваем
        auto r = ::ural::details::rotate(ural::shrink_front(r_left),
                                         s_right.traversed_front());

        // Возвращаем результат
        auto nt = ::ural::size(r_left.traversed_front());
        nt += ::ural::size(r[ural::_1].traversed_front());

        return ural::next(s_orig, nt);
    }

    template <class ForwardSequence, class UnaryPredicate>
    ForwardSequence
    stable_partition(ForwardSequence in, UnaryPredicate pred)
    {
        in.shrink_front();
        in = find_if_not_fn{}(std::move(in), pred);

        if(!in)
        {
            return in;
        }

        // Разделяем на месте
        auto s = ural::shrink_front(std::move(in));
        auto r =
            ::ural::details::inplace_stable_partition(std::move(s), pred);
        auto const nt = ural::size(r.traversed_front());
        return ural::next(in, nt);
    }

    template <class Input, class Output1, class Output2, class UnaryPredicate>
    ural::tuple<Input, Output1, Output2>
    partition_copy(Input in, Output1 out_true, Output2 out_false,
                   UnaryPredicate pred)
    {
        auto out = ural::make_partition_sequence(std::move(out_true),
                                                 std::move(out_false),
                                                 std::move(pred));
        auto r = copy_fn{}(std::move(in), std::move(out));

        typedef ural::tuple<Input, Output1, Output2> Tuple;
        return Tuple(r[ural::_1], r[ural::_2].true_sequence(),
                     r[ural::_2].false_sequence());
    }

    template <class ForwardSequence, class Predicate>
    ForwardSequence
    partition_point(ForwardSequence in, Predicate pred)
    {
        in.shrink_front();
        return find_if_not_fn{}(std::move(in), std::move(pred));
    }

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
}
// namespace ural
#endif
// Z_URAL_ALGO_BASE_HPP_INCLUDED
