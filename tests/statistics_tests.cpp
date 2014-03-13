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
