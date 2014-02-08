#include <string>
#include <memory>

#include <boost/test/unit_test.hpp>

#include <ural/memory.hpp>

BOOST_AUTO_TEST_CASE(make_unique_array)
{
    auto const n = 5;
    auto p = ural::make_unique<std::string[]>(n);

    // FAILS: auto p_fail = ural::make_unique<std::string[5]>(n);

    static_assert(std::is_same<std::unique_ptr<std::string[]>, decltype(p)>::value, "");

    const std::string value {};

    for(size_t i = 0; i < n; ++ i)
    {
        BOOST_CHECK_EQUAL(value, p[i]);
    }
}
