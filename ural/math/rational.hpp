#ifndef Z_URAL_MATH_RATIONAL_CPP_INCLUDED
#define Z_URAL_MATH_RATIONAL_CPP_INCLUDED

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

/** @file ural/math/rational.hpp
 @brief Класс рациональных чисел (в том числе --- с поддержкой @b constexpr).
*/

#include <ural/tuple.hpp>
#include <ural/math/common_factor.hpp>
#include <ural/defs.hpp>

#include <boost/operators.hpp>

#include <istream>
#include <ostream>
#include <utility>
#include <stdexcept>

namespace ural
{
    /** @brief Класс исключения для некорректных попыток создать рациональное
    число
    */
    class bad_rational
     : std::logic_error
    {
    public:
        /// @brief Конструктор
        bad_rational()
         : logic_error("bad_rational")
        {}
    };

    class rational_policy_no_checks
    {
    public:
    };

    /** @brief Класс для представления рациональных чисел
    @tparam IntegerType Целочисленный тип
    @todo Добавить проверки предусловий в небезопасных конструкциях в отладочном
    режиме
    @todo Уровень безопасности --- через стратегии
    */
    template <class IntegerType>
    class rational
     : boost::incrementable<rational<IntegerType>
     , boost::decrementable<rational<IntegerType>>>
    {
    template <class Char, class Tr>
    friend std::basic_istream<Char, Tr> &
    operator>>(std::basic_istream<Char, Tr> & is, rational & x)
    {
        // @todo Устранить временный объект?
        rational t;

        is >> t.numerator_ref();

        if(!is)
        {
            return is;
        }

        char reader = is.get();

        if(reader != '/')
        {
            is.setstate(std::ios::failbit);
            return is;
        }

        reader = is.get();
        if(std::isspace(reader))
        {
            is.setstate(std::ios::failbit);
            return is;
        }
        else
        {
            is.putback(reader);
        }

        is >> t.denominator_ref();
        x.assign(t.numerator(), t.denominator());
        return is;
    }

    private:
        IntegerType & numerator_ref()
        {
            return members_[ural::_1];
        }

        IntegerType & denominator_ref()
        {
            return members_[ural::_2];
        }

        static constexpr IntegerType
        prepare_numerator(IntegerType num, IntegerType denom)
        {
            return denom < 0 ? - std::move(num) : std::move(num);
        }

        static constexpr IntegerType
        prepare_denominator(IntegerType denom)
        {
            return denom != 0 ? absolute_value(std::move(denom)) : throw bad_rational{};
        }


    public:
        // Типы
        /** @brief Тип-тэг, используемый, чтобы показать, что вызывающая сторона
        сама отвечает за то, что числитель и знаменатель образуют несократимую
        дробь
        */
        struct unsafe_reduced_tag{};

        // Конструкторы и присваивание
        /** @brief Конструктор с предусловием
        @param num числитель
        @param denom знаменатель
        @pre <tt> НОД(num, denom) == 1 </tt>
        @pre <tt> denom > 0 </tt>
        @post <tt> this->numerator() == num </tt>
        @post <tt> this->denominator() == denom </tt>
        */
        constexpr rational(IntegerType num, IntegerType denom, unsafe_reduced_tag)
         : members_{std::move(num), std::move(denom)}
        {}

        /** @brief Конструктор с предусловием
        @param num числитель
        @param denom знаменатель
        @param g наибольший общий множитель @c num и @c denom
        @pre <tt> НОД(num / g, denom / g) == 1 </tt>
        @pre <tt> g > 0 </tt>
        @pre <tt> denom > 0 </tt>
        @post <tt> this->numerator() == num / g </tt>
        @post <tt> this->denominator() == denom / g </tt>
        */
        constexpr rational(IntegerType num, IntegerType denom, IntegerType g,
                           unsafe_tag)
         : rational{std::move(num) / g, std::move(denom) / g,
                    unsafe_reduced_tag{}}
        {}

        /** @brief Конструктор без параметров
        @post <tt> this->numerator() == 0 </tt>
        @post <tt> this->denominator() == 1 </tt>
        */
        constexpr rational()
         : rational(IntegerType{0}, IntegerType{1}, IntegerType{1},
                    unsafe_tag{})
        {}

        /** @brief Конструктор на основе целого числа
        @param x значение
        @post <tt> this->numerator() == x </tt>
        @post <tt> this->denominator() == 1 </tt>
        */
        explicit constexpr rational(IntegerType x)
         : rational(IntegerType{std::move(x)}, IntegerType(1),
                    unsafe_reduced_tag{})
        {}

        /** @brief Конструктор на основе числителя и знаменателя
        @param num числитель
        @param denom знаменатель
        @pre <tt> denom != 0 </tt>
        Пусть <tt> g = gcd(abs(num), abs(denom)) </tt>
        @post <tt> this->numerator() == num / g * sign(denom) </tt>
        @post <tt> this->denominator() == abs(denom) / g </tt>
        @todo оптимизация
        */
        explicit constexpr rational(IntegerType num, IntegerType denom)
         : rational(prepare_numerator(num, denom),
                    prepare_denominator(denom),
                    ural::gcd(num, denom),
                    unsafe_tag{})
        {}

        /** @brief Оператор присваивания с целым аргументом
        @param x значение
        @post <tt> this->numerator() == x </tt>
        @post <tt> this->denominator() == 1 </tt>
        @return <tt> *this </tt>
        */
        rational & operator=(IntegerType x)
        {
            members_[ural::_1] = std::move(x);
            members_[ural::_2] = IntegerType{1};

            return *this;
        }


        /** @brief Задание числителя и знаменателя
        @param num числитель
        @param denom знаменатель
        @post Пусть <tt> g = gcd(abs(num), abs(denom)) </tt>, тогда
        <tt> this->numerator() == num / g * sign(denom) </tt> и
        <tt> this->denominator() == abs(denom) </tt>
        @throw @c bad_rational, если <tt> denom == 0 </tt>.
        */
        void assign(IntegerType num, IntegerType denom)
        {
            /* Устранить дублирование с конструктором без создания временного
            объекта невозможно, так как конструктор должен быть constexpr
            */
            static auto const zero = IntegerType(0);

            if(denom == zero)
            {
                throw bad_rational{};
            }

            if(denom < zero)
            {
                num = - num;
                denom = - denom;
            }

            auto g = ural::gcd(num, denom);

            return this->assign(std::move(num), std::move(denom), std::move(g),
                                unsafe_tag{});
        }

        /** @brief Задание числителя и знаменателя с предусловием
        @param num числитель
        @param denom знаменатель
        @pre <tt> НОД(num, denom) == 1 </tt>
        @pre <tt> denom > 0 </tt>
        @post <tt> this->numerator() == num </tt>
        @post <tt> this->denominator() == denom </tt>
        */
        void assign(IntegerType num, IntegerType denom, unsafe_reduced_tag)
        {
            this->numerator_ref() = std::move(num);
            this->denominator_ref() = std::move(denom);
        }

        /** @brief Задание числителя и знаменателя с предусловием
        @param num числитель
        @param denom знаменатель
        @param g наибольший общий множитель @c num и @c denom
        @pre <tt> НОД(num / g, denom / g) == 1 </tt>
        @pre <tt> g > 0 </tt>
        @pre <tt> denom > 0 </tt>
        @post <tt> this->numerator() == num / g </tt>
        @post <tt> this->denominator() == denom / g </tt>
        */
        void assign(IntegerType num, IntegerType denom, IntegerType g,
                    unsafe_tag)
        {
            num /= g;
            denom /= g;

            this->assign(std::move(num), std::move(denom), unsafe_reduced_tag{});
        }

        // Инкремент и декремент
        /** @brief Увеличение значения на единицу
        @return <tt> *this </tt>
        */
        rational & operator++()
        {
            this->numerator_ref() += this->denominator();
            return *this;
        }

        /** @brief Уменьшение значения на единицу
        @return <tt> *this </tt>
        */
        rational & operator--()
        {
            this->numerator_ref() -= this->denominator();
            return *this;
        }

        // Числитель и знаменатель
        /** @brief Числитель
        @return Числитель
        */
        constexpr IntegerType const & numerator() const
        {
            return members_[ural::_1];
        }

        /** @brief Знаменатель
        @return Знаменатель
        */
        constexpr IntegerType const & denominator() const
        {
            return members_[ural::_2];
        }

        constexpr explicit operator bool() const
        {
            return !!*this;
        }

        // Составные операторы присваивания
        rational & operator+=(rational const & x)
        {
            return *this = *this + x;
        }

        rational & operator+=(IntegerType const & x)
        {
            this->numerator_ref() += x * this->denominator();
            return *this;
        }

        rational & operator-=(rational const & x)
        {
            return *this = *this - x;
        }

        rational & operator-=(IntegerType const & x)
        {
            this->numerator_ref() -= x * this->denominator();
            return *this;
        }

        rational & operator*=(rational const & x)
        {
            return *this = *this * x;
        }

        rational & operator*=(IntegerType const & x)
        {
            return *this *= rational{x};
        }

        rational & operator/=(rational const & x)
        {
            return *this = *this / x;
        }

        rational & operator/=(IntegerType const & x)
        {
            return *this /= rational{x};
        }
    private:
        ural::tuple<IntegerType, IntegerType> members_;
    };

    /** @brief Проверка равенства нулю
    @return <tt> !x.numerator() </tt>
    */
    template <class T>
    constexpr bool operator!(rational<T> const & x)
    {
        return !x.numerator();
    }

    /** @brief Модуль
    @param x число, модуль которого вычисляется
    @return <tt> rational<T>(abs(x.numerator()), x.denominator()) </tt>
    */
    template <class T>
    constexpr rational<T> abs(rational<T> x)
    {
        // @todo Оптимизация - модуль вместо условной конструкции?
        return x.numerator() < T{0} ? -std::move(x) : std::move(x);
    }

    /** @brief Унарный плюс
    @return <tt> x </tt>
    */
    template <class T>
    constexpr rational<T> operator+(rational<T> x)
    {
        return std::move(x);
    }

    /** @brief Унарный минус
    @return <tt> rational<T>(-x.numerator(), x.denominator()) </tt>
    @todo Реализовать версию с перемещением?
    */
    template <class T>
    constexpr rational<T> operator-(rational<T> const & x)
    {
        return rational<T>(-x.numerator(), x.denominator(), T{1}, unsafe_tag{});
    }

    /** @brief Оператор "равно"
    @param x левый операнд
    @param y правый опернад
    @return <tt> x.numerator() == y.numerator() && x.denominator() == y.denominator() </tt>
    */
    template <class T>
    constexpr bool operator==(rational<T> const & x, rational<T> const & y)
    {
        return x.numerator() == y.numerator()
            && x.denominator() == y.denominator();
    }

    template <class T>
    constexpr bool operator==(rational<T> const & x, T const & y)
    {
        return x.numerator() == y && x.denominator() == T{1};
    }

    template <class T>
    constexpr bool operator==(T const & x, rational<T> const & y)
    {
        return x == y.numerator() && T{1} == y.denominator();
    }

    /// @cond false
    template <class T>
    struct mixed_fraction
    {
        constexpr mixed_fraction(rational<T> const & x)
         : whole(x.numerator() / x.denominator())
         , num(x.numerator() % x.denominator())
         , denom(x.denominator())
        {}

        constexpr mixed_fraction(T numerator, T denominator)
         : whole(numerator / denominator)
         , num(numerator % denominator)
         , denom(denominator)
        {}

        constexpr mixed_fraction next() const
        {
            return mixed_fraction(denom, num);
        }

        T whole;
        T num;
        T denom;
    };

    template <class T>
    constexpr bool
    operator<(mixed_fraction<T> const & x, mixed_fraction<T> const & y)
    {
        return x.whole == y.whole
               ? (x.num == T{0} || y.num == T{0} ? x.num < y.num : y.next() < x.next())
               : x.whole < y.whole;
    }

    template <class T>
    constexpr bool
    operator<(T const & x, mixed_fraction<T> const & y)
    {
        return x < y.whole || (x == y.whole && y.num != 0);
    }

    template <class T>
    constexpr bool
    operator<(mixed_fraction<T> const & x, T const & y)
    {
        return x.whole < y;
    }
    /// @endcond

    /** @brief Оператор "меньше"
    @param x левый операнд
    @param y правый операнд
    */
    template <class T>
    constexpr bool operator<(rational<T> const & x, rational<T> const & y)
    {
        return mixed_fraction<T>(x) < mixed_fraction<T>(y);
    }

    template <class T>
    constexpr bool operator<(T const & x, rational<T> const & y)
    {
        return x < mixed_fraction<T>(y);
    }

    template <class T>
    constexpr bool operator<(rational<T> const & x, T const & y)
    {
        return mixed_fraction<T>(x) < y;
    }

    // Арифметические операторы
    template <class T>
    constexpr rational<T>
    sum_helper(rational<T> const & x, rational<T> const & y, T const & d)
    {
        return rational<T>(x.numerator() * (d / x.denominator())
                           + y.numerator() * (d / y.denominator()), d);
    }

    template <class T>
    constexpr rational<T>
    operator+(rational<T> const & x, rational<T> const & y)
    {
        return sum_helper(x, y, ural::lcm(x.denominator(), y.denominator()));
    }

    template <class T>
    constexpr rational<T>
    operator+(rational<T> const & x, T const & y)
    {
        return rational<T>(x.numerator() + y * x.denominator(),
                           x.denominator());
    }

    template <class T>
    constexpr rational<T>
    operator+(T const & x, rational<T> const & y)
    {
        return rational<T>(x * y.denominator() + y.numerator(),
                           y.denominator());
    }

    template <class T>
    constexpr rational<T>
    operator-(rational<T> const & x, rational<T> const & y)
    {
        // @todo Избегать переполнений
        return rational<T>(x.numerator() * y.denominator()
                           - y.numerator() * x.denominator(),
                           x.denominator() * y.denominator());
    }

    template <class T>
    constexpr rational<T>
    operator-(rational<T> const & x, T const & y)
    {
        return rational<T>(x.numerator() - y * x.denominator(),
                           x.denominator());
    }

    template <class T>
    constexpr rational<T>
    operator-(T const & x, rational<T> const & y)
    {
        return rational<T>(x * y.denominator() - y.numerator(),
                           y.denominator());
    }

    template <class T>
    constexpr rational<T>
    operator*(rational<T> const & x, rational<T> const & y)
    {
        // @todo Сокращать, если возможно.
        return rational<T>(x.numerator() * y.numerator(),
                           x.denominator() * y.denominator());
    }

    template <class T>
    constexpr rational<T>
    operator*(rational<T> const & x, T const & y)
    {
        return rational<T>(x.numerator() * y, x.denominator());
    }

    template <class T>
    constexpr rational<T>
    operator*(T const & x, rational<T> const & y)
    {
        return rational<T>(x * y.numerator(), y.denominator());
    }

    template <class T>
    constexpr rational<T>
    operator/(rational<T> const & x, rational<T> const & y)
    {
        // @todo Сокращать, если возможно.
        return rational<T>(x.numerator() * y.denominator(),
                           x.denominator() * y.numerator());
    }

    template <class T>
    constexpr rational<T>
    operator/(rational<T> const & x, T const & y)
    {
        return rational<T>(x.numerator(), x.denominator() * y);
    }

    template <class T>
    constexpr rational<T>
    operator/(T const & x, rational<T> const & y)
    {
        return rational<T>(x * y.denominator(), y.numerator());
    }

    // Ввод/Вывод
    /** @brief Запись в поток вывода
    @param os поток вывода
    @param x записываемый объект
    @return <tt> os </tt>
    */
    template <class Char, class Tr, class T>
    std::basic_ostream<Char, Tr> &
    operator<<(std::basic_ostream<Char, Tr> & os, rational<T> const & x)
    {
        os << x.numerator();

        if(x.denominator() != 1)
        {
            os << "/" << x.denominator();
        }

        return os;
    }

    // Преобразование
    /** @brief Преобразование (например, в тип с плавающей точкой)
    @tparam тип, в который осуществляется преобразование
    @param x преобразуемое значение
    @return <tt> To(x.numerator())/To(x.denominator())</tt>
    */
    template <class To, class IntegerType>
    constexpr To rational_cast(rational<IntegerType> const & x)
    {
        return static_cast<To>(x.numerator()) / static_cast<To>(x.denominator());
    }

    /** @brief Преобразование рационального числа в вещественное
    @tparam RealType тип вещественного числа
    @param r преобразуемое число
    @param требуемая точность
    @return такое вещественное число @c x, что <tt> abs(x - r) < eps </tt>
    @todo Перегрузка, задающая "естественную" точность
    @todo Обязательно ли использовать десятичную систему?
    */
    template <class RealType, class Rational>
    RealType rational_to_real(Rational r, RealType const & eps)
    {
        // @todo Выделить вычисление целой части
        auto result = RealType{0};

        for(auto q = RealType{1.0}; q >= eps; q *= RealType{0.1})
        {
            auto const n = r.numerator() / r.denominator();
            result += n * q;

            r -= n;
            r *= Rational{10};
        }

        return result;
    }

    /* @todo Преобразование рационального числа в обыкновенную (периодическую)
    дробь в произвольном основании
    */
}
// namespace ural

#endif
// Z_URAL_MATH_RATIONAL_CPP_INCLUDED
