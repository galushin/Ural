#ifndef Z_URAL_DISTRIBUTIONS_DISCRETE_HPP_INCLUDED
#define Z_URAL_DISTRIBUTIONS_DISCRETE_HPP_INCLUDED

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

/** @file ural/distributions/discrete.hpp
 @brief "Математическое" дискретное распределение
*/

#include <ural/statistics.hpp>
#include <ural/numeric.hpp>

#include <vector>

namespace ural
{
/** @namespace distributions
 @brief "Теоретические" распределения вероятностей.
*/
namespace distributions
{
    /** @brief "Математическое" дискретное распределение
    @tparam IntType тип значений
    @tparam Weight тип весов
    @todo Использовать для представления вероятностей probability<>?
    @todo Настройка способа диагностики
    @todo Конструктор на основе контейнера/последовательности
    @todo Конструктор discrete(size_t nw, double xmin, double xmax, UnaryOperation fw)
    @note Числовые характеристики вычисляются в конструкторе. Дело в том, что
    это позволяет съэкономить линейное время за счёт постоянных затрат памяти.
    Хранение значений @c cdf требует линейного объёма памяти и уменьшает
    сложность с O(N) до O(log(N)), что не является однозначным выигрышем.
    */
    template <class IntType = int, class Weight = double>
    class discrete
    {
    public:
        // Типы
        // @todo Уточнить этот тип, возможно нужно вообще отказаться от этого шаблонного параметра
        /// @brief Тип значения
        typedef IntType value_type;

        /// @brief Тип весов
        typedef Weight weight_type;

        /// @brief Тип вероятностей
        typedef typename ural::average_type<weight_type, weight_type>::type
            probability_type;

        /// @brief Тип контейнера для хранения вероятностей
        typedef std::vector<probability_type> probabilities_vector;

        /// @brief Тип математического ожидания
        typedef decltype(std::declval<value_type>() * std::declval<probability_type>())
            mean_type;

        /// @brief Тип квадрата
        typedef decltype(square(std::declval<value_type>())) square_type;

        /// @brief Тип дисперсии
        typedef  decltype(std::declval<square_type>() * std::declval<probability_type>())
            variance_type;

        // Конструкторы
        /** @brief Конструктор без параметров
        @post <tt> this->probabilities() = {1.0} </tt>
        */
        discrete()
         : ps_{weight_type{1}}
         , mean_(0)
         , variance_(0)
        {}

        /** @brief Конструктор на основе интервала, заданного парой итераторов
        @param first итератор, задающий начало интервала
        @param last итератор, задающий конеч интервала
        @pre <tt> [first; last) </tt> должен быть корректным интервалом
        @pre Все элементы <tt> [first; last) </tt> должны быть неотрицательными
        конечными числами.
        @post Если <tt> first == last </tt>, то данный конструктор эквивалентен
        конструктору без аргументов.
        @post Если <tt> first != last </tt>, для любого @c i из интервала
        <tt> [0; distance(first, last)) </tt> имеет место равенство
        <tt> this->probabilities()[i] == *(first + i) / w_sum </tt>, где
        @c w_sum --- сумма всех элементов интервала <tt> [first; last) </tt>.
        */
        template <class Iterator>
        discrete(Iterator first, Iterator last)
         : ps_(first, last)
         , mean_(0)
         , variance_(0)
        {
            if(ps_.empty())
            {
                ps_.resize(1u, weight_type(1));
            }
            else
            {
                auto const w_sum = ural::accumulate(ps_, weight_type{0});

                assert(w_sum > 0);

                // @todo Заменить на алгоритм
                for(auto & p : ps_)
                {
                    enforce_weight(p);

                    p /= w_sum;
                }
            }

            // @todo можно ли рассчитать числовые характеристики вместе с суммой весов?
            auto values = ural::numbers_sequence<value_type>(0, ps_.size());
            auto s = ural::describe(std::move(values),
                                    ural::statistics::tags::variance,
                                    ps_);
            mean_ = s.mean();
            variance_ = s.variance();
        }

        /** @brief Конструктор на основе списка инициализаторов
        @param ws список весов
        @post То же, что и <tt> discrete(ws.begin(), ws.end()) </tt>
        */
        discrete(std::initializer_list<weight_type> ws)
         : discrete(ws.begin(), ws.end())
        {}

        // Характеристики распределения
        /** @brief Математическое ожидание
        @param d распределение
        @return Значение математического ожидания
        */
        friend mean_type const & mean(discrete const & d)
        {
            return d.mean_;
        }

        /** @brief Дисперсия
        @param d распределение
        */
        friend variance_type const & variance(discrete const & d)
        {
            return d.variance_;
        }

        /** @brief Стандартное отклонение
        @return <tt> sqrt(variance(d)) </tt>
        @param d распределение
        @todo Уточнить тип возвращаемого значения
        */
        friend variance_type standard_deviation(discrete const & d)
        {
            using std::sqrt;
            return sqrt(variance(d));
        }

        /** Вычисляет значение функции распределения @c d в точке @c x
        @brief Функция распределения
        @param d распределение
        @param x точка в которой вычисляется фунция распределения
        */
        template <class T>
        friend probability_type cdf(discrete const & d, T const & x)
        {
            if(x < 0)
            {
                return probability_type(0);
            }

            if(x >= ural::to_signed(d.ps_.size()))
            {
                return probability_type(1);
            }

            auto result = probability_type(0);

            // @todo Заменить на алгоритм
            for(auto i : ural::indices_of(d.ps_))
            {
                if(ural::to_signed(i) > x)
                {
                    break;
                }

                result += d.ps_[i];
            }

            return result;
        }

        /** @brief Вектор вероятностей
        @return Вектор вероятностей
        */
        probabilities_vector const & probabilities() const
        {
            return this->ps_;
        }

        value_type min() const
        {
            return value_type(0);
        }

        value_type max() const
        {
            assert(!ps_.empty());
            // @todo всегда ли это безопасно
            return value_type(this->ps_.size() - 1u);
        }

    private:
        static weight_type const & enforce_weight(weight_type const & w)
        {
            using std::isfinite;
            assert(isfinite(w) == true);
            assert(w >= 0);

            return w;
        }

    private:
        probabilities_vector ps_;
        mean_type mean_;
        variance_type variance_;
    };
}
// namespace distributions
}
// namespace ural

#endif
// Z_URAL_DISTRIBUTIONS_DISCRETE_HPP_INCLUDED
