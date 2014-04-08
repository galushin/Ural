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

#include <boost/test/unit_test.hpp>

#include <ural/utility/expected.hpp>

BOOST_AUTO_TEST_CASE(expected_value_ctor_test)
{
    typedef ural::expected<int> Expected;

    Expected::value_type const init_value = 42;

    Expected const e{init_value};

    BOOST_CHECK(e.has_value());
    BOOST_CHECK_EQUAL(e.value(), init_value);

    BOOST_CHECK(e == init_value);
    BOOST_CHECK(init_value == e);
}

BOOST_AUTO_TEST_CASE(make_expected__test)
{
    typedef ural::expected<int> Expected;

    Expected::value_type const init_value = 42;

    auto e = ural::make_expected(init_value);

    BOOST_CHECK(e.has_value());
    BOOST_CHECK_EQUAL(e.value(), init_value);

    BOOST_CHECK(e == init_value);
    BOOST_CHECK(init_value == e);
}

BOOST_AUTO_TEST_CASE(expected_notconst_value_test)
{
    typedef ural::expected<int> Expected;

    Expected::value_type const x1 = 13;
    Expected::value_type const x2 = 42;

    Expected e{x1};
    e.value() = x2;

    BOOST_CHECK_EQUAL(static_cast<Expected const&>(e).value(), x2);

    BOOST_CHECK(e == x2);
    BOOST_CHECK(x2 == e);

    BOOST_CHECK(e != x1);
    BOOST_CHECK(x1 != e);
}

BOOST_AUTO_TEST_CASE(expected_from_exception_test)
{
    typedef ural::expected<int> Expected;

    typedef std::logic_error Exception;

    auto e = Expected::from_exception(Exception("Message"));

    BOOST_CHECK(!e.has_value());

    BOOST_CHECK_THROW(e.value(), Exception);

    BOOST_CHECK(e != 42);
    BOOST_CHECK(13 != e);
}

BOOST_AUTO_TEST_CASE(expected_copy_ctor_test)
{
    typedef ural::expected<int> Expected;

    typedef std::logic_error Exception;

    Expected const x1{42};
    auto const x2 = Expected::from_exception(Exception("Message"));

    Expected const x1_c = x1;
    Expected const x2_c = x2;

    BOOST_CHECK(x1_c.has_value() == true);
    BOOST_CHECK_EQUAL(x1_c.value(), x1.value());

    BOOST_CHECK(x2_c.has_value() == false);
    BOOST_CHECK_THROW(x2_c.value(), Exception);
}

namespace
{
    int may_throw(bool flag, int value)
    {
        if(flag)
        {
            return value;
        }
        else
        {
            throw value;
        }
    }
}

BOOST_AUTO_TEST_CASE(expected_from_call_test)
{
    auto const e1 = ural::expected_from_call(may_throw, true, 42);

    BOOST_CHECK(e1.has_value() == true);
    BOOST_CHECK(e1.value() == 42);

    BOOST_CHECK(e1.get_exception<int>() == nullptr);

    auto const e2 = ural::expected_from_call(may_throw, false, 42);

    BOOST_CHECK(e2.has_value() == false);
    BOOST_CHECK_THROW(e2.value(), int);

    auto * p_ex = e2.get_exception<int>();

    BOOST_CHECK(p_ex != nullptr);
    BOOST_CHECK_EQUAL(*p_ex, 42);
}

BOOST_AUTO_TEST_CASE(expected_swap_values_test)
{
    auto const e1_old = ural::expected_from_call(may_throw, true, 13);
    auto const e2_old = ural::expected_from_call(may_throw, true, 42);

    auto e1 = e1_old;
    auto e2 = e2_old;

    e1.swap(e2);

    BOOST_CHECK_EQUAL(e1.value(), e2_old.value());
    BOOST_CHECK_EQUAL(e2.value(), e1_old.value());
}

BOOST_AUTO_TEST_CASE(expected_swap_mixed_1_test)
{
    auto const e1_old = ural::expected_from_call(may_throw, false, 13);
    auto const e2_old = ural::expected_from_call(may_throw, true, 42);

    auto e1 = e1_old;
    auto e2 = e2_old;

    e1.swap(e2);

    BOOST_CHECK_EQUAL(e1.value(), e2_old.value());
    BOOST_CHECK_EQUAL(*e2.get_exception<int>(), *e1_old.get_exception<int>());
}

BOOST_AUTO_TEST_CASE(expected_swap_mixed_2_test)
{
    auto const e1_old = ural::expected_from_call(may_throw, true, 13);
    auto const e2_old = ural::expected_from_call(may_throw, false, 42);

    auto e1 = e1_old;
    auto e2 = e2_old;

    e1.swap(e2);

    BOOST_CHECK_EQUAL(*e1.get_exception<int>(), *e2_old.get_exception<int>());
    BOOST_CHECK_EQUAL(e2.value(), e1_old.value());
}

BOOST_AUTO_TEST_CASE(expected_swap_exceptions_test)
{
    auto const e1_old = ural::expected_from_call(may_throw, false, 13);
    auto const e2_old = ural::expected_from_call(may_throw, false, 42);

    auto e1 = e1_old;
    auto e2 = e2_old;

    e1.swap(e2);

    BOOST_CHECK_EQUAL(*e1.get_exception<int>(), *e2_old.get_exception<int>());
    BOOST_CHECK_EQUAL(*e2.get_exception<int>(), *e1_old.get_exception<int>());
}
