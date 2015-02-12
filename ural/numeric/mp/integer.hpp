#ifndef Z_URAL_NUMERIC_MP_INTEGER_HPP_INCLUDED
#define Z_URAL_NUMERIC_MP_INTEGER_HPP_INCLUDED

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

/** @file ural/numeric/mp/integer.hpp
 @brief Числа произвольной точности, представленные как последовательность
 цифр
*/

#include <ural/algorithm.hpp>
#include <ural/sequence/reversed.hpp>

#include <boost/io/ios_state.hpp>

#include <cmath>
#include <cstdlib>

namespace ural
{
    template <long radix>
    class digit_arithmetics_result;

    /** @brief Класс для представления цифр числа произвольной точности
    @tparam radix основание системы счисления
    @todo constexpr?
    */
    template <long radix>
    class digit
    {
    public:
        // Типы
        // @todo Минимизация размера: выбор типа цифры в зависимости от значения radix
        typedef long value_type;

        // Конструкторы
        explicit digit(value_type init_value)
         : value_(std::move(init_value))
        {
            assert(0 <= init_value && init_value < radix);
        }

        // Свйоства
        value_type const & value() const
        {
            return value_;
        }

    private:
        value_type value_;
    };

    template <long radix>
    bool
    operator==(digit<radix> const & x, digit<radix> const & y)
    {
        return x.value() == y.value();
    }

    template <class T, long radix>
    typename std::enable_if<std::is_integral<T>::value, bool>::type
    operator==(T const & n, digit<radix> const & d)
    {
        return n == d.value();
    }

    template <class T, long radix>
    typename std::enable_if<std::is_integral<T>::value, bool>::type
    operator==(digit<radix> const & d, T const & n)
    {
        return d.value() == n;
    }

    template <long radix>
    bool
    operator<(digit<radix> const & x, digit<radix> const & y)
    {
        return x.value() < y.value();
    }

    template <class T, long radix>
    typename std::enable_if<std::is_integral<T>::value, bool>::type
    operator<(T const & n, digit<radix> const & d)
    {
        return n < d.value();
    }

    template <class T, long radix>
    typename std::enable_if<std::is_integral<T>::value, bool>::type
    operator<(digit<radix> const & d, T const & n)
    {
        return d.value() < n;
    }

    /**
    @todo Для любых потоков?
    */
    template <class Char, class Traits, long radix>
    std::basic_ostream<Char, Traits> &
    operator<<(std::basic_ostream<Char, Traits> & os, digit<radix> const & x)
    {
        return os << x.value();
    }

    /**
    @todo value-range-propagaion вместо assert
    @todo div вместо двух разных значений
    */
    template <long radix>
    class digit_arithmetics_result
    {
    public:
        // Типы
        typedef digit<radix> value_type;

        // Конструкторы
        explicit digit_arithmetics_result(typename value_type::value_type word)
         : value_{word % radix}
         , carry_{word / radix}
        {}

        // Свойства
        value_type const & value() const
        {
            return value_;
        }

        value_type const & carry() const
        {
            return carry_;
        }

    private:
        value_type value_;
        value_type carry_;
    };

    template <long radix>
    digit_arithmetics_result<radix>
    operator+(digit<radix> x, digit<radix> y)
    {
        return digit_arithmetics_result<radix>{x.value() + y.value()};
    }

    /**
    @note Данная операция может быть хорошим кандидатам на оптимизацию,
    см. ru.wikipedia.org/wiki/Умножение-сложение
    */
    template <long radix>
    digit_arithmetics_result<radix>
    multiply_add(digit<radix> a, digit<radix> b, digit<radix> c)
    {
        return digit_arithmetics_result<radix>{a.value() + b.value() * c.value()};
    }

    /**
    @note Это аналог операции, выполняемой сумматором
    см. https://ru.wikipedia.org/wiki/Сумматор
    */
    template <long radix>
    digit_arithmetics_result<radix>
    add_with_carry(digit<radix> a, digit<radix> b, digit<radix> c)
    {
        return digit_arithmetics_result<radix>{a.value() + b.value() + c.value()};
    }

    /** @brief Представление числа в виде последовательности цифр по
    произвольному основанию, начиная с младшего разряда
    @tparam IntType тип целых чисел
    @tparam radix основание системы счисления
    */
    template <class IntType, IntType radix>
    class digits_sequence
     : public ural::sequence_base<digits_sequence<IntType, radix>>
    {
    public:
        /// @brief Категорию обхода
        typedef ural::single_pass_traversal_tag traversal_tag;

        /// @brief Тип значения
        typedef digit<radix> value_type;

        /// @brief Тип ссылки
        typedef value_type reference;

        /// @brief Тип указателя
        typedef IntType const * pointer;

        /// @brief Тип расстояния
        typedef IntType distance_type;

        /** @brief Конструктор
        @param value число
        */
        explicit digits_sequence(IntType value)
         : state_(std::div(value, radix))
        {
            assert(value >= 0);
        }

        // Однопроходная последовательность
        /** @brief Проверка исчерпания последовательности
        @return @b true, если последовательность исчерпана, иначе --- @b false
        */
        bool operator!() const
        {
            return state_.quot == 0 && state_.rem == 0;
        }

        /** @brief Текущий элемент
        @return Цифра Текущего разряд числа
        */
        reference front() const
        {
            return value_type{state_.rem};
        }

        /// @brief Переход к следующему разряду
        void pop_front()
        {
            using std::div;
            state_ = div(state_.quot, radix);
        }

    private:
        typedef decltype(std::div(radix, radix)) div_type;

        div_type state_;
    };

    // @todo устранить дублирование
    // @todo заменить циклы на алгоритмы
    // @todo выделить функции
    // @todo смешанные операции (с встроенными целыми числами)
    // @todo Шаблоны выражений
    /** @brief Класс чисел с произвольной точностью, представленный как
    последовательность (десятичных) цифр

    Нужно обратить внимание, что ноль (по крайней мере --- потенциально),
    не является уникально представленным
    */
    template <long base>
    class integer
    {
        typedef integer self_type;

    // Операторы сравнения
    /** @brief Оператор "равно"
    @param x левый операнд
    @param y правый операнд
    @return @b true, если числа @c x и @c y равны, иначе --- @b false
    */
    friend bool operator==(integer const & x, integer const & y)
    {
        return x.members_ == y.members_;
    }

    /** @brief Функция сравнения целых чисел произвольной точности по модулю
    @param x левый операнд
    @param y правый операнд
    @return @b true, если @c x меньше по модулю, чем @c y, иначе --- @b false
    */
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

    /** @brief Оператор "меньше"
    @param x левый операнд
    @param y правый операнд
    @return @b true, если @c x меньше, чем @c y, иначе --- @b false
    */
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
        if(x.size() == 0 || y.size() == 0)
        {
            return integer{};
        }

        // @todo оптимизация
        integer result;

        for(size_t i = 0; i != y.size(); ++ i)
        {
            result += integer::multiply_by_digit(x, y.digits()[i], i);
        }

        result.is_not_negative_ref()
            = (x.is_not_negative() == y.is_not_negative());

        return result;
    }

    friend integer operator%(integer x, integer const & d)
    {
        // @todo оптимизация
        // @todo Реализация для отрицательных чисел

        assert(x >= 0);
        assert(d > 0);
        for(; x >= d; x -= d)
        {}

        return x;
    }

    public:
        static_assert(base > 1, "Unsupported radix");

        // Типы
        /// @brief Тип цифр
        typedef digit<base> Digit;

        /// @brief Тип контейнера, используемого для хранения цифр
        typedef std::vector<Digit> Digits_container;

        /// @brief Тип для представления размера
        typedef typename Digits_container::size_type size_type;

        // Создание, копирование, уничтожение
        /** @brief Конструктор без параметров
        @post <tt> *this == 0 </tt>
        */
        integer()
         : members_{}
        {
            is_not_negative_ref() = true;
        }

        /** @brief Конструктор
        @param init_value значение числа
        @post <tt> *this == init_value </tt>
        */
        template <class T>
        explicit integer(T init_value)
        {
            static_assert(std::is_integral<T>::value, "Must be integral");

            is_not_negative_ref() = (init_value >= T{0});

            using std::abs;
            ural::copy(digits_sequence<T, base>{abs(std::move(init_value))},
                       this->digits_ref() | ural::back_inserter);
        }

        // Доступ к цифрам
        /** @brief Доступ к цифрам
        @return Возвращает константную ссылку на контейнер, содержащий цифры
        */
        Digits_container const & digits() const
        {
            return members_[ural::_1];
        }

        // Инкремент и декремент
        integer & operator++()
        {
            // @todo устранить дублирование

            if(this->is_not_negative_ref() == false)
            {
                this->is_not_negative_ref() = true;
                -- *this;
                this->is_not_negative_ref() = this->digits().empty();

                return *this;
            }

            auto added = Digit{1};

            for(size_t i = 0; added > 0 && i < this->size(); ++ i)
            {
                auto sum = this->digits()[i] + added;
                digits_ref()[i] = sum.value();
                added = sum.carry();
            }

            if(added > 0)
            {
                digits_ref().push_back(added);
            }

            return *this;

        }

        integer & operator--()
        {
            // @todo устранить дублирование

            if(this->is_not_negative_ref() == false || this->digits().empty())
            {
                this->is_not_negative_ref() = true;
                ++ *this;
                this->is_not_negative_ref() = false;

                return *this;
            }

            // Вычитаем, пока есть перенос
            for(size_t i = 0; i < this->size(); ++ i)
            {
                // @todo Более идиоматический код
                if(digits()[i] == 0)
                {
                    digits_ref()[i] = Digit{base - 1};
                }
                else
                {
                    digits_ref()[i] = Digit{digits()[i].value() - 1};
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
            result.is_not_negative_ref() = !result.is_not_negative_ref();
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
                digits_ref().resize(x.size(), Digit{0});
            }

            Digit carry{0};

            for(size_t i = 0; i < x.size(); ++ i)
            {
                auto sum = add_with_carry(digits()[i], x.digits()[i], carry);

                carry = sum.carry();
                digits_ref()[i] = sum.value();
            }

            for(size_t i = x.size(); i < this->size() && carry > 0; ++ i)
            {
                auto sum = digits()[i] + carry;

                digits_ref()[i] = sum.value();
                carry = sum.carry();
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

            auto carry = Digit{0};

            for(size_t k = 0; k < x.size(); ++ k)
            {
                // @todo Более идиоматический код
                // 0 <= res < 2 * base
                auto res = this->digits()[k].value() + base - x.digits()[k].value() - carry.value();

                assert(0 <= res && res < 2 * base);

                if(res < base)
                {
                    this->digits_ref()[k] = Digit{res};
                    carry = Digit{1};
                }
                else
                {
                    this->digits_ref()[k] = Digit{res - base};
                    carry = Digit{0};
                }
            }

            for(size_t k = x.size(); carry > 0; ++ k)
            {
                // @todo Более идиоматический код
                auto res = this->digits()[k].value() + base - carry.value();

                if(res < base)
                {
                    this->digits_ref()[k] = Digit{res};
                    carry = Digit{1};
                }
                else
                {
                    this->digits_ref()[k] = Digit{res - base};
                    carry = Digit{0};
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

            a.digits_ref().resize(i, Digit{0});

            Digit carry{0};

            for(auto const & digit : x.digits())
            {
                auto res = multiply_add(carry, digit, d);

                a.digits_ref().push_back(res.value());
                carry = res.carry();
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
            for(; !ural::empty(digits()) && digits().back() == 0; digits_ref().pop_back())
            {}

            if(digits().empty())
            {
                this->is_not_negative_ref() = true;
            }
        }

        Digits_container & digits_ref()
        {
            return members_[ural::_1];
        }

        bool & is_not_negative_ref()
        {
            return members_[ural::_2];
        }

    private:
        ural::tuple<Digits_container, bool> members_;

    };

    template <long radix>
    integer<radix>
    operator+(integer<radix> const & x, digit<radix> const & d)
    {
        // @todo Оптимизация
        return x + d.value();
    }

    template <long radix>
    integer<radix>
    operator+(digit<radix> const & d, integer<radix> const & x);

    template <class T, long radix>
    typename std::enable_if<std::is_integral<T>::value, integer<radix>>::type
    operator+(integer<radix> const & x, T const & a)
    {
        // @todo устранить дублирование
        // @todo без временного объекта
        return x + integer<radix>{a};
    }

    template <class T, long radix>
    typename std::enable_if<std::is_integral<T>::value, integer<radix>>::type
    operator+(T const & a, integer<radix> const & x);

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
    operator<(integer<radix> const & x, T const & a)
    {
        // @todo без временного объекта
        return x < integer<radix>{a};
    }

    template <class T, long radix>
    typename std::enable_if<std::is_integral<T>::value, bool>::type
    operator<(T const & a, integer<radix> const & x)
    {
        // @todo без временного объекта
        return integer<radix>{a} < x;
    }

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
        if(x.digits().empty())
        {
            return os << '0';
        }

        if(x.is_not_negative() == false)
        {
            os << '-';
        }

        boost::io::ios_flags_saver saver(os);

        if(10 < radix && radix <= 16)
        {
            os << std::hex;
        }

        if(radix <= 16)
        {
            ural::write_delimeted(os, x.digits() | ural::reversed,
                                  ural::no_delimeter{});
        }
        else
        {
            ural::write_delimeted(os, x.digits() | ural::reversed, ':');
        }


        return os;
    }
}
// namespace ural

#endif
// Z_URAL_NUMERIC_MP_INTEGER_HPP_INCLUDED
