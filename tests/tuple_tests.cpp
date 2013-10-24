#include <boost/test/unit_test.hpp>

#include <ural/tuple.hpp>

BOOST_AUTO_TEST_CASE(tuple_default_ctor)
{
    constexpr ural::tuple<int, long> x0{};

    BOOST_CHECK_EQUAL(0, std::get<0>(x0));
    BOOST_CHECK_EQUAL(0L, std::get<1>(x0));

    static_assert(0 == std::get<0>(x0), "");
    static_assert(0L == std::get<1>(x0), "");
}

BOOST_AUTO_TEST_CASE(tuple_array_like_access)
{
    constexpr ural::tuple<bool, int> x{true, 42};

    BOOST_CHECK_EQUAL(true, x[ural::_1]);
    BOOST_CHECK_EQUAL(42, x[ural::_2]);

    BOOST_CHECK_EQUAL(std::get<0>(x), x[ural::_1]);
    BOOST_CHECK_EQUAL(std::get<1>(x), x[ural::_2]);

    static_assert(true == x[ural::_1], "");
    static_assert(42 == x[ural::_2], "");
}
