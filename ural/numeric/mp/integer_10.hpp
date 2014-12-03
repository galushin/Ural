#ifndef Z_URAL_NUMERIC_MP_INTEGER_10_HPP_INCLUDED
#define Z_URAL_NUMERIC_MP_INTEGER_10_HPP_INCLUDED

#include <ural/algorithm.hpp>
#include <ural/sequence/reversed.hpp>

namespace ural
{
    // @todo учёт знака
    // @todo устранить дублирование
    // @todo заменить циклы на алгоритмы
    // @todo выделить функции
    // @todo смешанные операции (с встроенными целыми числами)
    class integer_10
    {
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

    friend bool operator<(integer_10 const & x, integer_10 const & y)
    {
        if(x.digits().size() < y.digits().size())
        {
            return true;
        }

        return ural::lexicographical_compare(x.digits() | ural::reversed,
                                             y.digits() | ural::reversed);
    }

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

    friend bool operator==(integer_10 const & x, integer_10 const & y)
    {
        return x.digits() == y.digits();
    }

    public:
        typedef short Digit;
        typedef std::vector<Digit> Digits_container;

        integer_10() = default;

        template <class T>
        explicit integer_10(T init_value)
        {
            assert(init_value >= 0);

            // @todo убедиться, что T - целочисленный типы
            for(; init_value > 0; init_value /= 10)
            {
                digits_.push_back(init_value % 10);
            }
        }

        Digits_container const & digits() const
        {
            return this->digits_;
        }

        integer_10 & operator+=(integer_10 const & x)
        {
            if(digits_.size() < x.digits_.size())
            {
                digits_.resize(x.digits().size(), 0);
            }

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

    private:
        Digits_container digits_;
    };
}
// namespace ural

#endif
// Z_URAL_NUMERIC_MP_INTEGER_10_HPP_INCLUDED
