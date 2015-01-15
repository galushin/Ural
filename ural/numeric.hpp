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
#include <ural/algorithm/details/copy.hpp>
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
                        ural::make_functor(std::move(op)));
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
            return impl(ural::sequence(std::forward<Input1>(in1)),
                        ural::sequence(std::forward<Input2>(in2)),
                        std::move(init_value),
                        ural::make_functor(std::move(add)),
                        ural::make_functor(std::move(mult)));
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
    @todo Можно ли ослабить требования к входным последовательностям
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
            // @todo Оптимизированная версия
            // @todo Поддержка контейнеров без возможности изменения размера
            assert(!x.empty() || !y.empty());

            Vector result;

            ural::details::copy(ural::make_convolution_sequence(x, y),
                                ural::sequence(result | ural::back_inserter));

            assert(result.size() == x.size() + y.size() - 1);

            return result;
        }
    };

    auto constexpr const discrete_convolution = discrete_convolution_functor{};

    /** @brief Функциональный объект для вычисления приближённого значения
    квадратого корня по итерационному методу Герона. Смотри, например
    en.wikipedia.org/wiki/Methods_of_computing_square_roots#Babylonian_method
    */
    class sqrt_heron_f
    {
    public:
        /** @brief S число, для которого вычисляется приближённое значение
        квадратного корня
        */
        template <class RealType>
        RealType
        operator()(RealType S, RealType x0, RealType const & eps) const
        {
            assert(S >= 0);

            if(S == RealType{0})
            {
                return S;
            }

            assert(x0 > 0);

            for(;;)
            {
                auto const x_old = x0;

                x0 = (x0 + S / x0) / RealType{2};

                using std::abs;
                if(abs(x0 - x_old) < eps)
                {
                    break;
                }
            }

            return x0;
        }
    };

    auto constexpr sqrt_heron = sqrt_heron_f{};
}
// namespace ural

#endif
// Z_URAL_NUMERIC_HPP_INCLUDED
