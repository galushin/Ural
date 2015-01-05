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
 @brief ѕоиск простых чисел

 ƒолжны возвращать функции, создающие списки простых чисел, std::vector?
 + std::vector --- это один из самых лучших контейнеров, который должен
 использоватьс€ по умолчанию
 - €вна€ зависимость от std::vector. Ќо многие ли "вычислительные" программы не
 будут зависеть от std::vector
 - ѕохоже, что пользователю в любом случае придЄтс€ задавать тип целых чисел
*/

#include <ural/sequence/make.hpp>
#include <ural/math/common_factor.hpp>

#include <cassert>
#include <vector>

namespace ural
{
    /** @brief ѕровер€ет, что @c x €вл€етс€ взаимно простым с элементами
    @c ds
    @param x число
    @param ds последовательность потенциальных делителей
    @return @b true, если @c x €вл€етс€ взаимно простым с элементами @c ds,
    иначе --- @b false.
    @todo ¬ыразить через all_of, при этом можно определить функциональный
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

    /** @brief ѕостроение списка первых простых чисел
    @tparam IntType тип, используемый дл€ представлени€ целых чисел
    @param n количество первых простых чисел, которых нужно найти
    @todo ¬ыделить алгоритмы
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
}
// namespace ural

#endif
// Z_URAL_MATH_PRIMES_HPP_INCLUDED
