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
 @brief Многочлены и средства для работы с ними
 @todo Тест многочленов с коэффициентами-векторами
 @todo Вся арифметика многочленов: разделить, вычислить остаток от деления
*/

#include <ural/sequence/sink.hpp>

#include <boost/operators.hpp>

namespace ural
{
    template <class R, class X>
    class horner_accumulator
    {
    public:
        explicit horner_accumulator(R r0, X x)
         : data_{std::move(r0), std::move(x)}
        {}

        template <class C>
        horner_accumulator &
        operator()(C && arg)
        {
            data_[ural::_1] = std::move(data_[ural::_1]) * data_[ural::_2]
                            + std::forward<C>(arg);
            return *this;
        }

        R const & value() const
        {
            return data_[ural::_1];
        }

    private:
        ural::tuple<R, X> data_;
    };

    /**
    С одной стороны, многочлен может иметь нулевую степень, тогда тип результата
    будет совпадать с типом коэффициента. С другой стороны, рассмотрим многочлен
    с целыми коэффициентами и вещественными переменными. Его значения должны
    быть вещественными.
    */
    template <class Input, class X>
    auto polynom(Input && in, X const & x)
    -> decltype(sequence(in).front() * x)
    {
        auto s = sequence(in);

        typedef decltype(sequence(in).front() * x) result_type;

        assert(!!s);

        horner_accumulator<result_type, X> acc(*s, std::cref(x));
        ++ s;

        return ural::for_each(std::move(s), acc).value();
    }

    template <class A, class X, class Alloc>
    class polynomial;

    template <class A, class X, class Alloc>
    class polynomial_base
    {
    public:
        static_assert(std::is_same<X, decltype(std::declval<X>() * std::declval<X>())>::value,
                      "Type of polynom variable must be closed relative to multiplication");

        /// @brief Тип аргумента
        typedef X argument_type;

        /// @brief Тип возвращаемого значения
        typedef decltype(std::declval<A>() * std::declval<X>()) result_type;

        /** @brief Оператор вызова функции
        @param x аргумент
        @return значение многочлена в точке @c x
        */
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
        /** @brief Оператор вызова функции
        @param x аргумент
        @return значение многочлена в точке @c x
        */
        template <class X>
        auto operator()(X && x) const
        -> decltype(std::declval<A>() * x)
        {
            static_assert(std::is_same<X, decltype(std::declval<X>() * std::declval<X>())>::value,
                      "Type of polynom variable must be closed relative to multiplication");

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
    @todo Конструктор с парой итераторов. Выразить через него конструктор со списком
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
        /// @brief Тип коэффициентов
        typedef A coefficient_type;

        /// @brief Тип контейнера коэффициентов
        typedef std::vector<coefficient_type, Alloc> coefficients_container;

        /// @brief Тип для представления размера
        typedef typename coefficients_container::size_type size_type;

        // Конструкторы
        /** @brief Конструктор без параметров
        @post <tt> this->coefficients() == coefficients_container(1, 0) </tt>
        */
        polynomial()
         : cs_(1, coefficient_type{0})
        {}

        /** @brief Конструктор на основе последовательности, заданной парой
        итераторов
        @param first итератор, задающий начало последовательности
        @param last итератор, задающий конец последовательности
        @pre <tt> [first; last) </tt> должен быть корректным интервалом
        @todo написать пост-условие
        */
        template <class InputIterator>
        polynomial(InputIterator first, InputIterator last)
         : cs_{}
        {
            auto const zero = coefficient_type{0};

            auto seq = find(ural::make_iterator_sequence(first, last),
                            zero, not_equal_to<>{});

            if (!seq)
            {
                cs_.assign(1, zero);
            }
            else
            {
                cs_.assign(seq.begin(), seq.end());
                ural::reverse(cs_);
            }
        }

        /** @brief Конструктор на основе списка коэффициентов
        @param cs список коэффициентов
        @post <tt> this->coefficients() == coefficients_container(cs) </tt>
        */
        polynomial(std::initializer_list<coefficient_type> cs)
         : polynomial(cs.begin(), cs.end())
        {}

        // Линейное пространство
        /** @brief Прибавление многочлена
        @param p многочлен-слагаемое
        @return <tt> *this </tt>
        */
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

            std::transform(cs_.begin(), cs_.begin() + n, p.cs_.begin(),
                           sink_sequence<>{},
                           ural::plus_assign<>{});

            this->drop_leading_zeros();

            return *this;
        }

        /** @brief Вычитание многочлена
        @param p многочлен-вычитаемое
        @return <tt> *this </tt>
        */
        polynomial & operator-=(polynomial const & p)
        {
            // @todo Устранить дублирование с +=
            auto const old_size = cs_.size();

            if(p.degree() > this->degree())
            {
                cs_.reserve(p.cs_.size());

                for(auto i : numbers(old_size, p.cs_.size()))
                {
                    cs_.push_back(-p.cs_[i]);
                }
            }

            assert(cs_.size() >= old_size);

            auto const n = std::min(old_size, p.cs_.size());

            std::transform(cs_.begin(), cs_.begin() + n, p.cs_.begin(),
                           sink_sequence<>{},
                           ural::minus_assign<>{});

            this->drop_leading_zeros();

            return *this;
        }

        /** @brief Умножение многочлена на скаляр
        @param a множитель
        @return <tt> *this </tt>
        */
        polynomial & operator*=(coefficient_type const & a)
        {
            for(auto & c : cs_)
            {
                c *= a;
            }
            return *this;
        }

        /** @brief Деление многочлена на скаляр
        @param a делитель
        @return <tt> *this </tt>
        */
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
        /** @brief Унарный плюс
        @return <tt> *this </tt>
        */
        polynomial operator+() const
        {
            return *this;
        }

        /** @brief Унарный минус
        */
        polynomial operator-() const
        {
            polynomial r = *this;

            // @todo заменить на алгоритм ural
            std::transform(r.cs_.begin(), r.cs_.end(), r.cs_.begin(),
                           ural::negate<coefficient_type>{});

            return r;
        }

        // Свойства
        /** @brief Степень многочлена
        @return <tt> this->coefficients().size() - 1 </tt>
        */
        size_type degree() const
        {
            assert(!ural::empty(cs_));
            return cs_.size() - 1;
        }

        //@{
        /** @brief Доступ к коэффициентам
        @param n степень члена, к которой относится коэффициент
        @return Ссылка на коэффициент, соответствующий данной степени.
        @pre <tt> n <= this->degree() </tt>

        Можно считать, что у многочленов отсутствующие коэффициенты равны нулю.
        Проблема в том, что это приведёт к семантическим различиям между
        константной и неконстантной версиями. Кроме того, условная логика будет
        замедлять доступ из-за сброса конвейера.
        */
        coefficient_type & operator[](size_type n);

        coefficient_type const & operator[](size_type n) const
        {
            return cs_[n];
        }
        //@}

        /** @brief Контейнер коэффициентов
        @return Ссылка на контейнер, содержащий коэффициенты многочлена
        */
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
