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
#include <ural/statistics.hpp>
#include <ural/math/rational.hpp>

#include <boost/math/distributions/normal.hpp>
#include <boost/math/distributions/chi_squared.hpp>
#include <boost/mpl/list.hpp>
#include <boost/test/unit_test.hpp>

namespace
{
    typedef boost::mpl::list<double, ural::rational<int>> Real_types;

    template <class R, class T>
    constexpr R make_fraction(T num, T denom)
    {
        return R(num) / denom;
    }
}

BOOST_AUTO_TEST_CASE(probability_default_param_type_test)
{
    typedef ural::probability<> P;

    static_assert(std::is_same<double, P::value_type>::value,
                  "default is double");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(probability_default_ctor_test, T, Real_types)
{
    ural::probability<T> constexpr p {};

    static_assert(p.value() == 0, "p must be 0");
    static_assert(p == 0, "p must be 0");
    static_assert(0 == p, "p must be 0");

    static_assert(sizeof(p) == sizeof(typename decltype(p)::value_type),
                  "Too big!");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(probability_value_ctor_test, T, Real_types)
{
    auto constexpr value = make_fraction<T>(1, 2);

    ural::probability<T> constexpr p0{};
    ural::probability<T> constexpr p{value};

    static_assert(p.value() == value, "incorrect value");
    static_assert(p == value, "incorrect value");
    static_assert(value == p, "incorrect value");

    static_assert(p0 == p0, "");
    static_assert(p == p, "");
    static_assert(p0 != p, "");
}

BOOST_AUTO_TEST_CASE_TEMPLATE(probability_bad_value_ctor_test, T, Real_types)
{
    auto const value = make_fraction<T>(3, 2);
    BOOST_CHECK_THROW(ural::probability<T>{value}, std::logic_error);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(probability_assign_value_test, T, Real_types)
{
    auto value = make_fraction<T>(1, 2);

    ural::probability<T> p{};

    BOOST_CHECK(value != p);
    BOOST_CHECK(p != value);

    p = value;

    BOOST_CHECK_EQUAL(value, p.value());
    BOOST_CHECK(value == p);
    BOOST_CHECK(p == value);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(probability_bad_value_assign_test, T, Real_types)
{
    ural::probability<T> p{};
    auto const value = make_fraction<T>(3, 2);
    BOOST_CHECK_THROW(p = value, std::logic_error);
}

BOOST_AUTO_TEST_CASE(average_type_test)
{
    typedef ural::rational<int> Rational;

    using std::is_same;
    using ural::average_type;

    static_assert(is_same<double, average_type<int, size_t>::type>::value, "");
    static_assert(is_same<double, average_type<double, size_t>::type>::value, "");
    // @todo static_assert(is_same<Rational, average_type<Rational, size_t>::type>::value, "");

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(describe_test)
{
    std::vector<int> const xs = {1, 2, 3, 4, 5, 6};

    using namespace ural::statistics::tags;

    auto ds = ural::describe(xs, variance | range);

    BOOST_CHECK_EQUAL(xs.size(), ds.count());
    BOOST_CHECK_EQUAL(xs.size(), ds[count]);

    BOOST_CHECK_EQUAL(xs.front(), ds.min());
    BOOST_CHECK_EQUAL(xs.front(), ds[min]);

    BOOST_CHECK_EQUAL(xs.back(), ds.max());
    BOOST_CHECK_EQUAL(xs.back(), ds[max]);

    BOOST_CHECK_EQUAL(xs.back() - xs.front(), ds.range());
    BOOST_CHECK_EQUAL(xs.back() - xs.front(), ds[range]);

    BOOST_CHECK_EQUAL((xs.front() + xs.back()) / 2.0, ds.mean());
    BOOST_CHECK_EQUAL((xs.front() + xs.back()) / 2.0, ds[mean]);

    BOOST_CHECK_CLOSE((ural::square(xs.size()) - 1) / 12.0, ds.variance(), 1e-3);
    BOOST_CHECK_CLOSE((ural::square(xs.size()) - 1) / 12.0, ds[variance], 1e-3);
}

BOOST_AUTO_TEST_CASE(describe_test_duplicated_tags)
{
    std::vector<int> const xs = {1, 2, 3, 4, 5, 6};

    using namespace ural::statistics::tags;

    auto ds = ural::describe(xs, count | mean | min | max | std_dev | range);

    BOOST_CHECK_EQUAL(xs.size(), ds.count());

    BOOST_CHECK_EQUAL(xs.front(), ds.min());
    BOOST_CHECK_EQUAL(xs.back(), ds.max());
    BOOST_CHECK_EQUAL(xs.back() - xs.front(), ds.range());

    BOOST_CHECK_EQUAL((xs.front() + xs.back()) / 2.0, ds.mean());
    BOOST_CHECK_CLOSE((ural::square(xs.size()) - 1) / 12.0, ds.variance(), 1e-3);

    BOOST_CHECK_EQUAL(std::sqrt(ds.variance()), ds.standard_deviation());
    BOOST_CHECK_EQUAL(std::sqrt(ds.variance()), ds[std_dev]);
}

BOOST_AUTO_TEST_CASE(z_score_test)
{
    std::vector<int> const xs = {1, 2, 3, 4, 5, 6};

    std::vector<double> zs;

    ural::z_score(xs, zs | ural::back_inserter);

    BOOST_CHECK_EQUAL(zs.size(), xs.size());

    auto const ds = ural::describe(xs, ural::statistics::tags::std_dev);

    auto const a = ds.standard_deviation();
    auto const b = ds.mean();

    for(size_t i = 0; i != xs.size(); ++ i)
    {
        BOOST_CHECK_CLOSE(xs[i], a * zs[i] + b, 1e-3);
    }
}

#include "rnd.hpp"

#include <ural/numeric/matrix.hpp>
#include <ural/random.hpp>
#include <ural/numeric/matrix_decomposition.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/symmetric.hpp>

BOOST_AUTO_TEST_CASE(principal_components_test)
{
    // @todo Выделить алгоритмы
    using namespace boost::numeric::ublas;

    typedef double Double;
    typedef boost::numeric::ublas::vector<Double> Vector;

    size_t const sample_size = 1000;

    // Выбираем ковариационную матрицу
    symmetric_matrix<Double> C(2);

    C(0, 0) = 4;
    C(0, 1) = 6;
    C(1, 1) = 25;

    BOOST_CHECK_EQUAL(C(0, 1), C(1, 0));

    auto const dim = C.size1();
    BOOST_CHECK_EQUAL(C.size2(), dim);

    // Добавить сдвиг мат. ожидания
    Vector mu(dim);
    mu[0] = -1;
    mu[1] = 1;

    // Вычисляем коррелированные случайные величины
    ural::multivariate_normal_distribution<Vector, symmetric_matrix<Double>>
        distr(mu, C);

    std::vector<Vector> sample;

    std::generate_n(sample | ural::back_inserter, sample_size,
                    std::bind(distr, std::ref(ural_test::random_engine())));

    BOOST_CHECK_EQUAL(sample_size, sample.size());

    // Вычисляем выборочную ковариационную матрицу
    auto acc = ural::for_each(sample,
                              ural::covariance_matrix_accumulator<Vector>(dim));

    auto S = acc.covariance_matrix();

    BOOST_CHECK_EQUAL(dim, S.size1());
    BOOST_CHECK_EQUAL(dim, S.size2());

    for(size_t i = 0; i != S.size1(); ++ i)
    {
        // @todo Выделить функцию
        boost::math::chi_squared_distribution<> chi_2(sample.size() - 1);
        auto const alpha = 0.05;
        auto const q1 = quantile(chi_2, alpha / 2);
        auto const q2 = quantile(chi_2, 1 - alpha / 2);

        BOOST_CHECK_LE(S(i, i), C(i, i) * q2  / chi_2.degrees_of_freedom());
        BOOST_CHECK_GE(S(i, i), C(i, i) * q1  / chi_2.degrees_of_freedom());

        for(size_t j = 0; j != i+1; ++ j)
        {
            // @todo проверять гипотезу о ковариации
            BOOST_CHECK_CLOSE(C(i, j), S(i, j), 5);
        }
    }

    auto m = acc.mean();

    BOOST_CHECK_EQUAL(mu.size(), m.size());

    for(size_t i = 0; i != m.size(); ++ i)
    {
        // @todo макрос для проверки статистической гипотезы
        using std::sqrt;
        double eps = C(i, i) / sqrt(sample.size())
                     * quantile(boost::math::normal_distribution<>(), 0.95);

        BOOST_CHECK_CLOSE_FRACTION(mu[i], m[i], eps);
    }

    // Вычисляем выборочную корреляционную матрицу
    // @todo Выделить алгоритм
    Vector s = ural::diag(S);

    for(auto & x : s)
    {
        assert(x >= 0);

        using std::sqrt;
        x = 1.0 / sqrt(x);
    }

    // @todo В матричном виде?
    for(size_t i = 0; i != S.size1(); ++ i)
    for(size_t j = 0; j != i+1; ++ j)
    {
        S(i, j) *= s[i] * s[j];
    }

    for(size_t i = 0; i != S.size1(); ++ i)
    {
        using std::abs;
        for(size_t j = 0; j != i; ++ j)
        {
            BOOST_CHECK(abs(S(i, j)) < 1.0);
        }

        BOOST_CHECK_CLOSE(1.0, abs(S(i, i)), 1e-6);
    }

    // Вычиляем собственные векторы и числа корреляционной матрицы
    auto const iter = 50;

    boost::numeric::ublas::matrix<double> A = S;
    boost::numeric::ublas::matrix<double> V =
        boost::numeric::ublas::identity_matrix<double>(dim);

    for(auto n = iter; n > 0; -- n)
    {
        auto qr = ural::QR_decomposition(A);

        A = prod(qr[ural::_2], qr[ural::_1]);
        V = prod(V, qr[ural::_1]);

        // @todo Дополнительные критерии остановки
    }

    // Проверить, что найдены собственные числа и векторы
    for(size_t i = 0; i != V.size2(); ++ i)
    {
        auto v = column(V, i);

        Vector Sv = prod(S, v);
        Vector lv = A(i, i) * v;

        for(size_t j = 0; j != V.size1(); ++ j)
        {
            BOOST_CHECK_CLOSE(Sv[j], lv[j], 1e-6);
        }
    }

    // @todo Сравнить с собственными числами и векторами корреляционной матрицы
    // @todo Вычисляем главные компоненты
}
