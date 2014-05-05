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

    BOOST_CHECK_EQUAL(xs.front(), ds.min());
    BOOST_CHECK_EQUAL(xs.back(), ds.max());
    BOOST_CHECK_EQUAL(xs.back() - xs.front(), ds.range());

    BOOST_CHECK_EQUAL((xs.front() + xs.back()) / 2.0, ds.mean());
    BOOST_CHECK_CLOSE((ural::square(xs.size()) - 1) / 12.0, ds.variance(), 1e-3);
    BOOST_CHECK_EQUAL(std::sqrt(ds.variance()), ds.standard_deviation());
}
