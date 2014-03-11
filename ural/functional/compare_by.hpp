#ifndef Z_URAL_FUNCTIONAL_COMPARE_BY_HPP_INCLUDED
#define Z_URAL_FUNCTIONAL_COMPARE_BY_HPP_INCLUDED

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

/** @file ural/functional/compare_by.hpp
 @brief Функциональный объект, сравнивающий свои аргументы по некоторому их
 свойству, а также вспомогательные функции.
*/

#include <ural/functional/cpp_operators.hpp>

namespace ural
{
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
// Z_URAL_FUNCTIONAL_COMPARE_BY_HPP_INCLUDED
