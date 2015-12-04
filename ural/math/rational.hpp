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
 @todo Оптимизация временных значений в арифметических операторах, возможно, за
 счёт шаблонов выражений.
*/

#include <ural/sequence/adaptors/taken_while.hpp>
#include <ural/sequence/progression.hpp>
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

    /** @brief Класс для представления рациональных чисел
    @tparam IntegerType Целочисленный тип
    @note Уровень безопасности настраивать через стратегии нецелесообразно, так
    как небезопасными операциями являются только конструкторы. Сделав безопасные
    конструкции более удобными в использовании, чем небезопасные, мы сделаем
    первые более привлекательными, тогда небезопасные конструкции чаще будут
    использоваться только если в тех случаях, когда это действительно необходимо
    для оптимизации.
    */
    template <class IntegerType>
    class rational
     : boost::incrementable<rational<IntegerType>
     , boost::decrementable<rational<IntegerType>>>
    {
    /** @brief Ввод рационального числа из потока
    @param is поток ввода
    @param x переменная, для которой производится ввод
    @return @c is
    */
    template <class Char, class Tr>
    friend std::basic_istream<Char, Tr> &
    operator>>(std::basic_istream<Char, Tr> & is, rational & x)
    {
        rational::int_type num(0);
        is >> num;

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

        rational::int_type denom(0);
        is >> denom;

        if(!is)
        {
            return is;
        }

        x.assign(std::move(num), std::move(denom));
        return is;
    }

    //@{
    /** @brief Унарный минус
    @return <tt> rational<T>(-x.numerator(), x.denominator()) </tt>
    */
    friend constexpr rational operator-(rational const & x)
    {
        return rational(-x.numerator(), x.denominator(), IntegerType(1),
                        unsafe_tag{});
    }

    friend constexpr rational operator-(rational && x)
    {
        return rational(-std::move(x.numerator_ref()),
                        std::move(x.denominator_ref()),
                        IntegerType(1), unsafe_tag{});
    }
    //@}

    /** @brief Модуль
    @param x число, модуль которого вычисляется
    @return <tt> rational<T>(abs(x.numerator()), x.denominator()) </tt>
    */
    friend constexpr rational abs(rational x)
    {
        return rational(ural::abs_constexpr(std::move(x.num_)),
                        std::move(x.denom_),
                        unsafe_reduced_tag{});
    }

    private:
        constexpr IntegerType & numerator_ref()
        {
            return this->num_;
        }

        constexpr IntegerType & denominator_ref()
        {
            return this->denom_;
        }

        /** @brief Подготовка числителя рационального числа
        @param num исходный числитель
        @param denom исходный знаменатель
        @pre <tt> gcd(num, denom) == 1</tt>
        @return Если <tt> denom < 0 </tt>, то <tt> - num </tt>, иначе ---
        <tt> num </tt>
        */
        static constexpr IntegerType
        prepare_numerator(IntegerType num, IntegerType const & denom)
        {
            return denom < 0 ? - std::move(num) : std::move(num);
        }

        /** @brief Подготовка знаменателя рационального числа
        @param denom исходный знаменатель
        @return <tt> abs(denom) </tt>
        @throw bad_rational, если </tt> denom == 0 </tt>
        */
        static constexpr IntegerType
        prepare_denominator(IntegerType denom)
        {
            return denom != 0 ? abs_constexpr(std::move(denom)) : throw bad_rational{};
        }

        static constexpr IntegerType
        check_numerator(IntegerType num, IntegerType const & denom,
                        IntegerType const & g)
        {
            return (num % g == 0 && denom % g == 0
                    && ural::gcd(num / g, denom / g) == 1)
                    ? num : throw std::logic_error("Incorrect gcd");
        }

    public:
        // Типы
        /** @brief Тип-тэг, используемый, чтобы показать, что вызывающая сторона
        сама отвечает за то, что числитель и знаменатель образуют несократимую
        дробь
        */
        struct unsafe_reduced_tag{};

        /// @brief Типы числителя и знаменателя
        typedef IntegerType int_type;

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
         : num_(std::move(num))
         , denom_(std::move(denom))
        {}

        /** @brief Конструктор на основе числителя и знаменателя
        @param num числитель
        @param denom знаменатель
        @throw bad_rational, если <tt> denom == 0 </tt>
        @post <tt> *this == rational(num, denom) </tt>
        */
        constexpr rational(IntegerType num, IntegerType denom, safe_tag)
         : rational(std::move(num), std::move(denom))
        {}

        /** @brief Конструктор с предусловием
        @param num числитель
        @param denom знаменатель
        @param g наибольший общий множитель @c num и @c denom
        @pre <tt> num % g == 0 </tt>
        @pre <tt> denom % g == 0 </tt>
        @pre <tt> НОД(num / g, denom / g) == 1 </tt>
        @pre <tt> g > 0 </tt>
        @pre <tt> denom > 0 </tt>
        @post <tt> this->numerator() == num / g </tt>
        @post <tt> this->denominator() == denom / g </tt>
        */
        constexpr rational(IntegerType num, IntegerType denom, IntegerType g,
                           unsafe_tag)
         : rational{static_cast<IntegerType>(std::move(num) / g),
                    static_cast<IntegerType>(std::move(denom) / g),
                    unsafe_reduced_tag{}}
        {}

        /** @brief Конструктор
        @param num числитель
        @param denom знаменатель
        @param g наибольший общий множитель @c num и @c denom
        @pre <tt> НОД(num / g, denom / g) == 1 </tt>
        @pre <tt> num % g == 0 </tt>
        @pre <tt> denom % g == 0 </tt>
        */
        constexpr rational(IntegerType num, IntegerType denom, IntegerType g,
                           safe_tag)
         : rational(this->check_numerator(std::move(num), denom, g),
                    std::move(denom))
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
        constexpr rational(IntegerType x)
         : rational(IntegerType{std::move(x)}, IntegerType(1),
                    unsafe_reduced_tag{})
        {}

        /** @brief Конструктор на основе числителя и знаменателя
        @param num числитель
        @param denom знаменатель
        @throw bad_rational, если <tt> denom == 0 </tt>
        Пусть <tt> g = gcd(abs(num), abs(denom)) </tt>
        @post <tt> this->numerator() == num / g * sign(denom) </tt>
        @post <tt> this->denominator() == abs(denom) / g </tt>
        */
        constexpr rational(IntegerType num, IntegerType denom)
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
            this->numerator_ref() = std::move(x);
            this->denominator_ref() = IntegerType{1};

            return *this;
        }


        /** @brief Задание числителя и знаменателя
        @param num числитель
        @param denom знаменатель
        @post Пусть <tt> g = gcd(abs(num), abs(denom)) </tt>, тогда
        <tt> this->numerator() == num / g * sign(denom) </tt> и
        <tt> this->denominator() == abs(denom) </tt>
        @throw bad_rational, если <tt> denom == 0 </tt>
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
            return this->num_;
        }

        /** @brief Знаменатель
        @return Знаменатель
        */
        constexpr IntegerType const & denominator() const
        {
            return this->denom_;
        }

        /** @brief Явное преобразование в @c bool
        @return <tt> *this != 0 </tt>
        */
        constexpr explicit operator bool() const
        {
            return !!*this;
        }

        // Составные операторы присваивания
        /** @brief Прибавление рационального числа
        @param x прибавляемое
        @return <tt> *this </tt>
        */
        rational & operator+=(rational const & x)
        {
            return *this = *this + x;
        }

        /** @brief Прибавление целого числа
        @param x прибавляемое
        @return <tt> *this </tt>
        */
        rational & operator+=(IntegerType const & x)
        {
            this->numerator_ref() += x * this->denominator();
            return *this;
        }

        /** @brief Вычитание рационального числа
        @param x вычитаемое
        @return <tt> *this </tt>
        */
        rational & operator-=(rational const & x)
        {
            return *this = *this - x;
        }

        /** @brief Вычитание целого числа
        @param x вычитаемое
        @return <tt> *this </tt>
        */
        rational & operator-=(IntegerType const & x)
        {
            this->numerator_ref() -= x * this->denominator();
            return *this;
        }

        /** @brief Умножение на рациональное число
        @param x делитель
        @return <tt> *this </tt>
        */
        rational & operator*=(rational const & x)
        {
            return *this = *this * x;
        }

        /** @brief Умножение на целое число
        @param x множитель
        @return <tt> *this </tt>
        */
        rational & operator*=(IntegerType const & x)
        {
            return *this *= rational{x};
        }

        /** @brief Деление на рациональное число
        @param x делитель
        @return <tt> *this </tt>
        */
        rational & operator/=(rational const & x)
        {
            return *this = *this / x;
        }

        /** @brief Деление на целое число
        @param x делитель
        @return <tt> *this </tt>
        */
        rational & operator/=(IntegerType const & x)
        {
            return *this /= rational{x};
        }

    private:
        IntegerType num_;
        IntegerType denom_;
    };

    /** @brief Проверка равенства нулю рационального числа
    @param x рациональное число
    @return <tt> !x.numerator() </tt>
    */
    template <class T>
    constexpr bool operator!(rational<T> const & x)
    {
        return !x.numerator();
    }


    /** @brief Унарный плюс
    @return <tt> x </tt>
    @note Разделение на две перегрузки: константная ссылка и ссылка на временный
    объект -- нецелесообразно, так как эта функция всегда должна возвращать
    копию аргумента. Если такое поведение (копирование) не нужно, то можно
    просто убрать унарный плюс из выражения
    */
    template <class T>
    constexpr rational<T> operator+(rational<T> x)
    {
        return x;
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

    //@{
    /** @brief Проверка на равенство рационального и целого чисел
    @param q рациональное число
    @param n целое число
    @return <tt> q.numerator() == n && q.denominator() == T{1} </tt>
    */
    template <class T>
    constexpr bool operator==(rational<T> const & q, T const & n)
    {
        return q.numerator() == n && q.denominator() == T{1};
    }

    template <class T>
    constexpr bool operator==(T const & n, rational<T> const & q)
    {
        return q == n;
    }
    //@}

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
    @return <tt> x.numerator() * y.denominator() < y.numerator() * x.denominator() </tt>,
    но таким способом, что гарантировано не происходит переполнение.
    */
    template <class T>
    constexpr bool operator<(rational<T> const & x, rational<T> const & y)
    {
        return mixed_fraction<T>(x) < mixed_fraction<T>(y);
    }

    /** @brief Оператор "меньше" для целого и рационального чисел
    @param x целое число
    @param y рациональное число
    @return <tt> x * y.denominator() < y.numerator() </tt>, но таким способом,
    что гарантировано не происходит переполнение.
    */
    template <class T>
    constexpr bool operator<(T const & x, rational<T> const & y)
    {
        return x < mixed_fraction<T>(y);
    }

    /** @brief Оператор "меньше" для рационального и целого чисел
    @param x рациональное число
    @param y целое число
    @return <tt> x.numerator() < y * x.denominator() </tt>, но таким способом,
    что гарантировано не происходит переполнение.
    */
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

    /** @brief Сумма рациональных чисел
    @param x, y слагаемые
    */
    template <class T>
    constexpr rational<T>
    operator+(rational<T> const & x, rational<T> const & y)
    {
        return sum_helper(x, y, ural::lcm(x.denominator(), y.denominator()));
    }

    /** @brief Сумма рационального и целого чисел
    @param x первое слагаемое -- рациональное число
    @param y второе слагаемое -- целое число
    @return <tt> rational<T>(x.numerator() + y * x.denominator(), x.denominator()) </tt>
    */
    template <class T>
    constexpr rational<T>
    operator+(rational<T> const & x, T const & y)
    {
        return rational<T>(x.numerator() + y * x.denominator(),
                           x.denominator());
    }

    /** @brief Сумма целого и рационального чисел
    @param x первое слагаемое -- рациональное число
    @param y второе слагаемое -- целое число
    @return <tt> y + x </tt>
    */
    template <class T>
    constexpr rational<T>
    operator+(T const & x, rational<T> const & y)
    {
        return rational<T>(x * y.denominator() + y.numerator(),
                           y.denominator());
    }

    /** @brief Разность рациональных чисел
    @param x уменьшаемое
    @param y вычитаемое
    @return <tt> rational<T>(x.numerator() * y.denominator() - y.numerator() * x.denominator(),
                             x.denominator() * y.denominator()) </tt>
    */
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

    /** @brief Вычитание рационального числа из целого
    @param x уменьшаемое -- целое число
    @param y вычитаемое -- рациональное число
    @return <tt> rational<T>(x * y.denominator() - y.numerator(), y.denominator()) </tt>
    */
    template <class T>
    constexpr rational<T>
    operator-(T const & x, rational<T> const & y)
    {
        return rational<T>(x * y.denominator() - y.numerator(),
                           y.denominator());
    }

    /** @brief Умножение рациональных чисел
    @param x, y множители
    @return <tt> rational<T>(x.numerator() * y.numerator(),
                             x.denominator() * y.denominator()) </tt>
    */
    template <class T>
    constexpr rational<T>
    operator*(rational<T> const & x, rational<T> const & y)
    {
        // @todo Сокращать, если возможно.
        return rational<T>(x.numerator() * y.numerator(),
                           x.denominator() * y.denominator());
    }

    /** @brief Умножение рационального числа на целое
    @param x рациональное число
    @param y целое число
    @return <tt> rational<T>(x.numerator() * y, x.denominator()) </tt>
    */
    template <class T>
    constexpr rational<T>
    operator*(rational<T> const & x, T const & y)
    {
        return rational<T>(x.numerator() * y, x.denominator());
    }

    /** @brief Умножение целого числа на рациональное
    @param x рациональное число
    @param y целое число
    @return <tt> rational<T>(x * y.numerator(), y.denominator()) </tt>
    */
    template <class T>
    constexpr rational<T>
    operator*(T const & x, rational<T> const & y)
    {
        return rational<T>(x * y.numerator(), y.denominator());
    }

    /** @brief Деление рациональных чисел
    @param x делимое
    @param y делитель
    @return <tt> rational<T>(x.numerator() * y.denominator(),
                             x.denominator() * y.numerator()) </tt>
    */
    template <class T>
    constexpr rational<T>
    operator/(rational<T> const & x, rational<T> const & y)
    {
        // @todo Сокращать, если возможно.
        return rational<T>(x.numerator() * y.denominator(),
                           x.denominator() * y.numerator());
    }

    /** @brief Деление рационального числа на целое
    @param x делимое
    @param y делитель
    @return <tt> rational<T>(x.numerator(), x.denominator() * y) </tt>
    */
    template <class T>
    constexpr rational<T>
    operator/(rational<T> const & x, T const & y)
    {
        return rational<T>(x.numerator(), x.denominator() * y);
    }

    /** @brief Деление целого числа на рациональное
    @param x делимое
    @param y делитель
    @return <tt> rational<T>(x * y.denominator(), y.numerator()) </tt>
    */
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
    @param eps требуемая точность
    @param Q множитель, используемый для преобразования
    @return такое вещественное число @c x, что <tt> abs(x - r) < eps </tt>
    */
    template <class RealType, class Rational>
    RealType rational_to_real(Rational r, RealType const & eps,
                              typename Rational::int_type Q)
    {
        auto result = RealType{0};

        for(auto q : ural::make_geometric_progression(1.0, 1.0 / Q)
                   | ural::taken_while([=](RealType const & x) { return x >= eps; }))
        {
            auto const n = rational_cast<typename Rational::int_type>(r);

            result += n * q;

            r -= n;
            r *= Q;
        }

        return result;
    }

    /** @brief Преобразование рационального числа в вещественное
    @tparam RealType тип вещественного числа
    @param r преобразуемое число
    @param eps требуемая точность
    @return такое вещественное число @c x, что <tt> abs(x - r) < eps </tt>
    */
    template <class RealType, class Rational>
    RealType rational_to_real(Rational r, RealType const & eps)
    {
        /* Здесь есть проблема выбора множителя:
        1. Большой множитель -- меньше шагов в цикле
        2. Маленький множитель -- меньше вероятность переполнения
        */
        return rational_to_real(std::move(r), eps,
                                typename Rational::int_type(2));
    }

    /* @todo Преобразование рационального числа в обыкновенную (периодическую)
    дробь в произвольном основании
    */

    /** @brief Является рациональное число бесконечным
    @param x рациональное число
    @return <tt> isfinite(x.numerator()) </tt>
    */
    template <class Integer>
    constexpr bool isfinite(rational<Integer> const & x)
    {
        using std::isfinite;
        return isfinite(x.numerator());
    }
}
// namespace ural

#endif
// Z_URAL_MATH_RATIONAL_CPP_INCLUDED
