#ifndef Z_URAL_MATH_COMMON_FACTOR_HPP_INCLUDED
#define Z_URAL_MATH_COMMON_FACTOR_HPP_INCLUDED

namespace ural
{
    template <typename IntegerType>
    class gcd_evaluator
    {
    private:
        constexpr IntegerType euclidean(IntegerType x, IntegerType y) const
        {
            return y == IntegerType(0) ? x : (*this)(y, x % y);
        }

    public:
        constexpr IntegerType operator()(IntegerType x, IntegerType y) const
        {
            using std::abs;
            return abs(euclidean(x, y));
        }
    };

    template <typename IntegerType>
    class lcm_evaluator
    {
    private:
        constexpr IntegerType impl(IntegerType x, IntegerType y) const
        {
            return (x == y) ? x : x / gcd_evaluator<IntegerType>{}(x, y) * y;
        }

    public:
        constexpr IntegerType operator()(IntegerType x, IntegerType y) const
        {
            using std::abs;
            return abs(impl(x, y));
        }
    };

    template <typename IntegerType>
    constexpr IntegerType gcd(IntegerType const &a, IntegerType const &b)
    {
        return gcd_evaluator<IntegerType>{}(a, b);
    }

    template <typename IntegerType>
    constexpr IntegerType lcm(IntegerType const &a, IntegerType const &b)
    {
        return lcm_evaluator<IntegerType>{}(a, b);
    }

    typedef int static_gcd_type;

    template <static_gcd_type Value1, static_gcd_type Value2>
    struct static_gcd
     : std::integral_constant<static_gcd_type, gcd(Value1, Value2)>
    {};

    template <static_gcd_type Value1, static_gcd_type Value2>
    struct static_lcm
     : std::integral_constant<static_gcd_type, lcm(Value1, Value2)>
    {};
}
// namespace ural

#endif
// Z_URAL_MATH_COMMON_FACTOR_HPP_INCLUDED
