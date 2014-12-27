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
    template <class IntType, long radix>
    class digits_sequence
     : public ural::sequence_base<digits_sequence<IntType, radix>>
    {
    public:
        typedef ural::single_pass_traversal_tag traversal_tag;

        typedef IntType value_type;
        typedef value_type reference;
        typedef IntType * pointer;

        explicit digits_sequence(IntType value)
         : value_{std::move(value)}
        {
            assert(value_ >= 0);
        }

        bool operator!() const
        {
            return value_ == 0;
        }

        reference front() const
        {
            return value_ % radix;
        }

        void pop_front()
        {
            value_ /= radix;
        }

    private:
        IntType value_;
    };

    // @todo учёт знака
    // @todo устранить дублирование
    // @todo заменить циклы на алгоритмы
    // @todo выделить функции
    // @todo смешанные операции (с встроенными целыми числами)
    /** @brief Класс чисел с произвольной точностью, представленный как
    последовательность (десятичных) цифр
    */
    template <long base>
    class integer
    {
        typedef integer self_type;

    // Операторы сравнения
    friend bool operator==(integer const & x, integer const & y)
    {
        return x.digits() == y.digits();
    }

    friend bool operator<(integer const & x, integer const & y)
    {
        if(x.size() < y.size())
        {
            return true;
        }
        if(x.size() > y.size())
        {
            return false;
        }

        return ural::lexicographical_compare(x.digits() | ural::reversed,
                                             y.digits() | ural::reversed);
    }

    // Ввод/вывод
    // @todo для любых потоков
    friend std::ostream & operator<<(std::ostream & os, integer const & x)
    {
        if(x.digits().empty())
        {
            return os << '0';
        }

        for(auto const & d : x.digits() | ural::reversed)
        {
            assert(0 <= d && d < base);
            os << d;
        }
        return os;
    }

    // Арифметические операции
    friend integer operator+(integer x, integer const & y)
    {
        x += y;
        return x;
    }

    friend integer operator-(integer x, integer const & y)
    {
        x -= y;
        return x;
    }

    friend integer operator*(integer const & x, integer const & y)
    {
        // @todo оптимизация
        integer result;

        for(size_t i = 0; i != y.size(); ++ i)
        {

            result += integer::multiply_by_digit(x, y.digits()[i], i);
        }

        return result;
    }

    friend integer operator%(integer x, integer const & d)
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
        typedef typename Digits_container::size_type size_type;

        // Создание, копирование, уничтожение
        /** @brief Конструктор без параметров
        @post <tt> *this == 0 </tt>
        */
        integer() = default;

        template <class T>
        explicit integer(T init_value)
        {
            assert(init_value >= 0);

            static_assert(std::is_integral<T>::value, "Must be integral");

            ural::copy(digits_sequence<T, base>{std::move(init_value)},
                       digits_ | ural::back_inserter);
        }

        // Доступ к цифрам
        Digits_container const & digits() const
        {
            return this->digits_;
        }

        // Инкремент и декремент
        integer & operator++()
        {
            // @todo устранить дублирование
            auto carry = Digit{1};

            for(size_t i = 0; carry > 0 && i < this->size(); ++ i)
            {
                auto new_value = digits_[i] + carry;
                carry = new_value / base;
                digits_[i] = new_value % base;
            }

            if(carry > 0)
            {
                digits_.push_back(carry);
            }

            return *this;

        }

        integer & operator--()
        {
            // @todo устранить дублирование
            assert(digits_.empty() == false);

            // Вычитаем, пока есть перенос
            for(size_t i = 0; i < this->size(); ++ i)
            {
                if(digits_[i] == 0)
                {
                    digits_[i] = base - 1;
                }
                else
                {
                    digits_[i] -= 1;
                    break;
                }
            }

            this->strip_leading_zeroes();

            return *this;

        }

        // Операции составного присваивания
        integer & operator+=(integer const & x)
        {
            if(this->size() < x.size())
            {
                digits_.resize(x.size(), 0);
            }

            // @note не может ли при таком типе возникнуть переполнение
            Digit carry = 0;

            for(size_t i = 0; i < x.size(); ++ i)
            {
                auto new_value = digits_[i] + x.digits_[i] + carry;
                digits_[i] = (new_value % base);
                carry = new_value / base;
            }

            for(size_t i = x.size();
                i < this->size() && carry > 0; ++ i)
            {
                auto new_value = digits_[i] + carry;
                digits_[i] = (new_value % base);
                carry = new_value / base;
            }

            if(carry > 0)
            {
                digits_.push_back(carry);
            }

            return *this;
        }

        integer & operator-=(integer const & x)
        {
            assert(*this >= x);

            Digit carry = 0;

            for(size_t k = 0; k < x.size(); ++ k)
            {
                this->digits_[k] -= x.digits_[k] + carry;

                if(this->digits_[k] < 0)
                {
                    this->digits_[k] += base;
                    carry = 1;
                }
                else
                {
                    carry = 0;
                }
            }

            for(size_t k = x.size(); carry > 0; ++ k)
            {
                this->digits_[k] -= carry;

                if(this->digits_[k] < 0)
                {
                    this->digits_[k] += base;
                    carry = 1;
                }
                else
                {
                    carry = 0;
                }
            }

            this->strip_leading_zeroes();

            return *this;
        }

        size_type size() const
        {
            return this->digits().size();
        }

    private:
        void strip_leading_zeroes()
        {
            for(; digits_.back() == 0; digits_.pop_back())
            {}
        }

    private:
        static integer
        multiply_by_digit(integer const & x, Digit const & d, size_type i)
        {
            integer a;

            a.digits_.resize(i, 0);

            Digit carry = 0;

            for(size_t j = 0; j != x.size(); ++ j)
            {
                auto new_value = carry + x.digits()[j] * d;
                a.digits_.push_back(new_value % base);
                carry = new_value / base;
            }

            if(carry > 0)
            {
                a.digits_.push_back(carry);
            }

            return a;
        }

    private:
        Digits_container digits_;
    };

    template <class T, long radix>
    typename std::enable_if<std::is_integral<T>::value, bool>::type
    operator==(integer<radix> const & x, T const & a)
    {
        return ural::equal(x.digits(), ural::digits_sequence<T, radix>{a});
    }

    template <class T, long radix>
    typename std::enable_if<std::is_integral<T>::value, bool>::type
    operator==(T const & a, integer<radix> const & x)
    {
        return x == a;
    }

    typedef integer<10> integer_10;
}
// namespace ural

#endif
// Z_URAL_NUMERIC_MP_INTEGER_10_HPP_INCLUDED
