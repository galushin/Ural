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
