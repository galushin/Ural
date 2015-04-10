#ifndef Z_URAL_NUMERIC_HPP_INCLUDED
#define Z_URAL_NUMERIC_HPP_INCLUDED

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

/** @file ural/numeric.hpp
 @brief Обобщённые численные операции
*/

#include <ural/math.hpp>
#include <ural/functional.hpp>
#include <ural/algorithm/copy.hpp>
#include <ural/sequence/all.hpp>

#include <ural/numeric/numbers_sequence.hpp>
#include <ural/numeric/partial_sums.hpp>
#include <ural/numeric/adjacent_differences.hpp>

namespace ural
{
    class
    {
    public:
        template <class ForwardSequence, class Incrementable>
        Incrementable
        operator()(ForwardSequence && seq, Incrementable init_value) const
        {
            return impl(ural::sequence(std::forward<ForwardSequence>(seq)),
                        std::move(init_value));
        }

    private:
        template <class ForwardSequence, class Incrementable>
        Incrementable
        impl(ForwardSequence seq, Incrementable init_value) const
        {
            for(; !!seq; ++ seq, ++ init_value)
            {
                *seq = init_value;
            }

            return init_value;
        }
    }
    constexpr iota{};

    class
    {
    public:
        template <class Input, class T, class BinaryOperation>
        T operator()(Input && in, T init_value, BinaryOperation op) const
        {
            return impl(sequence(std::forward<Input>(in)),
                        std::move(init_value),
                        ural::make_callable(std::move(op)));
        }

        template <class Input, class T>
        T operator()(Input && in, T init_value) const
        {
            return (*this)(std::forward<Input>(in), std::move(init_value),
                           ural::plus<>());
        }

    private:
        template <class InputSequence, class T, class BinaryOperation>
        T impl(InputSequence in, T init_value, BinaryOperation op) const
        {
            for(; !!in; ++ in)
            {
                init_value = op(std::move(init_value), *in);
            }

            return init_value;
        }
    }
    constexpr accumulate {};

    class
    {
    public:
        template <class Input1, class Input2, class T,
                class BinaryOperation1, class BinaryOperation2>
        T operator()(Input1 && in1, Input2 && in2, T init_value,
                     BinaryOperation1 add, BinaryOperation2 mult) const
        {
            return impl(sequence(std::forward<Input1>(in1)),
                        sequence(std::forward<Input2>(in2)),
                        std::move(init_value),
                        ural::make_callable(std::move(add)),
                        ural::make_callable(std::move(mult)));
        }

        template <class Input1, class Input2, class T>
        T operator()(Input1 && in1, Input2 && in2, T init_value) const
        {
            return (*this)(std::forward<Input1>(in1), std::forward<Input2>(in2),
                           std::move(init_value),
                           ural::plus<>{}, ural::multiplies<>{});
        }

    private:
        template <class Input1, class Input2, class T,
                  class BinaryOperation1, class BinaryOperation2>
        T impl(Input1 in1, Input2 in2, T value,
               BinaryOperation1 add, BinaryOperation2 mult) const
        {
            for(; !!in1 && !!in2; ++ in1, ++in2)
            {
                value = add(std::move(value), mult(*in1, *in2));
            }

            return value;
        }
    }
    constexpr inner_product{};

    /**
    @todo Можно ли ослабить требования к входным последовательностям?
    @todo Настройка операций сложения и умножения?
    @todo Проверка случая пустых контейнеров и контейнеров с одним элементом
    @tparam RASequence1 тип первой последовательности
    @tparam RASequence2 тип второй последовательности
    */
    template <class RASequence1, class RASequence2>
    class convolution_sequence
     : public sequence_base<convolution_sequence<RASequence1, RASequence2> >
    {
    public:
        /// @brief Категория обхода
        typedef forward_traversal_tag traversal_tag;

        /// @brief Тип расстояния
        typedef typename std::common_type<typename RASequence1::distance_type,
                                          typename RASequence2::distance_type>::type distance_type;

        /// @brief Тип значения
        typedef decltype(*std::declval<RASequence1>() * *std::declval<RASequence2>())
            value_type;

        /// @brief Тип ссылки
        typedef value_type const & reference;

        /// @brief Тип указателя
        typedef value_type const * pointer;

        /** @brief Конструктор
        @param s1 первая последовательность
        @param s2 вторая последовательность
        */
        convolution_sequence(RASequence1 s1, RASequence2 s2)
         : members_(std::move(s1), std::move(s2), 0, value_type{0})
        {
            this->calc();
        }

        // Однопроходная последовательность
        bool operator!() const
        {
            auto const n = members_[ural::_1].size() + members_[ural::_2].size();
            return members_[ural::_3] + 1 >= n;
        }

        reference front() const
        {
            return members_[ural::_4];
        }

        void pop_front()
        {
            ++ members_[ural::_3];
            this->calc();
        }

    private:
        void calc()
        {
            if(!members_[ural::_1] || !members_[ural::_2])
            {
                return;
            }

            auto const pos = members_[ural::_3];

            members_[ural::_4] = 0;

            // j = pos - i
            // 0 <= i <= n1 - 1
            // 0 <= pos - i <= n2-1
            // - pos <= -i <= n2-1 - pos
            // pos-n2+1 <= i <= pos

            auto const n2 = members_[ural::_2].size();
            auto const i_min = std::max(pos - n2 + 1, decltype(pos){0});
            auto const i_max = std::min(members_[ural::_1].size(), pos+1);

            for(auto i = i_min; i < i_max; ++ i)
            {
                assert(pos >= i);

                auto const j = pos - i;

                assert(i >= 0);
                assert(i < members_[ural::_1].size());

                assert(j >= 0);
                assert(j < members_[ural::_2].size());

                members_[ural::_4] += members_[ural::_1][i]* members_[ural::_2][j];
            }
        }

        ural::tuple<RASequence1, RASequence2, distance_type, value_type> members_;
    };

    template <class RASequence1, class RASequence2>
    auto make_convolution_sequence(RASequence1 && s1, RASequence2 && s2)
    -> convolution_sequence<decltype(sequence(std::forward<RASequence1>(s1))),
                            decltype(sequence(std::forward<RASequence2>(s2)))>
    {
        return {sequence(std::forward<RASequence1>(s1)),
                sequence(std::forward<RASequence2>(s2))};
    }

    class discrete_convolution_functor
    {
    public:
        template <class Vector>
        Vector operator()(Vector const & x, Vector const & y) const
        {
            // @todo Оптимизированная версия (быстрое преобразование Фурье)
            assert(!ural::empty(x) || !ural::empty(y));

            Vector result(x.size() + y.size() - 1);

            copy_fn{}(ural::make_convolution_sequence(x, y),
                      ural::sequence(result));

            return result;
        }
    };

    auto constexpr const discrete_convolution = discrete_convolution_functor{};

    /** @brief Последовательность для вычисления приближённого значения
    квадратого корня по итерационному методу Герона. Смотри, например
    http://en.wikipedia.org/wiki/Methods_of_computing_square_roots#Babylonian_method
    @todo отделить критерий остановки, пусть исходная последовательность будет
    бесконечной.
    */
    template <class RealType>
    class sqrt_heron_sequence
     : public sequence_base<sqrt_heron_sequence<RealType>>
    {
    public:
        // Типы
        typedef RealType value_type;

        typedef value_type const & reference;

        // Конструктор
        sqrt_heron_sequence(RealType S, RealType x0, RealType eps)
         : s_(std::move(S))
         , x0_(std::move(x0))
         , eps_(std::move(eps) * 0.1)
         , done_(false)
        {
            assert(s_ >= 0);

            if(s_ < eps_)
            {
                x0_ = s_;
            }
        }

        // Однопроходная последовательность
        bool operator!() const
        {
            return this->done_;
        }

        reference front() const
        {
            return this->x0_;
        }

        void pop_front()
        {
            using std::abs;
            if(abs(ural::square(x0_) - s_) < eps_)
            {
                done_ = true;
                return;
            }

            assert(x0_ > 0);

            x0_ = (x0_ + s_ / x0_) / RealType(2);
        }

    private:
        RealType s_;
        RealType x0_;
        RealType eps_;
        bool done_;
    };

    template <class RealType>
    sqrt_heron_sequence<RealType>
    make_sqrt_heron_sequence(RealType S, RealType x0, RealType eps)
    {
        return sqrt_heron_sequence<RealType>(std::move(S), std::move(x0),
                                             std::move(eps));
    }

    /** @brief Последовательность строк треугольника Паскаля
    @tparam Vector тип массива, используемого для хранения строк
    @todo Можно ли ослабить требования к контейнеру?
    @todo Можно ли усилит категорию обхода?
    */
    template <class Vector>
    class pascal_triangle_rows_sequence
     : public sequence_base<pascal_triangle_rows_sequence<Vector>>
    {
    public:
        // Типы
        /// @brief Тип значения
        typedef Vector value_type;

        /// @brief Тип ссылки
        typedef Vector const & reference;

        // Конструкторы
        /** @brief Конструктор без параметров
        @post <tt> this->front() == {1} </tt>
        */
        pascal_triangle_rows_sequence()
         : row_{1}
        {}

        // Однопроходная последовательность
        /** @brief Проверка исчерпания последовательности
        @return @b true, если последовательность исчерпана, иначе -- @b false
        */
        bool operator!() const
        {
            return false;
        }

        /** @brief Текущий элемент
        @return Ссылка на текущий элемент
        */
        reference front() const
        {
            return row_;
        }

        /// @brief Переход к следующему элементу
        void pop_front()
        {
            Vector new_row(row_.size() + 1, 1);

            auto const middle = (new_row.size() + 1) / 2;

            for(auto i : ural::numbers(1, middle))
            {
                new_row[i] = row_[i] + row_[i-1];
            }

            for(auto i : ural::numbers(middle, row_.size()))
            {
                new_row[i] = new_row[row_.size() - i];
            }

            row_.swap(new_row);
        }

    private:
        Vector row_;
    };
}
// namespace ural

#endif
// Z_URAL_NUMERIC_HPP_INCLUDED
