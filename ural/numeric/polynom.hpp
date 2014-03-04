#ifndef Z_URAL_NUMERIC_POLYNOM_HPP_INCLUDED
#define Z_URAL_NUMERIC_POLYNOM_HPP_INCLUDED

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

/** @file ural/numeric/polynom.hpp
 @todo ������� � ������� �����
 @todo ���������� � ��������������-���������
 @todo ���������, ��� ��� ���� ��������� ����������� decltype(x*x) == X
 @todo ��� ���������� �����������: ���������, �������, ��������� ������� ��
 �������
*/

#include <boost/operators.hpp>

namespace ural
{
    template <class Input, class X>
    auto polynom(Input && in, X const & x)
    -> decltype(sequence(in).front() * x)
    {
        auto s = sequence(in);

        typedef decltype(sequence(in).front() * x) result_type;

        assert(!!s);

        result_type r = *s;
        ++ s;

        for(; !!s; ++ s)
        {
            r = std::move(r) * x + *s;
        }

        return r;
    }

    /** @brief ����� ����������
    @tparam A ��� �������������
    @tparam X ��� ����������
    @todo Alloc ��� �������������� ������
    */
    template <class A, class X>
    class polynomial
     : boost::additive<polynomial<A, X>>
     , boost::multipliable<polynomial<A, X>, X>
    {
        friend bool operator==(polynomial const & x, polynomial const & y)
        {
            return x.coefficients() == y.coefficients();
        }

    public:
        // ����
        typedef A coefficient_type;
        typedef std::vector<coefficient_type> coefficients_container;
        typedef X argument_type;
        typedef decltype(std::declval<A>() * std::declval<X>()) result_type;
        typedef typename coefficients_container::size_type size_type;

        // ������������
        polynomial()
         : cs_(1, coefficient_type{0})
        {}

        polynomial(std::initializer_list<coefficient_type> cs)
         : cs_{}
        {
            auto seq = sequence(cs);

            auto const zero = coefficient_type{0};

            seq = find(std::move(seq), zero, not_equal_to<>{});

            if (!seq)
            {
                cs_.assign(1, coefficient_type{0});
            }
            else
            {
                cs_.assign(seq.begin(), seq.end());
                ural::reverse(cs_);
            }
        }

        // ���������� ��������
        result_type operator()(argument_type const & x) const
        {
            return polynom(cs_ | ural::reversed, x);
        }

        // �������� ������������
        polynomial & operator+=(polynomial const & p)
        {
            auto const old_size = cs_.size();

            if(p.degree() > this->degree())
            {
                cs_.reserve(p.degree());

                cs_.insert(cs_.end(), p.cs_.begin() + old_size, p.cs_.end());
            }

            assert(cs_.size() >= old_size);

            for(size_type i = 0; i != old_size; ++ i)
            {
                cs_[i] += p.cs_[i];
            }

            return *this;
        }

        polynomial & operator*=(X const & a)
        {
            for(auto & c : cs_)
            {
                c *= a;
            }
            return *this;
        }

        // ������� ���� � �����
        polynomial operator+() const
        {
            return *this;
        }

        polynomial operator-() const
        {
            polynomial r = *this;

            for(auto & c : r.cs_)
            {
                c = -c;
            }

            return r;
        }

        // ��������
        size_type degree() const
        {
            assert(cs_.empty() == false);
            return cs_.size() - 1;
        }

        coefficient_type const & operator[](size_type n) const
        {
            // @todo ����� �������, ��� ������������ ������ ������� ���������� ����� ����,
            // ����������� ��� ����� ���������?
            return cs_[n];
        }

        coefficients_container const & coefficients() const
        {
            return this->cs_;
        }

    private:
        coefficients_container cs_;
    };
}
// namespace ural

#endif
// Z_URAL_NUMERIC_POLYNOM_HPP_INCLUDED
