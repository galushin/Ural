#ifndef Z_URAL_ALGO_BASE_HPP_INCLUDED
#define Z_URAL_ALGO_BASE_HPP_INCLUDED

#include <ural/sequence/function_output.hpp>
#include <ural/sequence/generator.hpp>

#include <ural/algorithm/details/copy.hpp>
#include <ural/functional.hpp>

namespace ural
{
namespace details
{
    template <class ForwardSequence, class Compare>
    ForwardSequence
    is_sorted_until(ForwardSequence in, Compare cmp)
    {
        BOOST_CONCEPT_ASSERT((ural::concepts::ForwardSequence<decltype(in)>));
        BOOST_CONCEPT_ASSERT((ural::concepts::ReadableSequence<decltype(in)>));
        BOOST_CONCEPT_ASSERT((ural::concepts::Callable<Compare, bool(decltype(*in), decltype(*in))>));

        if(!in)
        {
            return in;
        }

        // @todo Заменить на одну операцию
        auto in_next = in;
        ++ in_next;

        for(; !!in_next; ++in_next, ++ in)
        {
            if(cmp(*in_next, *in))
            {
                break;
            }
        }

        return in_next;
    }

    template <class ForwardSequence, class Compare>
    bool is_sorted(ForwardSequence in, Compare cmp)
    {
        BOOST_CONCEPT_ASSERT((ural::concepts::ForwardSequence<decltype(in)>));
        BOOST_CONCEPT_ASSERT((ural::concepts::ReadableSequence<decltype(in)>));
        BOOST_CONCEPT_ASSERT((ural::concepts::Callable<Compare, bool(decltype(*in), decltype(*in))>));

        return !::ural::details::is_sorted_until(std::move(in), std::move(cmp));
    }

    template <class Input, class UnaryFunction>
    UnaryFunction for_each(Input in, UnaryFunction f)
    {
        BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassSequence<Input>));
        BOOST_CONCEPT_ASSERT((ural::concepts::ReadableSequence<Input>));
        BOOST_CONCEPT_ASSERT((ural::concepts::Callable<UnaryFunction, void(decltype(*in))>));

        auto r = ural::details::copy(in, ural::make_function_output_sequence(std::move(f)));
        return r[ural::_2].functor();
    }

    template <class Input, class UnaryPredicate>
    typename Input::distance_type
    count_if(Input in, UnaryPredicate pred)
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

    template <class Input, class T, class BinaryPredicate>
    typename Input::distance_type
    count(Input in, T const & value, BinaryPredicate pred)
    {
        BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassSequence<Input>));
        BOOST_CONCEPT_ASSERT((ural::concepts::ReadableSequence<Input>));
        BOOST_CONCEPT_ASSERT((ural::concepts::Callable<BinaryPredicate,
                                                       bool(decltype(*in), T const &)>));

        return ::ural::details::count_if(std::move(in),
                                         std::bind(ural::make_functor(std::move(pred)),
                                                   std::placeholders::_1,
                                                   std::ref(value)));
    }

    template <class Input, class Predicate>
    Input find_if(Input in, Predicate pred)
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

    template <class Input, class T, class BinaryPredicate>
    Input find(Input in, T const & value, BinaryPredicate bin_pred)
    {
        BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassSequence<Input>));
        BOOST_CONCEPT_ASSERT((ural::concepts::ReadableSequence<Input>));
        BOOST_CONCEPT_ASSERT((ural::concepts::Callable<BinaryPredicate,
                                                       bool(decltype(*in), T const &)>));

        auto pred = std::bind(std::move(bin_pred), std::placeholders::_1,
                              std::cref(value));
        return ::ural::details::find_if(std::move(in), std::move(pred));
    }

    template <class Input, class Predicate>
    Input find_if_not(Input in, Predicate pred)
    {
        BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassSequence<Input>));
        BOOST_CONCEPT_ASSERT((ural::concepts::ReadableSequence<Input>));
        BOOST_CONCEPT_ASSERT((ural::concepts::Callable<Predicate, bool(decltype(*in))>));

        return ::ural::details::find_if(std::move(in), ural::not_fn(std::move(pred)));
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
        BOOST_CONCEPT_ASSERT((ural::concepts::ForwardSequence<Forward1>));
        BOOST_CONCEPT_ASSERT((ural::concepts::ReadableSequence<Forward1>));

        BOOST_CONCEPT_ASSERT((ural::concepts::ForwardSequence<Forward2>));
        BOOST_CONCEPT_ASSERT((ural::concepts::ReadableSequence<Forward2>));

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
            auto r = ::ural::details::find(s, *in, bin_pred);

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
        BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassSequence<Input1>));
        BOOST_CONCEPT_ASSERT((ural::concepts::ReadableSequence<Input1>));
        BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassSequence<Input2>));
        BOOST_CONCEPT_ASSERT((ural::concepts::ReadableSequence<Input2>));
        BOOST_CONCEPT_ASSERT((ural::concepts::Callable<BinaryPredicate,
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

        // @todo заменить на одну операцию
        auto s_next = s;
        ++ s_next;

        // @todo можно ли унифицировать с mismatch?
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
            using std::swap;
            // @todo using ural::swap;
            swap(*in1, *in2);
        }
        return ural::tuple<Forward1, Forward2>{in1, in2};
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
                using std::swap;
                // @todo using ural::swap;
                swap(*seq, seq.back());
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

        // @todo Заменить рекурсию на итерацию
        // @todo Оптимизация кода
        auto r = ::ural::details::swap_ranges(in1, in2);

        if(!r[ural::_1] && !r[ural::_2])
        {
            return ural::tuple<Forward1, Forward2>{std::move(in1),
                                                   std::move(in2)};
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
        auto r = ::ural::details::rotate(seq.traversed_front(),
                                         ural::shrink_front(seq));
        return r[ural::_1];
    }

    // Заполнение и генерация
    template <class ForwardSequence, class Generator>
    ForwardSequence generate(ForwardSequence seq, Generator gen)
    {
        typedef decltype(gen()) result_type;

        BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassSequence<ForwardSequence>));
        BOOST_CONCEPT_ASSERT((ural::concepts::WritableSequence<ForwardSequence, result_type>));

        auto r = ural::details::copy(ural::make_generator_sequence(std::move(gen)),
                                     std::move(seq));
        return r[ural::_2];
    }

    template <class ForwardSequence, class T>
    ForwardSequence
    fill(ForwardSequence seq, T const & value)
    {
        BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassSequence<ForwardSequence>));
        BOOST_CONCEPT_ASSERT((ural::concepts::WritableSequence<ForwardSequence, T const &>));

        return ::ural::details::generate(std::move(seq),
                                         ural::value_functor<T const &>(value));
    }

    // Разделение
    template <class Input, class UnaryPredicate>
    bool is_partitioned(Input in, UnaryPredicate pred)
    {
        auto tail = ural::details::find_if_not(std::move(in), pred);
        return !::ural::details::find_if(std::move(tail), std::move(pred));
    }

    template <class ForwardSequence, class UnaryPredicate>
    ForwardSequence
    partition(ForwardSequence in, UnaryPredicate pred)
    {
        // @todo Специализации для более сильных категорий итераторов
        // пропускаем ведущие "хорошие" элеменнов
        auto sink = ::ural::details::find_if_not(std::move(in), pred);

        in = sink;
        ++ in;
        in = ural::details::find_if(std::move(in), pred);

        for(; !!in; ++ in)
        {
            if(pred(*in))
            {
                using ::std::swap;
                // @todo using ural::swap;
                swap(*sink, *in);
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
        auto s = s_orig;
        ural::advance(s, n_left);

        auto r_left = ::ural::details::inplace_stable_partition(s.traversed_front(), pred);

        // Разделяем вторую половину
        auto s_right = ::ural::details::find_if_not(ural::shrink_front(s), pred);

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
        nt += ::ural::size(r[ural::_2].traversed_front());
        nt += ::ural::size(s_right.traversed_front());

        s = s_orig;
        ural::advance(s, nt);
        return s;
    }

    template <class ForwardSequence, class UnaryPredicate>
    ForwardSequence
    stable_partition(ForwardSequence in, UnaryPredicate pred)
    {
        in.shrink_front();
        in = ::ural::details::find_if_not(std::move(in), pred);

        if(!in)
        {
            return in;
        }

        // @todo Попробовать получить буфер

        // Разделяем на месте
        auto s = ural::shrink_front(std::move(in));
        auto r =
            ::ural::details::inplace_stable_partition(std::move(s), pred);
        auto const nt = ural::size(r.traversed_front());
        ural::advance(in, nt);
        return in;
    }

    template <class Input, class Output1, class Output2, class UnaryPredicate>
    ural::tuple<Input, Output1, Output2>
    partition_copy(Input in, Output1 out_true, Output2 out_false,
                   UnaryPredicate pred)
    {
        // @todo Специальная последовательность?
        for(; !!in && !!out_true && !!out_false; ++ in)
        {
            if(pred(*in))
            {
                *out_true = *in;
                ++ out_true;
            }
            else
            {
                *out_false = *in;
                ++ out_false;
            }
        }

        typedef ural::tuple<Input, Output1, Output2> Tuple;
        return Tuple(std::move(in), std::move(out_true), std::move(out_false));
    }

    template <class ForwardSequence, class Predicate>
    ForwardSequence
    partition_point(ForwardSequence in, Predicate pred)
    {
        in.shrink_front();
        return ::ural::details::find_if_not(std::move(in), std::move(pred));
    }

    template <class RASequence, class T, class Compare>
    RASequence
    lower_bound(RASequence in, T const & value, Compare cmp)
    {
        auto pred = std::bind(std::move(cmp), std::placeholders::_1,
                              std::cref(value));
        return ::ural::details::partition_point(std::move(in), std::move(pred));
    }

    template <class RASequence, class T, class Compare>
    RASequence
    upper_bound(RASequence in, T const & value, Compare cmp)
    {
        auto pred = ural::not_fn(std::bind(std::move(cmp), std::cref(value), std::placeholders::_1));
        return ::ural::details::partition_point(std::move(in), std::move(pred));
    }

    template <class RASequence, class T, class Compare>
    bool binary_search(RASequence in, T const & value, Compare cmp)
    {
        in = ::ural::details::lower_bound(std::move(in), value, cmp);

        return !!in && !cmp(value, *in);
    }

    template <class RASequence, class T, class Compare>
    RASequence equal_range(RASequence in, T const & value, Compare cmp)
    {
        // @todo Оптимизация
        auto lower = ::ural::details::lower_bound(in, value, cmp);
        auto upper = ::ural::details::upper_bound(in, value, cmp);

        auto n_lower = lower.traversed_front().size();
        auto n_upper = in.size() - upper.traversed_front().size();

        in += n_lower;
        in.pop_back(n_upper);
        return in;
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
                using std::swap;
                swap(seq[parent], seq[index]);
            }

            index = parent;
        }
    }

    template <class RandomAccessSequence, class Size, class Compare>
    void heap_sink(RandomAccessSequence seq, Size first, Size last, Compare cmp)
    {
        assert(last <= seq.size());

        if(first == last)
        {
            return;
        }

        auto const c1 = heap_child_1(first);
        auto const c2 = heap_child_2(first);
        auto largest = first;

        if(c1 < last && cmp(seq[largest], seq[c1]))
        {
            largest = c1;
        }

        if (c2 < last && cmp(seq[largest], seq[c2]))
        {
            largest = c2;
        }

        if(largest != first)
        {
            using std::swap;
            swap(seq[largest], seq[first]);
            heap_sink(seq, largest, last, cmp);
        }
    }

    template <class RandomAccessSequence, class Compare>
    RandomAccessSequence
    is_heap_until(RandomAccessSequence seq, Compare cmp)
    {
        BOOST_CONCEPT_ASSERT((ural::concepts::RandomAccessSequence<decltype(seq)>));
        BOOST_CONCEPT_ASSERT((ural::concepts::Callable<Compare, bool(decltype(*seq), decltype(*seq))>));

        // Пустая последовательность - куча
        if(!seq)
        {
            return seq;
        }

        auto const n = seq.size();

        auto index = 1;

        for(; index != n; ++ index)
        {
            auto const p = ural::details::heap_parent(index);

            if(cmp(seq[p], seq[index]))
            {
                break;
            }
        }

        seq += index;
        return seq;
    }

    template <class RandomAccessSequence, class Compare>
    bool is_heap(RandomAccessSequence seq, Compare cmp)
    {
        BOOST_CONCEPT_ASSERT((ural::concepts::RandomAccessSequence<decltype(seq)>));
        BOOST_CONCEPT_ASSERT((ural::concepts::Callable<Compare, bool(decltype(*seq), decltype(*seq))>));

        return !::ural::details::is_heap_until(seq, cmp);
    }

    template <class RandomAccessSequence, class Compare>
    void make_heap(RandomAccessSequence seq, Compare cmp)
    {
        BOOST_CONCEPT_ASSERT((ural::concepts::RandomAccessSequence<decltype(seq)>));
        BOOST_CONCEPT_ASSERT((ural::concepts::Callable<Compare, bool(decltype(*seq), decltype(*seq))>));

        for(auto n = seq.size() / 2; n > 0; -- n)
        {
            heap_sink(seq, n - 1, seq.size(), cmp);
        }

        assert(ural::details::is_heap(seq, cmp));
    }

    template <class RandomAccessSequence, class Compare>
    void pop_heap(RandomAccessSequence seq, Compare cmp)
    {
        BOOST_CONCEPT_ASSERT((ural::concepts::RandomAccessSequence<decltype(seq)>));
        BOOST_CONCEPT_ASSERT((ural::concepts::Callable<Compare, bool(decltype(*seq), decltype(*seq))>));

        assert(ural::details::is_heap(seq, cmp));
        auto const N = seq.size();

        if(N <= 1)
        {
            return;
        }

        using std::swap;
        swap(seq[0], seq[N-1]);
        ::ural::details::heap_sink(seq, static_cast<decltype(N)>(0), N-1, cmp);
    }

    template <class RandomAccessSequence, class Compare>
    void push_heap(RandomAccessSequence seq, Compare cmp)
    {
        BOOST_CONCEPT_ASSERT((ural::concepts::RandomAccessSequence<decltype(seq)>));
        BOOST_CONCEPT_ASSERT((ural::concepts::Callable<Compare, bool(decltype(*seq), decltype(*seq))>));

        assert(ural::details::is_heap_until(seq, cmp).size() <= 1);

        if(seq.size() >= 1)
        {
            ::ural::details::heap_swim(seq, seq.size() - 1, cmp);
        }

        assert(ural::details::is_heap(seq, cmp));
    }

    template <class RandomAccessSequence, class Compare>
    void sort_heap(RandomAccessSequence seq, Compare cmp)
    {
        BOOST_CONCEPT_ASSERT((ural::concepts::RandomAccessSequence<decltype(seq)>));
        BOOST_CONCEPT_ASSERT((ural::concepts::Callable<Compare, bool(decltype(*seq), decltype(*seq))>));

        assert(ural::details::is_heap(seq, cmp));
        for(auto n = seq.size(); n > 0; --n)
        {
            ::ural::details::pop_heap(seq, cmp);
            seq.pop_back();
        }

        assert(::ural::details::is_sorted(seq, cmp));
    }

    // Сортировка
    template <class RASequence, class Size, class Compare>
    void partial_sort(RASequence s, Size const part, Compare cmp)
    {
        ::ural::details::make_heap(s, cmp);

        s.shrink_front();
        auto s_old = s;
        s += part;

        for(auto i = s; !!i; ++ i)
        {
            if(cmp(*i, *s_old))
            {
                using ::std::swap;
                // @todo using ::ural::swap;
                swap(*s_old, *i);
                ::ural::details::heap_sink(s.traversed_front(), 0, part, cmp);
            }
        }

        ::ural::details::sort_heap(s.traversed_front(), cmp);
    }

    template <class Input, class RASequence, class Compare>
    RASequence
    partial_sort_copy(Input in, RASequence out, Compare cmp)
    {
        out.shrink_front();
        std::tie(in, out) = ::ural::details::copy(std::move(in), std::move(out));

        auto to_sort = out.traversed_front();
        auto const part = to_sort.size();
        decltype(part) const zero = 0;

        ::ural::details::make_heap(to_sort, cmp);

        // @todo Устранить дублирование с partial_sort
        for(; !!in; ++ in)
        {
            if(cmp(*in, *to_sort))
            {
                using ::std::swap;
                // @todo using ::ural::swap;
                *to_sort = *in;
                ::ural::details::heap_sink(to_sort, zero, part, cmp);
            }
        }

        ::ural::details::sort_heap(std::move(to_sort), cmp);

        return out;
    }

    template <class Input1, class  Input2, class Compare>
    bool lexicographical_compare(Input1 in1, Input2 in2, Compare cmp)
    {
        BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassSequence<Input1>));
        BOOST_CONCEPT_ASSERT((ural::concepts::ReadableSequence<Input1>));
        BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassSequence<Input2>));
        BOOST_CONCEPT_ASSERT((ural::concepts::ReadableSequence<Input2>));
        BOOST_CONCEPT_ASSERT((ural::concepts::Callable<Compare, bool(decltype(*in1), decltype(*in2))>));

        for(; !!in1 && !!in2; ++ in1, ++ in2)
        {
            if(cmp(*in1, *in2))
            {
                return true;
            }
            else if(cmp(*in2, *in1))
            {
                return false;
            }
        }
        return !in1 && !!in2;
    }

    // Операции над множествами
    template <class Input1, class  Input2, class Compare>
    bool includes(Input1 in1, Input2 in2, Compare cmp)
    {
        BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassSequence<Input1>));
        BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassSequence<Input2>));
        BOOST_CONCEPT_ASSERT((ural::concepts::ReadableSequence<Input1>));
        BOOST_CONCEPT_ASSERT((ural::concepts::ReadableSequence<Input2>));
        BOOST_CONCEPT_ASSERT((ural::concepts::Callable<Compare, bool(decltype(*in1), decltype(*in2))>));

        for(; !!in1 && !!in2;)
        {
            if(cmp(*in1, *in2))
            {
                ++ in1;
            }
            else if(cmp(*in2, *in1))
            {
                return false;
            }
            else
            {
                ++ in1;
                ++ in2;
            }
        }

        return !in2;
    }

    // Поиск наибольшего и наименьшего
    template <class ForwardSequence, class Compare>
    ForwardSequence
    min_element(ForwardSequence in, Compare cmp)
    {
        if(!in)
        {
            return in;
        }

        ::ural::min_element_accumulator<ForwardSequence, Compare>
            acc(in++, std::move(cmp));

        /* @todo Избавиться от цикла: нужна последовательность, действующая
        подобно boost::counted_iterator и функциональный объект,
        разыменовывающий свои аргументы перед сравнением - это можно сделать
        через compare_by и dererference
        */
        for(; !!in; ++ in)
        {
            acc(in);
        }

        return acc.result();
    }

    template <class ForwardSequence, class Compare>
    ForwardSequence
    max_element(ForwardSequence in, Compare cmp)
    {
        auto transposed_cmp = std::bind(std::move(cmp), std::placeholders::_2,
                                        std::placeholders::_1);
        return ::ural::details::min_element(std::move(in),
                                            std::move(transposed_cmp));
    }

    template <class ForwardSequence, class Compare>
    ural::tuple<ForwardSequence, ForwardSequence>
    minmax_element(ForwardSequence in, Compare cmp)
    {
        typedef ural::tuple<ForwardSequence, ForwardSequence> Tuple;

        if(!in)
        {
            return Tuple{in, in};
        }

        ForwardSequence min_pos = in;
        ForwardSequence max_pos = in;
        ++ in;

        /* @todo Устранить дублирование
        Проблема в том, что введение двух накопителей приведёт к необходимости
        копировать функциональный объект
        */
        for(; !!in; ++ in)
        {
            auto in_next = in;
            ++ in_next;

            // остался только один элемент
            if(!in_next)
            {
                if(cmp(*in, *min_pos))
                {
                    min_pos = in;
                }
                else if(cmp(*max_pos, *in))
                {
                    max_pos = in;
                }
                break;
            }

            // осталось как минимум два элемента
            if(cmp(*in, *in_next))
            {
                if(cmp(*in, *min_pos))
                {
                    min_pos = in;
                }
                if(cmp(*max_pos, *in_next))
                {
                    max_pos = in_next;
                }
            }
            else
            {
                if(cmp(*in_next, *min_pos))
                {
                    min_pos = in_next;
                }
                if(cmp(*max_pos, *in))
                {
                    max_pos = in;
                }
            }

            in = in_next;
        }

        return Tuple{min_pos, max_pos};
    }

    template <class Forward1, class Forward2, class BinaryPredicate>
    bool is_permutation(Forward1 s1, Forward2 s2, BinaryPredicate pred)
    {
        // Пропустить общий префикс
        s1.shrink_front();
        s2.shrink_front();

        for(; !!s1; ++ s1)
        {
            // Пропускаем элементы, которые уже встречались
            if(!!::ural::details::find(s1.traversed_front(), *s1, pred))
            {
                continue;
            }

            auto s = s1;
            ++ s;
            auto const n1 = 1 + ::ural::details::count(s, *s1, pred);
            auto const n2 = ::ural::details::count(s2, *s1, pred);

            if(n1 != n2)
            {
                return false;
            }
        }
        return true;
    }
}
// namespace details
}
// namespace ural

#endif
// Z_URAL_ALGO_BASE_HPP_INCLUDED
