#include <ural/algorithm.hpp>
#include <ural/numeric/numbers_sequence.hpp>
#include <ural/sequence/all.hpp>

#include <set>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(cartesian_product_sequence_test)
{
    auto digits = ural::numbers(0, 10);
    auto s2 = ural::make_cartesian_product_sequence(digits, digits);

    std::set<int> r2;

    for(; !!s2; ++ s2)
    {
        auto t = *s2;
        r2.insert(t[ural::_1] * 10 + t[ural::_2]);
    }

    BOOST_CHECK_EQUAL(100U, r2.size());
    BOOST_CHECK_EQUAL(0,  *r2.begin());
    BOOST_CHECK_EQUAL(99, *r2.rbegin());
}

// @todo Тест того, что кортежи лексикографически упорядочены

BOOST_AUTO_TEST_CASE(cartesian_product_sequence_test_forward)
{
    auto digits = ural::numbers(0, 10);
    auto s2 = ural::make_cartesian_product_sequence(digits, digits);

    auto const n = 20;

    assert(n < ural::size(s2));

    using Value = ural::ValueType<decltype(s2)>;

    std::vector<Value> out(n);

    auto copy_result = ural::copy(s2, out);

    assert(!!copy_result[ural::_1]);
    assert(!copy_result[ural::_2]);

    auto s2_traversed = copy_result[ural::_1].traversed_front();

    BOOST_CHECK(ural::equal(s2_traversed, out));
}
