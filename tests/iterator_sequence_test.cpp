#include <boost/concept/assert.hpp>
#include <boost/test/unit_test.hpp>

#include <ural/algorithm.hpp>
#include <ural/sequence/all.hpp>

// @todo Тест возвращаемого значения для copy при копировании разной длины

BOOST_AUTO_TEST_CASE(copy_sequence_test_via_details)
{
    std::vector<int> const xs = {1, 2, 3, 4};

    std::vector<int> x1(xs.size());

    ural::details::copy(ural::sequence(xs), ural::sequence(x1));

    BOOST_CHECK_EQUAL_COLLECTIONS(x1.begin(), x1.end(), xs.begin(), xs.end());
}

BOOST_AUTO_TEST_CASE(copy_sequence_test)
{
    std::vector<int> const xs = {1, 2, 3, 4};

    std::vector<int> x1(xs.size());

    ural::copy(xs, x1);

    BOOST_CHECK_EQUAL_COLLECTIONS(x1.begin(), x1.end(), xs.begin(), xs.end());
}
