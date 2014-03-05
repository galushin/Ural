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
 @todo Тест многочленов с коэффициентами-векторами
 @todo Проверять, что для типа аргумента выполняется decltype(x*x) == X
 @todo Вся арифметика многочленов: разделить, вычесть, вычислить остаток от
 деления
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

    template <class A, class X, class Alloc>
    class polynomial;

    template <class A, class X, class Alloc>
    class polynomial_base
    {
    public:
        typedef X argument_type;
        typedef decltype(std::declval<A>() * std::declval<X>()) result_type;

        result_type operator()(argument_type const & x) const
        {
            auto const & r = static_cast<polynomial<A, X, Alloc> const &>(*this);

            return polynom(r.coefficients() | ural::reversed, x);
        }

    protected:
        ~polynomial_base() = default;
    };

    template <class A, class Alloc>
    class polynomial_base<A, void, Alloc>
    {
    public:
        template <class X>
        auto operator()(X && x) const
        -> decltype(std::declval<A>() * x)
        {
            auto const & r = static_cast<polynomial<A, void, Alloc> const &>(*this);

            return polynom(r.coefficients() | ural::reversed, x);
        }

    protected:
        ~polynomial_base() = default;
    };

    /** @brief Класс многочлена
    @tparam A тип коэффициентов
    @tparam X тип аргументов
    @tparam Alloc тип распределителя памяти
    @todo Ослабить требование к типу множителя
    */
    template <class A, class X = void, class Alloc = std::allocator<A> >
    class polynomial
     : boost::additive<polynomial<A, X, Alloc>>
     , boost::multiplicative<polynomial<A, X, Alloc>, A>
     , public polynomial_base<A, X, Alloc>
    {
        friend bool operator==(polynomial const & x, polynomial const & y)
        {
            return x.coefficients() == y.coefficients();
        }

    public:
        // Типы
        typedef A coefficient_type;
        typedef std::vector<coefficient_type, Alloc> coefficients_container;
        typedef typename coefficients_container::size_type size_type;

        // Конструкторы
        polynomial()
         : cs_(1, coefficient_type{0})
        {}

        polynomial(std::initializer_list<coefficient_type> cs)
         : cs_{}
        {
            auto const zero = coefficient_type{0};

            auto seq = find(cs, zero, not_equal_to<>{});

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

        // Линейное пространство
        polynomial & operator+=(polynomial const & p)
        {
            auto const old_size = cs_.size();

            if(p.degree() > this->degree())
            {
                cs_.reserve(p.cs_.size());
                cs_.insert(cs_.end(), p.cs_.begin() + old_size, p.cs_.end());
            }

            assert(cs_.size() >= old_size);

            auto const n = std::min(old_size, p.cs_.size());

            for(size_type i = 0; i != n; ++ i)
            {
                cs_[i] += p.cs_[i];
            }

            this->drop_leading_zeros();

            return *this;
        }

        polynomial & operator-=(polynomial const & p)
        {
            // @todo Устранить дублирование с +=
            auto const old_size = cs_.size();

            if(p.degree() > this->degree())
            {
                cs_.reserve(p.cs_.size());

                for(size_t i = old_size; i != p.cs_.size(); ++ i)
                {
                    cs_.push_back(-p.cs_[i]);
                }
            }

            assert(cs_.size() >= old_size);

            auto const n = std::min(old_size, p.cs_.size());

            for(size_type i = 0; i != n; ++ i)
            {
                cs_[i] -= p.cs_[i];
            }

            this->drop_leading_zeros();

            return *this;
        }

        polynomial & operator*=(coefficient_type const & a)
        {
            for(auto & c : cs_)
            {
                c *= a;
            }
            return *this;
        }

        polynomial & operator/=(coefficient_type const & a)
        {
            assert(a != coefficient_type{0});

            for(auto & c : cs_)
            {
                c /= a;
            }

            return *this;
        }

        // Унарные плюс и минус
        polynomial operator+() const
        {
            return *this;
        }

        polynomial operator-() const
        {
            polynomial r = *this;

            std::transform(r.cs_.begin(), r.cs_.end(), r.cs_.begin(),
                           ural::negate<coefficient_type>{});

            return r;
        }

        // Свойства
        size_type degree() const
        {
            assert(cs_.empty() == false);
            return cs_.size() - 1;
        }

        coefficient_type const & operator[](size_type n) const
        {
            // @todo Можно считать, что коэффициенты старше степени многочлена равны нулю,
            // реализовать это через стратегию?
            return cs_[n];
        }

        coefficients_container const & coefficients() const
        {
            return this->cs_;
        }

    private:
        void drop_leading_zeros()
        {
            static const auto zero = coefficient_type{0};
            for(; cs_.size() > 1 && cs_.back() == zero;)
            {
                cs_.pop_back();
            }
        }

    private:
        coefficients_container cs_;
    };
}
// namespace ural

#endif
// Z_URAL_NUMERIC_POLYNOM_HPP_INCLUDED
