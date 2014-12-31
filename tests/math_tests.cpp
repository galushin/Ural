// Основано на
// http://www.boost.org/doc/libs/1_55_0/libs/math/test/common_factor_test.cpp

#include <boost/test/unit_test.hpp>

// TODO: add polynominal/non-real type; especially after any switch to the
// binary-GCD algorithm for built-in types

#include <boost/mpl/list.hpp>
#include <boost/operators.hpp>

#include <ural/math/common_factor.hpp>

typedef ::boost::mpl::list<signed char, short, int, long>
    signed_test_types;

typedef ::boost::mpl::list<unsigned char, unsigned short, unsigned>
    unsigned_test_types;

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

BOOST_AUTO_TEST_SUITE( common_factor_test_suite )

BOOST_AUTO_TEST_CASE_TEMPLATE( gcd_int_test, T, signed_test_types )
{
    using ural::gcd;

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
    using ural::gcd;

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
    using ural::gcd;

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
    using ural::static_gcd;

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
    using ural::lcm;

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
    using ural::lcm;

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
    using ural::lcm;

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
    using ural::static_lcm;

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
