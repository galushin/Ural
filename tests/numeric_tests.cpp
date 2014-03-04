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

#include <numeric>
#include <forward_list>

#include <boost/test/unit_test.hpp>

#include <ural/algorithm.hpp>
#include <ural/numeric.hpp>

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
    std::vector<int> const v = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2};

    std::vector<int> x_std;
    std::partial_sum(v.begin(), v.end(), std::back_inserter(x_std));

    std::vector<int> x_ural;
    ural::copy(ural::partial_sums(v), std::back_inserter(x_ural));

    BOOST_CHECK_EQUAL_COLLECTIONS(x_std.begin(), x_std.end(),
                                  x_ural.begin(), x_ural.end());
}

BOOST_AUTO_TEST_CASE(adjacent_differences_sequence_test)
{
     std::vector<int> const xs = {1,2,3,5,9,11,12};

     std::vector<int> r_std;
     std::adjacent_difference(xs.begin(), xs.end(), std::back_inserter(r_std));

     std::vector<int> r_ural;
     ural::copy(ural::adjacent_differences(xs), std::back_inserter(r_ural));

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

    BOOST_CHECK_EQUAL(1, P.degree());
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
    BOOST_CHECK_EQUAL(P1.degree(), 1);
    BOOST_CHECK_EQUAL(P1[0], -P[0]);
    BOOST_CHECK_EQUAL(P1[1], -P[1]);
}
