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

#include <ural/sequence/progression.hpp>
#include <ural/sequence/base.hpp>
#include <ural/tuple.hpp>

namespace ural
{
    /** @brief Последовательность чисел, заданная наименьшим и наибольшим
    значением
    @tparam Number тип числа
    @tparam D тип приращения
    @todo устранить дублирование с arithmetic_progression - сделать адаптором
    от taken_exactly_sequence<arithmetic_progression>
    */
    template <class Number, class Step = use_default>
    class numbers_sequence
     : public sequence_base<numbers_sequence<Number, Step>>
    {
        typedef typename default_helper<Step, Number>::type step_type;
    public:
        /// @brief Тип значения
        typedef Number value_type;

        /// @brief Тип ссылки
        typedef value_type reference;

        /// @brief Тип указателя
        typedef value_type const * pointer;

        /// @brief Тип расстояния
        typedef std::ptrdiff_t distance_type;

        /// @brief Категория обхода
        typedef random_access_traversal_tag traversal_tag;

        // Создание, уничтожение, копирование, равенство
        /** @brief Конструктор
        @param x_min наименьшее значение
        @param n число шагов
        @post <tt> this->front() == x_min </tt>
        */
        numbers_sequence(Number x_min, distance_type n)
         : members_(Front(std::move(x_min)), Size(std::move(n)),
                    distance_type{1})
        {
            assert(n > 0);
        }

        /** @brief Конструктор
        @param x_min наименьшее значение
        @param x_max наибольшее значение
        @param step шаг
        @post <tt> this->front() == x_min </tt>
        */
        numbers_sequence(Number x_min, distance_type n, step_type step)
         : members_(Front(std::move(x_min)), Size(std::move(n)),
                    std::move(step))
        {}

        /** @brief Оператор "равно"
        @param x, y операнды
        @return <tt> x.front() == y.front() && x.size() == y.size() && x.step() == y.step() </tt>
        */
        friend bool operator==(numbers_sequence const & x,
                               numbers_sequence const & y)
        {
            return x.members_ == y.members_;
        }

        // Однопроходная последовательность
        /** @brief Проверка исчерпания последовательности
        @return @b true, если последовательность исчерпана, иначе -- @b false
        */
        bool operator!() const
        {
            return this->size() == distance_type(0);
        }

        /** @brief Текущий элемент
        @return Ссылка на текущий элемент
        */
        reference front() const
        {
            return members_[ural::_1].value();
        }

        /// @brief Переход к следующему элементу
        void pop_front()
        {
            assert(!!*this);

            -- this->mutable_count();
            this->mutable_front() += this->step();
        }

        // Прямая последовательность
        /** @brief Полная последовательность (вместе с пройденными частями)
        @return Исходная последовательность
        */
        numbers_sequence original() const
        {
            return numbers_sequence(members_[ural::_1].old_value(),
                                    members_[ural::_2].old_value(),
                                    this->step());
        }

        /** @brief Пройденная передняя часть последовательность
        @return Пройденная передняя часть последовательность
        */
        numbers_sequence traversed_front() const
        {
            return numbers_sequence(members_[ural::_1].old_value(),
                                    this->traversed_front_size(),
                                    this->step());
        }

        /** @brief Отбрасывание пройденной части последовательности
        @post <tt> !this->traversed_front() </tt>
        */
        void shrink_front()
        {
            this->reset_old_size(this->original_size() - this->traversed_front_size());
            this->members_[ural::_1].commit();
        }

        void exhaust_front()
        {
            *this += this->size();
        }

        // Двусторонняя последовательность
        /** @brief Задний элемент последовательности
        @pre <tt> !*this == false </tt>
        */
        reference back() const
        {
            assert(!!*this);
            return (*this)[this->size() - 1];
        }

        /// @brief Пропуск последнего элемента последовательности
        void pop_back()
        {
            assert(!!*this);
            -- this->mutable_count();
        }

        /** @brief Пройденная задняя часть последовательность
        @return Пройденная задняя часть последовательность
        */
        numbers_sequence traversed_back() const
        {
            auto n_back = this->traversed_back_size();

            auto first = this->front() + this->size() * this->step();

            return numbers_sequence(std::move(first), std::move(n_back),
                                    this->step());
        }

        /// @brief Отбрасывает пройденную заднюю часть последовательности
        void shrink_back()
        {
            this->reset_old_size(this->original_size() - this->traversed_back_size());
        }

        /** @brief Исчерпание последовательности в обратном порядке за
        константное время
        @post <tt> !*this == true </tt>
        */
        void exhaust_back()
        {
            this->pop_back(this->size());
        }

        // Последовательность произвольного доступа
        /** @brief Количество элементов
        @return Количество непройденных элементов
        */
        constexpr distance_type size() const
        {
            return this->members_[ural::_2].value();
        }

        /** @brief Индексированный доступ
        @param n индекс
        @pre <tt> 0 < this->size() && this->size() < n </tt>
        @return <tt> this->base()[n] </tt>
        */
        reference operator[](distance_type index) const
        {
            assert(0 <= index && index < this->size());
            return this->front() + index * this->step();
        }

        /** @brief Продвижение на заданное число элементов в передней части
        последовательности
        @param n число элементов, которые будут пропущены
        @pre <tt> 0 <= n && n <= this->size() </tt>
        @return <tt> *this </tt>
        */
        numbers_sequence & operator+=(distance_type n)
        {
            assert(0 <= n && n <= this->size());

            this->mutable_count() -= n;
            this->mutable_front() += (this->step() * n);
            return *this;
        }

        /** @brief Продвижение на заданное число элементов в задней части
        последовательности
        @param n число элементов, которые будут пропущены
        @pre <tt> 0 <= n && n <= this->size() </tt>
        */
        void pop_back(distance_type n)
        {
            assert(0 <= n && n <= this->size());
            this->mutable_count() -= n;
        }

    private:
        void reset_old_size(distance_type new_old_size)
        {
            auto cur_size = this->size();

            this->members_[ural::_2] = new_old_size;
            this->members_[ural::_2].commit();
            this->members_[ural::_2] = cur_size;
        }

        distance_type original_size() const
        {
            return members_[ural::_2].old_value();
        }

        distance_type traversed_front_size() const
        {
            return (this->front() - members_[ural::_1].old_value()) / this->step();
        }

        distance_type traversed_back_size() const
        {
             return this->original_size() - this->size() - this->traversed_front_size();
        }

        Number & mutable_front()
        {
            return this->members_[ural::_1].value();
        }

        distance_type & mutable_count()
        {
            return this->members_[ural::_2].value();
        }

        step_type const & step() const
        {
            return this->members_[ural::_3];
        }

        using Front = ural::with_old_value<Number>;
        using Size  = ural::with_old_value<distance_type>;

        tuple<Front, Size, step_type> members_;
    };

    /// @brief Тип Функционального объекта для создания @c numbers_sequence
    struct numbers_fn
    {
        // @todo Проверить переполнения и сужения
    public:
        /** @brief Создание последовательности чисел
        @param x_min наименьшее значение
        @param x_max наибольшее значение
        */
        template <class T1, class T2>
        numbers_sequence<CommonType<T1, T2>, unit_t>
        operator()(T1 x_min, T2 x_max) const
        {
            using T = CommonType<T1, T2>;
            assert(T(x_min) <= T(x_max));

            typedef numbers_sequence<T, unit_t> Seq;
            return Seq(std::move(x_min), std::move(x_max) - x_min, unit_t{});
        }

        /** @brief Создание последовательности чисел
        @param from, to границы интервала <tt> [from, to) </tt>
        @param step шаг
        @pre <tt> step != 0 </tt>
        @pre <tt> (step > 0) ? (first <= last) : (last <= first) </tt>
        */
        template <class T1, class T2, class D>
        numbers_sequence<CommonType<T1, T2>, D>
        operator()(T1 first, T2 last, D step) const
        {
            using T = CommonType<T1, T2>;

            assert(step != 0);
            assert((step > 0) ? (T(first) <= T(last)) : (T(last) <= T(first)));

            auto from = (step >= 0) ? first : last;
            auto to   = (step >= 0) ? last  : first;
            auto abs_step = (step >= 0) ? step : - step;

            auto n = (to - from) / abs_step + ((to - from) % abs_step != 0);

            assert(n >= 0);

            using Sequence = numbers_sequence<CommonType<T1, T2>, D>;
            return Sequence(std::move(first), std::move(n), std::move(step));
        }
    };

    /** @brief Тип функционального объекта для создания последовательности
    индексов контейнера.
    */
    struct indices_of_fn
    {
    public:
        /** @brief Создание последовательности индексов массива
        @param v массив
        @return <tt> ural::isequence(0, v.size()) </tt>
        */
        template <class Vector>
        auto operator()(Vector const & v) const
        {
            return ural::numbers_fn{}(0, v.size());
        }
    };

    namespace
    {
        /// @brief Функциональный объект для создания @c numbers_sequence
        constexpr auto const & numbers = odr_const<numbers_fn>;

        /** @brief Функциональный объект для создания последовательности
        индексов контейнера
        */
        constexpr auto const & indices_of = odr_const<indices_of_fn>;
    }
}
// namespace ural

#endif
// Z_URAL_NUMERIC_NUMBERS_RANGE_HPP_INCLUDED
