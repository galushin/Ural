#include <ural/utility.hpp>

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
