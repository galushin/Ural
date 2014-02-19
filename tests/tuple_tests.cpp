#include <boost/test/unit_test.hpp>

#include <ural/tuple.hpp>
#include <ural/utility.hpp>

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

namespace
{
    // Convert array into to tuple
    template<typename Array, std::size_t... I>
    auto a2t_impl(const Array& a, ural::index_sequence<I...>)
        -> decltype(std::make_tuple(a[I]...))
    {
        return std::make_tuple(a[I]...);
    }

    template<typename T, std::size_t N>
    auto a2t(const std::array<T, N>& a)
        -> decltype(a2t_impl(a, ural::make_index_sequence<N>()))
    {
        return a2t_impl(a, ural::make_index_sequence<N>());
    }

    // pretty-print a tuple (from http://stackoverflow.com/a/6245777/273767 )

    template<class Ch, class Tr, class Tuple, std::size_t... Is>
    void print_tuple_impl(std::basic_ostream<Ch,Tr>& os,
                          const Tuple & t,
                          ural::index_sequence<Is...>)
    {
        using swallow = int[];
        (void)swallow{0, (void(os << (Is == 0? "" : ", ") << std::get<Is>(t)), 0)...};
    }

    template<class Ch, class Tr, class... Args>
    void print_tuple(std::basic_ostream<Ch, Tr>& os, const std::tuple<Args...>& t)
    {
        os << "(";
        print_tuple_impl(os, t, ural::index_sequence_for<Args...>{});
        os << ")";
    }
}

BOOST_AUTO_TEST_CASE(integer_sequence_test)
{
     std::array<int, 4> an_array = {1,2,3,4};

    // convert an array into a tuple
    auto a_tuple = a2t(an_array);
    static_assert(std::is_same<decltype(a_tuple),
                               std::tuple<int, int, int, int>>::value, "");

    // print it
    std::ostringstream os;
    print_tuple(os, a_tuple);

    BOOST_CHECK_EQUAL(std::string("(1, 2, 3, 4)"), os.str());
}

namespace
{
    std::string print_123(int n, double x, std::string const & s)
    {
        std::ostringstream os;
        os << n << " " << x  << " " << s;
        return os.str();
    }
}

BOOST_AUTO_TEST_CASE(apply_function_to_tuple)
{
    auto const xs = std::make_tuple(42, 4.2, std::string("abs"));

    auto r = ural::apply(print_123, xs);

    BOOST_CHECK_EQUAL("42 4.2 abs", r);
}
