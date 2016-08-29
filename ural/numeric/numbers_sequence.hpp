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

#include <ural/sequence/adaptors/taken_exactly.hpp>
#include <ural/sequence/progression.hpp>
#include <ural/sequence/adaptor.hpp>
#include <ural/tuple.hpp>

namespace ural
{
namespace experimental
{
    /** @brief Курсор последовательности чисел, заданной наименьшим и наибольшим
    значением
    @tparam Number тип числа
    @tparam D тип приращения
    @tparam CursorTag желаемая категория курсора
    */
    template <class Number, class Step = use_default,
              class CursorTag = use_default>
    class numbers_cursor
     : public cursor_adaptor<numbers_cursor<Number, Step, CursorTag>,
                               taken_exactly_cursor<arithmetic_progression_cursor<Number, use_default, CursorTag, Step>, std::ptrdiff_t>>
    {
        using Progression = arithmetic_progression_cursor<Number, use_default, CursorTag, Step>;
        using Taken = taken_exactly_cursor<Progression, std::ptrdiff_t>;
        using Inherited = cursor_adaptor<numbers_cursor, Taken>;

    public:
        // Типы
        /// @brief Тип размера шага
        using step_type = typename Progression::step_type;

        /// @brief Тип расстояния
        using distance_type = std::ptrdiff_t;

        /// @brief Категория курсора
        using cursor_tag = make_finite_cursor_tag_t<typename Inherited::cursor_tag>;

        // Создание, уничтожение, копирование, равенство
        /** @brief Конструктор
        @param x_min наименьшее значение
        @param n число шагов
        @post <tt> this->front() == x_min </tt>
        */
        numbers_cursor(Number x_min, distance_type n)
         : numbers_cursor(std::move(x_min), std::move(n), step_type(1))
        {}

        /** @brief Конструктор
        @param x_min наименьшее значение
        @param x_max наибольшее значение
        @param step шаг
        @post <tt> this->front() == x_min </tt>
        */
        numbers_cursor(Number x_min, distance_type n, step_type step)
         : Inherited(Taken(Progression(std::move(x_min), std::move(step)), std::move(n)))
        {}

        /** @brief Размер шага
        @return Величина шага между последовательными значениями
        */
        step_type const & step() const
        {
            return Inherited::base().base().step();
        }

        // Прямой курсор - за счёт адаптора

    private:
        friend Inherited;

        explicit numbers_cursor(Taken s)
         : Inherited(std::move(s))
        {}
    };

    /// @brief Тип Функционального объекта для создания @c numbers_cursor
    struct numbers_fn
    {
        // @todo Проверить переполнения и сужения
    public:
        /** @brief Создание курсора последовательности чисел
        @param x_min наименьшее значение
        @param x_max наибольшее значение
        */
        template <class T1, class T2>
        numbers_cursor<common_type_t<T1, T2>, unit_t>
        operator()(T1 x_min, T2 x_max) const
        {
            using T = common_type_t<T1, T2>;
            assert(T(x_min) <= T(x_max));

            typedef numbers_cursor<T, unit_t> Seq;
            return Seq(std::move(x_min), std::move(x_max) - x_min, unit_t{});
        }

        /** @brief Создание курсора последовательности чисел
        @param from, to границы интервала <tt> [from, to) </tt>
        @param step шаг
        @pre <tt> step != 0 </tt>
        @pre <tt> (step > 0) ? (first <= last) : (last <= first) </tt>
        */
        template <class T1, class T2, class D>
        numbers_cursor<common_type_t<T1, T2>, D>
        operator()(T1 first, T2 last, D step) const
        {
            using T = common_type_t<T1, T2>;

            assert(step != 0);
            assert((step > 0) ? (T(first) <= T(last)) : (T(last) <= T(first)));

            auto from = (step >= 0) ? first : last;
            auto to   = (step >= 0) ? last  : first;
            auto abs_step = (step >= 0) ? step : - step;

            auto n = (to - from) / abs_step + ((to - from) % abs_step != 0);

            assert(n >= 0);

            using Cursor = numbers_cursor<common_type_t<T1, T2>, D>;
            return Cursor(std::move(first), std::move(n), std::move(step));
        }
    };

    /** @brief Тип функционального объекта для создания курсора
    последовательности индексов контейнера.
    */
    struct indices_of_fn
    {
    public:
        /** @brief Создание курсора последовательности индексов массива
        @param v массив
        @return <tt> ural::isequence(0, v.size()) </tt>
        */
        template <class Vector>
        auto operator()(Vector const & v) const
        {
            return ::ural::experimental::numbers_fn{}(0, v.size());
        }
    };
}
// namespace experimental

inline namespace v1
{
    namespace
    {
        /// @brief Функциональный объект для создания @c numbers_cursor
        constexpr auto const & numbers = odr_const<::ural::experimental::numbers_fn>;

        /** @brief Функциональный объект для создания последовательности
        индексов контейнера
        */
        constexpr auto const & indices_of = odr_const<::ural::experimental::indices_of_fn>;
    }
}
// inline namespace v1
}
// namespace ural

#endif
// Z_URAL_NUMERIC_NUMBERS_RANGE_HPP_INCLUDED
