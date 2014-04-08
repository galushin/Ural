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
    int may_throw(bool x)
    {
        if(x)
        {
            return 42;
        }
        else
        {
            throw std::logic_error("Error");
        }
    }
}

BOOST_AUTO_TEST_CASE(expected_from_call_test)
{
    auto const e1 = ural::expected_from_call([](){ return may_throw(true);});

    BOOST_CHECK(e1.has_value() == true);
    BOOST_CHECK(e1.value() == 42);

    auto const e2 = ural::expected_from_call([](){ return may_throw(false);});

    BOOST_CHECK(e2.has_value() == false);
    BOOST_CHECK_THROW(e2.value(), std::logic_error);
}
