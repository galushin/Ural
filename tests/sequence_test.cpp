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

#include <iterator>

#include <ural/abi.hpp>
#include <ural/algorithm.hpp>
#include <ural/sequence/all.hpp>
#include <ural/sequence/sink.hpp>

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

    BOOST_CHECK(r_begin.base() == rs.base().begin());
    BOOST_CHECK(r_end.base() == rs.base().end());
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
