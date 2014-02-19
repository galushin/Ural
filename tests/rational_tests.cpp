// Основано на boost_1_54_0\libs\rational\test\rational_test.cpp
#include <ostream>

#include <boost/test/unit_test.hpp>
#include <boost/mpl/list.hpp>
#include <boost/operators.hpp>

#include <ural/math/rational.hpp>

namespace {

class MyOverflowingUnsigned;

// This is a trivial user-defined wrapper around the built in int type.
// It can be used as a test type for rational<>
class MyInt : boost::operators<MyInt>
{
    friend class MyOverflowingUnsigned;
    int val;
public:
    constexpr MyInt(int n = 0) : val(n) {}
    friend MyInt operator+ (const MyInt&);
    friend MyInt operator- (const MyInt&);
    MyInt& operator+= (const MyInt& rhs) { val += rhs.val; return *this; }
    MyInt& operator-= (const MyInt& rhs) { val -= rhs.val; return *this; }
    MyInt& operator*= (const MyInt& rhs) { val *= rhs.val; return *this; }
    MyInt& operator/= (const MyInt& rhs) { val /= rhs.val; return *this; }
    MyInt& operator%= (const MyInt& rhs) { val %= rhs.val; return *this; }
    MyInt& operator|= (const MyInt& rhs) { val |= rhs.val; return *this; }
    MyInt& operator&= (const MyInt& rhs) { val &= rhs.val; return *this; }
    MyInt& operator^= (const MyInt& rhs) { val ^= rhs.val; return *this; }
    const MyInt& operator++() { ++val; return *this; }
    const MyInt& operator--() { --val; return *this; }
    bool operator< (const MyInt& rhs) const { return val < rhs.val; }
    bool operator== (const MyInt& rhs) const { return val == rhs.val; }
    bool operator! () const { return !val; }
    friend std::istream& operator>>(std::istream&, MyInt&);
    friend std::ostream& operator<<(std::ostream&, const MyInt&);
};

inline MyInt operator+(const MyInt& rhs) { return rhs; }
inline MyInt operator-(const MyInt& rhs) { return MyInt(-rhs.val); }
inline std::istream& operator>>(std::istream& is, MyInt& i) { is >> i.val; return is; }
inline std::ostream& operator<<(std::ostream& os, const MyInt& i) { os << i.val; return os; }
inline MyInt abs(MyInt rhs) { if (rhs < MyInt()) rhs = -rhs; return rhs; }

// This is an "unsigned" wrapper, that throws on overflow.  It can be used to
// test rational<> when an operation goes out of bounds.
class MyOverflowingUnsigned
    : private boost::unit_steppable<MyOverflowingUnsigned>
    , private boost::ordered_euclidian_ring_operators1<MyOverflowingUnsigned>
{
    // Helper type-aliases
    typedef MyOverflowingUnsigned  self_type;
    typedef unsigned self_type::*  bool_type;

    // Member data
    unsigned  v_;

public:
    // Exception base class
    class exception_base  { protected: virtual ~exception_base() throw() {} };

    // Divide-by-zero exception class
    class divide_by_0_error
        : public virtual exception_base
        , public         std::domain_error
    {
    public:
        explicit  divide_by_0_error( std::string const &w )
          : std::domain_error( w )  {}

        virtual  ~divide_by_0_error() throw()  {}
    };

    // Overflow exception class
    class overflowing_error
        : public virtual exception_base
        , public         std::overflow_error
    {
    public:
        explicit  overflowing_error( std::string const &w )
          : std::overflow_error( w )  {}

        virtual  ~overflowing_error() throw()  {}
    };

    // Lifetime management (use automatic dtr & copy-ctr)
              MyOverflowingUnsigned( unsigned v = 0 )  : v_( v )  {}
    explicit  MyOverflowingUnsigned( MyInt const &m )  : v_( m.val )  {}

    // Operators (use automatic copy-assignment); arithmetic & comparison only
    self_type &  operator ++()
    {
        if ( this->v_ == UINT_MAX )  throw overflowing_error( "increment" );
        else ++this->v_;
        return *this;
    }
    self_type &  operator --()
    {
        if ( !this->v_ )  throw overflowing_error( "decrement" );
        else --this->v_;
        return *this;
    }

    operator bool_type() const  { return this->v_ ? &self_type::v_ : 0; }

    bool       operator !() const  { return !this->v_; }
    self_type  operator +() const  { return self_type( +this->v_ ); }
    self_type  operator -() const  { return self_type( -this->v_ ); }

    bool  operator  <(self_type const &r) const  { return this->v_ <  r.v_; }
    bool  operator ==(self_type const &r) const  { return this->v_ == r.v_; }

    self_type &  operator *=( self_type const &r )
    {
        if ( r.v_ && this->v_ > UINT_MAX / r.v_ )
        {
            throw overflowing_error( "oversized factors" );
        }
        this->v_ *= r.v_;
        return *this;
    }
    self_type &  operator /=( self_type const &r )
    {
        if ( !r.v_ )  throw divide_by_0_error( "division" );
        this->v_ /= r.v_;
        return *this;
    }
    self_type &  operator %=( self_type const &r )
    {
        if ( !r.v_ )  throw divide_by_0_error( "modulus" );
        this->v_ %= r.v_;
        return *this;
    }
    self_type &  operator +=( self_type const &r )
    {
        if ( this->v_ > UINT_MAX - r.v_ )
        {
            throw overflowing_error( "oversized addends" );
        }
        this->v_ += r.v_;
        return *this;
    }
    self_type &  operator -=( self_type const &r )
    {
        if ( this->v_ < r.v_ )
        {
            throw overflowing_error( "oversized subtrahend" );
        }
        this->v_ -= r.v_;
        return *this;
    }

    // Input & output
    template < typename Ch, class Tr >
    friend  std::basic_istream<Ch, Tr> &
    operator >>( std::basic_istream<Ch, Tr> &i, self_type &x )
    { return i >> x.v_; }

    template < typename Ch, class Tr >
    friend  std::basic_ostream<Ch, Tr> &
    operator <<( std::basic_ostream<Ch, Tr> &o, self_type const &x )
    { return o << x.v_; }

};  // MyOverflowingUnsigned

inline MyOverflowingUnsigned abs( MyOverflowingUnsigned const &x ) { return x; }

} // namespace


// Specialize numeric_limits for the custom types
namespace std
{

template < >
class numeric_limits< MyInt >
{
    typedef numeric_limits<int>  limits_type;

public:
    static const bool is_specialized = limits_type::is_specialized;

    static MyInt min BOOST_PREVENT_MACRO_SUBSTITUTION () throw()  { return (limits_type::min)(); }
    static MyInt max BOOST_PREVENT_MACRO_SUBSTITUTION () throw()  { return (limits_type::max)(); }

    static const int digits      = limits_type::digits;
    static const int digits10    = limits_type::digits10;
    static const bool is_signed  = limits_type::is_signed;
    static const bool is_integer = limits_type::is_integer;
    static const bool is_exact   = limits_type::is_exact;
    static const int radix       = limits_type::radix;
    static MyInt epsilon() throw()      { return limits_type::epsilon(); }
    static MyInt round_error() throw()  { return limits_type::round_error(); }

    static const int min_exponent   = limits_type::min_exponent;
    static const int min_exponent10 = limits_type::min_exponent10;
    static const int max_exponent   = limits_type::max_exponent;
    static const int max_exponent10 = limits_type::max_exponent10;

    static const bool has_infinity             = limits_type::has_infinity;
    static const bool has_quiet_NaN            = limits_type::has_quiet_NaN;
    static const bool has_signaling_NaN        = limits_type::has_signaling_NaN;
    static const float_denorm_style has_denorm = limits_type::has_denorm;
    static const bool has_denorm_loss          = limits_type::has_denorm_loss;

    static MyInt infinity() throw()      { return limits_type::infinity(); }
    static MyInt quiet_NaN() throw()     { return limits_type::quiet_NaN(); }
    static MyInt signaling_NaN() throw() {return limits_type::signaling_NaN();}
    static MyInt denorm_min() throw()    { return limits_type::denorm_min(); }

    static const bool is_iec559  = limits_type::is_iec559;
    static const bool is_bounded = limits_type::is_bounded;
    static const bool is_modulo  = limits_type::is_modulo;

    static const bool traps                    = limits_type::traps;
    static const bool tinyness_before          = limits_type::tinyness_before;
    static const float_round_style round_style = limits_type::round_style;

};  // std::numeric_limits<MyInt>

template < >
class numeric_limits< MyOverflowingUnsigned >
{
    typedef numeric_limits<unsigned>  limits_type;

public:
    static const bool is_specialized = limits_type::is_specialized;

    static MyOverflowingUnsigned min BOOST_PREVENT_MACRO_SUBSTITUTION () throw()  { return (limits_type::min)(); }
    static MyOverflowingUnsigned max BOOST_PREVENT_MACRO_SUBSTITUTION () throw()  { return (limits_type::max)(); }

    static const int digits      = limits_type::digits;
    static const int digits10    = limits_type::digits10;
    static const bool is_signed  = limits_type::is_signed;
    static const bool is_integer = limits_type::is_integer;
    static const bool is_exact   = limits_type::is_exact;
    static const int radix       = limits_type::radix;
    static MyOverflowingUnsigned epsilon() throw()
        { return limits_type::epsilon(); }
    static MyOverflowingUnsigned round_error() throw()
        {return limits_type::round_error();}

    static const int min_exponent   = limits_type::min_exponent;
    static const int min_exponent10 = limits_type::min_exponent10;
    static const int max_exponent   = limits_type::max_exponent;
    static const int max_exponent10 = limits_type::max_exponent10;

    static const bool has_infinity             = limits_type::has_infinity;
    static const bool has_quiet_NaN            = limits_type::has_quiet_NaN;
    static const bool has_signaling_NaN        = limits_type::has_signaling_NaN;
    static const float_denorm_style has_denorm = limits_type::has_denorm;
    static const bool has_denorm_loss          = limits_type::has_denorm_loss;

    static MyOverflowingUnsigned infinity() throw()
        { return limits_type::infinity(); }
    static MyOverflowingUnsigned quiet_NaN() throw()
        { return limits_type::quiet_NaN(); }
    static MyOverflowingUnsigned signaling_NaN() throw()
        { return limits_type::signaling_NaN(); }
    static MyOverflowingUnsigned denorm_min() throw()
        { return limits_type::denorm_min(); }

    static const bool is_iec559  = limits_type::is_iec559;
    static const bool is_bounded = limits_type::is_bounded;
    static const bool is_modulo  = limits_type::is_modulo;

    static const bool traps                    = limits_type::traps;
    static const bool tinyness_before          = limits_type::tinyness_before;
    static const float_round_style round_style = limits_type::round_style;

};  // std::numeric_limits<MyOverflowingUnsigned>

}  // namespace std

typedef ::boost::mpl::list<short, int, long>     builtin_signed_test_types;
// @todo typedef ::boost::mpl::list<short, int, long, MyInt>  all_signed_test_types;
typedef builtin_signed_test_types all_signed_test_types;

BOOST_AUTO_TEST_CASE_TEMPLATE(rational_size_check, T, all_signed_test_types)
{
    typedef ural::rational<T> Rational;

    static_assert(sizeof(Rational) <= 2*sizeof(T), "too big");
    BOOST_CHECK(true);
}

// The basic test suite
BOOST_AUTO_TEST_SUITE(basic_rational_suite)

BOOST_AUTO_TEST_CASE_TEMPLATE( rational_initialization_test, T,
 all_signed_test_types )
{
    typedef ural::rational<T> Rational;

    constexpr auto r1 = Rational{};
    constexpr auto r2 = Rational( 0 );
    constexpr auto r3 = Rational( 1 );
    constexpr auto r4 = Rational( -3 );
    constexpr auto r5 = Rational( 7, 2 );
    constexpr auto r6 = Rational( 5, 15 );
    constexpr auto r7 = Rational( 14, -21 );
    constexpr auto r8 = Rational( -4, 6 );
    constexpr auto r9 = Rational( -14, -70 );

    static_assert(r1.numerator() == static_cast<T>( 0), "");
    static_assert( r2.numerator() == static_cast<T>( 0), "" );
    static_assert( r3.numerator() ==  static_cast<T>( 1), "" );
    static_assert( r4.numerator() == static_cast<T>(-3), "" );
    static_assert( r5.numerator() == static_cast<T>( 7), "" );
    static_assert( r6.numerator() == static_cast<T>( 1), "" );
    static_assert( r7.numerator() == static_cast<T>(-2), "" );
    static_assert( r8.numerator() == static_cast<T>(-2), "" );
    static_assert( r9.numerator() == static_cast<T>( 1), "" );

    static_assert( r1.denominator() == static_cast<T>(1), "" );
    static_assert( r2.denominator() == static_cast<T>(1), "" );
    static_assert( r3.denominator() == static_cast<T>(1), "" );
    static_assert( r4.denominator() == static_cast<T>(1), "" );
    static_assert( r5.denominator() == static_cast<T>(2), "" );
    static_assert( r6.denominator() == static_cast<T>(3), "" );
    static_assert( r7.denominator() == static_cast<T>(3), "" );
    static_assert( r8.denominator() == static_cast<T>(3), "" );
    static_assert( r9.denominator() == static_cast<T>(5), "" );

    BOOST_CHECK_THROW( ural::rational<T>( 3, 0), ural::bad_rational );
    BOOST_CHECK_THROW( ural::rational<T>(-2, 0), ural::bad_rational );
    BOOST_CHECK_THROW( ural::rational<T>( 0, 0), ural::bad_rational );
}

BOOST_AUTO_TEST_CASE_TEMPLATE( rational_assign_test, T, all_signed_test_types )
{
    typedef ural::rational<T> Rational;
    auto r = Rational{};

    r.assign( 6, 8 );
    BOOST_CHECK_EQUAL( r.numerator(),   static_cast<T>(3) );
    BOOST_CHECK_EQUAL( r.denominator(), static_cast<T>(4) );

    r.assign( 0, -7 );
    BOOST_CHECK_EQUAL( r.numerator(),   static_cast<T>(0) );
    BOOST_CHECK_EQUAL( r.denominator(), static_cast<T>(1) );

    BOOST_CHECK_THROW( r.assign( 4, 0), ural::bad_rational );
    BOOST_CHECK_THROW( r.assign( 0, 0), ural::bad_rational );
    BOOST_CHECK_THROW( r.assign(-7, 0), ural::bad_rational );
}

BOOST_AUTO_TEST_CASE_TEMPLATE( rational_comparison_test, T,
 all_signed_test_types )
{
    typedef ural::rational<T> Rational;

    constexpr auto r1 = Rational{};
    constexpr auto r2 = Rational( 0 );
    constexpr auto r3 = Rational( 1 );
    constexpr auto r4 = Rational( -3 );
    constexpr auto r5 = Rational( 7, 2 );
    constexpr auto r6 = Rational( 5, 15 );
    constexpr auto r7 = Rational( 14, -21 );
    constexpr auto r8 = Rational( -4, 6 );
    constexpr auto r9 = Rational( -14, -70 );

    static_assert( r1 == r2, "");
    static_assert( r2 != r3, "" );
    static_assert( r4 <  r3, "" );
    static_assert( r4 <= r5, "" );
    static_assert( r1 <= r2, "" );
    static_assert( r5 >  r6, "" );
    static_assert( r5 >= r6, "" );
    static_assert( r7 >= r8, "" );

    static_assert( !(r3 == r2), "" );
    static_assert( !(r1 != r2), "" );
    static_assert( !(r1 <  r2), "" );
    static_assert( !(r5 <  r6), "" );
    static_assert( !(r9 <= r2), "" );
    static_assert( !(r8 >  r7), "" );
    static_assert( !(r8 >  r2), "" );
    static_assert( !(r4 >= r6), "" );

    static_assert( r1 == static_cast<T>( 0), "" );
    static_assert( r2 != static_cast<T>(-1), "" );
    static_assert( r3 <  static_cast<T>( 2), "" );
    static_assert( r4 <= static_cast<T>(-3), "" );
    static_assert( r5 >  static_cast<T>( 3), "" );
    static_assert( r6 >= static_cast<T>( 0), "" );

    static_assert( static_cast<T>( 0) == r2, "" );
    static_assert( static_cast<T>( 0) != r7, "" );
    static_assert( static_cast<T>(-1) <  r8, "" );
    static_assert( static_cast<T>(-2) <= r9, "" );
    static_assert( static_cast<T>( 1) >  r1, "" );
    static_assert( static_cast<T>( 1) >= r3, "" );

    // Extra tests with values close in continued-fraction notation
    Rational constexpr  x1( static_cast<T>(9), static_cast<T>(4) );
    Rational constexpr  x2( static_cast<T>(61), static_cast<T>(27) );
    Rational constexpr  x3( static_cast<T>(52), static_cast<T>(23) );
    Rational constexpr  x4( static_cast<T>(70), static_cast<T>(31) );

    static_assert( x1 < x2, "" );
    static_assert( !(x1 < x1), "" );
    static_assert( !(x2 < x2), "" );
    static_assert( !(x2 < x1), "" );
    static_assert( x2 < x3, "" );
    static_assert( x4 < x2, "" );
    static_assert( !(x3 < x4), "" );
    static_assert( r7 < x1, "" );     // not actually close; wanted -ve v. +ve instead
    static_assert( !(x2 < r7), "" );
}

BOOST_AUTO_TEST_CASE_TEMPLATE( rational_1step_test, T, all_signed_test_types )
{
    typedef ural::rational<T> Rational;

    auto r1 = Rational{};
    auto r2 = Rational( 0 );
    auto r3 = Rational( 1 );
    auto r7 = Rational( 14, -21 );
    auto r8 = Rational( -4, 6 );

    BOOST_CHECK(   r1++ == r2 );
    BOOST_CHECK(   r1   != r2 );
    BOOST_CHECK(   r1   == r3 );
    BOOST_CHECK( --r1   == r2 );
    BOOST_CHECK(   r8-- == r7 );
    BOOST_CHECK(   r8   != r7 );
    BOOST_CHECK( ++r8   == r7 );
}

// Absolute value tests
BOOST_AUTO_TEST_CASE_TEMPLATE( rational_abs_test, T, all_signed_test_types )
{
    typedef ural::rational<T> Rational;

    constexpr auto r2 = Rational( 0 );
    constexpr auto r5 = Rational( 7, 2 );
    constexpr auto r8 = Rational( -4, 6 );

    static_assert( abs(r2) == r2, "" );
    static_assert( abs(r5) == r5, "" );
    static_assert( abs(r8) == Rational(2, 3), "");
}

// Unary operator tests
BOOST_AUTO_TEST_CASE_TEMPLATE( rational_unary_test, T, all_signed_test_types )
{
    typedef ural::rational<T> Rational;

    constexpr auto r2 = Rational( 0 );
    constexpr auto r3 = Rational( 1 );
    constexpr auto r4 = Rational( -3 );
    constexpr auto r5 = Rational( 7, 2 );

    static_assert( +r5 == r5, "");

    static_assert( -r3 != r3, "" );
    static_assert( -(-r3) == r3, "" );
    static_assert( -r4 == static_cast<T>(3), "");

    static_assert( !r2, "");
    static_assert( !!r3, "");

    BOOST_CHECK( !static_cast<bool>(r2) );
    BOOST_CHECK( r3 );
}

BOOST_AUTO_TEST_SUITE_END()

// The rational arithmetic operations suite
BOOST_AUTO_TEST_SUITE( rational_arithmetic_suite )

#define URAL_STATIC_ASSERT_EQUAL(E, G) static_assert( ((E) == (G)) , "");

// Addition & subtraction tests
BOOST_AUTO_TEST_CASE_TEMPLATE( rational_additive_test, T,
 all_signed_test_types )
{
    typedef ural::rational<T>  rational_type;

    URAL_STATIC_ASSERT_EQUAL( rational_type( 1, 2) + rational_type(1, 2),
     static_cast<T>(1) );
    URAL_STATIC_ASSERT_EQUAL( rational_type(11, 3) + rational_type(1, 2),
     rational_type( 25,  6) );
    URAL_STATIC_ASSERT_EQUAL( rational_type(-8, 3) + rational_type(1, 5),
     rational_type(-37, 15) );
    URAL_STATIC_ASSERT_EQUAL( rational_type(-7, 6) + rational_type(1, 7),
     rational_type(  1,  7) - rational_type(7, 6) );
    URAL_STATIC_ASSERT_EQUAL( rational_type(13, 5) - rational_type(1, 2),
     rational_type( 21, 10) );
    URAL_STATIC_ASSERT_EQUAL( rational_type(22, 3) + static_cast<T>(1),
     rational_type( 25,  3) );
    URAL_STATIC_ASSERT_EQUAL( rational_type(12, 7) - static_cast<T>(2),
     rational_type( -2,  7) );
    URAL_STATIC_ASSERT_EQUAL(    static_cast<T>(3) + rational_type(4, 5),
     rational_type( 19,  5) );
    URAL_STATIC_ASSERT_EQUAL(    static_cast<T>(4) - rational_type(9, 2),
     rational_type( -1,  2) );

    rational_type  r( 11 );

    r -= rational_type( 20, 3 );
    BOOST_CHECK_EQUAL( r, rational_type(13,  3) );

    r += rational_type( 1, 2 );
    BOOST_CHECK_EQUAL( r, rational_type(29,  6) );

    r -= static_cast<T>( 5 );
    BOOST_CHECK_EQUAL( r, rational_type( 1, -6) );

    r += rational_type( 1, 5 );
    BOOST_CHECK_EQUAL( r, rational_type( 1, 30) );

    r += static_cast<T>( 2 );
    BOOST_CHECK_EQUAL( r, rational_type(61, 30) );
}

BOOST_AUTO_TEST_CASE_TEMPLATE( rational_assignment_test, T,
 all_signed_test_types )
{
    typedef ural::rational<T>  rational_type;

    rational_type  r;

    r = rational_type( 1, 10 );
    BOOST_CHECK_EQUAL( r, rational_type( 1, 10) );

    r = static_cast<T>( -9 );
    BOOST_CHECK_EQUAL( r, rational_type(-9,  1) );
}

// Multiplication tests
BOOST_AUTO_TEST_CASE_TEMPLATE( rational_multiplication_test, T,
 all_signed_test_types )
{
    typedef ural::rational<T>  rational_type;

    URAL_STATIC_ASSERT_EQUAL(rational_type(1, 3) * rational_type(-3, 4),
                             rational_type(-1, 4) );
    URAL_STATIC_ASSERT_EQUAL( rational_type(2, 5) * static_cast<T>(7),
                             rational_type(14, 5) );
    URAL_STATIC_ASSERT_EQUAL(  static_cast<T>(-2) * rational_type(1, 6),
                             rational_type(-1, 3) );

    rational_type  r = rational_type( 3, 7 );

    r *= static_cast<T>( 14 );
    BOOST_CHECK_EQUAL( r, static_cast<T>(6) );

    r *= rational_type( 3, 8 );
    BOOST_CHECK_EQUAL( r, rational_type(9, 4) );
}

BOOST_AUTO_TEST_CASE_TEMPLATE( rational_division_test, T,
 all_signed_test_types )
{
    typedef ural::rational<T>  rational_type;

    URAL_STATIC_ASSERT_EQUAL( rational_type(-1, 20) / rational_type(4, 5),
                             rational_type(-1, 16) );
    URAL_STATIC_ASSERT_EQUAL( rational_type( 5,  6) / static_cast<T>(7),
                             rational_type( 5, 42) );
    URAL_STATIC_ASSERT_EQUAL( static_cast<T>(8) / rational_type(2, 7),
                             static_cast<T>(28) );

    BOOST_CHECK_THROW( rational_type(23, 17) / rational_type(),
     ural::bad_rational );
    BOOST_CHECK_THROW( rational_type( 4, 15) / static_cast<T>(0),
     ural::bad_rational );

    rational_type  r = rational_type( 4, 3 );

    r /= rational_type( 5, 4 );
    BOOST_CHECK_EQUAL( r, rational_type(16, 15) );

    r /= static_cast<T>( 4 );
    BOOST_CHECK_EQUAL( r, rational_type( 4, 15) );

    BOOST_CHECK_THROW( r /= rational_type(), ural::bad_rational );
    BOOST_CHECK_THROW( r /= static_cast<T>(0), ural::bad_rational );

    URAL_STATIC_ASSERT_EQUAL( rational_type(-1) / rational_type(-3),
                             rational_type(1, 3) );
}

// Tests for operations on self
BOOST_AUTO_TEST_CASE_TEMPLATE( rational_self_operations_test, T,
 all_signed_test_types )
{
    typedef ural::rational<T>  rational_type;

    rational_type  r = rational_type( 4, 3 );

    r += r;
    BOOST_CHECK_EQUAL( r, rational_type( 8, 3) );

    r *= r;
    BOOST_CHECK_EQUAL( r, rational_type(64, 9) );

    r /= r;
    BOOST_CHECK_EQUAL( r, rational_type( 1, 1) );

    r -= r;
    BOOST_CHECK_EQUAL( r, rational_type( 0, 1) );

    BOOST_CHECK_THROW( r /= r, ural::bad_rational );
}

BOOST_AUTO_TEST_SUITE_END()

// The non-basic rational operations suite
BOOST_AUTO_TEST_SUITE( rational_extras_suite )

// Output test
BOOST_AUTO_TEST_CASE_TEMPLATE( rational_output_test, T, all_signed_test_types )
{
    std::ostringstream  oss;

    oss << ural::rational<T>( 44, 14 );
    BOOST_CHECK_EQUAL( oss.str(), "22/7" );
}

// Input test, failing
BOOST_AUTO_TEST_CASE_TEMPLATE( rational_input_failing_test, T,
 all_signed_test_types )
{
    std::istringstream  iss( "" );
    ural::rational<T>  r;

    iss >> r;
    BOOST_CHECK( !iss );

    iss.clear();
    iss.str( "42" );
    iss >> r;
    BOOST_CHECK( !iss );

    iss.clear();
    iss.str( "57A" );
    iss >> r;
    BOOST_CHECK( !iss );

    iss.clear();
    iss.str( "20-20" );
    iss >> r;
    BOOST_CHECK( !iss );

    iss.clear();
    iss.str( "1/" );
    iss >> r;
    BOOST_CHECK( !iss );

    iss.clear();
    iss.str( "1/ 2" );
    iss >> r;
    BOOST_CHECK( !iss );

    iss.clear();
    iss.str( "1 /2" );
    iss >> r;
    BOOST_CHECK( !iss );
}

// Input test, passing
BOOST_AUTO_TEST_CASE_TEMPLATE( rational_input_passing_test, T,
 all_signed_test_types )
{
    typedef ural::rational<T>  rational_type;

    std::istringstream  iss( "1/2 12" );
    rational_type       r;
    int                 n = 0;

    BOOST_CHECK( iss >> r >> n );
    BOOST_CHECK_EQUAL( r, rational_type(1, 2) );
    BOOST_CHECK_EQUAL( n, 12 );

    iss.clear();
    iss.str( "34/67" );
    BOOST_CHECK( iss >> r );
    BOOST_CHECK_EQUAL( r, rational_type(34, 67) );

    iss.clear();
    iss.str( "-3/-6" );
    BOOST_CHECK( iss >> r );
    BOOST_CHECK_EQUAL( r, rational_type(1, 2) );
}

// Dice tests (a non-main test)
BOOST_AUTO_TEST_CASE_TEMPLATE( dice_roll_test, T, all_signed_test_types )
{
    typedef ural::rational<T>  rational_type;

    // Determine the mean number of times a fair six-sided die
    // must be thrown until each side has appeared at least once.
    rational_type  r {T( 0 )};

    for ( int  i = 1 ; i <= 6 ; ++i )
    {
        r += rational_type( 1, i );
    }
    r *= static_cast<T>( 6 );

    BOOST_CHECK_EQUAL( r, rational_type(147, 10) );
}

BOOST_AUTO_TEST_SUITE_END()
