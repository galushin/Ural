#include <numeric>
#include <forward_list>

#include <boost/test/unit_test.hpp>

#include <ural/algorithm.hpp>
#include <ural/numeric.hpp>

// @todo тестировать с минимальными типами

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

BOOST_AUTO_TEST_CASE(partial_sums_sequence_test)
{
    std::vector<int> const v = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2};

    std::vector<int> x_std;
    std::partial_sum(v.begin(), v.end(), std::back_inserter(x_std));

    std::vector<int> x_ural;
    ural::copy(ural::partial_sums(v), std::back_inserter(x_ural));

    BOOST_CHECK_EQUAL_COLLECTIONS(x_std.begin(), x_std.end(),
                                  x_ural.begin(), x_ural.end());
}

BOOST_AUTO_TEST_CASE(adjacent_differences_sequence_test)
{
     std::vector<int> const xs = {1,2,3,5,9,11,12};

     std::vector<int> r_std;
     std::adjacent_difference(xs.begin(), xs.end(), std::back_inserter(r_std));

     std::vector<int> r_ural;
     ural::copy(ural::adjacent_differences(xs), std::back_inserter(r_ural));

     BOOST_CHECK_EQUAL_COLLECTIONS(r_std.begin(), r_std.end(),
                                   r_ural.begin(), r_ural.end());
}
