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

#include <ural/sequence/sink.hpp>
#include <ural/algorithm.hpp>
#include <ural/sequence/all.hpp>
#include <ural/sequence/zip.hpp>
#include <ural/sequence/map.hpp>
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

BOOST_AUTO_TEST_CASE(istream_sequence_regression_1)
{
    std::istringstream str1("0.1 0.2 0.3 0.4");
    std::istringstream str2("0.1 0.2 0.3 0.4");

    std::vector<double> r_std;
    std::copy(std::istream_iterator<double>(str1),
              std::istream_iterator<double>(),
              r_std | ural::back_inserter);

    std::vector<double> r_ural;

    for(auto const & x : ural::make_istream_sequence<double>(str2))
    {
        r_ural.push_back(x);
    }

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

    typedef decltype(seq) Sequence;

    BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassSequence<Sequence>));
    BOOST_CONCEPT_ASSERT((ural::concepts::ReadableSequence<Sequence>));

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

    BOOST_CHECK_EQUAL(100U, r2.size());
    BOOST_CHECK_EQUAL(0,  *r2.begin());
    BOOST_CHECK_EQUAL(99, *r2.rbegin());
}

BOOST_AUTO_TEST_CASE(geometric_progression_test)
{
    std::vector<int> const zs = {1, 2, 4, 8, 16, 32, 64};

    auto const xs = ural::make_arithmetic_progression(1, 2, ural::multiplies<>{})
                  | ural::taken(zs.size())
                  | ural::to_container<std::vector>{};

    BOOST_CHECK_EQUAL_COLLECTIONS(zs.begin(), zs.end(),
                                  xs.begin(), xs.end());
}

BOOST_AUTO_TEST_CASE(arithmetic_progression_concept_check)
{
    using namespace ural;
    typedef arithmetic_progression<int, use_default, single_pass_traversal_tag> SP;
    typedef arithmetic_progression<int, use_default, forward_traversal_tag> Fw;
    typedef arithmetic_progression<int, use_default, random_access_traversal_tag> RA;

    BOOST_CHECK_LE(sizeof(SP), 2 * sizeof(int));
    BOOST_CHECK_LE(sizeof(Fw), 3 * sizeof(int));
    BOOST_CHECK_LE(sizeof(RA), 3 * sizeof(int));

    BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassSequence<SP>));
    BOOST_CONCEPT_ASSERT((ural::concepts::ForwardSequence<Fw>));
    BOOST_CONCEPT_ASSERT((ural::concepts::ForwardSequence<RA>));
    // @todo BOOST_CONCEPT_ASSERT((ural::concepts::RandomAccessSequence<RA>));
}

BOOST_AUTO_TEST_CASE(arithmetic_progression_equality_test)
{
    struct Inner
    {
        static int add_1(int x, int y)
        {
            return x+y;
        }

        static int add_2(int x, int y)
        {
            return x*y;
        }
    };

    auto a1 = ural::make_arithmetic_progression(2, 2, &Inner::add_1);
    auto a1c = a1;
    auto a2 = ural::make_arithmetic_progression(1, 2, &Inner::add_1);
    auto a3 = ural::make_arithmetic_progression(2, 1, &Inner::add_1);
    auto a4 = ural::make_arithmetic_progression(2, 2, &Inner::add_2);

    BOOST_CHECK(a1 == a1);
    BOOST_CHECK(a1 == a1c);

    BOOST_CHECK(a1 != a2);
    BOOST_CHECK(a1 != a3);
    BOOST_CHECK(a1 != a4);

    BOOST_CHECK(a2 != a3);
    BOOST_CHECK(a2 != a4);

    BOOST_CHECK(a3 != a4);
}

BOOST_AUTO_TEST_CASE(all_tuples_is_sorted_test)
{
    auto digits = ural::make_arithmetic_progression(0, 1) | ural::taken(10);
    auto s2 = ural::make_all_tuples_sequence(digits, digits);

    std::vector<decltype(s2)::value_type> r2;

    ural::copy(s2, r2 | ural::back_inserter);

    BOOST_CHECK_EQUAL(100U, r2.size());
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

BOOST_AUTO_TEST_CASE(zip_sequence_test)
{
    std::vector<int> const x = {1, 2, 3, 4, 5};
    std::vector<char> const y = {'a', 'b', 'c', 'd', 'e'};

    assert(x.size() == y.size());

    // std
    std::vector<std::tuple<int, char>> r_std;

    for(size_t i = 0; i < x.size(); ++ i)
    {
        r_std.emplace_back(x[i], y[i]);
    }

    // ural
    auto const r_ural = ural::make_zip_sequence(x, y)
                      | ural::to_container<std::vector>{};

    // проверка
    BOOST_CHECK_EQUAL(x.size(), r_ural.size());
    BOOST_CHECK_EQUAL(r_std.size(), r_ural.size());

    for(size_t i = 0; i < x.size(); ++ i)
    {
        BOOST_CHECK(r_std[i] == r_ural[i]);
    }
}

BOOST_AUTO_TEST_CASE(map_keys_and_values_test)
{
    std::vector<int> const x = {1, 2, 3, 4, 5};
    std::vector<char> const y = {'a', 'b', 'c', 'd', 'e'};

    assert(x.size() == y.size());

    std::map<int, char> const xy
        = ural::make_zip_sequence(x, y) | ural::to_map<std::map>{};

    BOOST_CHECK(ural::equal(xy | ural::map_keys, x));
    BOOST_CHECK(ural::equal(xy | ural::map_values, y));
}

BOOST_AUTO_TEST_CASE(set_inserter_test)
{
    std::vector<int> const xs = {1, 2, 3, 4, 5, 1, 3, 5, 2, 4, 6};

    std::set<int> const z(xs.begin(), xs.end());

    std::set<int> z_ural;
    ural::copy(xs, z_ural | ural::set_inserter);

    BOOST_CHECK_EQUAL_COLLECTIONS(z.begin(), z.end(),
                                  z_ural.begin(), z_ural.end());
}

#include <ural/numeric/numbers_sequence.hpp>

BOOST_AUTO_TEST_CASE(numbers_range_test)
{
    std::vector<int> const z = {0, 1, 2, 3, 4, 5, 6};

    std::vector<int> x;

    for(auto i : ural::numbers(0, 7))
    {
        x.push_back(i);
    }

    BOOST_CHECK_EQUAL_COLLECTIONS(z.begin(), z.end(), x.begin(), x.end());
}

BOOST_AUTO_TEST_CASE(numbers_stride_range_test)
{
    std::vector<int> const z = {0, 2, 4, 6};

    std::vector<int> x;

    for(auto i : ural::numbers(0, 7, 2))
    {
        x.push_back(i);
    }

    BOOST_CHECK_EQUAL_COLLECTIONS(z.begin(), z.end(), x.begin(), x.end());
}

// @todo Интервал чисел с отрицательным шагом?


// Итераторы последовательностей
#include <ural/sequence/make.hpp>
BOOST_AUTO_TEST_CASE(iterator_sequence_iterators)
{
    typedef std::vector<int> Container;
    Container v1 = {0, 2, 4, 6};
    auto const v2 = v1;

    auto const s1 = ural::sequence(v1);
    auto const s2 = ural::sequence(v2);

    static_assert(std::is_same<decltype(s1.begin()), Container::iterator>::value, "");
    static_assert(std::is_same<decltype(s2.begin()), Container::const_iterator>::value, "");

    BOOST_CHECK(s1.begin() == v1.begin());
    BOOST_CHECK(s1.end() == v1.end());

    BOOST_CHECK(s2.begin() == v2.begin());
    BOOST_CHECK(s2.end() == v2.end());
}

BOOST_AUTO_TEST_CASE(reversed_iterator_sequence_iterators)
{
    typedef std::vector<int> Container;
    Container v1 = {0, 2, 4, 6};
    auto const v2 = v1;

    auto const rs1 = ural::sequence(v1) | ural::reversed;
    auto const rs2 = ural::sequence(v2) | ural::reversed;

    static_assert(std::is_same<decltype(begin(rs1)), Container::reverse_iterator>::value, "");
    static_assert(std::is_same<decltype(begin(rs2)), Container::const_reverse_iterator>::value, "");

    BOOST_CHECK(begin(rs1) == v1.rbegin());
    BOOST_CHECK(end(rs1) == v1.rend());

    BOOST_CHECK(begin(rs2) == v2.rbegin());
    BOOST_CHECK(end(rs2) == v2.rend());
}

BOOST_AUTO_TEST_CASE(moved_iterator_sequence_iterators)
{
    typedef std::vector<int> Container;
    Container v1 = {0, 2, 4, 6};
    auto const v2 = v1;

    auto const rs1 = ural::sequence(v1) | ural::moved;
    auto const rs2 = ural::sequence(v2) | ural::moved;

    static_assert(std::is_same<decltype(begin(rs1)), std::move_iterator<Container::iterator>>::value, "");
    static_assert(std::is_same<decltype(begin(rs2)), std::move_iterator<Container::const_iterator>>::value, "");

    BOOST_CHECK(begin(rs1) == std::make_move_iterator(v1.begin()));
    BOOST_CHECK(end(rs1) == std::make_move_iterator(v1.end()));

    BOOST_CHECK(begin(rs2) == std::make_move_iterator(v2.begin()));
    BOOST_CHECK(end(rs2) == std::make_move_iterator(v2.end()));
}
