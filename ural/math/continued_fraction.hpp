#ifndef Z_URAL_MATH_CONTINUED_FRACTION_H_INCLUDED
#define Z_URAL_MATH_CONTINUED_FRACTION_H_INCLUDED

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

/** @file ural/math/continued_fraction.hpp
 @brief Классы и функции, связанные с цепными дробями.
*/

#include <ural/sequence/make.hpp>
#include <ural/math/rational.hpp>
#include <cassert>

namespace ural
{
    /** @brief Класс для вычисления подходящего значения непрерывной дроби
    @tparam IntType целочисленный типы

    Класс для вычисления подходящего значения непрерывной дроби. Конструктор
    без аргумента не предоставляется, чтобы значение всегда было определено.
    */
    template <class IntType>
    class convergent
    {
    public:
        /// @brief Тип результата
        typedef ural::rational<IntType> result_type;

        /** @brief Конструктор
        @param a целая часть непрерывной дроби
        @post <tt> this->value() == a/1 </tt>

        Было решено отказаться от использования комбинации "конструктор без
        аргументов + update(a)", так как аргумент @c a в конструкторе может быть
        отрицательным числом, а у функции @c update --- натуральным
        */
        convergent(IntType const & a)
         : h_1(a)
         , k_1(1)
         , h_2(1)
         , k_2(0)
        {}

        /** @brief Текущее значение подходящей дроби
        @return Обыкновенная дробь, являющаяся подходящей дробью
        */
        result_type value() const
        {
            assert(k_1 != 0);
            return result_type{h_1, k_1};
        }

        /** @brief Уточнение значения дроби
        @param a новое число
        @pre <tt> a > 0 </tt>
        */
        void update(IntType const & a)
        {
            assert(a > 0);

            h_2 += a * h_1;
            k_2 += a * k_1;

            using std::swap;
            swap(h_2, h_1);
            swap(k_2, k_1);
        }

    private:
        IntType h_1;
        IntType k_1;
        IntType h_2;
        IntType k_2;
    };

    /**
    @todo Превратить в бесконечную последовательность. Конечную
    последовательность, покрывающую один период, реализовать как адаптер
    @tparam IntType целочисленный тип
    @tparam D тип расстояния, по умолчанию используется std::intmax_t
    */
    template <class IntType, class D = use_default>
    class sqrt_as_continued_fraction_sequence
     : public ural::sequence_base<sqrt_as_continued_fraction_sequence<IntType>>
    {
    public:
        // Типы
        /// @brief Тип значения
        typedef IntType value_type;

        /// @brief Тип ссылки
        typedef value_type const & reference;

        /// @brief Тип указателя
        typedef value_type const * pointer;

        // @todo усилить категорию обхода до прямой
        /// @brief Категория обхода
        typedef single_pass_traversal_tag traversal_tag;

        /// @brief Тип расстояния
        typedef typename default_helper<D, std::intmax_t>::type distance_type;

        // Конструкторы
        /** @brief Конструктор
        @param value число, из которого извлекается корень
        @post <tt> this->front() == IntType(sqrt(value)) </tt>
        */
        explicit sqrt_as_continued_fraction_sequence(IntType value)
         : N_(std::move(value))
         , a_0_(sqrt_as_continued_fraction_sequence::sqrt_impl(N_))
         , a_new_(a_0_)
         , x_(0)
         , denom_(1)
        {
            // Порядок инициализации важен: N_ до a_0_
        }

        // Однопроходная последовательность
        /** @brief Проверка исчерпания последовательностей
        @return @b true, если последовательность исчерпана, иначе --- @b false.
        */
        bool operator!() const
        {
            return (denom_ == IntType(0));
        }

        /** @brief Текущий элемент последовательности
        @pre <tt> !*this == false </tt>
        @return Ссылка на текущий элемент последовательности
        */
        reference front() const
        {
            return a_new_;
        }

        /** @brief Переход к следующему элементу
        @pre <tt> !*this == false </tt>
        */
        void pop_front()
        {
            if(ural::square(a_0_) == N_ || (denom_ == 1 && x_ == a_0_))
            {
                denom_ = IntType(0);
                return;
            }

            if(x_ == IntType(0))
            {
                x_ = a_0_;
            }

            // denom / (sqrt(N) - x) = (sqrt(N) + x) * denom / (N - x^2)
            auto const new_denom = (N_ - ural::square(x_)) / denom_;

            a_new_ = (a_0_ + x_) / new_denom;

            x_ = (a_new_ * new_denom - x_);

            denom_ = new_denom;
        }

    private:
        IntType N_;
        IntType a_0_;
        IntType a_new_;
        IntType x_;
        IntType denom_;

    private:
        static IntType sqrt_impl(IntType x)
        {
            // Учитывая, что альтернатива вещественной функции - цикл, я думаю
            // что лучше оставить как есть
            using std::sqrt;
            return sqrt(x);
        }
    };

    /** @brief Создание последовательности подходящих дробей для корня из целого
    числа
    @param n Число, из которого извлекается корень.
    @return <tt> sqrt_as_continued_fraction_sequence<IntType>(std::move(n)) </tt>
    */
    template <class IntType>
    sqrt_as_continued_fraction_sequence<IntType>
    sqrt_as_continued_fraction(IntType n)
    {
        return sqrt_as_continued_fraction_sequence<IntType>(std::move(n));
    }
}
// namespace ural

#endif // Z_URAL_MATH_CONTINUED_FRACTION_H_INCLUDED
