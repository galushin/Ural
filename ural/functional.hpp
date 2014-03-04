#ifndef Z_URAL_FUNCTIONAL_HPP_INCLUDED
#define Z_URAL_FUNCTIONAL_HPP_INCLUDED

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

/** @file ural/functional.hpp
 @brief Функциональные объекты и средства для работы с ними
 @todo Разбить на более мелкие файлы
*/

#include <boost/compressed_pair.hpp>

#include <ural/tuple.hpp>
#include <ural/functional/adjoin.hpp>
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
            if(this->compare()(s, this->result()))
            {
                impl_.first() = s;
            }

            return *this;
        }

        ForwardSequence const & result() const
        {
            return impl_.first();
        }

        Compare const & compare() const
        {
            return impl_.second();
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

    template <class T = void>
    class dereference
    {
    public:
        auto operator()(T const & x)
        -> decltype(*x)
        {
            return *x;
        }
    };

    /// @brief Специализация с выводом типа аргумента
    template <>
    class dereference<void>
    {
    public:
        template <class T>
        constexpr auto operator()(T && x) const
        -> decltype(*x)
        {
            return *x;
        }
    };

    template <class BinaryFunctor>
    class binary_reverse_args_functor
     : private BinaryFunctor
    {
        typedef BinaryFunctor Base;

    public:
        explicit binary_reverse_args_functor(BinaryFunctor f)
         : Base(std::move(f))
        {}

        template <class T1, class T2>
        auto operator()(T1 && x, T2 && y) const
        -> decltype(std::declval<BinaryFunctor>()(std::forward<T2>(y), std::forward<T1>(x)))
        {
            return this->functor()(std::forward<T2>(y), std::forward<T1>(x));
        }

        BinaryFunctor const & functor() const
        {
            return static_cast<BinaryFunctor const &>(*this);
        }
    };

    template <class BinaryFunctor>
    auto make_binary_reverse_args(BinaryFunctor f)
    -> binary_reverse_args_functor<decltype(make_functor(std::move(f)))>
    {
        typedef binary_reverse_args_functor<decltype(make_functor(std::move(f)))>
            Functor;
        return Functor{make_functor(std::move(f))};
    }
}
// namespace ural

#endif
// Z_URAL_FUNCTIONAL_HPP_INCLUDED
