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

BOOST_AUTO_TEST_CASE(accumulate_test)
{
    std::vector<int> const  v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto const sum_std = std::accumulate(v.begin(), v.end(), 0);
    auto const sum_ural = ural::accumulate(v, 0);

    BOOST_CHECK_EQUAL(sum_std, sum_ural);
}

BOOST_AUTO_TEST_CASE(inner_product_test)
{

    std::vector<int> a{0, 1, 2, 3, 4};
    std::vector<int> b{5, 4, 2, 3, 1};

    auto const r_std = std::inner_product(a.begin(), a.end(), b.begin(), 0);
    auto const r_ural = ural::inner_product(a, b, 0);

    BOOST_CHECK_EQUAL(r_std, r_ural);
}
