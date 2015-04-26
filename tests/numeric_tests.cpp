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

#include <ural/format.hpp>
#include <ural/numeric/matrix.hpp>
#include <ural/algorithm.hpp>
#include <ural/numeric.hpp>

#include <numeric>
#include <forward_list>
#include <cmath>

#include <boost/test/unit_test.hpp>

// @todo тестировать с архетипами

BOOST_AUTO_TEST_CASE(iota_test)
{
    auto const n = 10;
    auto const init_value = -4;
    std::forward_list<int> x1(n);
    std::forward_list<int> x2(n);

    std::iota(x1.begin(), x1.end(), init_value);
    auto const result = ural::iota(x2, init_value);

    BOOST_CHECK_EQUAL_COLLECTIONS(x1.begin(), x1.end(), x2.begin(), x2.end());
    BOOST_CHECK_EQUAL(init_value + n, result);
}

BOOST_AUTO_TEST_CASE(accumulate_test)
{
    std::vector<int> const  v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto const sum_std = std::accumulate(v.begin(), v.end(), 0);
    auto const sum_ural = ural::accumulate(v, 0);

    BOOST_CHECK_EQUAL(sum_std, sum_ural);
}

BOOST_AUTO_TEST_CASE(inner_product_test)
{
    std::vector<int> a{0, 1, 2, 3, 4};
    std::vector<int> b{5, 4, 2, 3, 1};

    auto const r_std = std::inner_product(a.begin(), a.end(), b.begin(), 0);
    auto const r_ural = ural::inner_product(a, b, 0);

    BOOST_CHECK_EQUAL(r_std, r_ural);
}

BOOST_AUTO_TEST_CASE(partial_sums_sequence_test)
{
    // Подготовка
    std::vector<int> const v = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2};

    // std
    std::vector<int> x_std;
    std::partial_sum(v.begin(), v.end(), std::back_inserter(x_std));

    // ural
    std::vector<int> x_ural;
    ural::copy(ural::partial_sums(v), std::back_inserter(x_ural));

    // Проверка
    BOOST_CHECK_EQUAL_COLLECTIONS(x_std.begin(), x_std.end(),
                                  x_ural.begin(), x_ural.end());
}

BOOST_AUTO_TEST_CASE(partial_sum_test)
{
    // Подготовка
    std::vector<int> const v = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2};

    // std
    std::vector<int> x_std;
    std::partial_sum(v.begin(), v.end(), std::back_inserter(x_std));

    // ural
    std::vector<int> x_ural;
    ural::partial_sum(v, std::back_inserter(x_ural));

    // Проверка
    BOOST_CHECK_EQUAL_COLLECTIONS(x_std.begin(), x_std.end(),
                                  x_ural.begin(), x_ural.end());
}

BOOST_AUTO_TEST_CASE(adjacent_differences_sequence_test)
{
    // Подготовка
    std::vector<int> const xs = {1,2,3,5,9,11,12};

    // std
    std::vector<int> r_std;
    std::adjacent_difference(xs.begin(), xs.end(), std::back_inserter(r_std));

    // ural
    std::vector<int> r_ural;
    ural::copy(ural::adjacent_differences(xs), std::back_inserter(r_ural));

    // Проверка
    BOOST_CHECK_EQUAL_COLLECTIONS(r_std.begin(), r_std.end(),
                                  r_ural.begin(), r_ural.end());
}

BOOST_AUTO_TEST_CASE(adjacent_difference_test)
{
    // Подготовка
    std::vector<int> const xs = {1,2,3,5,9,11,12};

    // std
    std::vector<int> r_std;
    std::adjacent_difference(xs.begin(), xs.end(), std::back_inserter(r_std));

    // ural
    std::vector<int> r_ural;
    ural::adjacent_difference(xs, std::back_inserter(r_ural));

    // Проверка
    BOOST_CHECK_EQUAL_COLLECTIONS(r_std.begin(), r_std.end(),
                                  r_ural.begin(), r_ural.end());
}

namespace
{
    template <class T>
    class no_default_ctor
    {
    public:
        no_default_ctor(T value)
         : value_(std::move(value))
        {}

        operator T const &() const
        {
            return this->value_;
        }

    private:
        T value_;
    };
}

BOOST_AUTO_TEST_CASE(partial_sums_sequence_test_no_default_ctor)
{
    typedef no_default_ctor<int> Integer;
    std::vector<Integer> const v = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2};

    std::vector<Integer> x_std;
    std::partial_sum(v.begin(), v.end(), std::back_inserter(x_std));

    std::vector<Integer> x_ural;
    ural::copy(ural::partial_sums(v), std::back_inserter(x_ural));

    BOOST_CHECK_EQUAL_COLLECTIONS(x_std.begin(), x_std.end(),
                                  x_ural.begin(), x_ural.end());
}

BOOST_AUTO_TEST_CASE(adjacent_differences_sequence_test_no_default_ctor)
{
     typedef no_default_ctor<int> Integer;
     std::vector<Integer> const xs = {1,2,3,5,9,11,12};

     std::vector<Integer> r_std;
     std::adjacent_difference(xs.begin(), xs.end(), std::back_inserter(r_std));

     std::vector<Integer> r_ural;
     ural::copy(ural::adjacent_differences(xs), std::back_inserter(r_ural));

     BOOST_CHECK_EQUAL_COLLECTIONS(r_std.begin(), r_std.end(),
                                   r_ural.begin(), r_ural.end());
}

#include <ural/numeric/polynom.hpp>
#include <boost/mpl/list.hpp>

typedef boost::mpl::list<ural::polynomial<int, double>, ural::polynomial<int>>
    Polynom_types;

BOOST_AUTO_TEST_CASE(polynomial_types_test)
{
    typedef ural::polynomial<int, double> Polynom;
    typedef typename Polynom::coefficient_type coefficient_type;
    typedef typename Polynom::argument_type argument_type;
    typedef typename Polynom::result_type result_type;

    static_assert(std::is_same<int, coefficient_type>::value, "");
    static_assert(std::is_same<double, argument_type>::value, "");
    static_assert(std::is_same<double, result_type>::value, "");

    BOOST_CHECK_EQUAL(sizeof(Polynom),
                      sizeof(typename Polynom::coefficients_container));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(polynomial_default_ctor_test, Polynom, Polynom_types)
{
    auto const p0 = Polynom{};

    BOOST_CHECK_EQUAL(0U, p0.degree());
    BOOST_CHECK_EQUAL(0.0, p0(42));
    BOOST_CHECK_EQUAL(0.0, p0[0]);
}

BOOST_AUTO_TEST_CASE(polynomial_init_ctor_test)
{
    typedef ural::polynomial<int, double> Polynom;

    auto const x1 = 1;
    auto const y1 = 1;

    auto const x2 = 2;
    auto const y2 = 3;

    auto const a = (y2 - y1) / (x2 - x1);
    auto const b = y1 - a * x1;

    auto const p = Polynom{a, b};

    BOOST_CHECK_EQUAL(1U, p.degree());
    BOOST_CHECK_EQUAL(a, p[1]);
    BOOST_CHECK_EQUAL(b, p[0]);

    BOOST_CHECK_CLOSE(y1, p(x1), 1e-10);
    BOOST_CHECK_CLOSE(y2, p(x2), 1e-10);

    for(double x = x1; x < x2; x += 0.01)
    {
        BOOST_CHECK_CLOSE(a * x + b, p(x), 1e-10);
    }
}

BOOST_AUTO_TEST_CASE(polynomial_init_test_leading_zeros)
{
    typedef ural::polynomial<int, double> Polynom;
    auto const P = Polynom{0, 1, 2};

    BOOST_CHECK_EQUAL(1U, P.degree());
    BOOST_CHECK_EQUAL(2, P[0]);
    BOOST_CHECK_EQUAL(1, P[1]);
}

BOOST_AUTO_TEST_CASE(polynomial_equality_test)
{
    typedef ural::polynomial<int, double> Polynom;

    auto const p0 = Polynom{};
    auto const p = Polynom{1, 2, 1};

    BOOST_CHECK(p0 == p0);
    BOOST_CHECK(p == p);
    BOOST_CHECK(p0 != p);

    BOOST_CHECK_EQUAL(p0 == p, false);
}

BOOST_AUTO_TEST_CASE(polynomial_add_test)
{
    typedef ural::polynomial<int, double> Polynom;

    auto const p1 = Polynom{1, 1, 1};
    auto const p2 = Polynom{2, 4};

    auto const p = p1 + p2;

    BOOST_CHECK_EQUAL(std::max(p1.degree(), p2.degree()), p.degree());

    BOOST_CHECK_EQUAL(p[0], p1[0] + p2[0]);
    BOOST_CHECK_EQUAL(p[1], p1[1] + p2[1]);
    BOOST_CHECK_EQUAL(p[2], p1[2]);

    auto const p1_new = p - p2;
    auto const p2_new = p - p1;

    BOOST_CHECK(p1 == p1_new);
    BOOST_CHECK(p2 == p2_new);
}

BOOST_AUTO_TEST_CASE(polynomial_add_different_degree_test)
{
    typedef ural::polynomial<int, double> Polynom;

    auto const p1 = Polynom{1, 1};
    auto const p2 = Polynom{1, 2, 4};

    auto const p = p1 + p2;

    BOOST_CHECK_EQUAL(std::max(p1.degree(), p2.degree()), p.degree());

    BOOST_CHECK_EQUAL(p[0], p1[0] + p2[0]);
    BOOST_CHECK_EQUAL(p[1], p1[1] + p2[1]);
    BOOST_CHECK_EQUAL(p[2], p2[2]);

    auto const p1_new = p - p2;
    auto const p2_new = p - p1;

    BOOST_CHECK(p1 == p1_new);
    BOOST_CHECK(p2 == p2_new);

    BOOST_CHECK_EQUAL(p2.degree(), p2_new.degree());
    for(size_t i = 0; i != p2.degree() + 1; ++ i)
    {
        BOOST_CHECK_EQUAL(p2[i], p2_new[i]);
    }
}

BOOST_AUTO_TEST_CASE(polynomial_scalar_mult_test)
{
    typedef ural::polynomial<int, double> Polynom;

    auto const P = Polynom{2, 4};
    auto const a = 5;

    auto const Pa = P*a;
    auto const aP = a*P;

    BOOST_CHECK(aP == Pa);

    BOOST_CHECK_EQUAL(P.degree(), Pa.degree());
    BOOST_CHECK_EQUAL(P.degree(), aP.degree());

    for(double x = - 5; x < 5; x += 0.1)
    {
        BOOST_CHECK_CLOSE(P(x) * a, aP(x), 1e-10);
        BOOST_CHECK_CLOSE(P(x) * a, Pa(x), 1e-10);
    }

    auto const P1 = aP / a;
    auto const P2 = Pa / a;

    BOOST_CHECK(P == P1);
    BOOST_CHECK(P == P2);
}

BOOST_AUTO_TEST_CASE(polynomial_unary_plus_test)
{
    typedef ural::polynomial<int, double> Polynom;

    auto const P = Polynom{2, 4};

    auto const P1 = +P;

    BOOST_CHECK(P1 == P);
}

BOOST_AUTO_TEST_CASE(polynomial_unary_minus_test)
{
    typedef ural::polynomial<int, double> Polynom;

    auto const P = Polynom{2, 4};

    auto const P1 = -P;

    BOOST_CHECK(P1 != P);

    BOOST_CHECK_EQUAL(P1.degree(), P.degree());
    BOOST_CHECK_EQUAL(P1.degree(), 1U);
    BOOST_CHECK_EQUAL(P1[0], -P[0]);
    BOOST_CHECK_EQUAL(P1[1], -P[1]);
}

// Интерполяционный многочлен Ньютона
#include <ural/numeric/interpolation.hpp>
#include <boost/math/constants/constants.hpp>

BOOST_AUTO_TEST_CASE(newton_interpolation_test)
{
    auto f = [](double x) { return std::cos(x); };

    const double x1 = 0.0;
    const double x2 = boost::math::constants::half_pi<double>();
    auto const x_mid = (x1 + x2)/2;

    auto const f1 = f(x1);
    auto const f2 = f(x2);
    auto const f_mid = f(x_mid);

    ural::newton_polynomial<double> P{};

    BOOST_CHECK_EQUAL(0U, P.degree());

    BOOST_CHECK_EQUAL(0.0, P(x1));
    BOOST_CHECK_EQUAL(0.0, P(x2));
    BOOST_CHECK_EQUAL(0.0, P(x_mid));

    P.update(x1, f1);

    BOOST_CHECK_EQUAL(0U, P.degree());

    BOOST_CHECK_EQUAL(f1, P(x1));
    BOOST_CHECK_EQUAL(f1, P(x2));
    BOOST_CHECK_EQUAL(f1, P(x_mid));

    P.update(x2, f(x2));

    BOOST_CHECK_EQUAL(1U, P.degree());

    BOOST_CHECK_EQUAL(f1, P(x1));
    BOOST_CHECK_LE(std::abs(f2 - P(x2)), 1e-10);

    using std::abs;
    auto const eps = 0.5 * abs(x_mid - x1) * abs(x_mid - x2);

    BOOST_CHECK_LE(abs(f_mid - P(x_mid)), eps);
}

// QR разложение матрицы
#include <ural/numeric/matrix_decomposition.hpp>
#include <ural/numeric/matrix.hpp>

#include <boost/numeric/ublas/matrix.hpp>

// @todo Последовательность столбцов матрицы
BOOST_AUTO_TEST_CASE(qr_decomposition_test)
{
    using namespace boost::numeric::ublas;

    auto const dim = 3;
    boost::numeric::ublas::matrix<double> A(dim, dim);
    A(0, 0) = 12;   A(0, 1) = -51;  A(0, 2) = 4;
    A(1, 0) = 6;    A(1, 1) = 167;  A(1, 2) = -68;
    A(2, 0) = -4;   A(2, 1) = 24;   A(2, 2) = -41;

    auto QR = ural::QR_decomposition(A);
    auto Q = QR[ural::_1];
    auto R = QR[ural::_2];

    BOOST_CHECK_EQUAL(A.size1(), Q.size1());
    BOOST_CHECK_EQUAL(A.size2(), Q.size2());

    BOOST_CHECK_EQUAL(A.size1(), R.size1());
    BOOST_CHECK_EQUAL(A.size2(), R.size2());

    boost::numeric::ublas::identity_matrix<> const I{Q.size1()};
    BOOST_CHECK_LE(norm_1(prod(trans(Q), Q) - I), 1e-6);

    auto A_QR = prod(Q, R);

    BOOST_CHECK_EQUAL(A.size1(), A_QR.size1());
    BOOST_CHECK_EQUAL(A.size2(), A_QR.size2());

    BOOST_CHECK_LE(norm_1(A - A_QR), 1e-6);
}

BOOST_AUTO_TEST_CASE(matrix_init_test)
{
    ural::matrix<double> const A(3, 3, 0.0);

    for(size_t i = 0; i != A.size1(); ++ i)
    for(size_t j = 0; j != A.size2(); ++ j)
    {
        BOOST_CHECK_EQUAL(0.0, A(i, j));
    }
}

BOOST_AUTO_TEST_CASE(qr_decomposition_test_init_list)
{
    using namespace boost::numeric::ublas;

    ural::matrix<double> const A
        = {{12, -51, 4}, {6, 167, -68}, {-4, 24, -41}};

    BOOST_CHECK_EQUAL(3U, A.size1());
    BOOST_CHECK_EQUAL(3U, A.size2());

    auto QR = ural::QR_decomposition(A);
    auto Q = QR[ural::_1];
    auto R = QR[ural::_2];

    // столбцы Q - ортогональны
    for(auto s = ural::matrix_by_rows(Q); !!s; ++ s)
    {
        for(auto r = s.traversed_front(); !!r; ++ r)
        {
            BOOST_CHECK_CLOSE(0.0, inner_prod(*s, *r), 1e-6);
        }
    }

    // Q^T * Q = I
    boost::numeric::ublas::identity_matrix<> const I{Q.size1()};
    BOOST_CHECK_LE(norm_1(prod(trans(Q), Q) - I), 1e-6);

    // A = Q * R
    auto A_QR = prod(Q, R);

    BOOST_CHECK_EQUAL(A.size1(), A_QR.size1());
    BOOST_CHECK_EQUAL(A.size2(), A_QR.size2());

    BOOST_CHECK_LE(norm_1(A - A_QR), 1e-6);
}

BOOST_AUTO_TEST_CASE(cholesky_descomposition_test)
{
    using namespace boost::numeric::ublas;

    ural::matrix<double> const A
        = {{4, 12, -16}, {12, 37, -43}, {-16, -43, 98}};

    boost::numeric::ublas::triangular_matrix<double, lower> const
        L = ural::cholesky_decomposition(A);

    BOOST_CHECK_EQUAL(2, L(0, 0));
    BOOST_CHECK_EQUAL(6, L(1, 0));
    BOOST_CHECK_EQUAL(-8, L(2, 0));

    BOOST_CHECK_EQUAL(1, L(1, 1));
    BOOST_CHECK_EQUAL(5, L(2, 1));

    BOOST_CHECK_EQUAL(3, L(2, 2));


    auto A1 = prod(L, trans(L));

    BOOST_CHECK_EQUAL(A.size1(), A1.size1());
    BOOST_CHECK_EQUAL(A.size2(), A1.size2());

    BOOST_CHECK_LE(norm_1(A - A1), 1e-6);
}

BOOST_AUTO_TEST_CASE(square_constexpr_test)
{
    constexpr auto x = 4;

    constexpr auto xx = ural::square(x);

    static_assert(x*x == xx, "");
}

BOOST_AUTO_TEST_CASE(cube_constexpr_test)
{
    constexpr auto x = 4;

    constexpr auto xxx = ural::cube(x);

    static_assert(x*x*x == xxx, "");
}

// Числа произвольной точности
#include <ural/numeric/mp/integer_10.hpp>
typedef ural::integer_10 integer;

BOOST_AUTO_TEST_CASE(MP_integer_10_init_negative)
{
    auto const x = - 142;

    auto const x_mp = integer{x};

    BOOST_CHECK_EQUAL(x, x_mp);

    auto const x_s = ural::to_string(x);
    auto const x_mp_s = ural::to_string(x_mp);

    BOOST_CHECK_EQUAL(x_s, x_mp_s);

    BOOST_CHECK_EQUAL(-x, -x_mp);
    BOOST_CHECK_EQUAL(+x, +x_mp);
    BOOST_CHECK_EQUAL(x_mp, +x_mp);
    BOOST_CHECK_NE(-x_mp, +x_mp);
}

integer digit_sum(integer const & x)
{
    assert(x > 0);

    return ural::accumulate(x.digits(), integer{0});
}

#include <ural/sequence/progression.hpp>
BOOST_AUTO_TEST_CASE(MP_integer_10_PE_56)
{
    auto const a_max = 100;
    auto const b_max = 100;

    auto max_digit_sum
        = ural::make_min_element_accumulator(integer{0}, ural::greater<>{});

    for(auto a : ural::make_arithmetic_progression(1, 1) | ural::taken(a_max))
    for(auto b : ural::make_arithmetic_progression(1, 1) | ural::taken(b_max))
    {
        auto const r = ural::natural_power(integer{a}, b);
        auto const new_value = digit_sum(r);

        max_digit_sum.update(new_value);
    }

    BOOST_CHECK_EQUAL(max_digit_sum.result(), 972);
    BOOST_CHECK_EQUAL(972, max_digit_sum.result());
}

BOOST_AUTO_TEST_CASE(MP_integer_10_x_minus_x_equals_zero)
{
    auto const x = 128;
    auto const x_mp = integer{x};

    BOOST_CHECK_EQUAL(x_mp - x_mp, integer{0});
}

BOOST_AUTO_TEST_CASE(MP_integer_10_reminder)
{
    auto const x = 128;
    auto const d = 13;
    auto const r = x % d;

    auto const x_mp = integer{x};
    auto const d_mp = integer{d};

    auto const r_mp = x_mp % d_mp;

    BOOST_CHECK_EQUAL(r_mp, integer{r});
}

BOOST_AUTO_TEST_CASE(MP_integer_10_reminder_zero_regression)
{
    auto const x = 128;
    auto const x_mp = integer{x};

    BOOST_CHECK_EQUAL(x_mp % x_mp, integer{0});
}

BOOST_AUTO_TEST_CASE(MP_integer_10_zero_consitency)
{
    BOOST_CHECK_EQUAL(integer{0}, integer{});
}

#include <ural/format.hpp>

BOOST_AUTO_TEST_CASE(MP_integer_10_ostreaming_zero)
{
    auto const zero = integer{0};

    BOOST_CHECK_EQUAL(ural::to_string(zero), "0");
    BOOST_CHECK(ural::to_wstring(zero) == L"0");
}

BOOST_AUTO_TEST_CASE(MP_integer_10_wide_ostreaming_test)
{
    auto const N_max = 100;
    for(int n = -N_max; n <= N_max; ++ n)
    {
        auto const n_mp = integer{n};

        BOOST_CHECK(ural::to_wstring(n) == ural::to_wstring(n_mp));
    }
}

BOOST_AUTO_TEST_CASE(MP_integer_16_ostreaming)
{
    auto const n = 0xA7;
    auto const n_mp = ural::integer<16>(n);

    std::ostringstream os;
    os << std::hex << n;

    BOOST_CHECK_EQUAL(os.str(), ural::to_string(n_mp));
}

BOOST_AUTO_TEST_CASE(MP_integer_16_ostreaming_negative)
{
    auto const n = -0xA7;
    auto const n_mp = ural::integer<16>(n);

    BOOST_CHECK(n < 0);

    std::ostringstream os;
    os << std::hex << abs(n);

    BOOST_CHECK_EQUAL('-' + os.str(), ural::to_string(n_mp));
}

BOOST_AUTO_TEST_CASE(MP_integer_10_reminder_regression_1)
{
    auto x_mp = integer{10143};
    auto const d_mp = integer{100};

    BOOST_CHECK_EQUAL(x_mp % d_mp, integer{43});
}

BOOST_AUTO_TEST_CASE(MP_integer_10_plus_plus_test)
{
    auto const N_max = 100;
    for(int n = - N_max; n <= N_max; ++ n)
    {
        auto x = n;
        auto x_mp = integer{x};

        ++ x;
        ++ x_mp;

        auto const z_mp = integer{x};

        BOOST_CHECK_EQUAL(z_mp, x_mp);
    }
}

BOOST_AUTO_TEST_CASE(MP_integer_10_postfix_plus_plus_test)
{
    auto x = 100;
    auto x_mp = integer{x};

    auto const x_old = ++ x;
    auto const x_mp_old = ++ x_mp;

    auto const z_mp = integer{x};

    BOOST_CHECK_EQUAL(z_mp, x_mp);
    BOOST_CHECK_EQUAL(integer{x_old}, x_mp_old);
}

BOOST_AUTO_TEST_CASE(MP_integer_10_minus_minus_test)
{
    auto const N_max = 100;
    for(int n = -N_max; n <= N_max; ++ n)
    {
        auto x = n;
        auto x_mp = integer{x};

        -- x;
        -- x_mp;

        auto const z_mp = integer{x};

        BOOST_CHECK_EQUAL(z_mp, x_mp);
    }
}

BOOST_AUTO_TEST_CASE(MP_integer_10_postfix_minus_minus_test)
{
    auto x = 100;
    auto x_mp = integer{x};

    auto const x_old = -- x;
    auto const x_mp_old = -- x_mp;

    auto const z_mp = integer{x};

    BOOST_CHECK_EQUAL(z_mp, x_mp);
    BOOST_CHECK_EQUAL(integer{x_old}, x_mp_old);
}

BOOST_AUTO_TEST_CASE(MP_integer_10_operator_plus)
{
    int const a = 67;
    int const b = 63;

    auto const a_mp = integer{a};
    auto const b_mp = integer{b};

    BOOST_CHECK_EQUAL(integer{a+b}, a_mp + b_mp);
}

BOOST_AUTO_TEST_CASE(MP_integer_10_less_test)
{
    auto const N_max = 100;

    for(auto a = -N_max; a <= N_max; ++ a)
    for(auto b = -N_max; b <= N_max; ++ b)
    {
        auto const a_mp = integer{a};
        auto const b_mp = integer{b};

        BOOST_CHECK_EQUAL(a < b, a_mp < b_mp);
    }
}

BOOST_AUTO_TEST_CASE(MP_integer_10_abs_less_test)
{
    auto const N_max = 100;

    for(auto a = -N_max; a <= N_max; ++ a)
    for(auto b = -N_max; b <= N_max; ++ b)
    {
        auto const a_mp = integer{a};
        auto const b_mp = integer{b};

        BOOST_CHECK_EQUAL(abs(a) < abs(b), abs_less(a_mp, b_mp));
    }
}

BOOST_AUTO_TEST_CASE(MP_integer_10_mixed_equality_regression_1)
{
    BOOST_CHECK_EQUAL(ural::to_string(-100), ural::to_string(integer{-100}));
}

BOOST_AUTO_TEST_CASE(MP_integer_10_multiplies_test)
{
    auto const N_max = 100;

    for(auto a = -N_max; a <= N_max; ++ a)
    for(auto b = -N_max; b <= N_max; ++ b)
    {
        auto const a_mp = integer{a};
        auto const b_mp = integer{b};

        BOOST_CHECK_EQUAL(a*b, a_mp * b_mp);
    }
}

BOOST_AUTO_TEST_CASE(MP_integer_10_multiplies_assign)
{
    auto const N_max = 100;

    for(auto a = -N_max; a <= N_max; ++ a)
    for(auto b = -N_max; b <= N_max; ++ b)
    {
        auto a_mp = integer{a};
        auto const b_mp = integer{b};

        a_mp *= b_mp;

        BOOST_CHECK_EQUAL(a*b, a_mp);

        auto a_mp_1 = integer{a};
        a_mp_1 *= b;

        BOOST_CHECK_EQUAL(a*b, a_mp_1);
    }
}

BOOST_AUTO_TEST_CASE(MP_integer_10_mixed_multiplies_test)
{
    auto const N_max = 100;

    for(auto a = -N_max; a <= N_max; ++ a)
    for(auto b = -N_max; b <= N_max; ++ b)
    {
        auto const a_mp = integer{a};
        auto const b_mp = integer{b};

        BOOST_CHECK_EQUAL(a*b, a * b_mp);
        BOOST_CHECK_EQUAL(a*b, a_mp * b);
    }
}

BOOST_AUTO_TEST_CASE(MP_integer_10_plus_test)
{
    auto const N_max = 100;

    for(auto a = -N_max; a <= N_max; ++ a)
    for(auto b = -N_max; b <= N_max; ++ b)
    {
        auto const a_mp = integer{a};
        auto const b_mp = integer{b};

        BOOST_CHECK_EQUAL(a+b, a_mp + b_mp);
    }
}

BOOST_AUTO_TEST_CASE(MP_integer_10_minus_test)
{
    auto const N_max = 100;

    for(auto a = -N_max; a <= N_max; ++ a)
    for(auto b = -N_max; b <= N_max; ++ b)
    {
        auto const a_mp = integer{a};
        auto const b_mp = integer{b};

        BOOST_CHECK_EQUAL(a-b, a_mp - b_mp);
    }
}

// @todo деление и взятие остатка

BOOST_AUTO_TEST_CASE(MP_integer_60_output)
{
    ural::integer<60> const x(2*60*60 - 1);

    BOOST_CHECK_EQUAL(x.digits()[0], 59);
    BOOST_CHECK_EQUAL(x.digits()[1], 59);
    BOOST_CHECK_EQUAL(x.digits()[2], 1);

    std::string const z = "1:59:59";

    auto const s = ural::to_string(x);

    BOOST_CHECK_EQUAL(s, z);
}

// Цепные дроби
#include <ural/math/continued_fraction.hpp>

BOOST_AUTO_TEST_CASE(continued_fraction_sqrt_2_convergents)
{
    typedef int Integer;
    typedef ural::rational<Integer> Rational;

    ural::convergent<Integer> as(1);
    BOOST_CHECK_EQUAL(as.value(), Rational{1});

    as.update(2);
    BOOST_CHECK_EQUAL(as.value(), (Rational{3, 2}));

    as.update(2);
    BOOST_CHECK_EQUAL(as.value(), (Rational{7, 5}));

    as.update(2);
    BOOST_CHECK_EQUAL(as.value(), (Rational{17, 12}));

    as.update(2);
    BOOST_CHECK_EQUAL(as.value(), (Rational{41, 29}));
}

// Свёртки
BOOST_AUTO_TEST_CASE(convolution_test)
{
    // PE 205
    typedef long long Integer;

    std::vector<Integer> const p0(4, 1);
    std::vector<Integer> const c0(6, 1);

    typedef std::vector<Integer> Vector;

    auto const n_p = 36 / p0.size();
    auto const n_c = 36 / c0.size();

    auto const p = ural::natural_power(p0, n_p, ural::discrete_convolution, Vector{});
    auto const c = ural::natural_power(c0, n_c, ural::discrete_convolution, Vector{});

    auto total = Integer{0};
    auto favor = Integer{0};

    for(size_t i = 0; i != p.size(); ++ i)
    for(size_t j = 0; j != c.size(); ++ j)
    {
        auto const value = p[i] * c[j];
        total += value;
        favor += value * (i + n_p > j + n_c);
    }

    double const P = double(favor) / total;

    BOOST_CHECK_CLOSE_FRACTION(P, 0.5731441, 1e-7);

    /* @todo
    auto constexpr piramid_d = ural::distributions::uniform_int(1, 4);
    auto constexpr cube_d = ural::distributions::uniform_int(1, 6);

    auto constexpr n_piramid = 36 / (range(piramid_d) + 1);
    auto constexpr n_cube = 36 / (range(cube_d) + 1);

    auto const piramids_d = ural::distributions::iid_sum(piramid_d, n_piramid);
    auto const cubes_d = ural::distributions::iid_sum(cube_d, n_cube);

    auto const d = (piramids_d > cubes_d);

    BOOST_CHECK_CLOSE_FRACTION(d.p(), 0.5731441, 1e-7);
    */
}

BOOST_AUTO_TEST_CASE(convolution_test_valarray)
{
    // PE 205
    typedef long long Integer;

    typedef std::valarray<Integer> Vector;

    Vector const p0(1, 4);
    Vector const c0(1, 6);

    auto const n_p = 36 / p0.size();
    auto const n_c = 36 / c0.size();

    auto const p = ural::natural_power(p0, n_p, ural::discrete_convolution, Vector{});
    auto const c = ural::natural_power(c0, n_c, ural::discrete_convolution, Vector{});

    auto total = Integer{0};
    auto favor = Integer{0};

    for(size_t i = 0; i != p.size(); ++ i)
    for(size_t j = 0; j != c.size(); ++ j)
    {
        auto const value = p[i] * c[j];
        total += value;
        favor += value * (i + n_p > j + n_c);
    }

    double const P = double(favor) / total;

    BOOST_CHECK_CLOSE_FRACTION(P, 0.5731441, 1e-7);
}

// Метод Ньютона, вычисление квадратных корней
BOOST_AUTO_TEST_CASE(square_root_iterative_zero_test)
{
    auto const eps = 1e-6;

    auto const S  = 0.0;
    auto x_0 = S / 2;

    auto seq = ural::make_sqrt_heron_sequence(S, x_0, eps);

    for(; !!seq; ++seq)
    {
        x_0 = *seq;
    }

    BOOST_CHECK_CLOSE_FRACTION(std::sqrt(S), x_0, eps);
}

BOOST_AUTO_TEST_CASE(square_root_iterative_test)
{
    auto const eps = 1e-6;

    for(auto const S : ural::numbers(1.0, 200.0))
    {
        auto x_0 = S / 2;

        auto seq = ural::make_sqrt_heron_sequence(S, x_0, eps);

        for(; !!seq; ++ seq)
        {
            x_0 = *seq;
        }

        BOOST_CHECK_CLOSE_FRACTION(std::sqrt(S), x_0, eps);
    }
}

BOOST_AUTO_TEST_CASE(square_root_iterative_less_then_1_test)
{
    auto const eps = 1e-6;

    for(auto S = 0.0; S < 1.0; S += 0.01)
    {
        auto x_0 = S / 2;

        auto seq = ural::make_sqrt_heron_sequence(S, x_0, eps);

        for(; !!seq; ++ seq)
        {
            x_0 = *seq;
        }

        BOOST_CHECK_CLOSE_FRACTION(std::sqrt(S), x_0, eps);
    }
}
