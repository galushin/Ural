#ifndef Z_URAL_NUMERIC_INTERPOLATION_HPP_INCLUDED
#define Z_URAL_NUMERIC_INTERPOLATION_HPP_INCLUDED

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

/** @file ural/numeric/interpolation.hpp
 @brief Интерполяция
*/

namespace ural
{
    /** @brief Интерполяционный многочлен Ньютона
    @param X тип аргумента
    @param Y тип значения
    */
    template <class X = double, class Y = X>
    class newton_polynomial
    {
    public:
        // Типы
        /// @brief Тип размера
        typedef size_t size_type;

        /// @brief Тип возвращаемого значения
        typedef Y result_type;

        /// @brief Тип аргумента
        typedef X argument_type;

        /// @brief Отпимальный тип для передачи аргумента
        typedef typename boost::call_traits<argument_type>::param_type param_type;

        // Конструкторы

        // Добавление новых значений
        /** @brief Добавления новой точки к многочлену
        @param x значение аргумента
        @param y значение функции
        @todo Настройка обработки повторяющихся @c x
        */
        void update(X const & x, Y const & y)
        {
            auto denom = X{1};

            for(auto & d : data_)
            {
                denom *= (x - d.first);
            }

            assert(std::abs(denom) > 0);

            auto K = (y - (*this)(x)) / denom;

            data_.emplace_back(x, std::move(K));
        }

        // Свойства
        /** @brief Степень многочлена
        @return Степень многочлена
        */
        size_type degree() const
        {
            return data_.empty() ? 0 : data_.size() - 1;
        }

        /** @brief Вычисление значения многочлена в точке
        @param x точка, в которой вычисляется значение многочлена
        @return Значение многочлена в данной точке
        */
        result_type operator()(param_type x) const
        {
            if(data_.empty())
            {
                return result_type{0};
            }

            auto i = ural::sequence(data_);

            auto r = result_type{i->second};
            auto denom = (x - i->first);
            ++ i;

            for(; !!i; ++ i)
            {
                r += i->second * denom;
                denom *= (x - i->first);
            }

            return r;
        }

    private:
        typedef std::pair<X, Y> Pair;
        std::vector<Pair> data_;
    };
}
// namespace ural

#endif
// Z_URAL_NUMERIC_INTERPOLATION_HPP_INCLUDED
