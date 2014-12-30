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
        typedef IntType distance_type;

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

    // @todo устранить дублирование
    // @todo заменить циклы на алгоритмы
    // @todo выделить функции
    // @todo смешанные операции (с встроенными целыми числами)
    // @todo Шаблоны выражений
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
        return x.members_ == y.members_;
    }

    friend bool abs_less(integer const & x, integer const & y)
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

    friend bool operator<(integer const & x, integer const & y)
    {
        // Отрицательные всегда меньше не отрицательных
        if(x.is_not_negative() == false && y.is_not_negative() == true)
        {
            return true;
        }

        // Неотрицательные всегда больше отрицательных
        if(x.is_not_negative() == true && y.is_not_negative() == false)
        {
            return false;
        }

        // Здесь x и y имеют одинаковый знак
        return x.is_not_negative() ? abs_less(x, y) : abs_less(y, x);
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

        if(x.size() > 0 && y.size() > 0)
        {
            for(size_t i = 0; i != y.size(); ++ i)
            {
                result += integer::multiply_by_digit(x, y.digits()[i], i);
            }

            result.is_positive_ref()
                = (x.is_not_negative() == y.is_not_negative());
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
        integer()
         : members_{}
        {
            is_positive_ref() = true;
        }

        template <class T>
        explicit integer(T init_value)
        {
            if(init_value < 0)
            {
                is_positive_ref() = false;
                init_value = -init_value;
            }
            else
            {
                is_positive_ref() = true;
            }

            static_assert(std::is_integral<T>::value, "Must be integral");

            assert(init_value >= 0);

            ural::copy(digits_sequence<T, base>{std::move(init_value)},
                       this->digits_ref() | ural::back_inserter);
        }

        // Доступ к цифрам
        Digits_container const & digits() const
        {
            return members_[ural::_1];
        }

        // Инкремент и декремент
        integer & operator++()
        {
            // @todo устранить дублирование

            if(this->is_positive_ref() == false)
            {
                this->is_positive_ref() = true;
                -- *this;
                this->is_positive_ref() = this->digits().empty();

                return *this;
            }

            auto carry = Digit{1};

            for(size_t i = 0; carry > 0 && i < this->size(); ++ i)
            {
                auto new_value = this->digits()[i] + carry;
                carry = new_value / base;
                digits_ref()[i] = new_value % base;
            }

            if(carry > 0)
            {
                digits_ref().push_back(carry);
            }

            return *this;

        }

        integer & operator--()
        {
            // @todo устранить дублирование

            if(this->is_positive_ref() == false || this->digits().empty())
            {
                this->is_positive_ref() = true;
                ++ *this;
                this->is_positive_ref() = false;

                return *this;
            }

            // Вычитаем, пока есть перенос
            for(size_t i = 0; i < this->size(); ++ i)
            {
                if(digits()[i] == 0)
                {
                    digits_ref()[i] = base - 1;
                }
                else
                {
                    digits_ref()[i] -= 1;
                    break;
                }
            }

            this->strip_leading_zeroes();

            return *this;

        }

        // Унарные операции
        integer operator+() const
        {
            return *this;
        }

        integer operator-() const
        {
            auto result = *this;
            result.is_positive_ref() = !result.is_positive_ref();
            return result;
        }

        // Операции составного присваивания
        integer & operator+=(integer const & x)
        {
            if(this->is_not_negative() != x.is_not_negative())
            {
                return *this -= (-x);
            }

            if(this->size() < x.size())
            {
                digits_ref().resize(x.size(), 0);
            }

            // @note не может ли при таком типе возникнуть переполнение
            Digit carry = 0;

            for(size_t i = 0; i < x.size(); ++ i)
            {
                auto new_value = digits()[i] + x.digits()[i] + carry;
                digits_ref()[i] = (new_value % base);
                carry = new_value / base;
            }

            for(size_t i = x.size(); i < this->size() && carry > 0; ++ i)
            {
                auto new_value = digits()[i] + carry;
                digits_ref()[i] = (new_value % base);
                carry = new_value / base;
            }

            if(carry > 0)
            {
                digits_ref().push_back(carry);
            }

            return *this;
        }

        integer & operator-=(integer const & x)
        {
            if(this->is_not_negative() != x.is_not_negative())
            {
                return *this += (-x);
            }

            if(abs_less(*this, x))
            {
                *this = - (x - *this);
                return *this;
            }

            Digit carry = 0;

            for(size_t k = 0; k < x.size(); ++ k)
            {
                this->digits_ref()[k] -= x.digits()[k] + carry;

                if(this->digits()[k] < 0)
                {
                    this->digits_ref()[k] += base;
                    carry = 1;
                }
                else
                {
                    carry = 0;
                }
            }

            for(size_t k = x.size(); carry > 0; ++ k)
            {
                this->digits_ref()[k] -= carry;

                if(this->digits()[k] < 0)
                {
                    this->digits_ref()[k] += base;
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

        integer & operator*=(integer const & y)
        {
            *this = *this * y;
            return *this;
        }

        template <class T>
        typename std::enable_if<std::is_integral<T>::value, integer &>::type
        operator*=(T const & a)
        {
            *this = *this * a;
            return *this;
        }

        size_type size() const
        {
            return this->digits().size();
        }

        bool is_not_negative() const
        {
            return members_[ural::_2];
        }

        static integer
        multiply_by_digit(integer const & x, Digit const & d, size_type i)
        {
            integer a;

            a.digits_ref().resize(i, 0);

            Digit carry = 0;

            for(size_t j = 0; j != x.size(); ++ j)
            {
                auto new_value = carry + x.digits()[j] * d;
                a.digits_ref().push_back(new_value % base);
                carry = new_value / base;
            }

            if(carry > 0)
            {
                a.digits_ref().push_back(carry);
            }

            return a;
        }

    private:
        void strip_leading_zeroes()
        {
            for(; !digits().empty() && digits().back() == 0; digits_ref().pop_back())
            {}

            if(digits().empty())
            {
                this->is_positive_ref() = true;
            }
        }

        Digits_container & digits_ref()
        {
            return members_[ural::_1];
        }

        bool & is_positive_ref()
        {
            return members_[ural::_2];
        }

    private:
        ural::tuple<Digits_container, bool> members_;

    };

    template <class T, long radix>
    typename std::enable_if<std::is_integral<T>::value, integer<radix>>::type
    operator*(integer<radix> const & x, T const & a)
    {
        // @todo оптимизация
        // @todo устранить дублирование
        integer<radix> result;

        if(x.size() > 0 && a != 0)
        {
            using std::abs;
            auto seq = digits_sequence<T, radix>(abs(a));

            for(size_t i = 0; !!seq; ++ i, ++ seq)
            {
                result += integer<radix>::multiply_by_digit(x, *seq, i);
            }

            if(x.is_not_negative() != (a >= 0))
            {
                result = -result;
            }
        }

        return result;
    }

    template <class T, long radix>
    typename std::enable_if<std::is_integral<T>::value, integer<radix>>::type
    operator*(T const & a, integer<radix> const & x)
    {
        return x * a;
    }

    template <class T, long radix>
    typename std::enable_if<std::is_integral<T>::value, bool>::type
    operator<(integer<radix> const & x, T const & a);

    template <class T, long radix>
    typename std::enable_if<std::is_integral<T>::value, bool>::type
    operator<(T const & a, integer<radix> const & x);

    template <class T, long radix>
    typename std::enable_if<std::is_integral<T>::value, bool>::type
    operator==(integer<radix> const & x, T const & a)
    {
        using std::abs;

        return x.is_not_negative() == (a >= 0)
                && ural::equal(x.digits(), digits_sequence<T, radix>{abs(a)});
    }

    template <class T, long radix>
    typename std::enable_if<std::is_integral<T>::value, bool>::type
    operator==(T const & a, integer<radix> const & x)
    {
        return x == a;
    }

    // Ввод/вывод
    template <class Char, class Traits, long radix>
    std::basic_ostream<Char, Traits> &
    operator<<(std::basic_ostream<Char, Traits> & os, integer<radix> const & x)
    {
        static_assert(1 < radix && radix <= 10, "Unsupported radix");

        if(x.digits().empty())
        {
            return os << '0';
        }

        if(x.is_not_negative() == false)
        {
            os << '-';
        }

        for(auto const & d : x.digits() | ural::reversed)
        {
            assert(0 <= d && d < radix);
            os << d;
        }
        return os;
    }

    typedef integer<10> integer_10;
}
// namespace ural

#endif
// Z_URAL_NUMERIC_MP_INTEGER_10_HPP_INCLUDED
