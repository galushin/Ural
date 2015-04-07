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
    @todo Диагностика отрицательных, бесконечных и nan весов, а также нулевой
    суммы весов
    @todo Хранить или вычислять cdf? А остальные характеристики?
    @todo Конструктор discrete(size_t nw, double xmin, double xmax, UnaryOperation fw)
    @todo Одновременное вычисление математического ожидания и дисперсии?
    */
    template <class IntType = int, class Weight = double>
    class discrete
    {
    public:
        // Типы
        /// @brief Тип значения
        typedef IntType value_type;

        /// @brief Тип весов
        typedef Weight weight_type;

        /// @brief Тип вероятностей
        typedef weight_type probability_type;

        /// @brief Тип контейнера для хранения вероятностей
        typedef std::vector<probability_type> probabilities_vector;

        // Конструкторы
        /** @brief Конструктор без параметров
        @post <tt> this->probabilities() = {1.0} </tt>
        */
        discrete()
         : ps_{weight_type{1.0}}
        {}

        /** @brief Конструктор на основе интервала, заданного парой итераторов
        @param first итератор, задающий начало интервала
        @param last итератор, задающий конеч интервала
        @pre <tt> [first; last) </tt> должен быть корректным интервалом
        @pre Все элементы <tt> [first; last) </tt> должны быть неотрицательными
        конечными числами.
        @post Если <tt> first == last </tt>, то данный конструктор эквивалентен
        конструктору без аргументов.
        @todo Постусловие для <tt> first != last </tt>
        */
        template <class Iterator>
        discrete(Iterator first, Iterator last)
         : ps_(first, last)
        {
            if(ps_.empty())
            {
                ps_.resize(1u, weight_type{1.0});
            }
            else
            {
                auto const w_sum = ural::accumulate(ps_, weight_type{0});

                assert(w_sum > 0);

                // @todo Заменить на алгоритм
                for(auto & p : ps_)
                {
                    assert(p >= 0);

                    p /= w_sum;
                }
            }
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
        @todo Тип возвращаемого значения
        */
        friend weight_type mean(discrete const & d)
        {
            return ural::inner_product(ural::indices_of(d.ps_),
                                       d.ps_, weight_type{0});
        }

        /** @brief Дисперсия
        @param d распределение
        @todo Тип возвращаемого значения
        @todo Более устойчивый алгоритм вычисления дисперсии
        */
        friend weight_type variance(discrete const & d)
        {
            auto const m = mean(d);

            // @todo Заменить на алгоритм
            auto result = weight_type{0};

            for(auto i : ural::indices_of(d.ps_))
            {
                using ural::square;
                result += square(i - m) * d.ps_[i];
            }

            return result;
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

        /** @brief Вектор вероятностей
        @return Вектор вероятностей
        */
        probabilities_vector const & probabilities() const
        {
            return this->ps_;
        }

    private:
        probabilities_vector ps_;
    };
}
// namespace distributions
}
// namespace ural

#endif
// Z_URAL_DISTRIBUTIONS_DISCRETE_HPP_INCLUDED
