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

#include <ural/abi.hpp>
#include <ural/algorithm.hpp>
#include <ural/sequence/all.hpp>
#include <ural/sequence/map.hpp>
#include <ural/sequence/sink.hpp>
#include <ural/sequence/progression.hpp>

#include <boost/test/unit_test.hpp>

#include <iterator>
#include <set>
#include <map>

BOOST_AUTO_TEST_CASE(istream_sequence_test)
{
    std::istringstream str1("0.1 0.2 0.3 0.4");
    std::istringstream str2("0.1 0.2 0.3 0.4");

    std::vector<double> r_std;
    std::copy(std::istream_iterator<double>(str1),
              std::istream_iterator<double>(),
              r_std | ural::back_inserter);

    std::vector<double> r_ural;
    ural::copy(ural::make_istream_sequence<double>(str2),
               r_ural | ural::back_inserter);

    BOOST_CHECK_EQUAL_COLLECTIONS(r_std.begin(), r_std.end(),
                                  r_ural.begin(), r_ural.end());
}

BOOST_AUTO_TEST_CASE(ostream_sequence_test)
{
    std::vector<int> const xs = {1, 2, 3, 4, 5};

    std::ostringstream os_std;
    std::ostringstream os_ural;

    std::copy(xs.begin(), xs.end(), std::ostream_iterator<int>(os_std, " "));
    ural::copy(xs, ural::make_ostream_sequence<int>(os_ural, " "));

    BOOST_CHECK_EQUAL(os_std.str(), os_ural.str());
}

BOOST_AUTO_TEST_CASE(ostream_sequence_test_auto)
{
    std::vector<int> const xs = {1, 2, 3, 4, 5};

    std::ostringstream os_std;
    std::ostringstream os_ural;

    std::copy(xs.begin(), xs.end(), std::ostream_iterator<int>(os_std, " "));
    ural::copy(xs, ural::make_ostream_sequence(os_ural, " "));

    BOOST_CHECK_EQUAL(os_std.str(), os_ural.str());
}

BOOST_AUTO_TEST_CASE(ostream_sequence_test_no_delim)
{
    std::vector<int> const xs = {1, 2, 3, 4, 5};

    std::ostringstream os_std;
    std::ostringstream os_ural;

    std::copy(xs.begin(), xs.end(), std::ostream_iterator<int>(os_std));
    ural::copy(xs, ural::make_ostream_sequence<int>(os_ural));

    BOOST_CHECK_EQUAL(os_std.str(), os_ural.str());
}

BOOST_AUTO_TEST_CASE(ostream_sequence_test_auto_no_delim)
{
    std::vector<int> const xs = {1, 2, 3, 4, 5};

    std::ostringstream os_std;
    std::ostringstream os_ural;

    std::copy(xs.begin(), xs.end(), std::ostream_iterator<int>(os_std));
    ural::copy(xs, ural::make_ostream_sequence(os_ural));

    BOOST_CHECK_EQUAL(os_std.str(), os_ural.str());
}

BOOST_AUTO_TEST_CASE(reversed_reversed_test)
{
    std::vector<int> const xs = {1, 2, 3, 4, 5};
    auto s = ural::sequence(xs);
    auto rr = s | ural::reversed | ural::reversed;

    BOOST_CHECK_EQUAL(ural::abi::demangle_name(typeid(s).name()),
                      ural::abi::demangle_name(typeid(rr).name()));
    BOOST_CHECK(typeid(s).name() == typeid(rr).name());
    BOOST_CHECK(typeid(s) == typeid(rr));
}

BOOST_AUTO_TEST_CASE(reversed_iterators_to_sequence_test)
{
    std::vector<int> const xs = {1, 2, 3, 4, 5};
    auto r_begin = xs.rbegin();
    auto r_end = xs.rend();

    auto rs = ural::make_iterator_sequence(r_begin, r_end);

    typedef ural::reverse_sequence<ural::iterator_sequence<decltype(xs.begin())>>
        RSequence;

    static_assert(std::is_same<decltype(rs), RSequence>::value, "");

    BOOST_CHECK(r_begin.base() == rs.base().end());
    BOOST_CHECK(r_end.base() == rs.base().begin());

    std::vector<int> result;
    ural::copy(rs, std::back_inserter(result));

    BOOST_CHECK_EQUAL_COLLECTIONS(xs.begin(), xs.end(),
                                  result.rbegin(), result.rend());
}

BOOST_AUTO_TEST_CASE(move_iterators_to_sequence_test)
{
    std::vector<int> xs = {1, 2, 3, 4, 5};
    auto m_begin = std::make_move_iterator(xs.begin());
    auto m_end = std::make_move_iterator(xs.end());

    auto ms = ural::make_iterator_sequence(m_begin, m_end);

    typedef ural::move_sequence<ural::iterator_sequence<decltype(xs.begin())>>
        MSequence;

    static_assert(std::is_same<decltype(ms), MSequence>::value, "");

    BOOST_CHECK(m_begin.base() == ms.base().begin());
    BOOST_CHECK(m_end.base() == ms.base().end());
}

BOOST_AUTO_TEST_CASE(sink_output_sequence_test_auto)
{
    ural::sink_sequence<> sink {};

    static_assert(std::is_empty<decltype(sink)>::value, "too big");
    BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassSequence<decltype(sink)>));
    BOOST_CONCEPT_ASSERT((ural::concepts::WritableSequence<decltype(sink), int>));

    *sink = 42;
}

 BOOST_AUTO_TEST_CASE(sink_output_sequence_test)
{
    ural::sink_sequence<int> sink;

    static_assert(std::is_empty<decltype(sink)>::value, "too big");

    BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassSequence<decltype(sink)>));
    BOOST_CONCEPT_ASSERT((ural::concepts::WritableSequence<decltype(sink), int>));

    *sink = 42;
}

BOOST_AUTO_TEST_CASE(istream_sequence_no_default_ctor_test)
{
    int var = 13;

    std::istringstream is("42");

    ural::istream_sequence<std::reference_wrapper<int>, std::istream>
        seq{is, std::ref(var)};

    std::ref(var) = *seq;

    BOOST_CHECK_EQUAL(42, var);
}

BOOST_AUTO_TEST_CASE(ostream_sequence_default_test)
{
    typedef std::ostream OStream;

    typedef ural::ostream_sequence<OStream, int> S1;

    static_assert(std::is_same<std::string, S1::delimeter_type>::value, "");

    typedef ural::ostream_sequence<OStream> S2;

    static_assert(std::is_same<std::string, S2::delimeter_type>::value, "");

    typedef ural::ostream_sequence<> S3;

    static_assert(std::is_same<std::ostream, S3::ostream_type>::value, "");
    static_assert(std::is_same<std::string, S3::delimeter_type>::value, "");
}

BOOST_AUTO_TEST_CASE(arithmetic_progression_size_test)
{
    auto s = ural::make_arithmetic_progression(0, 1);

    BOOST_CHECK_LE(sizeof(s), sizeof(0) * 3);
}

BOOST_AUTO_TEST_CASE(all_tuples_test)
{
    auto digits = ural::make_arithmetic_progression(0, 1) | ural::taken(10);
    auto s2 = ural::make_all_tuples_sequence(digits, digits);

    std::set<int> r2;

    for(; !!s2; ++ s2)
    {
        auto t = *s2;
        r2.insert(t[ural::_1] * 10 + t[ural::_2]);
    }

    BOOST_CHECK_EQUAL(100, r2.size());
    BOOST_CHECK_EQUAL(0,  *r2.begin());
    BOOST_CHECK_EQUAL(99, *r2.rbegin());
}

BOOST_AUTO_TEST_CASE(all_tuples_is_sorted_test)
{
    auto digits = ural::make_arithmetic_progression(0, 1) | ural::taken(10);
    auto s2 = ural::make_all_tuples_sequence(digits, digits);

    std::vector<decltype(s2)::value_type> r2;

    ural::copy(s2, r2 | ural::back_inserter);

    BOOST_CHECK_EQUAL(100, r2.size());
    BOOST_CHECK(std::is_sorted(r2.begin(), r2.end()));
}

BOOST_AUTO_TEST_CASE(sequence_for_each_test)
{
    std::vector<int> x_std = {1, 2, 3, 4, 5};
    auto x_ural = x_std;

    for(auto & x : x_std)
    {
        x *= 2;
    }

    for(auto & x : ural::sequence(x_ural))
    {
        x *= 2;
    }

    BOOST_CHECK_EQUAL_COLLECTIONS(x_std.begin(), x_std.end(),
                                  x_ural.begin(), x_ural.end());
}

BOOST_AUTO_TEST_CASE(filtered_sequence_for_each)
{
    std::vector<int> x = {1, 2, 3, 4, 5, 6, 7, 8};
    auto s = x | ural::filtered([](int & x) { return x % 3 == 0;});

    std::vector<int> r;
    std::vector<int> const z  = {3, 6};

    for(auto & x : s)
    {
        r.push_back(x);
    }

    BOOST_CHECK_EQUAL_COLLECTIONS(z.begin(), z.end(), r.begin(), r.end());
}

BOOST_AUTO_TEST_CASE(map_keys_test)
{
    std::vector<int> const x = {1, 2, 3, 4, 5};
    std::vector<char> const y = {'a', 'b', 'c', 'd', 'e'};

    assert(x.size() == y.size());

    std::map<int, char> xy;

    for(size_t i = 0; i < x.size(); ++ i)
    {
        xy[x[i]] = y[i];
    }

    auto x_seq = xy | ural::map_keys;

    BOOST_CHECK(ural::equal(x, x_seq));

    // @todo тест map_values
}

BOOST_AUTO_TEST_CASE(set_inserter_test)
{
    std::vector<int> const xs = {1, 2, 3, 4, 5, 1, 3, 5, 2, 4, 6};

    std::set<int> z;

    for(auto & x : xs)
    {
        z.insert(x);
    }

    std::set<int> z_ural;
    ural::copy(xs, z_ural | ural::set_inserter);

    BOOST_CHECK_EQUAL_COLLECTIONS(z.begin(), z.end(),
                                  z_ural.begin(), z_ural.end());
}
