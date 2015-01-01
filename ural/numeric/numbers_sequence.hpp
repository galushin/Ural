#ifndef Z_URAL_NUMERIC_NUMBERS_RANGE_HPP_INCLUDED
#define Z_URAL_NUMERIC_NUMBERS_RANGE_HPP_INCLUDED

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

/** @file ural/numeric/numbers_sequence.hpp
 @brief Последовательность чисел
*/

#include <ural/tuple.hpp>
#include <ural/sequence/base.hpp>

namespace ural
{
    template <class Number>
    class numbers_sequence
     : public sequence_base<numbers_sequence<Number>>
    {
    public:
        /// @brief Тип значения
        typedef Number value_type;

        /// @brief Тип ссылки
        typedef value_type const & reference;

        /// @brief Тип указателя
        typedef value_type const & pointer;

        /// @brief Тип расстояния
        typedef Number distance_type;

        // Создание, уничтожение, копирование
        numbers_sequence(Number x_min, Number x_max)
         : data_(std::move(x_min), std::move(x_max))
        {}

        // Однопроходная последовательность
        bool operator!() const
        {
            return data_[ural::_1] >= data_[ural::_2];
        }

        reference front() const
        {
            return data_[ural::_1];
        }

        void pop_front()
        {
            data_[ural::_1] += 1;
        }

    private:
        ural::tuple<Number, Number> data_;
    };

    template <class T1, class T2>
    numbers_sequence<typename std::common_type<T1, T2>::type>
    numbers(T1 x_min, T2 x_max)
    {
        typedef typename std::common_type<T1, T2>::type Number;

        return numbers_sequence<Number>(std::move(x_min), std::move(x_max));
    }
}
// namespace ural

#endif
// Z_URAL_NUMERIC_NUMBERS_RANGE_HPP_INCLUDED
