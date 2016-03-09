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
namespace experimental
{
    template <class X>
    class numeric_policy
    {
    public:
        /** Функция проверки неравенства нулю, если @c x равно нулю, выдаётся
        ошибка нарушения утверждения.
        @brief Функция проверки неравенства нулю
        @param x проверяемое значение
        @return @b true
        */
        static bool check_not_zero(X const & x)
        {
            using std::abs;
            assert(abs(x) > 0);

            return true;
        }
    };

    /** @brief Интерполяционный многочлен Ньютона
    @param X тип аргумента
    @param Y тип значения
    @tparam Policy стратегия обработки ошибок
    */
    template <class X = use_default, class Y = use_default, class Policy = use_default>
    class newton_polynomial
    {
    public:
        // Типы
        /// @brief Тип размера
        typedef size_t size_type;

        /// @brief Тип аргумента
        using argument_type = experimental::DefaultedType<X, double>;

        /// @brief Тип возвращаемого значения
        using result_type = experimental::DefaultedType<Y, argument_type>;

        /// @brief Тип стратегии
        using policy_type = experimental::DefaultedType<Policy, numeric_policy<argument_type>>;

        /// @brief Отпимальный тип для передачи аргумента
        typedef typename boost::call_traits<argument_type>::param_type param_type;

        /// @brief Отпимальный тип для передачи типа результата
        typedef typename boost::call_traits<result_type>::param_type result_param_type;

        // Конструкторы

        // Добавление новых значений
        /** @brief Добавления новой точки к многочлену
        @param x значение аргумента
        @param y значение функции
        */
        void update(argument_type x, result_param_type y)
        {
            // @todo Заменить на алгоритм
            auto denom = argument_type{1};

            for(auto & d : data_)
            {
                denom *= (x - d.first);
            }

            if(policy_type::check_not_zero(denom) == false)
            {
                return;
            }

            auto K = (y - (*this)(x)) / denom;

            data_.emplace_back(std::move(x), std::move(K));
        }

        // Свойства
        /** @brief Степень многочлена
        @return Степень многочлена
        */
        size_type degree() const
        {
            return ural::empty(data_) ? 0 : data_.size() - 1;
        }

        /** @brief Вычисление значения многочлена в точке
        @param x точка, в которой вычисляется значение многочлена
        @return Значение многочлена в данной точке
        */
        result_type operator()(param_type x) const
        {
            auto r = result_type{0};
            auto denom = argument_type{1};

            for(auto const & p : data_)
            {
                r += p.second * denom;
                denom *= (x - p.first);
            }

            return r;
        }

    private:
        typedef std::pair<argument_type, result_type> Pair;
        std::vector<Pair> data_;
    };
}
// namespace experimental
}
// namespace ural

#endif
// Z_URAL_NUMERIC_INTERPOLATION_HPP_INCLUDED
