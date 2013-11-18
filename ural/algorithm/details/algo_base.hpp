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
