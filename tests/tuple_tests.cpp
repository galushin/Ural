/*  This file is part of Ural.

    Ural is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Ural is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Ural.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <boost/test/unit_test.hpp>

#include <ural/functional.hpp>
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

BOOST_AUTO_TEST_CASE(tuple_constexpr_get)
{
    constexpr auto const v1 = 42;
    constexpr auto const v2 = 4.2;
    constexpr auto const ts = ural::make_tuple(v1, v2);

    static_assert(std::get<0>(ts) == v1, "");
    static_assert(std::get<1>(ts) == v2, "");

    BOOST_CHECK(true);
}

#ifdef __GNUC__
#if __GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__ >= 50101
BOOST_AUTO_TEST_CASE(tuple_constexpr_get_regeression_first_empty_type)
{
    constexpr auto const v0 = ural::equal_to<>{};
    constexpr auto const v1 = 42;
    constexpr auto const v2 = 4.2;
    constexpr auto const ts = ural::make_tuple(v0, v1, v2);

    static_assert(std::get<1>(ts) == v1, "");
    static_assert(std::get<2>(ts) == v2, "");

    BOOST_CHECK(true);
}
#endif // >= 50101
#endif // ifdef __GNUC__

BOOST_AUTO_TEST_CASE(tuple_ostreaming_test)
{
    std::ostringstream os;

    auto const xs = ural::make_tuple(42, 4.2, std::string("abs"));

    os << xs;

    BOOST_CHECK_EQUAL(os.str(), "{42, 4.2, abs}");
}

BOOST_AUTO_TEST_CASE(tuple_like_ostreaming_test)
{
    std::ostringstream os;

    auto const xs = std::make_pair(42, std::string("abs"));

    using namespace ural::tuple_io;

    os << xs;

    BOOST_CHECK_EQUAL(os.str(), "{42, abs}");
}
