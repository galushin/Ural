#ifndef Z_URAL_NUMERIC_MP_INTEGER_10_HPP_INCLUDED
#define Z_URAL_NUMERIC_MP_INTEGER_10_HPP_INCLUDED

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

/** @file ural/numeric/mp/integer_10.hpp
 @brief Числа произвольной точности, представленные как последовательность
 (десятичных) цифр
*/

#include <ural/algorithm.hpp>
#include <ural/sequence/reversed.hpp>

namespace ural
{
    // @todo учёт знака
    // @todo устранить дублирование
    // @todo заменить циклы на алгоритмы
    // @todo выделить функции
    // @todo смешанные операции (с встроенными целыми числами)
    // @todo тест integer_10{} == integer_10{0};
    /** @brief Класс чисел с произвольной точностью, представленный как
    последовательность (десятичных) цифр
    */
    class integer_10
    {
    // Операторы сравнения
    friend bool operator==(integer_10 const & x, integer_10 const & y)
    {
        return x.digits() == y.digits();
    }

    friend bool operator<(integer_10 const & x, integer_10 const & y)
    {
        if(x.digits().size() < y.digits().size())
        {
            return true;
        }
        if(x.digits().size() > y.digits().size())
        {
            return false;
        }

        return ural::lexicographical_compare(x.digits() | ural::reversed,
                                             y.digits() | ural::reversed);
    }

    // Ввод/вывод
    // @todo для любых потоков
    friend std::ostream & operator<<(std::ostream & os, integer_10 const & x)
    {
        for(auto const & d : x.digits() | ural::reversed)
        {
            assert(0 <= d && d < 10);
            os << d;
        }
        return os;
    }

    // Арифметические операции
    friend integer_10 operator-(integer_10 x, integer_10 const & y)
    {
        x -= y;
        return x;
    }

    friend integer_10 operator*(integer_10 const & x, integer_10 const & y)
    {
        // @todo оптимизация
        integer_10 result;

        for(size_t i = 0; i != y.digits().size(); ++ i)
        {
            integer_10 a;

            a.digits_.resize(i, 0);

            Digit carry = 0;

            for(size_t j = 0; j != x.digits().size(); ++ j)
            {
                auto new_value = carry + x.digits()[j] * y.digits()[i];
                a.digits_.push_back(new_value % 10);
                carry = new_value / 10;
            }

            if(carry > 0)
            {
                a.digits_.push_back(carry);
            }

            result += a;
        }

        return result;
    }

    friend integer_10 operator%(integer_10 x, integer_10 const & d)
    {
        // @todo оптимизация
        // @todo assert(x >= 0)
        for(; x >= d; x -= d)
        {}

        return x;
    }

    public:
        // Типы
        typedef short Digit;
        typedef std::vector<Digit> Digits_container;

        // Создание, копирование, уничтожение
        /** @brief Конструктор без параметров
        @post <tt> *this == 0 </tt>
        */
        integer_10() = default;

        template <class T>
        explicit integer_10(T init_value)
        {
            assert(init_value >= 0);

            static_assert(std::is_integral<T>::value, "Must be integral");

            for(; init_value > 0; init_value /= 10)
            {
                digits_.push_back(init_value % 10);
            }
        }

        // Доступ к цифрам
        Digits_container const & digits() const
        {
            return this->digits_;
        }

        // Операции составного присваивания
        integer_10 & operator+=(integer_10 const & x)
        {
            if(digits_.size() < x.digits_.size())
            {
                digits_.resize(x.digits().size(), 0);
            }

            // @note не может ли при таком типе возникнуть переполнение
            Digit carry = 0;

            for(size_t i = 0; i < x.digits().size(); ++ i)
            {
                auto new_value = digits_[i] + x.digits_[i] + carry;
                digits_[i] = (new_value % 10);
                carry = new_value / 10;
            }

            for(size_t i = x.digits().size(); i < this->digits().size(); ++ i)
            {
                auto new_value = digits_[i] + carry;
                digits_[i] = (new_value % 10);
                carry = new_value / 10;
            }

            if(carry > 0)
            {
                digits_.push_back(carry);
            }

            return *this;
        }

        integer_10 & operator-=(integer_10 const & x)
        {
            assert(*this >= x);

            for(auto k = x.digits_.size(); k > 0; -- k)
            {
                if(this->digits_[k-1] < x.digits_[k-1])
                {
                    this->digits_[k] -= 1;
                    this->digits_[k-1] += 10 - x.digits_[k-1];
                }
                else
                {
                    this->digits_[k-1] -= x.digits_[k-1];
                }
            }

            for(; digits_.back() == 0; digits_.pop_back())
            {}

            return *this;
        }

    private:
        Digits_container digits_;
    };
}
// namespace ural

#endif
// Z_URAL_NUMERIC_MP_INTEGER_10_HPP_INCLUDED
