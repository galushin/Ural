// Основано на
// http://www.boost.org/doc/libs/1_55_0/libs/math/test/common_factor_test.cpp

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


#include <ural/math.hpp>
#include <ural/numeric.hpp>
#include <ural/math/common_factor.hpp>

#include <boost/test/unit_test.hpp>
#include "../tests/defs.hpp"

#include <forward_list>

// TODO: add polynominal/non-real type; especially after any switch to the
// binary-GCD algorithm for built-in types

#include <boost/mpl/list.hpp>
#include <boost/operators.hpp>

namespace
{
    namespace ural_ex = ::ural::experimental;

    using signed_test_types = ::boost::mpl::list<signed char, short, int, long>;

    using unsigned_test_types = ::boost::mpl::list<unsigned char, unsigned short, unsigned>;

    template < typename IntType, int ID = 0 >
    class my_wrapped_integer
        : private ::boost::shiftable1<my_wrapped_integer<IntType, ID>,
            ::boost::operators<my_wrapped_integer<IntType, ID> > >
    {
        // Helper type-aliases
        typedef my_wrapped_integer    self_type;
        typedef IntType self_type::*  bool_type;

        // Member data
        IntType  v_;

    public:
        // Template parameters
        typedef IntType  int_type;

        BOOST_STATIC_CONSTANT(int,id = ID);

        // Lifetime management (use automatic destructor and copy constructor)
        my_wrapped_integer( int_type const &v = int_type() )  : v_( v )  {}

        // Accessors
        int_type  value() const  { return this->v_; }

        // Operators (use automatic copy assignment)
        operator bool_type() const  { return this->v_ ? &self_type::v_ : 0; }

        self_type &  operator ++()  { ++this->v_; return *this; }
        self_type &  operator --()  { --this->v_; return *this; }

        self_type  operator ~() const  { return self_type( ~this->v_ ); }
        self_type  operator !() const  { return self_type( !this->v_ ); }
        self_type  operator +() const  { return self_type( +this->v_ ); }
        self_type  operator -() const  { return self_type( -this->v_ ); }

        bool  operator  <( self_type const &r ) const  { return this->v_ < r.v_; }
        bool  operator ==( self_type const &r ) const  { return this->v_ == r.v_; }

        self_type &operator *=(self_type const &r) {this->v_ *= r.v_; return *this;}
        self_type &operator /=(self_type const &r) {this->v_ /= r.v_; return *this;}
        self_type &operator %=(self_type const &r) {this->v_ %= r.v_; return *this;}
        self_type &operator +=(self_type const &r) {this->v_ += r.v_; return *this;}
        self_type &operator -=(self_type const &r) {this->v_ -= r.v_; return *this;}
        self_type &operator<<=(self_type const &r){this->v_ <<= r.v_; return *this;}
        self_type &operator>>=(self_type const &r){this->v_ >>= r.v_; return *this;}
        self_type &operator &=(self_type const &r) {this->v_ &= r.v_; return *this;}
        self_type &operator |=(self_type const &r) {this->v_ |= r.v_; return *this;}
        self_type &operator ^=(self_type const &r) {this->v_ ^= r.v_; return *this;}

        // Input & output
        friend std::istream & operator >>( std::istream &i, self_type &x )
        { return i >> x.v_; }

        friend std::ostream & operator <<( std::ostream &o, self_type const &x )
        { return o << x.v_; }

    };  // my_wrapped_integer

    template < typename IntType, int ID >
    my_wrapped_integer<IntType, ID>  abs( my_wrapped_integer<IntType, ID> const &x )
    { return ( x < my_wrapped_integer<IntType, ID>(0) ) ? -x : +x; }

    typedef my_wrapped_integer<int, 1>       MyInt2;
    typedef my_wrapped_integer<unsigned, 1>  MyUnsigned2;

}
// namespace

BOOST_AUTO_TEST_SUITE( common_factor_test_suite )

BOOST_AUTO_TEST_CASE_TEMPLATE( gcd_int_test, T, signed_test_types )
{
    using ::ural_ex::gcd;

    // Originally from Boost.Rational tests
    BOOST_CHECK_EQUAL( gcd<T>(  1,  -1), static_cast<T>( 1) );
    BOOST_CHECK_EQUAL( gcd<T>( -1,   1), static_cast<T>( 1) );
    BOOST_CHECK_EQUAL( gcd<T>(  1,   1), static_cast<T>( 1) );
    BOOST_CHECK_EQUAL( gcd<T>( -1,  -1), static_cast<T>( 1) );
    BOOST_CHECK_EQUAL( gcd<T>(  0,   0), static_cast<T>( 0) );
    BOOST_CHECK_EQUAL( gcd<T>(  7,   0), static_cast<T>( 7) );
    BOOST_CHECK_EQUAL( gcd<T>(  0,   9), static_cast<T>( 9) );
    BOOST_CHECK_EQUAL( gcd<T>( -7,   0), static_cast<T>( 7) );
    BOOST_CHECK_EQUAL( gcd<T>(  0,  -9), static_cast<T>( 9) );
    BOOST_CHECK_EQUAL( gcd<T>( 42,  30), static_cast<T>( 6) );
    BOOST_CHECK_EQUAL( gcd<T>(  6,  -9), static_cast<T>( 3) );
    BOOST_CHECK_EQUAL( gcd<T>(-10, -10), static_cast<T>(10) );
    BOOST_CHECK_EQUAL( gcd<T>(-25, -10), static_cast<T>( 5) );
    BOOST_CHECK_EQUAL( gcd<T>(  3,   7), static_cast<T>( 1) );
    BOOST_CHECK_EQUAL( gcd<T>(  8,   9), static_cast<T>( 1) );
    BOOST_CHECK_EQUAL( gcd<T>(  7,  49), static_cast<T>( 7) );
}

// GCD on unmarked signed integer type
BOOST_AUTO_TEST_CASE( gcd_unmarked_int_test )
{
    using ural_ex::gcd;

    // The regular signed-integer GCD function performs the unsigned version,
    // then does an absolute-value on the result.  Signed types that are not
    // marked as such (due to no std::numeric_limits specialization) may be off
    // by a sign.
    BOOST_CHECK_EQUAL( abs(gcd<MyInt2>(   1,  -1 )), MyInt2( 1) );
    BOOST_CHECK_EQUAL( abs(gcd<MyInt2>(  -1,   1 )), MyInt2( 1) );
    BOOST_CHECK_EQUAL( abs(gcd<MyInt2>(   1,   1 )), MyInt2( 1) );
    BOOST_CHECK_EQUAL( abs(gcd<MyInt2>(  -1,  -1 )), MyInt2( 1) );
    BOOST_CHECK_EQUAL( abs(gcd<MyInt2>(   0,   0 )), MyInt2( 0) );
    BOOST_CHECK_EQUAL( abs(gcd<MyInt2>(   7,   0 )), MyInt2( 7) );
    BOOST_CHECK_EQUAL( abs(gcd<MyInt2>(   0,   9 )), MyInt2( 9) );
    BOOST_CHECK_EQUAL( abs(gcd<MyInt2>(  -7,   0 )), MyInt2( 7) );
    BOOST_CHECK_EQUAL( abs(gcd<MyInt2>(   0,  -9 )), MyInt2( 9) );
    BOOST_CHECK_EQUAL( abs(gcd<MyInt2>(  42,  30 )), MyInt2( 6) );
    BOOST_CHECK_EQUAL( abs(gcd<MyInt2>(   6,  -9 )), MyInt2( 3) );
    BOOST_CHECK_EQUAL( abs(gcd<MyInt2>( -10, -10 )), MyInt2(10) );
    BOOST_CHECK_EQUAL( abs(gcd<MyInt2>( -25, -10 )), MyInt2( 5) );
    BOOST_CHECK_EQUAL( abs(gcd<MyInt2>(   3,   7 )), MyInt2( 1) );
    BOOST_CHECK_EQUAL( abs(gcd<MyInt2>(   8,   9 )), MyInt2( 1) );
    BOOST_CHECK_EQUAL( abs(gcd<MyInt2>(   7,  49 )), MyInt2( 7) );
}

// GCD on unsigned integer types
BOOST_AUTO_TEST_CASE_TEMPLATE( gcd_unsigned_test, T, unsigned_test_types )
{
    using ural_ex::gcd;

    // Note that unmarked types (i.e. have no std::numeric_limits
    // specialization) are treated like non/unsigned types
    BOOST_CHECK_EQUAL( gcd<T>( 1u,   1u), static_cast<T>( 1u) );
    BOOST_CHECK_EQUAL( gcd<T>( 0u,   0u), static_cast<T>( 0u) );
    BOOST_CHECK_EQUAL( gcd<T>( 7u,   0u), static_cast<T>( 7u) );
    BOOST_CHECK_EQUAL( gcd<T>( 0u,   9u), static_cast<T>( 9u) );
    BOOST_CHECK_EQUAL( gcd<T>(42u,  30u), static_cast<T>( 6u) );
    BOOST_CHECK_EQUAL( gcd<T>( 3u,   7u), static_cast<T>( 1u) );
    BOOST_CHECK_EQUAL( gcd<T>( 8u,   9u), static_cast<T>( 1u) );
    BOOST_CHECK_EQUAL( gcd<T>( 7u,  49u), static_cast<T>( 7u) );
}

// GCD at compile-time
BOOST_AUTO_TEST_CASE( gcd_static_test )
{
    using ural_ex::static_gcd;

    // Can't use "BOOST_CHECK_EQUAL", otherwise the "value" member will be
    // disqualified as compile-time-only constant, needing explicit definition
    BOOST_CHECK( (static_gcd< 1,  1>::value) == 1 );
    BOOST_CHECK( (static_gcd< 0,  0>::value) == 0 );
    BOOST_CHECK( (static_gcd< 7,  0>::value) == 7 );
    BOOST_CHECK( (static_gcd< 0,  9>::value) == 9 );
    BOOST_CHECK( (static_gcd<42, 30>::value) == 6 );
    BOOST_CHECK( (static_gcd< 3,  7>::value) == 1 );
    BOOST_CHECK( (static_gcd< 8,  9>::value) == 1 );
    BOOST_CHECK( (static_gcd< 7, 49>::value) == 7 );
}

// TODO: non-built-in signed and unsigned integer tests, with and without
// numeric_limits specialization; polynominal tests; note any changes if
// built-ins switch to binary-GCD algorithm

// LCM on signed integer types
BOOST_AUTO_TEST_CASE_TEMPLATE( lcm_int_test, T, signed_test_types )
{
    using ural_ex::lcm;

    // Originally from Boost.Rational tests
    BOOST_CHECK_EQUAL( lcm<T>(  1,  -1), static_cast<T>( 1) );
    BOOST_CHECK_EQUAL( lcm<T>( -1,   1), static_cast<T>( 1) );
    BOOST_CHECK_EQUAL( lcm<T>(  1,   1), static_cast<T>( 1) );
    BOOST_CHECK_EQUAL( lcm<T>( -1,  -1), static_cast<T>( 1) );
    BOOST_CHECK_EQUAL( lcm<T>(  0,   0), static_cast<T>( 0) );
    BOOST_CHECK_EQUAL( lcm<T>(  6,   0), static_cast<T>( 0) );
    BOOST_CHECK_EQUAL( lcm<T>(  0,   7), static_cast<T>( 0) );
    BOOST_CHECK_EQUAL( lcm<T>( -5,   0), static_cast<T>( 0) );
    BOOST_CHECK_EQUAL( lcm<T>(  0,  -4), static_cast<T>( 0) );
    BOOST_CHECK_EQUAL( lcm<T>( 18,  30), static_cast<T>(90) );
    BOOST_CHECK_EQUAL( lcm<T>( -6,   9), static_cast<T>(18) );
    BOOST_CHECK_EQUAL( lcm<T>(-10, -10), static_cast<T>(10) );
    BOOST_CHECK_EQUAL( lcm<T>( 25, -10), static_cast<T>(50) );
    BOOST_CHECK_EQUAL( lcm<T>(  3,   7), static_cast<T>(21) );
    BOOST_CHECK_EQUAL( lcm<T>(  8,   9), static_cast<T>(72) );
    BOOST_CHECK_EQUAL( lcm<T>(  7,  49), static_cast<T>(49) );
}

// LCM on unmarked signed integer type
BOOST_AUTO_TEST_CASE( lcm_unmarked_int_test )
{
    using ural_ex::lcm;

    // The regular signed-integer LCM function performs the unsigned version,
    // then does an absolute-value on the result.  Signed types that are not
    // marked as such (due to no std::numeric_limits specialization) may be off
    // by a sign.
    BOOST_CHECK_EQUAL( abs(lcm<MyInt2>(   1,  -1 )), MyInt2( 1) );
    BOOST_CHECK_EQUAL( abs(lcm<MyInt2>(  -1,   1 )), MyInt2( 1) );
    BOOST_CHECK_EQUAL( abs(lcm<MyInt2>(   1,   1 )), MyInt2( 1) );
    BOOST_CHECK_EQUAL( abs(lcm<MyInt2>(  -1,  -1 )), MyInt2( 1) );
    BOOST_CHECK_EQUAL( abs(lcm<MyInt2>(   0,   0 )), MyInt2( 0) );
    BOOST_CHECK_EQUAL( abs(lcm<MyInt2>(   6,   0 )), MyInt2( 0) );
    BOOST_CHECK_EQUAL( abs(lcm<MyInt2>(   0,   7 )), MyInt2( 0) );
    BOOST_CHECK_EQUAL( abs(lcm<MyInt2>(  -5,   0 )), MyInt2( 0) );
    BOOST_CHECK_EQUAL( abs(lcm<MyInt2>(   0,  -4 )), MyInt2( 0) );
    BOOST_CHECK_EQUAL( abs(lcm<MyInt2>(  18,  30 )), MyInt2(90) );
    BOOST_CHECK_EQUAL( abs(lcm<MyInt2>(  -6,   9 )), MyInt2(18) );
    BOOST_CHECK_EQUAL( abs(lcm<MyInt2>( -10, -10 )), MyInt2(10) );
    BOOST_CHECK_EQUAL( abs(lcm<MyInt2>(  25, -10 )), MyInt2(50) );
    BOOST_CHECK_EQUAL( abs(lcm<MyInt2>(   3,   7 )), MyInt2(21) );
    BOOST_CHECK_EQUAL( abs(lcm<MyInt2>(   8,   9 )), MyInt2(72) );
    BOOST_CHECK_EQUAL( abs(lcm<MyInt2>(   7,  49 )), MyInt2(49) );
}

// LCM on unsigned integer types
BOOST_AUTO_TEST_CASE_TEMPLATE( lcm_unsigned_test, T, unsigned_test_types )
{
    using ural_ex::lcm;

    // Note that unmarked types (i.e. have no std::numeric_limits
    // specialization) are treated like non/unsigned types
    BOOST_CHECK_EQUAL( lcm<T>( 1u,   1u), static_cast<T>( 1u) );
    BOOST_CHECK_EQUAL( lcm<T>( 0u,   0u), static_cast<T>( 0u) );
    BOOST_CHECK_EQUAL( lcm<T>( 6u,   0u), static_cast<T>( 0u) );
    BOOST_CHECK_EQUAL( lcm<T>( 0u,   7u), static_cast<T>( 0u) );
    BOOST_CHECK_EQUAL( lcm<T>(18u,  30u), static_cast<T>(90u) );
    BOOST_CHECK_EQUAL( lcm<T>( 3u,   7u), static_cast<T>(21u) );
    BOOST_CHECK_EQUAL( lcm<T>( 8u,   9u), static_cast<T>(72u) );
    BOOST_CHECK_EQUAL( lcm<T>( 7u,  49u), static_cast<T>(49u) );
}

// LCM at compile-time
BOOST_AUTO_TEST_CASE( lcm_static_test )
{
    using ural_ex::static_lcm;

    // Can't use "BOOST_CHECK_EQUAL", otherwise the "value" member will be
    // disqualified as compile-time-only constant, needing explicit definition
    BOOST_CHECK( (static_lcm< 1,  1>::value) ==  1 );
    BOOST_CHECK( (static_lcm< 0,  0>::value) ==  0 );
    BOOST_CHECK( (static_lcm< 6,  0>::value) ==  0 );
    BOOST_CHECK( (static_lcm< 0,  7>::value) ==  0 );
    BOOST_CHECK( (static_lcm<18, 30>::value) == 90 );
    BOOST_CHECK( (static_lcm< 3,  7>::value) == 21 );
    BOOST_CHECK( (static_lcm< 8,  9>::value) == 72 );
    BOOST_CHECK( (static_lcm< 7, 49>::value) == 49 );
}

// TODO: see GCD to-do

BOOST_AUTO_TEST_SUITE_END()

#include <ural/math.hpp>

BOOST_AUTO_TEST_CASE(natural_power_constexpr_test)
{
    static_assert(ural::natural_power(2, 0) == 1, "");
    static_assert(ural::natural_power(2, 1) == 2, "");
    static_assert(ural::natural_power(2, 2) == 4, "");
    static_assert(ural::natural_power(2, 3) == 8, "");

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(natural_power_constexpr_with_plus_explicit_unit)
{
    static_assert(ural::natural_power(3, 0, ural::plus<>{}, 0) == 0, "");
    static_assert(ural::natural_power(3, 1, ural::plus<>{}, 0) == 3, "");
    static_assert(ural::natural_power(3, 2, ural::plus<>{}, 0) == 6, "");
    static_assert(ural::natural_power(3, 3, ural::plus<>{}, 0) == 9, "");
    static_assert(ural::natural_power(3, 4, ural::plus<>{}, 0) == 12, "");
    static_assert(ural::natural_power(3, 5, ural::plus<>{}, 0) == 15, "");

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(natural_power_constexpr_with_plus_test)
{
    static_assert(ural::natural_power(3, 0, ural::plus<>{}) == 0, "");
    static_assert(ural::natural_power(3, 1, ural::plus<>{}) == 3, "");
    static_assert(ural::natural_power(3, 2, ural::plus<>{}) == 6, "");
    static_assert(ural::natural_power(3, 3, ural::plus<>{}) == 9, "");
    static_assert(ural::natural_power(3, 4, ural::plus<>{}) == 12, "");
    static_assert(ural::natural_power(3, 5, ural::plus<>{}) == 15, "");

    BOOST_CHECK(true);
}

#include <ural/algorithm.hpp>
#include <ural/math/continued_fraction.hpp>
#include <ural/sequence/insertion.hpp>

BOOST_AUTO_TEST_CASE(square_root_23_as_continued_fraction_back_inserter)
{
    auto const N = 23;

    std::vector<int> a;

    std::vector<int> const a_expected = {4, 1, 3, 1, 8};

    ural::copy(ural_ex::sqrt_as_continued_fraction(N), a | ural::back_inserter);

    URAL_CHECK_EQUAL_RANGES(a, a_expected);
}

BOOST_AUTO_TEST_CASE(square_root_16_as_continued_fraction_back_inserter)
{
    auto const N = 16;

    std::vector<int> a;

    std::vector<int> const a_expected = {4};

    ural::copy(ural_ex::sqrt_as_continued_fraction(N), a | ural::back_inserter);

    URAL_CHECK_EQUAL_RANGES(a, a_expected);
}

BOOST_AUTO_TEST_CASE(square_root_23_as_continued_fraction)
{
    auto const N = 23;

    std::vector<int> const a_expected = {4, 1, 3, 1, 8};

    std::vector<int> a(a_expected.size(), 0);

    ural::copy(ural_ex::sqrt_as_continued_fraction(N), a);

    URAL_CHECK_EQUAL_RANGES(a, a_expected);
}

#include <ural/math/primes.hpp>
BOOST_AUTO_TEST_CASE(nth_prime_test_PE_7)
{
    typedef long Integer;

    auto const N = 10001U;

    auto const primes = ural_ex::make_first_n_primes<Integer>(N);

    BOOST_CHECK_EQUAL(primes.size(), N);
    BOOST_CHECK_EQUAL(primes[6-1], 13);
    BOOST_CHECK_EQUAL(primes.back(), 104743);
}

BOOST_AUTO_TEST_CASE(primes_below_PE_10)
{
    typedef long long Integer;

    auto const primes_10 = ural_ex::make_primes_below(Integer{10});

    BOOST_CHECK_EQUAL(ural::accumulate(primes_10, Integer{0}), 17);

    auto const primes_2M = ural_ex::make_primes_below(Integer{2000000});

    BOOST_CHECK_EQUAL(ural::accumulate(primes_2M, Integer{0}), 142913828922);
}

BOOST_AUTO_TEST_CASE(is_prime_test_PE_58)
{
    typedef long long Integer;

    Integer n_max_old = 1;

    size_t primes_on_diagonal = 0;

    auto lenght = Integer{1};

    for(auto h = Integer{1};; ++ h)
    {
        lenght = 2 * h + 1;
        auto const step   = 2 * h;
        auto const n_max = ural::square(lenght);

        auto const diagonals = 1 + 4 * h;

        // Проверяем диагонали
        for(size_t k = 1; k <= 4; ++ k)
        {
            primes_on_diagonal += ural_ex::is_prime(n_max_old + k * step);
        }

        auto const r = double(primes_on_diagonal) / diagonals;

        if(r < 0.1)
        {
            break;
        }

        // Переход к следующей итерации
        n_max_old = n_max;
    }

    BOOST_CHECK_EQUAL(26241, lenght);
}

BOOST_AUTO_TEST_CASE(is_coprime_with_sequence_test)
{
    typedef int Integer;

    Integer const x = 8;

    std::forward_list<Integer> const v_true = {15, 49};

    std::forward_list<Integer> const v_false_1 = {6, 9};
    std::forward_list<Integer> const v_false_2 = {9, 6};
    std::forward_list<Integer> const v_empty {};

    BOOST_CHECK(ural_ex::is_coprime_with_all(x, v_true));
    BOOST_CHECK(ural_ex::is_coprime_with_all(x, v_empty));

    BOOST_CHECK(ural_ex::is_coprime_with_all(x, v_false_1) == false);
    BOOST_CHECK(ural_ex::is_coprime_with_all(x, v_false_2) == false);
}

// Функциональный объект для модуля
BOOST_AUTO_TEST_CASE(abs_fn_test)
{
    auto constexpr abs_f = ural::abs();

    BOOST_CHECK_EQUAL(abs_f(5), 5);
    BOOST_CHECK_EQUAL(abs_f(-5), 5);
    BOOST_CHECK_EQUAL(abs_f(4.5), 4.5);
    BOOST_CHECK_EQUAL(abs_f(-4.5), 4.5);

    auto constexpr r = ural_ex::rational<int>{18, 12};

    BOOST_CHECK(r >= 0);
    BOOST_CHECK_EQUAL(abs_f(r), r);
    BOOST_CHECK_EQUAL(abs_f(-r), r);

    auto const z = std::complex<double>(0.6, 0.8);

    BOOST_CHECK_CLOSE(abs_f(z), 1.0, 1e-6);
    BOOST_CHECK_CLOSE(abs_f(conj(z)), 1.0, 1e-6);
    BOOST_CHECK_CLOSE(abs_f(-z), 1.0, 1e-6);
    BOOST_CHECK_CLOSE(abs_f(-conj(z)), 1.0, 1e-6);
}

// Функциональный объект для модуля с поддержкой constexpr
BOOST_AUTO_TEST_CASE(abs_constexpr_fn_test)
{
    auto constexpr abs_f = ural::abs_constexpr();

    URAL_STATIC_ASSERT_EQUAL(abs_f(5), 5);
    URAL_STATIC_ASSERT_EQUAL(abs_f(-5), 5);
    URAL_STATIC_ASSERT_EQUAL(abs_f(4.5), 4.5);
    URAL_STATIC_ASSERT_EQUAL(abs_f(-4.5), 4.5);

    auto constexpr r = ural_ex::rational<int>{18, 12};

    static_assert(r >= 0, "");
    URAL_STATIC_ASSERT_EQUAL(abs_f(r), r);
    URAL_STATIC_ASSERT_EQUAL(abs_f(-r), r);
}

// Треугольник Паскаля
#include <ural/numeric.hpp>
#include <set>
#include <ural/container/flat_set.hpp>

namespace
{
    typedef boost::mpl::list<std::set<int>, ural_ex::flat_set<int>> PE_203_Set_types;
}

BOOST_AUTO_TEST_CASE_TEMPLATE(pascal_triangle_PE_203, Set, PE_203_Set_types)
{
    Set const z = {1, 2, 3, 4, 5, 6, 7, 10, 15, 20, 21, 35};

    Set r;

    auto seq = ural_ex::pascal_triangle_rows_cursor<std::vector<int>>{};

    for(auto i : ural::numbers(0, 8))
    {
        auto const & row = *seq;

        BOOST_CHECK_EQUAL(row.size(), ural::to_unsigned(i+1));

        ural::copy(row, r | ural_ex::set_inserter);

        BOOST_CHECK_EQUAL_COLLECTIONS(row.begin(), row.end(),
                                      row.rbegin(), row.rend());

        ++ seq;
    }

    URAL_CHECK_EQUAL_RANGES(r, z);
}

BOOST_AUTO_TEST_CASE(is_even_and_is_odd_test)
{
    static_assert(ural::is_even(0) == true, "");
    static_assert(ural::is_odd(0) == false, "");

    static_assert(ural::is_even(2) == true, "");
    static_assert(ural::is_odd(2) == false, "");

    static_assert(ural::is_even(3) == false, "");
    static_assert(ural::is_odd(3) == true, "");

    static_assert(ural::is_even(-2) == true, "");
    static_assert(ural::is_odd(-2) == false, "");

    static_assert(ural::is_even(-3) == false, "");
    static_assert(ural::is_odd(-3) == true, "");

    BOOST_CHECK(true);
}

// Последовательность чисел Фибоначчи
#include <ural/math/fibonacci.hpp>

BOOST_AUTO_TEST_CASE(fibonacci_sequence_custom_init_values)
{
    constexpr auto const x1 = 2;
    constexpr auto const x2 = 3;

    using FS = decltype(ural_ex::make_fibonacci_cursor(x1, x2));

    BOOST_CONCEPT_ASSERT((ural::concepts::ReadableCursor<FS>));
    BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassCursor<FS>));

    constexpr auto const seq = ural_ex::make_fibonacci_cursor(x1, x2);

    static_assert(*seq == x1, "");

    BOOST_CHECK_EQUAL(*seq, x1);
    BOOST_CHECK_EQUAL(*ural::next(seq), x2);
    BOOST_CHECK_EQUAL(*ural::next(seq, 2), x1 + x2);
}

BOOST_AUTO_TEST_CASE(fibonacci_sequence_custom_init_values_and_operations)
{
    constexpr auto const x1 = 2;
    constexpr auto const x2 = 3;
    constexpr auto const op = ural::multiplies<decltype(x1)>{};

    constexpr auto const seq = ural_ex::make_fibonacci_cursor(x1, x2, op);

    static_assert(sizeof(seq) == 2 * sizeof(x1), "");

    BOOST_CHECK(op == seq.operation());

    static_assert(op == seq.operation(), "");

    static_assert(seq.front() == x1, "");
    static_assert(*seq == x1, "");

    BOOST_CHECK_EQUAL(*seq, x1);
    BOOST_CHECK_EQUAL(*ural::next(seq), x2);
    BOOST_CHECK_EQUAL(*ural::next(seq, 2), x1 * x2);
}

BOOST_AUTO_TEST_CASE(fibonacci_sequence_explicit_single_pass)
{
    using Integer = int;

    using FS = ural_ex::fibonacci_cursor<Integer, ural::use_default,
                                         ural::single_pass_cursor_tag>;

    BOOST_CONCEPT_ASSERT((ural::concepts::ReadableCursor<FS>));
    BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassCursor<FS>));
}

BOOST_AUTO_TEST_CASE(fibonacci_sequence_explicit_forward)
{
    using Integer = int;

    using FS = ural_ex::fibonacci_cursor<Integer, ural::use_default,
                                           ural::forward_cursor_tag>;

    BOOST_CONCEPT_ASSERT((ural::concepts::ReadableCursor<FS>));
    BOOST_CONCEPT_ASSERT((ural::concepts::ForwardCursor<FS>));
}

BOOST_AUTO_TEST_CASE(fibonacci_sequence_with_operation_single_pass_traversal)
{
    using Integer = int;

    using Operation = Integer(*)(Integer, Integer);

    using FS = ural_ex::fibonacci_cursor<Integer, Operation>;

    BOOST_CONCEPT_ASSERT((ural::concepts::ReadableCursor<FS>));
    BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassCursor<FS>));
}

BOOST_AUTO_TEST_CASE(fibonacci_sequence_forward_traversal)
{
    using Integer = int;

    using Operation = Integer(*)(Integer, Integer);

    using FS = ural_ex::fibonacci_cursor<Integer, Operation, ural::forward_cursor_tag>;

    BOOST_CONCEPT_ASSERT((ural::concepts::ReadableCursor<FS>));
    BOOST_CONCEPT_ASSERT((ural::concepts::ForwardCursor<FS>));

    auto const op1 = +[](Integer x, Integer y) { return x + y; };
    auto const op2 = +[](Integer x, Integer y) { return x * y; };

    FS s1(1, 1, op1);
    FS s2(1, 2, op1);
    FS s3(1, 1, op2);

    BOOST_CHECK(s1 == s1);
    BOOST_CHECK(s1 != s2);
    BOOST_CHECK(s1 != s3);

    BOOST_CHECK(s2 == s2);
    BOOST_CHECK(s2 != s3);

    BOOST_CHECK(s3 == s3);
}

BOOST_AUTO_TEST_CASE(fibonacci_cursor_shrink_front)
{
    auto seq = ural_ex::fibonacci_cursor<int, ural::use_default,
                                         ural::forward_cursor_tag>{};

    ural::advance(seq, 3);

    BOOST_CHECK(seq != seq.original());

    seq.shrink_front();
    BOOST_CHECK(seq == seq.original());
}
