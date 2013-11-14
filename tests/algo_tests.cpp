#include <boost/test/unit_test.hpp>

#include <ural/algorithm.hpp>

BOOST_AUTO_TEST_CASE(for_each_test)
{
    std::vector<int> x_std = {1, 2, 3, 4, 5};
    std::vector<int> x_ural = x_std;

    auto const action = [](int & x) {x *= 2;};

    auto const r_std = std::for_each(x_std.begin(), x_std.end(), +action);
    auto const r_ural = ural::for_each(x_ural, +action);

    BOOST_CHECK_EQUAL(r_std, r_ural.target());

    BOOST_CHECK_EQUAL_COLLECTIONS(x_std.begin(), x_std.end(),
                                  x_ural.begin(), x_ural.end());
}

BOOST_AUTO_TEST_CASE(fill_test)
{
    std::vector<int> x_std = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    auto x_ural = x_std;

    auto const value = -1;
    std::vector<int> const z(x_std.size(), value);

    std::fill(x_std.begin(), x_std.end(), value);
    ural::fill(x_ural, value);

    BOOST_CHECK_EQUAL_COLLECTIONS(x_std.begin(), x_std.end(),
                                  x_ural.begin(), x_ural.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(x_ural.begin(), x_ural.end(),
                                  z.begin(), z.end());
}

BOOST_AUTO_TEST_CASE(equal_test)
{
     std::string const x1("radar");
     std::string const y1("hello");

     auto const x2 = x1;
     auto const y2 = y1;

     BOOST_CHECK(ural::equal(x1, x1) == true);
     BOOST_CHECK(ural::equal(x1, x2) == true);
     BOOST_CHECK(ural::equal(x2, x1) == true);
     BOOST_CHECK(ural::equal(x2, x2) == true);

     BOOST_CHECK(ural::equal(y1, y1) == true);
     BOOST_CHECK(ural::equal(y2, y1) == true);
     BOOST_CHECK(ural::equal(y1, y2) == true);
     BOOST_CHECK(ural::equal(y2, y2) == true);

     BOOST_CHECK(ural::equal(x1, y1) == false);
     BOOST_CHECK(ural::equal(x1, y2) == false);
     BOOST_CHECK(ural::equal(x2, y1) == false);
     BOOST_CHECK(ural::equal(x2, y2) == false);
}
