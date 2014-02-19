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
            // @todo Оптимизировать?
            *this = rational(std::move(num), std::move(denom));
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

        rational & operator*=(rational const & x)
        {
            return *this = *this * x;
        }

        rational & operator*=(IntegerType const & x)
        {
            return *this *= rational{x};
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
    operator+(rational<T> const & x, rational<T> const & y)
    {
        return rational<T>(x.numerator() * y.denominator()
                           + y.numerator() * x.denominator(),
                           x.denominator() * y.denominator());
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

    // Ввод/Вывод
    template <class Char, class Tr, class T>
    std::basic_ostream<Char, Tr> &
    operator<<(std::basic_ostream<Char, Tr> & os, rational<T> const & x)
    {
        return os << x.numerator() << "/" << x.denominator();
    }
}
// namespace ural

#endif
// Z_URAL_MATH_RATIONAL_CPP_INCLUDED
