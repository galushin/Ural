#ifndef Z_URAL_MATH_PRIMES_HPP_INCLUDED
#define Z_URAL_MATH_PRIMES_HPP_INCLUDED

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

/** @file ural/math/primes.hpp
 @brief Поиск простых чисел

 Должны возвращать функции, создающие списки простых чисел, std::vector?
 + std::vector --- это один из самых лучших контейнеров, который должен
 использоваться по умолчанию
 - явная зависимость от std::vector. Но многие ли "вычислительные" программы не
 будут зависеть от std::vector
 - Похоже, что пользователю в любом случае придётся задавать тип целых чисел
*/

#include <ural/algorithm.hpp>
#include <ural/sequence/make.hpp>
#include <ural/math/common_factor.hpp>

#include <cassert>
#include <vector>

namespace ural
{
    /** @brief Проверяет, что @c x является взаимно простым с элементами
    @c ds
    @param x число
    @param ds последовательность потенциальных делителей
    @return @b true, если @c x является взаимно простым с элементами @c ds,
    иначе --- @b false.
    @todo Выразить через all_of, при этом можно определить функциональный
    объект, который будет полезен и в других местах
    */
    class is_coprime_with_sequence_functor
    {
    public:
        template <class IntType, class Input>
        bool operator()(IntType const & x, Input && ds) const
        {
            for(auto seq = ural::sequence(ds); !!seq; ++ seq)
            {
                auto const & value = *seq;

                if(value*value > x)
                {
                    break;
                }

                if(ural::gcd(x, value) != IntType{1})
                {
                    return false;
                }
            }

            return true;
        }
    };

    auto constexpr is_coprime_with_sequence
        = is_coprime_with_sequence_functor{};

    /** @brief Построение списка первых простых чисел
    @tparam IntType тип, используемый для представления целых чисел
    @param n количество первых простых чисел, которых нужно найти
    @todo Выделить алгоритмы
    */
    template <class IntType, class Size>
    std::vector<IntType>
    make_first_n_primes(Size n)
    {
        assert(n >= 0);
        std::vector<IntType> primes;

        if(n == 0)
        {
            return primes;
        }

        primes.emplace_back(2);

        for(auto k = primes.back() + IntType{1}; primes.size() < n; k += IntType{2})
        {
            if(::ural::is_coprime_with_sequence(k, primes))
            {
                primes.push_back(k);
            }
        }

        return primes;
    }

    /** @brief Создание списка простых чисел, меньших данного
    @return вектор, содержащий простые числа, меньшие @c p_max
    @todo не вставлять в решето чётные числа
    */
    template <class IntType>
    std::vector<IntType>
    make_primes_below(IntType p_max)
    {
        std::vector<IntType> primes;

        if(p_max <= IntType{2})
        {
            return primes;
        }

        std::vector<bool> seive(p_max - 2, true);

        for(auto pos = ural::sequence(seive); !!pos; pos = ural::find(pos, true))
        {
            auto value = pos.traversed_front().size() + 2;

            for(auto i = value - 2; i < seive.size(); i += value)
            {
                seive[i] = false;
            }

            primes.emplace_back(std::move(value));
        }

        return primes;
    }
}
// namespace ural

#endif
// Z_URAL_MATH_PRIMES_HPP_INCLUDED
