#ifndef Z_URAL_MATH_RATIONAL_CPP_INCLUDED
#define Z_URAL_MATH_RATIONAL_CPP_INCLUDED

/**
 @todo Оптимизация
*/

#include <utility>

#include <ural/math/common_factor.hpp>
#include <ural/defs.hpp>

namespace ural
{
    class bad_rational
    {};

    template <class IntegerType>
    class rational_base
    {
    public:
        constexpr rational_base(IntegerType x)
         : numerator{std::move(x)}
         , denominator{1}
        {}

        constexpr explicit rational_base(IntegerType num, IntegerType denom, IntegerType g)
         : numerator(std::move(num) / g)
         , denominator(std::move(denom) / g)
        {}

        IntegerType numerator;
        IntegerType denominator;
    };

    template <class IntegerType>
    class rational
     : private rational_base<IntegerType>
    {
        typedef rational_base<IntegerType> Base;

    template <class Char, class Tr>
    friend std::basic_istream<Char, Tr> &
    operator>>(std::basic_istream<Char, Tr> & is, rational & x)
    {
        rational t;
        auto & base = static_cast<Base &>(t);

        is >> base.numerator;

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

        is >> base.denominator;
        x.assign(t.numerator(), t.denominator());
        return is;
    }

    public:
        // Конструкторы и присваивание
        constexpr rational()
         : Base(IntegerType{0})
        {}

        explicit constexpr rational(IntegerType x)
         : Base{std::move(x)}
        {}

        constexpr rational(IntegerType num, IntegerType denom)
         : Base(denom < 0 ? - std::move(num) : std::move(num),
                denom != 0 ? absolute_value(denom) : throw bad_rational{},
                ural::gcd(num, denom))
        {}

        rational & operator=(IntegerType x)
        {
            Base::numerator = std::move(x);
            Base::denominator = IntegerType{1};
            return *this;
        }

        void assign(IntegerType num, IntegerType denom)
        {
            // @todo Можно ли устранить дублирование с конструктором?
            static auto const zero = IntegerType(0);

            if(denom == zero)
            {
                throw bad_rational{};
            }

            auto g = ural::gcd(num, denom);
            num /= g;
            denom /= g;

            if(denom < zero)
            {
                num = - num;
                denom = - denom;
            }

            Base::numerator = std::move(num);
            Base::denominator = std::move(denom);
        }

        // Инкремент и декремент
        rational operator++(int)
        {
            auto old = *this;
            ++*this;
            return old;
        }

        rational & operator++()
        {
            Base::numerator += Base::denominator;
            return *this;
        }

        rational operator--(int)
        {
            auto old = *this;
            --*this;
            return old;
        }

        rational & operator--()
        {
            Base::numerator -= Base::denominator;
            return *this;
        }

        // Числитель и знаменатель
        constexpr IntegerType const & numerator() const
        {
            return Base::numerator;
        }

        constexpr IntegerType const & denominator() const
        {
            return Base::denominator;
        }

        constexpr explicit operator bool() const
        {
            return !!*this;
        }

        rational & operator+=(rational const & x)
        {
            return *this = *this + x;
        }

        rational & operator+=(IntegerType const & x)
        {
            Base::numerator += x * this->denominator();
            return *this;
        }

        rational & operator-=(rational const & x)
        {
            return *this = *this - x;
        }

        rational & operator-=(IntegerType const & x)
        {
            Base::numerator -= x * this->denominator();
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
    };

    template <class T>
    constexpr bool operator!(rational<T> const & x)
    {
        return !x.numerator();
    }

    template <class T>
    constexpr rational<T> abs(rational<T> x)
    {
        using std::abs;
        return rational<T>(x.numerator() < T{0} ? - x.numerator() : x.numerator(),
                           x.denominator());
    }

    template <class T>
    constexpr rational<T> operator+(rational<T> x)
    {
        return x;
    }

    template <class T>
    constexpr rational<T> operator-(rational<T> x)
    {
        // @todo Оптимизация?
        return rational<T>(-x.numerator(), x.denominator());
    }

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

    template <class T>
    constexpr bool operator<(rational<T> const & x, rational<T> const & y)
    {
        // @todo Устойчивость к переполнениям (при умножении)
        return x.numerator() * y.denominator() < y.numerator() * x.denominator();
    }

    template <class T>
    constexpr bool operator<(T const & x, rational<T> const & y)
    {
        return x * y.denominator() < y.numerator();
    }

    template <class T>
    constexpr bool operator<(rational<T> const & x, T const & y)
    {
        return x.numerator() < y * x.denominator();
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
    template <class Char, class Tr, class T>
    std::basic_ostream<Char, Tr> &
    operator<<(std::basic_ostream<Char, Tr> & os, rational<T> const & x)
    {
        return os << x.numerator() << "/" << x.denominator();
    }

    // Преобразование
    template <class To, class IntegerType>
    To rational_cast(rational<IntegerType> const & x)
    {
        return static_cast<To>(x.numerator()) / static_cast<To>(x.denominator());
    }

}
// namespace ural

#endif
// Z_URAL_MATH_RATIONAL_CPP_INCLUDED
