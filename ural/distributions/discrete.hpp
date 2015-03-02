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

namespace ural
{
namespace distributions
{
    /** @brief "Математическое" дискретное распределение
    @tparam IntType тип значений
    @tparam Weight тип весов
    @todo Добавить конструктор на основе списка инициализации
    */
    template <class IntType = int, class Weight = double>
    class discrete
    {
    public:
        /// @brief Тип значения
        typedef IntType value_type;

        /// @brief Тип весов
        typedef Weight weight_type;

        /** @brief Конструктор без параметров
        @todo Добавить пост-условия
        */
        discrete()
         : ps_{weight_type{1.0}}
        {}

        /** @brief Конструктор на основе интервала, заданного парой итераторов
        @param first итератор, задающий начало интервала
        @param last итератор, задающий конеч интервала
        @pre <tt> [first; last) </tt> должен быть корректным интервалом
        @todo Добавить пост-условия
        */
        template <class Iterator>
        discrete(Iterator first, Iterator last)
         : ps_(first, last)
        {
            auto const w_sum = ural::accumulate(ps_, weight_type{0});

            // @todo Заменить на алгоритм
            for(auto & p : ps_)
            {
                p /= w_sum;
            }
        }

        // Характеристики распределения
        /** @brief Математическое ожидание
        @param d распределение
        @todo Тип возвращаемого значения
        */
        friend weight_type mean(discrete const & d)
        {
            return ural::inner_product(ural::numbers(0, d.ps_.size()),
                                       d.ps_, weight_type{0});
        }

        /** @brief Дисперсия
        @param d распределение
        @todo Тип возвращаемого значения
        @todo Более устойчивый алгоритм вычисления дисперсии
        */
        friend weight_type variance(discrete const & d)
        {
            using ural::square;

            auto result = weight_type{0};

            for(auto i : ural::indices_of(d.ps_))
            {
                result += square(i) * d.ps_[i];
            }

            return result - square(mean(d));
        }

        /** @brief Стандартное отклонение
        @return <tt> sqrt(variance(d)) </tt>
        @param d распределение
        */
        friend weight_type standard_deviation(discrete const & d)
        {
            using std::sqrt;
            return sqrt(variance(d));
        }

        /** Вычисляет значение функции распределения @c d в точке @c x
        @brief Функция распределения
        @param d распределение
        @param x точка в которой вычисляется фунция распределения
        @todo Тип возвращаемого значения
        */
        friend weight_type cdf(discrete const & d, weight_type const & x)
        {
            if(x >= d.ps_.size())
            {
                return weight_type{1.0};
            }

            auto result = weight_type{0};

            for(auto i : ural::indices_of(d.ps_))
            {
                if(i > x)
                {
                    break;
                }

                result += d.ps_[i];
            }

            return result;
        }

    private:
        std::vector<weight_type> ps_;
    };
}
// namespace distributions
}
// namespace ural

#endif
// Z_URAL_DISTRIBUTIONS_DISCRETE_HPP_INCLUDED
