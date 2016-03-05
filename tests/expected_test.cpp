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

#include <ural/utility/expected.hpp>
#include <ural/memory.hpp>

#include <boost/test/unit_test.hpp>

namespace
{
    namespace ural_ex = ::ural::experimental;
}

BOOST_AUTO_TEST_CASE(expected_value_ctor_test)
{
    typedef ural_ex::expected<int> Expected;

    ::ural::ValueType<Expected> const init_value = 42;

    Expected const e{init_value};

    BOOST_CHECK(e.has_value());
    BOOST_CHECK_EQUAL(e.value(), init_value);

    BOOST_CHECK(e == init_value);
    BOOST_CHECK(init_value == e);
}

BOOST_AUTO_TEST_CASE(expected_move_value_ctor_test)
{
    typedef std::unique_ptr<int> Type;
    typedef ural_ex::expected<Type> Expected;

    auto const init_value = 42;
    ::ural::ValueType<Expected> init = ural::make_unique<Type::element_type>(init_value);

    Expected const e{std::move(init)};

    BOOST_CHECK(e.has_value());
    BOOST_CHECK_EQUAL(*e.value(), init_value);

    BOOST_CHECK(e != init);
    BOOST_CHECK(init != e);
}

BOOST_AUTO_TEST_CASE(make_expected_test)
{
    typedef ural_ex::expected<int> Expected;

    ::ural::ValueType<Expected> const init_value = 42;

    auto e = ural_ex::make_expected(init_value);

    BOOST_CHECK(e.has_value());
    BOOST_CHECK_EQUAL(e.value(), init_value);

    BOOST_CHECK(e == init_value);
    BOOST_CHECK(init_value == e);
}

BOOST_AUTO_TEST_CASE(expected_notconst_value_test)
{
    typedef ural_ex::expected<int> Expected;

    ::ural::ValueType<Expected> const x1 = 13;
    ::ural::ValueType<Expected> const x2 = 42;

    Expected e{x1};
    e.value() = x2;

    BOOST_CHECK_EQUAL(ural::as_const(e).value(), x2);

    BOOST_CHECK(e == x2);
    BOOST_CHECK(x2 == e);

    BOOST_CHECK(e != x1);
    BOOST_CHECK(x1 != e);
}

BOOST_AUTO_TEST_CASE(expected_from_exception_test)
{
    typedef ural_ex::expected<int> Expected;

    typedef std::logic_error Exception;

    auto e = Expected::from_exception(Exception("Message"));

    BOOST_CHECK(!e.has_value());

    BOOST_CHECK_THROW(e.value(), Exception);

    BOOST_CHECK(e != 42);
    BOOST_CHECK(13 != e);
}

BOOST_AUTO_TEST_CASE(expected_copy_ctor_test)
{
    typedef ural_ex::expected<int> Expected;

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

BOOST_AUTO_TEST_CASE(expected_move_ctor_test)
{
    typedef std::vector<int> Type;
    typedef ural_ex::expected<Type> Expected;

    typedef std::logic_error Exception;

    Type const init_value = {1, 2, 3, 4, 5};

    Expected x1{init_value};
    auto x2 = Expected::from_exception(Exception("Message"));

    Expected const x1_c = std::move(x1);
    Expected const x2_c = std::move(x2);

    BOOST_CHECK(x1_c.has_value() == true);
    BOOST_CHECK(x1_c.value() == init_value);

    BOOST_CHECK(x2_c.has_value() == false);
    BOOST_CHECK_THROW(x2_c.value(), Exception);
}

namespace
{
    struct may_throw_t
    {
        template <class T>
        T operator()(bool flag, T value) const
        {
            if(flag)
            {
                return std::move(value);
            }
            else
            {
                throw value;
            }
        }
    };

    constexpr may_throw_t may_throw {};
}

BOOST_AUTO_TEST_CASE(expected_from_call_test)
{
    auto const e1 = ural_ex::expected_from_call(may_throw, true, 42);

    BOOST_CHECK(e1.has_value() == true);
    BOOST_CHECK(e1.value() == 42);

    BOOST_CHECK(e1.get_exception<int>() == nullptr);

    auto const e2 = ural_ex::expected_from_call(may_throw, false, 42);

    BOOST_CHECK(e2.get_exception<std::exception>() == nullptr);

    BOOST_CHECK(e2.has_value() == false);
    BOOST_CHECK_THROW(e2.value(), int);

    auto * p_ex = e2.get_exception<int>();

    BOOST_CHECK(p_ex != nullptr);
    BOOST_CHECK_EQUAL(*p_ex, 42);
}

BOOST_AUTO_TEST_CASE(expected_swap_values_test)
{
    auto const e1_old = ural_ex::expected_from_call(may_throw, true, 13);
    auto const e2_old = ural_ex::expected_from_call(may_throw, true, 42);

    auto e1 = e1_old;
    auto e2 = e2_old;

    e1.swap(e2);

    BOOST_CHECK_EQUAL(e1.value(), e2_old.value());
    BOOST_CHECK_EQUAL(e2.value(), e1_old.value());
}

BOOST_AUTO_TEST_CASE(expected_swap_mixed_1_test)
{
    auto const e1_old = ural_ex::expected_from_call(may_throw, false, 13);
    auto const e2_old = ural_ex::expected_from_call(may_throw, true, 42);

    auto e1 = e1_old;
    auto e2 = e2_old;

    e1.swap(e2);

    BOOST_CHECK_EQUAL(e1.value(), e2_old.value());
    BOOST_CHECK_EQUAL(*e2.get_exception<int>(), *e1_old.get_exception<int>());
}

BOOST_AUTO_TEST_CASE(expected_swap_mixed_2_test)
{
    auto const e1_old = ural_ex::expected_from_call(may_throw, true, 13);
    auto const e2_old = ural_ex::expected_from_call(may_throw, false, 42);

    auto e1 = e1_old;
    auto e2 = e2_old;

    e1.swap(e2);

    BOOST_CHECK_EQUAL(*e1.get_exception<int>(), *e2_old.get_exception<int>());
    BOOST_CHECK_EQUAL(e2.value(), e1_old.value());
}

BOOST_AUTO_TEST_CASE(expected_swap_exceptions_test)
{
    auto const e1_old = ural_ex::expected_from_call(may_throw, false, 13);
    auto const e2_old = ural_ex::expected_from_call(may_throw, false, 42);

    auto e1 = e1_old;
    auto e2 = e2_old;

    e1.swap(e2);

    BOOST_CHECK_EQUAL(*e1.get_exception<int>(), *e2_old.get_exception<int>());
    BOOST_CHECK_EQUAL(*e2.get_exception<int>(), *e1_old.get_exception<int>());
}

BOOST_AUTO_TEST_CASE(expected_value_or_test)
{
    auto const e1 = ural_ex::expected_from_call(may_throw, true, 42);
    auto const e2 = ural_ex::expected_from_call(may_throw, false, 42);

    auto const r1 = e1.value_or(13);
    auto const r2 = e2.value_or(13);

    BOOST_CHECK_EQUAL(42, r1);
    BOOST_CHECK_EQUAL(13, r2);
}

BOOST_AUTO_TEST_CASE(expected_copy_assign_to_value)
{
    auto const old_value = 42;
    auto const new_value = 13;

    auto e = ural_ex::expected_from_call(may_throw, true, old_value);

    auto e_good = ural_ex::expected_from_call(may_throw, true, new_value);
    auto e_bad = ural_ex::expected_from_call(may_throw, false, new_value);

    e = e_good;

    BOOST_CHECK(e.has_value());
    BOOST_CHECK_EQUAL(new_value, e.value());

    e = e_bad;

    BOOST_CHECK(e.has_value() == false);
    BOOST_CHECK_EQUAL(new_value, *e.get_exception<int>());
}

BOOST_AUTO_TEST_CASE(expected_copy_assign_to_exception)
{
    auto const old_value = 42;
    auto const new_value = 13;

    auto e = ural_ex::expected_from_call(may_throw, false, old_value);

    auto e_good = ural_ex::expected_from_call(may_throw, true, new_value);
    auto e_bad = ural_ex::expected_from_call(may_throw, false, new_value);

    e = e_good;

    BOOST_CHECK(e.has_value());
    BOOST_CHECK_EQUAL(new_value, e.value());

    e = e_bad;

    BOOST_CHECK(e.has_value() == false);
    BOOST_CHECK_EQUAL(new_value, *e.get_exception<int>());
}


BOOST_AUTO_TEST_CASE(expected_move_assign_to_value)
{
    typedef std::vector<int> Type;
    Type const old_value{4, 2};
    Type const new_value{1, 3};

    auto e = ural_ex::expected_from_call(may_throw, true, old_value);

    auto e_good = ural_ex::expected_from_call(may_throw, true, new_value);
    auto e_bad = ural_ex::expected_from_call(may_throw, false, new_value);

    e = std::move(e_good);

    BOOST_CHECK(e.has_value());
    BOOST_CHECK(new_value == e.value());
    BOOST_CHECK(ural::empty(e_good.value()));

    e = std::move(e_bad);

    BOOST_CHECK(e.has_value() == false);
    BOOST_CHECK(new_value == *e.get_exception<Type>());
}

BOOST_AUTO_TEST_CASE(expected_move_assign_to_exception)
{
    typedef std::vector<int> Type;
    Type const old_value{4, 2};
    Type const new_value{1, 3};

    auto e = ural_ex::expected_from_call(may_throw, false, old_value);

    auto e_good = ural_ex::expected_from_call(may_throw, true, new_value);
    auto e_bad = ural_ex::expected_from_call(may_throw, false, new_value);

    e = std::move(e_good);

    BOOST_CHECK(e.has_value());
    BOOST_CHECK(new_value == e.value());
    BOOST_CHECK(ural::empty(e_good.value()));

    e = std::move(e_bad);

    BOOST_CHECK(e.has_value() == false);
    BOOST_CHECK(new_value == *e.get_exception<Type>());
}

BOOST_AUTO_TEST_CASE(expected_fmap_test)
{
    auto const value = 3;

    auto e_good = ural_ex::expected_from_call(may_throw, true, value);

    auto e_sq = e_good.fmap([](int x) { return x*x;});

    BOOST_CHECK_EQUAL(value*value, e_sq.value());
}

BOOST_AUTO_TEST_CASE(unexpected_fmap_test)
{
    auto const value = 3;

    auto e_good = ural_ex::expected_from_call(may_throw, false, value);

    auto e_sq = e_good.fmap([](int x) { return x*x;});

    BOOST_CHECK(e_sq.has_value() == false);

    try
    {
        e_sq.value();
    }
    catch(int const & x)
    {
        BOOST_CHECK_EQUAL(value, x);
    }
}


BOOST_AUTO_TEST_CASE(expected_copy_assign_value_to_value)
{
    auto const value = std::string{"Good"};
    auto e_good = ural_ex::expected_from_call(may_throw, false, value);

    std::string const s{"Bad"};

    e_good = s;

    BOOST_CHECK_EQUAL(e_good.value(), s);
}

BOOST_AUTO_TEST_CASE(expected_copy_assign_value_to_exception)
{
    auto const value = std::string{"Good"};
    auto e = ural_ex::expected_from_call(may_throw, true, value);

    std::string const s{"Bad"};

    e = s;

    BOOST_CHECK_EQUAL(e.value(), s);
}

BOOST_AUTO_TEST_CASE(expected_move_assign_value_to_value)
{
    auto const value = std::string{"Good"};
    auto e = ural_ex::expected_from_call(may_throw, false, value);

    std::string s{"Bad"};
    auto const s_old = s;

    e = std::move(s);

    BOOST_CHECK_EQUAL(e.value(), s_old);
    BOOST_CHECK_NE(s, s_old);
}

BOOST_AUTO_TEST_CASE(expected_move_assign_value_to_exception)
{
    auto const value = std::string{"Good"};
    auto e = ural_ex::expected_from_call(may_throw, true, value);

    std::string s{"Bad"};
    auto const s_old = s;

    e = std::move(s);

    BOOST_CHECK_EQUAL(e.value(), s_old);
    BOOST_CHECK_NE(s, s_old);
}
