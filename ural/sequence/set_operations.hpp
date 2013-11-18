#ifndef Z_URAL_SEQUENCE_SET_OPERATIONS_HPP_INCLUDED
#define Z_URAL_SEQUENCE_SET_OPERATIONS_HPP_INCLUDED

/** @file ural/sequence/set_operations.hpp
 @brief Последовательности для операций над отсортированными множествами
*/

#include <ural/functional.hpp>

namespace ural
{
    template <class Input1, class Input2, class Compare = ural::less<> >
    class set_intersection_sequence
     : private sequence_base<set_intersection_sequence<Input1, Input2, Compare>,
                             Compare>
    {
        typedef sequence_base<set_intersection_sequence, Compare> Base_class;
    public:
        typedef typename Input1::reference reference;

        explicit set_intersection_sequence(Input1 in1, Input2 in2, Compare cmp)
         : Base_class{std::move(cmp)}
         , in1_(std::move(in1))
         , in2_(std::move(in2))
        {
            this->seek();
        }

        bool operator!() const
        {
            return !in1_ || !in2_;
        }

        reference operator*() const
        {
            return *in1_;
        }

        set_intersection_sequence & operator++()
        {
            ++ in1_;
            this->seek();
            return *this;
        }

        Compare const & functor() const
        {
            return static_cast<Compare const &>(*this);
        }

    private:
        void seek()
        {
            for(; !!in1_ && !!in2_;)
            {
                if(functor()(*in1_, *in2_))
                {
                    ++ in1_;
                }
                else if(functor()(*in2_, *in1_))
                {
                    ++ in2_;
                }
                else
                {
                    break;
                }
            }
        }

    private:
        Input1 in1_;
        Input2 in2_;
    };

    template <class Input1, class Input2, class Compare>
    auto set_intersection(Input1 && in1, Input2 && in2, Compare cmp)
    -> set_intersection_sequence<decltype(sequence(std::forward<Input1>(in1))),
                                 decltype(sequence(std::forward<Input2>(in2))),
                                 decltype(ural::make_functor(std::move(cmp)))>
    {
        typedef set_intersection_sequence<decltype(sequence(std::forward<Input1>(in1))),
                                 decltype(sequence(std::forward<Input2>(in2))),
                                 decltype(ural::make_functor(std::move(cmp)))> Result;

        return Result{sequence(std::forward<Input1>(in1)),
                      sequence(std::forward<Input2>(in2)),
                      ural::make_functor(std::move(cmp))};
    }

    template <class Input1, class Input2>
    auto set_intersection(Input1 && in1, Input2 && in2)
    -> set_intersection_sequence<decltype(sequence(std::forward<Input1>(in1))),
                                 decltype(sequence(std::forward<Input2>(in2)))>
    {
        return set_intersection(std::forward<Input1>(in1),
                                std::forward<Input2>(in2), ural::less<>{});
    }

    template <class Input1, class Input2, class Compare = ural::less<> >
    class set_difference_sequence
     : private sequence_base<set_difference_sequence<Input1, Input2, Compare>,
                             Compare>
    {
        typedef sequence_base<set_difference_sequence, Compare> Base_class;
    public:
        typedef typename Input1::reference reference;

        explicit set_difference_sequence(Input1 in1, Input2 in2, Compare cmp)
         : Base_class{std::move(cmp)}
         , in1_(std::move(in1))
         , in2_(std::move(in2))
        {
            this->seek();
        }

        bool operator!() const
        {
            return !in1_;
        }

        reference operator*() const
        {
            return *in1_;
        }

        set_difference_sequence & operator++()
        {
            ++ in1_;
            this->seek();
            return *this;
        }

        Compare const & functor() const
        {
            return static_cast<Compare const &>(*this);
        }

    private:
        void seek()
        {
            for(; !!in1_ && !!in2_;)
            {
                if(functor()(*in1_, *in2_))
                {
                    break;
                }
                else if(functor()(*in2_, *in1_))
                {
                    ++ in2_;
                }
                else
                {
                    ++in1_;
                    ++in2_;
                }
            }
        }

    private:
        Input1 in1_;
        Input2 in2_;
    };

    template <class Input1, class Input2, class Compare>
    auto set_difference(Input1 && in1, Input2 && in2, Compare cmp)
    -> set_difference_sequence<decltype(sequence(std::forward<Input1>(in1))),
                                 decltype(sequence(std::forward<Input2>(in2))),
                                 decltype(ural::make_functor(std::move(cmp)))>
    {
        typedef set_difference_sequence<decltype(sequence(std::forward<Input1>(in1))),
                                 decltype(sequence(std::forward<Input2>(in2))),
                                 decltype(ural::make_functor(std::move(cmp)))> Result;

        return Result{sequence(std::forward<Input1>(in1)),
                      sequence(std::forward<Input2>(in2)),
                      ural::make_functor(std::move(cmp))};
    }

    template <class Input1, class Input2>
    auto set_difference(Input1 && in1, Input2 && in2)
    -> set_difference_sequence<decltype(sequence(std::forward<Input1>(in1))),
                                 decltype(sequence(std::forward<Input2>(in2)))>
    {
        return set_difference(std::forward<Input1>(in1),
                              std::forward<Input2>(in2), ural::less<>{});
    }
}
// namespace ural

#endif
// Z_URAL_SEQUENCE_SET_OPERATIONS_HPP_INCLUDED
