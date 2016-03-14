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

#include <ural/numeric/numbers_sequence.hpp>
#include <ural/algorithm.hpp>
#include <ural/sequence/make.hpp>
#include <ural/math/common_factor.hpp>

#include <cassert>
#include <vector>

namespace ural
{
namespace experimental
{
    /** @brief Тип функционального объекта, для проверки того, что число
    является простым.

    Заметим, что эту функцию не следует использовать во всех случаях. Если
    нужно проверять простоту большого количества чисел, то может оказаться более
    целесообразным построение списка простых чисел.
    */
    class is_prime_f
    {
    public:
        /** @brief Проверка простоты числа
        @eturn @b true, если @c x --- простое число, иначе --- @b false
        */
        template <class IntType>
        bool operator()(IntType const & x) const
        {
            if(x == IntType{1})
            {
                return false;
            }

            if(x == IntType{2} || x == IntType{3})
            {
                return true;
            }

            if(x % IntType(2) == 0)
            {
                return false;
            }

            if(x % IntType(3) == 0)
            {
                return false;
            }

            auto const step = IntType{2};

            for(auto d = IntType(5);; d += step)
            {
                if(ural::square(d) > x)
                {
                    break;
                }

                if(x % d == 0)
                {
                    return false;
                }
            }

            return true;
        }
    };

    /** @brief Проверяет, что @c x является взаимно простым с элементами
    @c ds
    @param x число
    @param ds последовательность потенциальных делителей
    @return @b true, если @c x является взаимно простым с элементами @c ds,
    иначе --- @b false.
    @todo Выразить через all_of, при этом можно определить функциональный
    объект, который будет полезен и в других местах
    */
    class is_not_divisible_by_all_sorted_f
    {
    public:
        /** @pre <tt> Input упорядоченная по возрастанию последовательность </tt>
        */
        template <class IntType, class Input>
        bool operator()(IntType const & x, Input && ds) const
        {
            for(auto seq = ural::sequence_fwd<Input>(ds); !!seq; ++ seq)
            {
                auto const & value = *seq;

                if(ural::square(value) > x)
                {
                    break;
                }

                if((x % value) == 0)
                {
                    return false;
                }
            }

            return true;
        }
    };

    /** @brief Функциональный объект, проверяющий, что число является
    взаимно-простым со всеми числами последовательности
    */
    class is_coprime_with_all_f
    {
    public:
        template <class IntType, class Input>
        bool operator()(IntType const & x, Input && ds) const
        {
            for(auto seq = ::ural::sequence_fwd<Input>(ds); !!seq; ++ seq)
            {
                using ::ural::experimental::gcd;
                if(gcd(x, *seq) != 1)
                {
                    return false;
                }
            }

            return true;
        }
    };

    namespace
    {
        /** @brief Функциональный объект, для проверки того, что число является
        простым.
        */
        constexpr auto const & is_prime = odr_const<is_prime_f>;

        /** @brief Функциональный объект, для проверки того, что число не
        делится ни на одно из набора упорядоченных по возрастанию чисел.
        */
        constexpr auto const & is_not_divisible_by_all_sorted =
            odr_const<is_not_divisible_by_all_sorted_f>;

        /** @brief Функциональный объект, для проверки того, что число является
        взаимно простым со всеми числами из упорядоченной по возрастанию
        последовательности.
        */
        constexpr auto const & is_coprime_with_all =
            odr_const<is_coprime_with_all_f>;
    }


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
            if(::ural::experimental::is_not_divisible_by_all_sorted(k, primes))
            {
                primes.push_back(k);
            }
        }

        return primes;
    }

    /** @brief Создание списка простых чисел, меньших данного
    @return вектор, содержащий простые числа, меньшие @c p_max
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

        primes.emplace_back(2);

        if(p_max == IntType{3})
        {
            return primes;
        }

        auto const x_min = IntType{3};

        std::vector<bool> seive((p_max - x_min + 1)/2, true);

        for(auto pos = ural::sequence(seive); !!pos; pos = ural::find(pos, true))
        {
            auto const index = pos.traversed_front().size();
            auto value = 2 * index + x_min;

            for(auto i : ural::numbers(index, seive.size(), value))
            {
                seive[i] = false;
            }

            primes.emplace_back(std::move(value));
        }

        return primes;
    }
}
// namespace experimental
}
// namespace ural

#endif
// Z_URAL_MATH_PRIMES_HPP_INCLUDED
