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

    template <class Input, class Predicate>
    Input find_if(Input in, Predicate pred)
    {
        for(; !!in; ++ in)
        {
            if(pred(*in))
            {
                return in;
            }
        }

        return in;
    }

    template <class Input, class Predicate>
    Input find_if_not(Input in, Predicate pred)
    {
        return find_if(std::move(in), ural::not_fn(std::move(pred)));
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

        for(; !!in1 && !!in2; ++ in1, ++ in2)
        {
            if(!pred(*in1, *in2))
            {
                return false;
            }
        }

        return !in1 && !in2;
    }

    template <class ForwardSequence, class Generator>
    void generate(ForwardSequence seq, Generator gen)
    {
        typedef decltype(gen()) result_type;

        BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassSequence<ForwardSequence>));
        BOOST_CONCEPT_ASSERT((ural::concepts::WritableSequence<ForwardSequence, result_type>));

        ural::details::copy(ural::make_generator_sequence(std::move(gen)),
                            std::move(seq));
    }

    template <class ForwardSequence, class T>
    void fill(ForwardSequence seq, T const & value)
    {
        BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassSequence<ForwardSequence>));
        BOOST_CONCEPT_ASSERT((ural::concepts::WritableSequence<ForwardSequence, T const &>));

        ::ural::details::generate(std::move(seq),
                                  ural::value_functor<T const &>(value));
    }

    // Разделение
    template <class Input, class UnaryPredicate>
    bool is_partitioned(Input in, UnaryPredicate pred)
    {
        auto tail = ural::details::find_if_not(std::move(in), pred);
        return !::ural::details::find_if(std::move(tail), std::move(pred));
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
        ::ural::details::heap_sink(seq, 0, N-1, cmp);
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
    template <class Input1, class  Input2, class Compare>
    bool lexicographical_compare(Input1 in1, Input2 in2, Compare cmp)
    {
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

        auto result = in++;

        for(; !!in; ++ in)
        {
            if(cmp(*in, *result))
            {
                result = in;
            }
        }
        return result;
    }
}
// namespace details
}
// namespace ural

#endif
// Z_URAL_ALGO_BASE_HPP_INCLUDED
