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
