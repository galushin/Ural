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
