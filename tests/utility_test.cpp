#include <ural/utility.hpp>
#include <ural/placeholders.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(with_old_value_rollback)
{
    ural::with_old_value<std::string> x{"old"};

    x.value() = "new";

    BOOST_CHECK_EQUAL("new", x.value());
    BOOST_CHECK_EQUAL("old", x.old_value());

    x.rollback();

    BOOST_CHECK_EQUAL("old", x.value());
    BOOST_CHECK_EQUAL("old", x.old_value());
}

BOOST_AUTO_TEST_CASE(with_old_value_copy_value_construct)
{
    auto const value = std::string {"abc"};

    ural::with_old_value<std::string> const x{value};

    BOOST_CHECK_EQUAL(value, x.value());
    BOOST_CHECK_EQUAL(value, x.old_value());
}

BOOST_AUTO_TEST_CASE(with_old_value_copy_assign_new_value)
{
    ural::with_old_value<std::string> x{"old"};

    BOOST_CHECK_EQUAL("old", x.value());
    BOOST_CHECK_EQUAL("old", x.old_value());

    auto const new_value = std::string {"new"};

    x = new_value;

    BOOST_CHECK_EQUAL(new_value, x.value());
    BOOST_CHECK_EQUAL("old", x.old_value());

}

BOOST_AUTO_TEST_CASE(with_old_value_inplace_construction)
{
    auto const n = 5;
    auto const value = 'a';

    std::string const x(n, value);

    ural::with_old_value<std::string> const z(ural::inplace, n, value);

    BOOST_CHECK_EQUAL(x, z.value());
    BOOST_CHECK_EQUAL(x, z.old_value());
}

BOOST_AUTO_TEST_CASE(make_with_old_value_test)
{
    auto const value = std::string {"abc"};

    ural::with_old_value<std::string> const x
        = ural::make_with_old_value(value);

    BOOST_CHECK_EQUAL(value, x.value());
    BOOST_CHECK_EQUAL(value, x.old_value());
}

BOOST_AUTO_TEST_CASE(make_with_old_value_from_rvalue_test)
{
    auto const value = std::string {"abc"};

    ural::with_old_value<std::string> const x
        = ural::make_with_old_value(std::string(value));

    BOOST_CHECK_EQUAL(value, x.value());
    BOOST_CHECK_EQUAL(value, x.old_value());
}

BOOST_AUTO_TEST_CASE(make_with_old_value_constexpr)
{
    typedef int T;

    auto constexpr value = T{42};

    ural::with_old_value<T> constexpr x = ural::make_with_old_value(value);

    static_assert(value == x.value(), "");
    static_assert(value == x.old_value(), "");

    BOOST_CHECK_EQUAL(value, x.value());
    BOOST_CHECK_EQUAL(value, x.old_value());

    ural::with_old_value<T> constexpr x2 = ural::make_with_old_value(T{value});

    static_assert(value == x2.value(), "");
    static_assert(value == x2.old_value(), "");

    BOOST_CHECK_EQUAL(value, x2.value());
    BOOST_CHECK_EQUAL(value, x2.old_value());
}

#include <ural/math/rational.hpp>

BOOST_AUTO_TEST_CASE(with_old_value_default_constructor_constexpr)
{
    typedef ural::rational<int> T;
    auto constexpr value = T{};

    auto constexpr x = ural::with_old_value<T>{};

    static_assert(value == x.value(), "");
    static_assert(value == x.old_value(), "");

    BOOST_CHECK_EQUAL(value, x.value());
    BOOST_CHECK_EQUAL(value, x.old_value());
}

// Универсальный интерфейс к Кортежам
BOOST_AUTO_TEST_CASE(pair_tuple_access_test)
{
    auto const p = std::make_pair(42, 'p');

    BOOST_CHECK_EQUAL(p.first, ural::get(p, ural::_1));
    BOOST_CHECK_EQUAL(p.second, ural::get(p, ural::_2));
}

BOOST_AUTO_TEST_CASE(compressed_pair_tuple_access_test)
{
    boost::compressed_pair<int, char> const p(42, 'p');

    BOOST_CHECK_EQUAL(p.first(), ural::get(p, ural::_1));
    BOOST_CHECK_EQUAL(p.second(), ural::get(p, ural::_2));
}

BOOST_AUTO_TEST_CASE(complex_tuple_access_test)
{
    double const x = 4;
    double const y = 5;

    BOOST_CHECK_NE(x, y);

    std::complex<double> z{x, y};

    BOOST_CHECK_EQUAL(x, ural::get(z, ural::_1));
    BOOST_CHECK_EQUAL(y, ural::get(z, ural::_2));
}

BOOST_AUTO_TEST_CASE(complex_tuple_access_assign_test)
{
    double const x = 4;
    double const y = 5;

    BOOST_CHECK_NE(x, y);

    std::complex<double> z{x, y};

    BOOST_CHECK_EQUAL(x, ural::get(z, ural::_1));
    BOOST_CHECK_EQUAL(y, ural::get(z, ural::_2));

    const double x_new = 13;
    const double y_new = 23;

    BOOST_CHECK_NE(x_new, y_new);

    ural::get(z, ural::_1) = x_new;

    BOOST_CHECK_EQUAL(x_new, ural::get(z, ural::_1));
    BOOST_CHECK_EQUAL(y, ural::get(z, ural::_2));

    ural::get(z, ural::_2) = y_new;

    BOOST_CHECK_EQUAL(x_new, ural::get(z, ural::_1));
    BOOST_CHECK_EQUAL(y_new, ural::get(z, ural::_2));
}

BOOST_AUTO_TEST_CASE(tuple_uniform_access_test)
{
    auto const n = 42;
    auto const s = "abc";
    auto const c = "!";

    auto const x = std::make_tuple(n, s, c);

    BOOST_CHECK_EQUAL(n, ural::get(x, ural::_1));
    BOOST_CHECK_EQUAL(s, ural::get(x, ural::_2));
    BOOST_CHECK_EQUAL(c, ural::get(x, ural::_3));
}

BOOST_AUTO_TEST_CASE(tuple_uniform_nonconst_access_test)
{
    auto const n = 42;
    auto const s = "abc";
    auto const c = "!";

    auto x = std::make_tuple(n, s, c);

    BOOST_CHECK_EQUAL(n, ural::get(x, ural::_1));
    BOOST_CHECK_EQUAL(s, ural::get(x, ural::_2));
    BOOST_CHECK_EQUAL(c, ural::get(x, ural::_3));

    auto const n_new = 13;

    ural::get(x, ural::_1) = n_new;

    BOOST_CHECK_EQUAL(n_new, ural::get(x, ural::_1));
    BOOST_CHECK_EQUAL(s, ural::get(x, ural::_2));
    BOOST_CHECK_EQUAL(c, ural::get(x, ural::_3));
}
