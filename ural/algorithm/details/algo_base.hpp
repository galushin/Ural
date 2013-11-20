#ifndef Z_URAL_ALGO_BASE_HPP_INCLUDED
#define Z_URAL_ALGO_BASE_HPP_INCLUDED

#include <ural/sequence/function_output.hpp>
#include <ural/algorithm/details/copy.hpp>
#include <ural/functional.hpp>

namespace ural
{
namespace details
{
    template <class Input, class UnaryFunction>
    UnaryFunction for_each(Input in, UnaryFunction f)
    {
        // @todo Проверка концепций
        auto r = ural::details::copy(in, ural::make_function_output_sequence(std::move(f)));
        return r[ural::_2].functor();
    }

    template <class Input1, class Input2, class BinaryPredicate>
    bool equal(Input1 in1, Input2 in2, BinaryPredicate pred)
    {
        // @todo Проверка концепций
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
        // @todo Проверка концепций
        // @todo через copy, используя generate_sequence?
        for(; !!seq; ++ seq)
        {
            *seq = gen();
        }
    }

    template <class ForwardSequence, class T>
    void fill(ForwardSequence seq, T const & value)
    {
        // @todo Проверка концепций
        BOOST_CONCEPT_ASSERT((::ural::concepts::WritableSequence<ForwardSequence, T>));

        ::ural::details::generate(std::move(seq),
                                  ural::value_functor<T const &>(value));
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
        // @todo Проверка концепций
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
    void make_heap(RandomAccessSequence seq, Compare cmp)
    {
        // @todo Проверка концепций
        // @todo пропустить элементы, у которых заведомо нет дочерних
        for(auto n = seq.size(); n > 0; -- n)
        {
            heap_sink(seq, n - 1, seq.size(), cmp);
        }
    }

    template <class RandomAccessSequence, class Compare>
    void pop_heap(RandomAccessSequence seq, Compare cmp)
    {
        // @todo assert(ural::is_heap(seq));
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
        // @todo Проверка концепций
        if(seq.size() >= 1)
        {
            ::ural::details::heap_swim(seq, seq.size() - 1, cmp);
        }
    }

    template <class RandomAccessSequence, class Compare>
    void sort_heap(RandomAccessSequence seq, Compare cmp)
    {
        // @todo Проверка концепций
        // @todo assert(ural::is_heap(seq));
        for(auto n = seq.size(); n > 0; --n)
        {
            ::ural::details::pop_heap(seq, cmp);
            seq.pop_back();
        }

        // @todo assert(ural::is_sorted(seq));
    }

    // Операции над множествами
    template <class Input1, class  Input2, class Compare>
    bool includes(Input1 in1, Input2 in2, Compare cmp)
    {
        // @todo Проверка концепций
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
}
// namespace details
}
// namespace ural

#endif
// Z_URAL_ALGO_BASE_HPP_INCLUDED
