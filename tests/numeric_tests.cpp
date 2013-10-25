#include <numeric>
#include <forward_list>

#include <boost/test/unit_test.hpp>

#include <ural/numeric.hpp>

BOOST_AUTO_TEST_CASE(iota_test)
{
    auto const n = 10;
    auto const init_value = -4;
    std::forward_list<int> x1(n);
    std::forward_list<int> x2(n);

    std::iota(x1.begin(), x1.end(), init_value);
    auto const result = ural::iota(x2, init_value);

    BOOST_CHECK_EQUAL_COLLECTIONS(x1.begin(), x1.end(), x2.begin(), x2.end());
    BOOST_CHECK_EQUAL(init_value + n, result);
}
