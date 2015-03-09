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

    typedef std::common_type<int, size_t>::type int_and_size_t;

    static_assert(is_same<double, average_type<int, size_t>::type>::value, "");
    static_assert(is_same<double, average_type<double, size_t>::type>::value, "");
    static_assert(is_same<Rational, average_type<Rational, int>::type>::value, "");
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

#include "../rnd.hpp"

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
    typedef ural::multivariate_normal_distribution<Vector, symmetric_matrix<Double>>
        Vector_distribution;
    typedef ural::iid_adaptor<Vector_distribution> Sample_distribution;

    Sample_distribution sample_distr(sample_size, Vector_distribution{mu, C});

    auto const sample = sample_distr(ural_test::random_engine());

    BOOST_CHECK_EQUAL(sample_size, sample.size());

    // Вычисляем выборочную ковариационную матрицу
    auto acc = ural::for_each(sample,
                              ural::covariance_matrix_accumulator<Vector>(dim));

    auto S = acc.covariance_matrix();

    BOOST_CHECK_EQUAL(dim, S.size1());
    BOOST_CHECK_EQUAL(dim, S.size2());

    for(size_t i = 0; i != S.size1(); ++ i)
    {
        // @todo Макрос для проверки гипотезы о дисперсии?
        auto const alpha = 0.05;

        auto p = ural::variance_hypothesis_test(S(i, i), C(i, i), sample.size());

        BOOST_CHECK_LE(alpha / 2, p);
        BOOST_CHECK_LE(p, 1 - alpha / 2);

        for(size_t j = 0; j != i+1; ++ j)
        {
            // @todo проверять гипотезу о ковариации
            BOOST_CHECK_CLOSE(C(i, j), S(i, j), 5);
        }
    }

    auto const m = acc.mean();

    BOOST_CHECK_EQUAL(mu.size(), m.size());

    for(size_t i = 0; i != m.size(); ++ i)
    {
        // @todo макрос для проверки статистической гипотезы
        auto const alpha = 0.05;

        auto p = ural::mean_hypothesis_test_known_variance(m[i], mu[i], C(i, i),
                                                           sample.size());

        BOOST_CHECK_LE(alpha / 2, p);
        BOOST_CHECK_LE(p, 1 - alpha / 2);
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

    BOOST_CHECK(ural::is_correlational_matrix(S, 1e-6));

    // Вычиляем собственные векторы и числа корреляционной матрицы
    boost::numeric::ublas::matrix<double> A = S;

    auto const iter = 50;
    auto LV = ural::qr_eigenvectors(std::move(A), iter, 1e-4);
    A = std::move(LV[ural::_1]);
    auto V = std::move(LV[ural::_2]);

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

    // Вычисляем главные компоненты
    std::vector<Vector> scores;

    for(auto & x : sample)
    {
        scores.push_back(prod(trans(V), element_prod(x - m, s)));
    }

    // Проверить, что scores - некоррелированные, m = 0, s = 1
    {
        auto acc = ural::for_each(scores,
                                  ural::covariance_matrix_accumulator<Vector>(dim));

        auto S = acc.covariance_matrix();

        auto const C = boost::numeric::ublas::identity_matrix<double>(dim);
        auto const mu = Vector(dim, 0);

        // @todo макрос для проверки близости матриц
        BOOST_CHECK_EQUAL(C.size1(), S.size1());
        BOOST_CHECK_EQUAL(C.size2(), S.size2());

        for(size_t row = 0; row != C.size1(); ++ row)
        for(size_t col = 0; col != C.size2(); ++ col)
        {
            // @todo Заменить на BOOST_CHECK_CLOSE
            BOOST_CHECK_LE(abs(C(row, col) - S(row, col)), 1e-3);
        }
        auto const m = acc.mean();

        // @todo макрос для проверки близости векторов
        BOOST_CHECK_EQUAL(mu.size(), m.size());

        for(size_t i = 0; i != m.size(); ++ i)
        {
            BOOST_CHECK_LE(abs(mu[i] - m[i]), 1e-3);
        }
    }
}
