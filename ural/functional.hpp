#ifndef Z_URAL_FUNCTIONAL_HPP_INCLUDED
#define Z_URAL_FUNCTIONAL_HPP_INCLUDED

/** @file ural/functional.hpp
 @brief Функциональные объекты и средства для работы с ними
*/

#include <boost/compressed_pair.hpp>

#include <ural/tuple.hpp>
#include <ural/functional/replace.hpp>
#include <ural/functional/cpp_operators.hpp>
#include <ural/functional/make_functor.hpp>

namespace ural
{
    template <class T>
    class value_functor
    {
    public:
        typedef T const & result_type;

        explicit value_functor(T value)
         : value_(std::move(value))
        {}

        result_type operator()() const
        {
            return this->value_;
        }

    private:
        T value_;
    };

    // @note Подлежит переработке
    template <class ForwardSequence, class Compare>
    class min_element_accumulator
    {
    public:
        explicit min_element_accumulator(ForwardSequence s, Compare cmp)
         : impl_{std::move(s), std::move(cmp)}
        {}

        min_element_accumulator &
        operator()(ForwardSequence s)
        {
            if(impl_.second()(*s, *this->result()))
            {
                impl_.first() = s;
            }

            return *this;
        }

        ForwardSequence const & result() const
        {
            return impl_.first();
        }

    private:
        boost::compressed_pair<ForwardSequence, Compare> impl_;
    };

    template <class UnaryFunction, class Compare = ural::less<> >
    class comparer_by
     : boost::compressed_pair<UnaryFunction, Compare>
    {
        typedef boost::compressed_pair<UnaryFunction, Compare> Base;

    public:
        explicit comparer_by(UnaryFunction f)
         : Base{std::move(f)}
        {}

        explicit comparer_by(UnaryFunction f, Compare cmp)
         : Base{std::move(f), std::move(cmp)}
        {}

        template <class T1, class T2>
        bool operator()(T1 const & x, T2 const & y) const
        {
            return this->compare()(this->transformation()(x),
                                   this->transformation()(y));
        }

        UnaryFunction const & transformation() const
        {
            return Base::first();
        }

        Compare const & compare() const
        {
            return Base::second();
        }
    };

    template <class F, class Tr>
    bool operator==(comparer_by<F, Tr> const & x, comparer_by<F, Tr> const & y)
    {
        return x.compare() == y.compare()
            && x.transformation() == y.transformation();
    }

    template <class UnaryFunction, class Compare>
    auto compare_by(UnaryFunction f, Compare cmp)
    -> comparer_by<decltype(make_functor(std::move(f))),
                   decltype(make_functor(std::move(cmp)))>
    {
        typedef comparer_by<decltype(make_functor(std::move(f))),
                   decltype(make_functor(std::move(cmp)))> Functor;
        return Functor(make_functor(std::move(f)), make_functor(std::move(cmp)));
    }

    template <class UnaryFunction>
    auto compare_by(UnaryFunction f)
    -> comparer_by<decltype(make_functor(std::move(f)))>
    {
        typedef comparer_by<decltype(make_functor(std::move(f)))> Functor;
        return Functor(make_functor(std::move(f)));
    }
}
// namespace ural

#endif
// Z_URAL_FUNCTIONAL_HPP_INCLUDED
