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

#include <ural/numeric/numbers_sequence.hpp>
#include <ural/math/rational.hpp>
#include <ural/sequence/all.hpp>
#include <ural/algorithm.hpp>
#include <ural/container/flat_set.hpp>

#include <boost/mpl/list.hpp>
#include <boost/test/unit_test.hpp>
#include "defs.hpp"

#include <forward_list>
#include <iterator>
#include <set>
#include <map>

namespace
{
    namespace ural_ex = ural::experimental;
}

BOOST_AUTO_TEST_CASE(assumed_infinite_test_empty_becomes_not)
{
    std::vector<int> ev;

    auto s = ural::cursor(ev);
    auto is = s | ural_ex::assumed_infinite;

    BOOST_CHECK(!s);
    BOOST_CHECK(!!is);

    BOOST_CHECK(is.base() == s);
}

BOOST_AUTO_TEST_CASE(assumed_infinite_traversed_front)
{
    std::vector<int> const z{11, 11, 22, 33, 55};

    auto s    = ural::cursor(z);
    auto s_ai = s | ural_ex::assumed_infinite;

    ural::advance(s, 3);
    ural::advance(s_ai, 3);

    auto const actual = s_ai.traversed_front();
    auto const expected = s.traversed_front();;

    BOOST_CHECK(actual == expected);
}

using ::ural::value_type_t;

BOOST_AUTO_TEST_CASE(cursor_for_lvalue_container)
{
    std::vector<int> const z{11, 11, 22, 33, 55};

    auto seq = ::ural::cursor(z);

    typedef ural::iterator_cursor<decltype(z.begin())> IS;

    static_assert(std::is_same<decltype(seq), IS>::value, "");
}

BOOST_AUTO_TEST_CASE(cursor_for_rvalue_container)
{
    std::vector<int> const z{11, 11, 22, 33, 55};
    auto x = z;
    auto const old_x_data = x.data();

    auto seq = ural::cursor(std::move(x));

    BOOST_CHECK_EQUAL(seq.cargo().data(), old_x_data);

    URAL_CHECK_EQUAL_RANGES(seq.cargo(), z);

    BOOST_CHECK(ural::equal(seq, z) == true);
}

BOOST_AUTO_TEST_CASE(rvalue_container_sort_test)
{
    std::vector<int> const z{3, 1, 4, 1, 5};

    auto x1 = z;
    ural::sort(x1);

    auto x2 = z;
    auto seq = ::ural::cursor(std::move(x2));
    ural::sort(seq);

    URAL_CHECK_EQUAL_RANGES(x1, seq.cargo());

    BOOST_CHECK(ural::equal(x1, seq));
}

BOOST_AUTO_TEST_CASE(rvalue_container_reverse_test)
{
    std::vector<int> const z{3, 1, 4, 1, 5};

    auto x1 = z;
    ural::reverse(x1);

    auto x2 = z;
    auto seq = ::ural::cursor(std::move(x2));
    ural::reverse(seq);

    URAL_CHECK_EQUAL_RANGES(x1, seq.cargo());

    BOOST_CHECK(ural::equal(x1, seq));
}

BOOST_AUTO_TEST_CASE(rvalue_container_partial_sort_test)
{
    std::vector<int> const z{3, 1, 4, 1, 5};
    auto const part = z.size() / 2;

    auto x1 = z;
    ural::partial_sort(x1, part);

    auto x2 = z;
    auto seq = ::ural::cursor(std::move(x2));
    ural::partial_sort(seq, part);

    URAL_CHECK_EQUAL_RANGES(x1, seq.cargo());

    BOOST_CHECK(ural::equal(x1, seq));
}

BOOST_AUTO_TEST_CASE(istream_cursor_test)
{
    std::istringstream str1("0.1 0.2 0.3 0.4");
    std::istringstream str2("0.1 0.2 0.3 0.4");

    std::vector<double> r_std;
    std::copy(std::istream_iterator<double>(str1),
              std::istream_iterator<double>(),
              r_std | ural::back_inserter);

    std::vector<double> r_ural;
    ural::copy(ural_ex::make_istream_cursor<double>(str2),
               r_ural | ural::back_inserter);

    URAL_CHECK_EQUAL_RANGES(r_std, r_ural);
}

BOOST_AUTO_TEST_CASE(istream_cursor_regression_1)
{
    std::istringstream str1("0.1 0.2 0.3 0.4");
    std::istringstream str2("0.1 0.2 0.3 0.4");

    std::vector<double> r_std;
    std::copy(std::istream_iterator<double>(str1),
              std::istream_iterator<double>(),
              r_std | ural::back_inserter);

    std::vector<double> r_ural;

    // @todo можно ли это заменить на for для интервалаов?
    for(auto s = ural_ex::make_istream_cursor<double>(str2); !!s; ++ s)
    {
        r_ural.push_back(*s);
    }

    URAL_CHECK_EQUAL_RANGES(r_std, r_ural);
}

BOOST_AUTO_TEST_CASE(ostream_cursor_test)
{
    typedef int Value;
    std::vector<Value> const xs = {1, 2, 3, 4, 5};

    std::ostringstream os_std;
    std::ostringstream os_ural;

    std::copy(xs.begin(), xs.end(), std::ostream_iterator<Value>(os_std, " "));

    auto cur = ural_ex::make_ostream_cursor<Value>(os_ural, " ");
    std::copy(xs.begin(), xs.end(), std::move(cur));

    BOOST_CHECK_EQUAL(os_std.str(), os_ural.str());

    BOOST_CONCEPT_ASSERT((boost::OutputIterator<decltype(cur), Value>));
}

BOOST_AUTO_TEST_CASE(ostream_cursor_test_auto)
{
    typedef int Value;
    std::vector<Value> const xs = {1, 2, 3, 4, 5};

    std::ostringstream os_std;
    std::ostringstream os_ural;

    std::copy(xs.begin(), xs.end(), std::ostream_iterator<Value>(os_std, " "));

    auto cur = ural_ex::make_ostream_cursor(os_ural, " ");
    ural::copy(xs, std::move(cur));

    BOOST_CHECK_EQUAL(os_std.str(), os_ural.str());

    BOOST_CONCEPT_ASSERT((boost::OutputIterator<decltype(cur), Value>));
}

BOOST_AUTO_TEST_CASE(ostream_cursor_test_no_delim)
{
    std::vector<int> const xs = {1, 2, 3, 4, 5};

    std::ostringstream os_std;
    std::ostringstream os_ural;

    std::copy(xs.begin(), xs.end(), std::ostream_iterator<int>(os_std));
    ural::copy(xs, ural_ex::make_ostream_cursor<int>(os_ural));

    BOOST_CHECK_EQUAL(os_std.str(), os_ural.str());
}

BOOST_AUTO_TEST_CASE(ostream_cursor_test_auto_no_delim)
{
    std::vector<int> const xs = {1, 2, 3, 4, 5};

    std::ostringstream os_std;
    std::ostringstream os_ural;

    std::copy(xs.begin(), xs.end(), std::ostream_iterator<int>(os_std));
    ural::copy(xs, ural_ex::make_ostream_cursor(os_ural));

    BOOST_CHECK_EQUAL(os_std.str(), os_ural.str());
}

BOOST_AUTO_TEST_CASE(move_iterators_to_cursor_test)
{
    std::vector<int> xs = {1, 2, 3, 4, 5};
    auto m_begin = std::make_move_iterator(xs.begin());
    auto m_end = std::make_move_iterator(xs.end());

    auto ms = ural_ex::make_iterator_cursor(m_begin, m_end);

    using MCursor = ural_ex::move_cursor<ural::iterator_cursor<decltype(xs.begin())>>;

    static_assert(std::is_same<decltype(ms), MCursor>::value, "");

    BOOST_CHECK(m_begin.base() == ms.base().begin());
    BOOST_CHECK(m_end.base() == ms.base().end());
}

BOOST_AUTO_TEST_CASE(ural_move_iterators_to_cursor_test)
{
    std::vector<int> xs = {1, 2, 3, 4, 5};
    auto m_begin = ural::make_move_iterator(xs.begin());
    auto m_end = ural::make_move_iterator(xs.end());

    auto mc = ural_ex::make_iterator_cursor(m_begin, m_end);

    using MCursor = ural_ex::move_cursor<ural::iterator_cursor<decltype(xs.begin())>>;

    static_assert(std::is_same<decltype(mc), MCursor>::value, "");

    BOOST_CHECK(m_begin.base() == mc.base().begin());
    BOOST_CHECK(m_end.base() == mc.base().end());
}

BOOST_AUTO_TEST_CASE(sink_output_cursor_test_auto)
{
    ural_ex::sink_cursor<> sink {};

    static_assert(std::is_empty<decltype(sink)>::value, "too big");
    BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassCursor<decltype(sink)>));
    BOOST_CONCEPT_ASSERT((ural::concepts::WritableCursor<decltype(sink), int>));
    BOOST_CONCEPT_ASSERT((ural::concepts::WritableCursor<decltype(sink), std::string>));

    *sink = 42;
    *sink = std::string("42");

     BOOST_CONCEPT_ASSERT((boost::OutputIterator<decltype(sink), int>));
     BOOST_CONCEPT_ASSERT((boost::OutputIterator<decltype(sink), std::string>));
}

 BOOST_AUTO_TEST_CASE(sink_output_cursor_test)
{
    typedef int Value;
    ural_ex::sink_cursor<Value> sink;

    static_assert(std::is_empty<decltype(sink)>::value, "too big");
    BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassCursor<decltype(sink)>));
    BOOST_CONCEPT_ASSERT((ural::concepts::WritableCursor<decltype(sink), Value>));

    *sink = 42;

    using Cursor = decltype(sink);

     BOOST_CONCEPT_ASSERT((boost::OutputIterator<Cursor, Value>));
}

BOOST_AUTO_TEST_CASE(istream_cursor_no_default_ctor_test)
{
    int var = 13;

    std::istringstream is("42");

    ural_ex::istream_cursor<std::istream &, std::reference_wrapper<int>>
        cur(is, std::ref(var));

    using Cursor = decltype(cur);

    BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassCursor<Cursor>));
    BOOST_CONCEPT_ASSERT((ural::concepts::ReadableCursor<Cursor>));

    std::ref(var) = *cur;

    BOOST_CHECK_EQUAL(42, var);
}

BOOST_AUTO_TEST_CASE(ostream_cursor_default_test)
{
    typedef std::ostream OStream;

    typedef ural_ex::ostream_cursor<OStream, int> S1;

    static_assert(std::is_same<std::string, S1::delimiter_type>::value, "");

    typedef ural_ex::ostream_cursor<OStream> S2;

    static_assert(std::is_same<std::string, S2::delimiter_type>::value, "");

    typedef ural_ex::ostream_cursor<> S3;

    static_assert(std::is_same<std::ostream, S3::ostream_type>::value, "");
    static_assert(std::is_same<std::string, S3::delimiter_type>::value, "");
}

BOOST_AUTO_TEST_CASE(arithmetic_progression_cursor_size_test)
{
    auto s = ural_ex::make_arithmetic_progression(0, 1);

    BOOST_CHECK_LE(sizeof(s), sizeof(0) * 3);
}

BOOST_AUTO_TEST_CASE(geometric_progression_test)
{
    std::vector<int> const zs = {1, 2, 4, 8, 16, 32, 64};

    auto const xs = ural_ex::make_arithmetic_progression(1, 2, ural::multiplies<>{})
                  | ural_ex::taken(zs.size())
                  | ural_ex::to_container<std::vector>{};

    URAL_CHECK_EQUAL_RANGES(zs, xs);
}

BOOST_AUTO_TEST_CASE(arithmetic_progression_cursor_concept_check)
{
    using namespace ural_ex;
    using namespace ural;

    typedef arithmetic_progression_cursor<int, use_default, single_pass_cursor_tag> SP;
    typedef arithmetic_progression_cursor<int, use_default, forward_cursor_tag> Fw;
    typedef arithmetic_progression_cursor<int, use_default, random_access_cursor_tag> RA;

    BOOST_CHECK_LE(sizeof(SP), 2 * sizeof(int));
    BOOST_CHECK_LE(sizeof(Fw), 3 * sizeof(int));
    BOOST_CHECK_LE(sizeof(RA), 3 * sizeof(int));

    BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassCursor<SP>));
    BOOST_CONCEPT_ASSERT((ural::concepts::ForwardCursor<Fw>));
    BOOST_CONCEPT_ASSERT((ural::concepts::ForwardCursor<RA>));
    // @todo BOOST_CONCEPT_ASSERT((ural::concepts::RandomAccessCursor<RA>));
}

BOOST_AUTO_TEST_CASE(arithmetic_progression_cursor_equality_test)
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

    auto a1 = ural_ex::make_arithmetic_progression(2, 2, &Inner::add_1);
    auto a1c = a1;
    auto a2 = ural_ex::make_arithmetic_progression(1, 2, &Inner::add_1);
    auto a3 = ural_ex::make_arithmetic_progression(2, 1, &Inner::add_1);
    auto a4 = ural_ex::make_arithmetic_progression(2, 2, &Inner::add_2);

    BOOST_CHECK(a1 == a1);
    BOOST_CHECK(a1 == a1c);

    BOOST_CHECK(a1 != a2);
    BOOST_CHECK(a1 != a3);
    BOOST_CHECK(a1 != a4);

    BOOST_CHECK(a2 != a3);
    BOOST_CHECK(a2 != a4);

    BOOST_CHECK(a3 != a4);
}

BOOST_AUTO_TEST_CASE(arithmetic_progression_cursor_single_pass_test_check)
{
    using Cursor = ural_ex::arithmetic_progression_cursor<int, ural::use_default,
                                                          ural::single_pass_cursor_tag>;

    BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassCursor<Cursor>));
    BOOST_CONCEPT_ASSERT((ural::concepts::ReadableCursor<Cursor>));

    auto seq = Cursor(1, 2);
    BOOST_CHECK_EQUAL(seq.front(), 1);

    ++ seq;
    BOOST_CHECK_EQUAL(seq.front(), 1+2);
}

BOOST_AUTO_TEST_CASE(geometric_progression_plus_assign_regression)
{
    auto const b0 = 3;
    auto const q  = 2;

    auto const gp = ural_ex::make_geometric_progression(b0, q);

    auto const index = 5;

    auto gp1 = gp;
    gp1 += index;

    BOOST_CHECK_EQUAL(*gp1, b0 * ural::natural_power(q, index));
}

BOOST_AUTO_TEST_CASE(geometric_progression_function_pointer_regression_77)
{
    using Integer = int;
    auto const b0 = Integer(3);
    auto const q  = Integer(2);

    auto const op = +[](Integer x, Integer y) { return x * y; };
    using BinOp = Integer(*)(Integer, Integer);

    auto const gp = ural_ex::arithmetic_progression_cursor<Integer, BinOp>(b0, q, op);

    auto const index = 5;

    auto gp1 = gp;
    gp1 += index;

    BOOST_CHECK_EQUAL(*gp1, b0 * ural::natural_power(q, index));
}

BOOST_AUTO_TEST_CASE(cartesian_product_cursor_is_sorted_test)
{
    auto digits = ural::numbers(0, 10);
    auto cur2 = ural_ex::make_cartesian_product_cursor(digits, digits);

    std::vector<value_type_t<decltype(cur2)>> r2;

    ural::copy(cur2, r2 | ural::back_inserter);

    BOOST_CHECK_EQUAL(100U, r2.size());
    BOOST_CHECK(std::is_sorted(r2.begin(), r2.end()));
}

BOOST_AUTO_TEST_CASE(cartesian_product_cursor_regression_ctor)
{
    std::vector<int> const x_std = {1, 2, 3, 4, 5};

    auto seq = ural::cursor(x_std);
    ++ seq;

    auto ts = ural_ex::make_cartesian_product_cursor(seq, seq);

    for(; !!ts; ++ ts)
    {
        BOOST_CHECK_NE(1, (*ts)[ural::_1]);
        BOOST_CHECK_NE(1, (*ts)[ural::_2]);
    }
}

BOOST_AUTO_TEST_CASE(cursor_for_each_test)
{
    std::vector<int> x_std = {1, 2, 3, 4, 5};
    auto x_ural = x_std;

    for(auto & x : x_std)
    {
        x *= 2;
    }

    for(auto & x : ural::cursor(x_ural))
    {
        x *= 2;
    }

    URAL_CHECK_EQUAL_RANGES(x_std, x_ural);
}

BOOST_AUTO_TEST_CASE(filtered_test)
{
    typedef int Type;
    std::vector<Type> const xs = {25, -15, 5, -5, 15};

    auto const pred = [](Type i){return !(i<0);};

    using Cursor = decltype(ural::cursor(xs));

    static_assert(std::is_empty<decltype(pred)>::value, "");
    BOOST_CHECK_EQUAL(sizeof(Cursor), sizeof(boost::compressed_pair<Cursor, decltype(pred)>));

    std::vector<Type> r_std;

    std::copy_if (xs.begin(), xs.end(), std::back_inserter(r_std) , pred);

    auto const r_ural
        = xs | ural_ex::filtered(pred) | ural_ex::to_container<std::vector>{};

    URAL_CHECK_EQUAL_RANGES(r_std, r_ural);
}

BOOST_AUTO_TEST_CASE(filtered_test_input)
{
    typedef int Type;
    std::vector<Type> const src_std = {25, -15, 5, -5, 15};
    ural_test::istringstream_helper<Type> const src_ural(src_std.begin(), src_std.end());

    auto const pred = +[](Type i){return !(i<0);};

    std::vector<Type> r_std;
    std::copy_if(src_std.begin(), src_std.end(), std::back_inserter(r_std), pred);

    auto seq = std::move(src_ural) | ural_ex::filtered(pred);

    std::vector<Type> r_ural;
    ural::copy(std::move(seq), r_ural | ural::back_inserter)[ural::_1];

    URAL_CHECK_EQUAL_RANGES(r_std, r_ural);
}

BOOST_AUTO_TEST_CASE(filtered_getters_test)
{
    typedef int Type;
    std::vector<Type> const xs = {25, -15, 5, -5, 15};
    std::vector<Type> const xs_1 = xs;

    auto const pred_1 = +[](Type i){return !(i<0);};
    auto const pred_2 = +[](Type i){return !(i<=0);};

    auto s1 = xs | ural_ex::filtered(pred_1);
    auto s11 = xs | ural_ex::filtered(pred_1);
    auto s2 = xs | ural_ex::filtered(pred_2);
    auto s3 = xs_1 | ural_ex::filtered(pred_1);

    BOOST_CHECK(pred_1 == s1.predicate());
    BOOST_CHECK(pred_2 == s2.predicate());

    BOOST_CHECK(s1 == s1);
    BOOST_CHECK(s1 == s11);
    BOOST_CHECK(s2 == s2);
    BOOST_CHECK(s1 != s2);
    BOOST_CHECK(s1 != s3);
}

BOOST_AUTO_TEST_CASE(filtered_cursor_for_each)
{
    std::forward_list<int> xs = {1, 2, 3, 4, 5, 6, 7, 8};
    auto cur = xs | ural_ex::filtered([](int const & x) { return x % 3 == 0;});

    BOOST_CONCEPT_ASSERT((ural::concepts::ForwardCursor<decltype(cur)>));

    std::vector<int> r;
    std::vector<int> const z  = {3, 6};

    // Цикл вместо алгоритма используется специально, чтобы проверить, что
    // тип ссылки - неконстантная ссылка
    for(value_type_t<decltype(xs)> & x : cur)
    {
        r.push_back(x);
    }

    URAL_CHECK_EQUAL_RANGES(z, r);
}

BOOST_AUTO_TEST_CASE(filtered_cursor_is_permuation)
{
    std::list<int> const xs1 = {3, 1, 4, 1, 5, 9, 2, 6};
    std::forward_list<int> const xs2(xs1.rbegin(), xs1.rend());

    auto const pred = +[](int const & x) { return x % 3 == 0;};

    BOOST_CHECK(ural::is_permutation(xs1 | ural_ex::filtered(pred),
                                     xs2 | ural_ex::filtered(pred)));
}

BOOST_AUTO_TEST_CASE(taken_exactly_taken_exactly_test_shorter)
{
    std::vector<int> const src = {1, 2, 3, 4, 5};

    auto const n1 = 4;
    auto const n2 = 3;

    auto s_composite = src | ural_ex::taken_exactly(n1) | ural_ex::taken_exactly(n2);
    auto s_direct = src | ural_ex::taken_exactly(std::min(n1, n2));

    static_assert(std::is_same<decltype(s_composite), decltype(s_direct)>::value, "");

    BOOST_CHECK(s_direct == s_composite);
}

BOOST_AUTO_TEST_CASE(taken_exactly_taken_exactly_test_longer)
{
    std::vector<int> const src = {1, 2, 3, 4, 5};

    auto const n1 = 3;
    auto const n2 = 4;

    auto s_composite = src | ural_ex::taken_exactly(n1) | ural_ex::taken_exactly(n2);
    auto s_direct = src | ural_ex::taken_exactly(std::min(n1, n2));

    static_assert(std::is_same<decltype(s_composite), decltype(s_direct)>::value, "");

    BOOST_CHECK(s_direct == s_composite);
}

// @todo различные комбинации taken taken_exactly, большей и меньшей длины

BOOST_AUTO_TEST_CASE(zip_cursor_bases_access)
{
    std::vector<int> const x = {1, 2, 3, 4, 5};
    std::vector<char> const y = {'a', 'b', 'c', 'd', 'e'};

    auto xy_zip = ural_ex::make_zip_cursor(x, y);

    BOOST_CHECK(xy_zip.bases()[ural::_1].begin() == x.begin());
    BOOST_CHECK(xy_zip.bases()[ural::_1].end() == x.end());
    BOOST_CHECK(xy_zip.bases()[ural::_2].begin() == y.begin());
    BOOST_CHECK(xy_zip.bases()[ural::_2].end() == y.end());

    URAL_CHECK_EQUAL_RANGES(xy_zip.bases()[ural::_1], x);
    URAL_CHECK_EQUAL_RANGES(xy_zip.bases()[ural::_2], y);
}

BOOST_AUTO_TEST_CASE(zip_cursor_test)
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
    auto const r_ural = ural_ex::make_zip_cursor(x, y)
                      | ural_ex::to_container<std::vector>{};

    // проверка
    BOOST_CHECK_EQUAL(x.size(), r_ural.size());
    BOOST_CHECK_EQUAL(r_std.size(), r_ural.size());

    for(size_t i = 0; i < x.size(); ++ i)
    {
        BOOST_CHECK(r_std[i] == r_ural[i]);
    }
}

BOOST_AUTO_TEST_CASE(zip_cursor_traversed_front_test)
{
    std::vector<int> const x = {1, 2, 3, 4, 5};
    std::vector<char> const y = {'a', 'b', 'c', 'd', 'e', 'f'};

    auto const n = std::min(x.size() / 2, y.size() / 2);

    auto s1 = ural_ex::make_zip_cursor(x, y);
    auto s2 = ural_ex::make_zip_cursor(x | ural_ex::assumed_infinite,
                                         y | ural_ex::assumed_infinite);

    ural::advance(s1, n);
    ural::advance(s2, n);

    BOOST_CHECK(s1.traversed_front() == s2.traversed_front());
}

BOOST_AUTO_TEST_CASE(map_keys_and_values_test)
{
    std::vector<int> const x = {1, 2, 3, 4, 5};
    std::vector<char> const y = {'a', 'b', 'c', 'd', 'e'};

    assert(x.size() == y.size());

    std::map<int, char> const xy
        = ural_ex::make_zip_cursor(x, y) | ural_ex::to_map<std::map>{};

    auto sx = xy | ural_ex::map_keys;
    auto sy = xy | ural_ex::map_values;

    using Base_cursor = ural::cursor_type_t<decltype(xy) const &>;

    Base_cursor const sx_base = sx.base();
    Base_cursor const sy_base = sy.base();

    BOOST_CHECK(sx_base == ural::cursor(xy));
    BOOST_CHECK(sy_base == ural::cursor(xy));

    BOOST_CHECK(ural::equal(sx, x));
    BOOST_CHECK(ural::equal(sy, y));
}

BOOST_AUTO_TEST_CASE(set_inserter_container_access)
{
    std::set<int> s;

    auto seq = s | ural_ex::set_inserter;

    BOOST_CHECK_EQUAL(&s, &seq.container());
}

namespace
{
    typedef boost::mpl::list<std::set<int>, ural::experimental::flat_set<int>> Int_set_types;
}

BOOST_AUTO_TEST_CASE_TEMPLATE(set_inserter_test, Set, Int_set_types)
{
    std::vector<int> const xs = {1, 2, 3, 4, 5, 1, 3, 5, 2, 4, 6};

    Set const z(xs.begin(), xs.end());

    Set z_ural;
    ural::copy(xs, z_ural | ural_ex::set_inserter);

    URAL_CHECK_EQUAL_RANGES(z, z_ural);
}

BOOST_AUTO_TEST_CASE(numbers_sanity_test)
{
    auto const from = 2;
    auto const to = 7;

    // Инициализация
    auto const ns0 = ::ural::numbers(from, to);
    auto ns = ns0;
    auto const n1 = ns.size();

    BOOST_CONCEPT_ASSERT((ural::concepts::BidirectionalCursor<decltype(ns)>));
    BOOST_CONCEPT_ASSERT((ural::concepts::RandomAccessCursor<decltype(ns)>));

    BOOST_CHECK_EQUAL(*ns, from);
    BOOST_CHECK_GT(n1, 0);
    BOOST_CHECK(ns == ns.original());

    // Продвижение
    ++ ns;
    auto const n2 = ns.size();

    BOOST_CHECK_EQUAL(*ns, from+1);
    BOOST_CHECK_LT(n2, n1);

    BOOST_CHECK(ns.original() == ns0);

    // Отбрасывание пройденной передней части
    BOOST_CHECK(ns != ns.original());

    ns.shrink_front();

    BOOST_CHECK(!ns.traversed_front());
    BOOST_CHECK(!ns.traversed_back());
    BOOST_CHECK_EQUAL(ns.size(), n2);
    BOOST_CHECK(ns == ns.original());

    // Двусторонняя последовательность
    ns.pop_back();
    auto const n3 = ns.size();

    BOOST_CHECK_LT(n3, n2);

    BOOST_CHECK(ns != ns.original());

    ns.shrink_back();

    BOOST_CHECK(!ns.traversed_front());
    BOOST_CHECK(!ns.traversed_back());
    BOOST_CHECK_EQUAL(ns.size(), n3);
    BOOST_CHECK(ns == ns.original());

    // Произвольный доступ
    BOOST_CHECK(!ural::is_heap(ns));
    BOOST_CHECK(ural::is_heap(ns | ural_ex::reversed));

    auto const v = ns | ural_ex::to_container<std::vector>{};
    BOOST_CHECK(std::is_heap(v.rbegin(), v.rend()));
}

BOOST_AUTO_TEST_CASE(numbers_exhaust_test)
{
    auto const from = 2;
    auto const to = 7;

    // Передняя часть
    auto const ns0 = ::ural::numbers(from, to);
    auto ns1 = ns0;
    ural::exhaust_front(ns1);

    BOOST_CHECK(ns0 == ns1.traversed_front());
    BOOST_CHECK(!ns1.traversed_back());
    BOOST_CHECK(!ns1);

    // Задняя часть
    auto ns2 = ns0;
    ns2.exhaust_back();

    BOOST_CHECK(ns0 == ns2.traversed_back());
    BOOST_CHECK(!ns2.traversed_front());
    BOOST_CHECK(!ns2);
}

BOOST_AUTO_TEST_CASE(numbers_range_test)
{
    std::vector<int> const z = {0, 1, 2, 3, 4, 5, 6};

    std::vector<int> x;

    for(auto i : ural::numbers(0, 7))
    {
        x.push_back(i);
    }

    URAL_CHECK_EQUAL_RANGES(z, x);
}

BOOST_AUTO_TEST_CASE(numbers_step_sanity_test)
{
    auto const from = 2;
    auto const to = 8;
    auto const step = 3;

    auto ns = ::ural::numbers(from, to, step);
    auto const n1 = ns.size();

    BOOST_CHECK_EQUAL(*ns, from);
    BOOST_CHECK_GE(n1, 0);

    ++ ns;
    auto const n2 = ns.size();

    BOOST_CHECK_EQUAL(*ns, from + step);
    BOOST_CHECK_LE(n2, n1);

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(numbers_size_exact_test)
{
    auto const ns = ::ural::numbers(2, 8, 3);

    auto const v = ns | ural_ex::to_container<std::vector>{};

    BOOST_CHECK_EQUAL(ns.size(), ural::to_signed(v.size()));
}

BOOST_AUTO_TEST_CASE(numbers_size_test)
{
    auto const ns = ::ural::numbers(2, 7, 3);

    auto const v = ns | ural_ex::to_container<std::vector>{};

    BOOST_CHECK_EQUAL(ns.size(), ural::to_signed(v.size()));
}

BOOST_AUTO_TEST_CASE(numbers_copy_test)
{
    auto const ns = ::ural::numbers(2, 7, 3);

    std::vector<int> v;
    auto result = ural::copy(ns, v | ural::back_inserter);

    BOOST_CHECK(result[ural::_1].traversed_front() == ns);
}

BOOST_AUTO_TEST_CASE(numbers_copy_backward_test)
{
    auto const ns = ::ural::numbers(2, 7, 3);

    std::vector<int> v(ns.size());
    auto result = ural::copy_backward(ns, v);

    BOOST_CHECK(!result[ural::_1]);
    BOOST_CHECK(!result[ural::_1].traversed_front());
    BOOST_CHECK(result[ural::_1].original() == ns);
    BOOST_CHECK(result[ural::_1].traversed_back() == ns);
}

BOOST_AUTO_TEST_CASE(numbers_stride_range_test)
{
    std::vector<int> const z = {0, 2, 4, 6};

    std::vector<int> x;

    for(auto i : ural::numbers(0, 7, 2))
    {
        x.push_back(i);
    }

    URAL_CHECK_EQUAL_RANGES(z, x);
}

BOOST_AUTO_TEST_CASE(numbers_negative_stride_range_test)
{
    std::vector<int> const z = {7, 5, 3, 1};

    std::vector<int> x;

    for(auto i : ural::numbers(7, 0, -2))
    {
        x.push_back(i);
    }

    URAL_CHECK_EQUAL_RANGES(x, z);
}

// Итераторы последовательностей
#include <ural/sequence/make.hpp>
BOOST_AUTO_TEST_CASE(iterator_cursor_iterators)
{
    typedef std::vector<int> Container;
    Container v1 = {0, 2, 4, 6};
    auto const v2 = v1;

    auto const s1 = ural::cursor(v1);
    auto const s2 = ural::cursor(v2);

    static_assert(std::is_convertible<decltype(s1.begin()), Container::iterator>::value, "");
    static_assert(std::is_convertible<decltype(s2.begin()), Container::const_iterator>::value, "");

    BOOST_CHECK(s1.begin() == v1.begin());
    BOOST_CHECK(s1.end() == v1.end());

    BOOST_CHECK(s2.begin() == v2.begin());
    BOOST_CHECK(s2.end() == v2.end());
}

BOOST_AUTO_TEST_CASE(moved_from_value_cpp_17_test)
{
    std::string const s("hello");
    std::string x_std;
    auto x_ural = x_std;

    auto f = std::ptr_fun<int, int>(std::toupper);

    std::transform(s.begin(), s.end(), std::back_inserter(x_std), f);

    auto cur = ural_ex::make_transform_cursor(f, s)
             | ural_ex::moved;

    using Cursor = decltype(cur);

    static_assert(std::is_same<Cursor::reference, int>::value, "");

    ural::copy(cur, x_ural | ural::back_inserter);

    URAL_CHECK_EQUAL_RANGES(x_std, x_ural);
}

BOOST_AUTO_TEST_CASE(moved_test)
{
    typedef std::unique_ptr<int> Type;

    std::vector<int> const ys = {25, -15, 5, -5, 15};

    auto xs1 = ys | ural_ex::transformed(ural_ex::to_unique_ptr)
                  | ural_ex::to_container<std::vector>{};

    auto xs2 = ys | ural_ex::transformed(ural_ex::to_unique_ptr)
                  | ural_ex::to_container<std::vector>{};

    std::vector<Type> r_std;
    std::vector<Type> r_ural;

    std::move(xs1.begin(), xs1.end(), std::back_inserter(r_std));

    ural::copy(xs2 | ural_ex::moved, r_ural | ural::back_inserter);

    BOOST_CHECK_EQUAL(r_std.size(), r_ural.size());
    BOOST_CHECK(std::none_of(r_ural.begin(), r_ural.end(),
                             [](Type const & x) {return !x;}));

    BOOST_CHECK(ural::equal(r_std, r_ural, ural_ex::equal_by(ural_ex::dereference<>{})));
}

BOOST_AUTO_TEST_CASE(move_iterator_cpp_11_moving_from_return_by_value_regression)
{
    std::string const s("hello");
    std::string x_std;

    auto f = std::ptr_fun<int, int>(std::toupper);

    std::transform(s.begin(), s.end(), std::back_inserter(x_std), f);

    auto cur = ural_ex::make_transform_cursor(f, s)
             | ural_ex::moved;

    using Cursor = decltype(cur);

    static_assert(std::is_same<Cursor::reference, int>::value, "");

    auto x_ural = cur | ural_ex::to_container<std::basic_string>{};

    URAL_CHECK_EQUAL_RANGES(x_std, x_ural);
}

BOOST_AUTO_TEST_CASE(moved_iterator_cursor_iterators)
{
    typedef std::vector<int> Container;
    Container v1 = {0, 2, 4, 6};
    auto const v2 = v1;

    auto const rs1 = ural::cursor(v1) | ural_ex::moved;
    auto const rs2 = ural::cursor(v2) | ural_ex::moved;

    static_assert(std::is_same<decltype(begin(rs1)), ural::move_iterator<Container::iterator>>::value, "");
    static_assert(std::is_same<decltype(begin(rs2)), ural::move_iterator<Container::const_iterator>>::value, "");

    BOOST_CHECK(begin(rs1) == ural::make_move_iterator(v1.begin()));
    BOOST_CHECK(end(rs1) == ural::make_move_iterator(v1.end()));

    BOOST_CHECK(begin(rs2) == ural::make_move_iterator(v2.begin()));
    BOOST_CHECK(end(rs2) == ural::make_move_iterator(v2.end()));
}

BOOST_AUTO_TEST_CASE(function_output_cursor_as_iterator)
{
    int result = 0;

    std::function<void(int)> acc = [&result](int x) { result += x; };

    auto out = ural_ex::make_function_output_cursor(acc);

    // Концепция
    BOOST_CONCEPT_ASSERT((boost::OutputIterator<decltype(out), int>));

    // Пример использования
    std::vector<int> const xs = {1, 2, 3, 4, 5};
    std::copy(xs.begin(), xs.end(), out);

    BOOST_CHECK_EQUAL(15, result);
}

#include <boost/iterator/transform_iterator.hpp>
BOOST_AUTO_TEST_CASE(transform_cursor_iterators)
{
    std::string const s("hello");

    auto f = ural::function_ptr_wrapper<int(int)>(std::toupper);

    auto const seq = ural_ex::make_transform_cursor(f, s);

    typedef boost::transform_iterator<decltype(f), decltype(s.begin())>
        Iterator;

    Iterator bf = begin(seq);
    Iterator ef = end(seq);

    BOOST_CHECK(bf.base() == s.begin());
    BOOST_CHECK(ef.base() == s.end());

    BOOST_CHECK(bf.functor() == f);
    BOOST_CHECK(ef.functor() == f);
}

BOOST_AUTO_TEST_CASE(transform_cursor_test)
{
    std::string const s("hello");
    std::string x_std;

    auto f = std::ptr_fun<int, int>(std::toupper);

    std::transform(s.begin(), s.end(), std::back_inserter(x_std), f);

    auto const x_ural
        = ural_ex::make_transform_cursor(f, s)
        | ural_ex::to_container<std::basic_string>{};

    URAL_CHECK_EQUAL_RANGES(x_std, x_ural);
}

BOOST_AUTO_TEST_CASE(transform_reverse_cursor_test)
{
    // начальные условия
    std::string const source("hello, world!");
    std::list<char> x_std;

    auto const f = static_cast<int(*)(int)>(std::toupper);

    // Выполнение операций
    std::transform(source.begin(), source.end(), std::front_inserter(x_std), f);

    auto seq = source | ural_ex::transformed(f) | ural_ex::reversed;

    std::list<char> x_ural;
    auto result = ural::copy(seq, x_ural | ural::back_inserter);

    // Проверки
    URAL_CHECK_EQUAL_RANGES(x_std, x_ural);

    BOOST_CHECK(result[ural::_1].base().original()
                == (source | ural_ex::transformed(f)));

    BOOST_CHECK(result[ural::_1].base().traversed_front()
                == (source | ural_ex::transformed(f)).traversed_front());

    BOOST_CHECK(result[ural::_1].base().traversed_back()
                == (source | ural_ex::transformed(f)));

    BOOST_CHECK(!ural::is_heap(seq));

    auto f1 = std::move(result)[ural::_1].base().function();

    BOOST_CHECK_EQUAL(f1.target(), f);
}

// @todo Аналогинчые тесты с одним из параметров, обёрнутых в cref
BOOST_AUTO_TEST_CASE(replace_cursor_test_cref)
{
    std::vector<int> s_std = {5, 7, 4, 2, 8, 6, 1, 9, 0, 3};
    std::vector<int> s_ural = s_std;

    auto const old_value = ural_ex::rational<int>(8);
    auto const new_value = 88;

    BOOST_CHECK_EQUAL(old_value.denominator(), 1);

    // std
    std::replace(s_std.begin(), s_std.end(), old_value.numerator(), new_value);

    // ural
    auto seq = ural_ex::make_replace_cursor(s_ural, std::cref(old_value),
                                            std::cref(new_value));
    ural::copy(seq, s_ural);

    URAL_CHECK_EQUAL_RANGES(s_std, s_ural);
}

BOOST_AUTO_TEST_CASE(replace_cursor_test)
{
    std::vector<int> s_std = {5, 7, 4, 2, 8, 6, 1, 9, 0, 3};
    std::vector<int> s_ural = s_std;

    auto const old_value = 8;
    auto const new_value = 88;

    std::replace(s_std.begin(), s_std.end(), old_value, new_value);
    ural::copy(ural_ex::make_replace_cursor(s_ural, old_value, new_value),
               s_ural);

    URAL_CHECK_EQUAL_RANGES(s_std, s_ural);
}

BOOST_AUTO_TEST_CASE(replace_sequence_custom_predicate)
{
    std::vector<int> s_std = {5, 7, 4, 2, 8, 6, 1, -8, 0, 3};
    std::vector<int> s_ural = s_std;

    auto const old_value = -8;
    auto const new_value = 88;

    auto abs_eq = ural_ex::equal_by(ural::abs());

    // std
    auto pred_std = [=](int const & x) { return abs_eq(x, old_value);};
    std::replace_if(s_std.begin(), s_std.end(), pred_std, new_value);

    // ural
    auto seq = s_ural | ural_ex::replaced(old_value, new_value, abs_eq);
    ural::copy(seq, s_ural);

    URAL_CHECK_EQUAL_RANGES(s_std, s_ural);

    BOOST_CHECK(seq.predicate() == abs_eq);
    BOOST_CHECK_EQUAL(seq.old_value(), old_value);
    BOOST_CHECK_EQUAL(seq.new_value(), new_value);
}

BOOST_AUTO_TEST_CASE(generate_cursor_test)
{
    std::vector<int> r_std(5);
    auto r_ural = r_std;

    auto counter = int{0};
    auto gen = [&]{ return counter++; };
    std::generate(r_std.begin(), r_std.end(), gen);

    counter = 0;

    ural::copy(ural_ex::make_generator_cursor(gen), r_ural);

    URAL_CHECK_EQUAL_RANGES(r_std, r_ural);
}

BOOST_AUTO_TEST_CASE(unique_cursor_test)
{
    std::forward_list<int> v1{1, 2, 2, 2, 3, 3, 2, 2, 1};
    auto const v2 = v1;

    auto const last = std::unique(v1.begin(), v1.end());
    std::forward_list<int> r_std(v1.begin(), last);

    auto const r_ural
        = v2 | ural_ex::uniqued | ural_ex::to_container<std::forward_list>{};

    URAL_CHECK_EQUAL_RANGES(r_std, r_ural);
}

BOOST_AUTO_TEST_CASE(uniqued_delimited_cursor_test)
{
    // Настройки
    std::forward_list<int> const src1{1, 2, 2, 2, 3, 3, 2, 2, 1};

    auto const guard = 3;

    auto const s1 = ural::find(src1 | ural_ex::uniqued, guard);
    auto const s2 = ural::find(src1 | ural_ex::assumed_infinite | ural_ex::uniqued, guard);

    // Проверка результатов
    BOOST_CHECK(s1.traversed_front() == s2.traversed_front());
}

BOOST_AUTO_TEST_CASE(unique_cursor_test_custom_predicate)
{
    std::string const src = "The      string    with many       spaces!";

    auto const pred = [](char c1, char c2){ return c1 == ' ' && c2 == ' '; };

    std::string s_std;
    std::unique_copy(src.begin(), src.end(), std::back_inserter(s_std), pred);

    auto const s_ural = src
                      | ural_ex::adjacent_filtered(pred)
                      | ural_ex::to_container<std::basic_string>{};

    BOOST_CHECK_EQUAL(s_std, s_ural);
}

BOOST_AUTO_TEST_CASE(unique_cursor_from_istream_to_ostream)
{
    // Готовим строку с данными
    std::list<int> const v1{1, 2, 2, 2, 3, 3, 2, 2, 1};

    std::ostringstream src;
    ural::copy(v1, ural_ex::make_ostream_cursor(src, ' '));

    auto v2 = v1;
    ural_ex::unique_erase(v2);

    std::ostringstream z;
    ural::copy(v2, ural_ex::make_ostream_cursor(z, ' '));

    // Сам алгоритм
    std::istringstream is(src.str());
    std::ostringstream os;

    auto in_seq = ural_ex::make_istream_cursor<int>(is) | ural_ex::uniqued;
    ural::copy(std::move(in_seq), ural_ex::make_ostream_cursor(os, ' '));

    BOOST_CHECK_EQUAL(z.str(), os.str());
}

BOOST_AUTO_TEST_CASE(unique_cursor_move_only)
{
    std::forward_list<int> src{1, 2, 2, 2, 3, 3, 2, 2, 1};

    typedef std::unique_ptr<int> Pointer;

    std::vector<Pointer> v1;
    std::vector<Pointer> v2;

    for(auto & y : src)
    {
        auto constexpr f = ural_ex::to_unique_ptr;

        v1.emplace_back(f(y));
        v2.emplace_back(f(y));
    }

    auto const eq = [](Pointer const & x, Pointer const & y)
    {
        return (!!x && !!y) ? (*x == *y) : (!x && !y);
    };

    auto const last = std::unique(v1.begin(), v1.end(), eq);

    auto const r_ural
        = v2 | ural_ex::adjacent_filtered(eq) | ural_ex::moved
        | ural_ex::to_container<std::vector>{};

    BOOST_CHECK_EQUAL(last - v1.begin(), r_ural.end() - r_ural.begin());

    for(auto i : ural::numbers(0, last - v1.begin()))
    {
        BOOST_CHECK(!!v1[i]);
        BOOST_CHECK(!!r_ural[i]);
        BOOST_CHECK_EQUAL(*v1[i], *r_ural[i]);
    }
}

BOOST_AUTO_TEST_CASE(merged_test)
{
    // Исходные данные
    std::vector<int> const v1{1,2,3,4,5,6,7,8};
    std::vector<int> const v2{        5,  7,  9,10};

    // std
    std::vector<int> std_merge;
    std::merge(v1.begin(), v1.end(), v2.begin(), v2.end(),
               std::back_inserter(std_merge));

    // ural
    auto const ural_merge
        = ural_ex::merged(v1, v2) | ural_ex::to_container<std::vector>{};

    // Проверка
    URAL_CHECK_EQUAL_RANGES(std_merge, ural_merge);
}

BOOST_AUTO_TEST_CASE(to_container_additional_parameters)
{
    typedef ural::greater<> Compare;

    auto const ns = ural::numbers(1, 10)
                  | ural_ex::to_container<std::set, Compare>{};

    static_assert(std::is_same<Compare, decltype(ns)::key_compare>::value, "");

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(as_container_test)
{
    auto const seq = ural::numbers(1, 10);

    std::set<int> const v1 = seq | ural_ex::as_container;

    using std::begin;
    using std::end;
    std::set<int> const v2(begin(seq), end(seq));

    URAL_CHECK_EQUAL_RANGES(v1, v2);
}

BOOST_AUTO_TEST_CASE(to_map_additional_parameters)
{
    typedef ural::greater<> Compare;

    std::map<int, char> f;

    auto f1 = ural::cursor(f) | ural_ex::to_map<std::map, Compare>{};

    static_assert(std::is_same<Compare, decltype(f1)::key_compare>::value, "");

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(iterator_cursor_for_istream_iterator_regression)
{
    std::vector<int> const z{11, 11, 22, 33, 55};

    std::ostringstream os;
    ural::copy(z, ural_ex::make_ostream_cursor(os, " "));

    std::istringstream is(os.str());

    auto first = std::istream_iterator<int>(is);
    auto last = std::istream_iterator<int>();

    auto seq = ural::make_iterator_cursor(first, last);

    std::vector<int> x;
    ural::copy(seq, x | ural::back_inserter);

    URAL_CHECK_EQUAL_RANGES(x, z);
}

BOOST_AUTO_TEST_CASE(delimit_cursor_test)
{
    std::vector<int> const src2 = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3};
    ural_test::istringstream_helper<int> const src1(src2);

    auto const value = 5;

    auto cur = ::ural_ex::make_delimit_cursor(src1, value);
    BOOST_CONCEPT_ASSERT((ural::concepts::InputCursor<decltype(cur)>));

    std::vector<int> result;
    ural::copy(std::move(cur), result | ural::back_inserter);

    auto const expected = ural::find(src2, value).traversed_front();

    URAL_CHECK_EQUAL_RANGES(result, expected);
}

BOOST_AUTO_TEST_CASE(delimeted_cursor_regression_87)
{
    std::vector<int> const src = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3};

    auto const value = 5;
    auto const expected = ::ural_ex::make_delimit_cursor(src, value);

    auto cur = expected;
    ural::exhaust_front(cur);

    BOOST_CHECK(expected == cur.traversed_front());
}

BOOST_AUTO_TEST_CASE(delimit_cursor_test_cref)
{
    std::vector<int> const src2 = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3};
    ural_test::istringstream_helper<int> const src1(src2);

    auto const value = 5;

    auto cur = src1 | ural_ex::delimited(std::cref(value));
    BOOST_CONCEPT_ASSERT((ural::concepts::InputCursor<decltype(cur)>));

    std::vector<int> result;

    ural::copy(std::move(cur), result | ural::back_inserter)[ural::_1];

    auto const expected = ural::find(src2, value).traversed_front();

    URAL_CHECK_EQUAL_RANGES(result, expected);
}

BOOST_AUTO_TEST_CASE(delimit_cursor_forward_test)
{
    std::forward_list<int> src = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3};
    auto const value = 5;

    auto const cur = ural_ex::make_delimit_cursor(src, value);
    BOOST_CONCEPT_ASSERT((ural::concepts::ForwardCursor<std::decay_t<decltype(cur)>>));

    auto const expected_pos = ural::find(src, value).traversed_front();
    auto const expected = ural_ex::make_delimit_cursor(expected_pos, value);

    std::vector<int> result;
    auto const cur_after_copy
        = ural::copy(cur, result | ural::back_inserter)[ural::_1];

    auto const cur_traversed = cur_after_copy.traversed_front();

    BOOST_CHECK(cur_traversed == expected);
    BOOST_CHECK(cur == cur_after_copy.original());
}

BOOST_AUTO_TEST_CASE(delimit_cursor_forward_test_cref)
{
    std::forward_list<int> src = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3};
    auto const value = 5;

    auto const cur = ::ural_ex::make_delimit_cursor(src, std::cref(value));
    BOOST_CONCEPT_ASSERT((ural::concepts::ForwardCursor<std::decay_t<decltype(cur)>>));

    auto const expected_pos = ural::find(src, value).traversed_front();
    auto const expected = ural_ex::make_delimit_cursor(expected_pos, std::cref(value));

    BOOST_CHECK(cur != expected);

    std::vector<int> result;
    auto const cur_after_copy
        = ural::copy(cur, result | ural::back_inserter)[ural::_1];
    BOOST_CHECK(cur_after_copy != expected);

    auto const cur_traversed = cur_after_copy.traversed_front();

    BOOST_CHECK(cur_traversed == expected);
    BOOST_CHECK(cur == cur_after_copy.original());
}

BOOST_AUTO_TEST_CASE(delimit_cursor_equality_test)
{
    std::forward_list<int> src1 = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3};
    std::forward_list<int> src0;

    auto const value1 = 5;
    auto const value2 = 9;

    BOOST_CHECK(value1 != value2);

    auto const eq1 = +[](int x, int y) { return x == y; };
    auto const eq2 = +[](int x, int y) { return x != y; };

    BOOST_CHECK(eq1 != eq2);

    auto s1   = ::ural_ex::make_delimit_cursor(src1, value1, eq1);
    auto s1_1 = s1;

    auto s2 = ::ural_ex::make_delimit_cursor(src0, value1, eq1);
    auto s3 = ::ural_ex::make_delimit_cursor(src1, value2, eq1);
    auto s4 = ::ural_ex::make_delimit_cursor(src1, value1, eq2);

    BOOST_CHECK(s1 == s1);
    BOOST_CHECK(s1 == s1_1);

    BOOST_CHECK(s1 != s2);
    BOOST_CHECK(s1 != s3);
    BOOST_CHECK(s1 != s4);
}

BOOST_AUTO_TEST_CASE(delimit_cursor_shrink_front_test)
{
    std::vector<int> src = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3};

    auto const d = 2;
    auto const value = 5;

    auto s1 = ::ural::make_iterator_cursor(src.begin(), src.end());
    auto ds1 = ::ural_ex::make_delimit_cursor(s1, value);

    auto s2 = ::ural::make_iterator_cursor(src.begin() + d, src.end());
    BOOST_CHECK(s1 != s2);

    auto ds2 = ::ural_ex::make_delimit_cursor(s2, value);
    BOOST_CHECK(ds1 != ds2);

    ural::advance(ds1, d);
    ds1.shrink_front();

    BOOST_CHECK(ds1.base() == ds2.base());
    BOOST_CHECK(ds1 == ds2);
}

BOOST_AUTO_TEST_CASE(outdirected_rvalue_base)
{
    std::string const source("AlexStepanov");
    std::istringstream is(source);

    auto s1 = ural::cursor(is);
    auto so = std::move(s1) | ural_ex::outdirected;
    auto s2 = std::move(so).base();

    std::string str;
    ural::copy(std::move(s2), str | ural::back_inserter);

    BOOST_CHECK_EQUAL(source, str);
}

BOOST_AUTO_TEST_CASE(delimited_rvalue_base)
{
    std::string const source("AlexanderStepanov");
    std::istringstream is(source);

    const char d = 'a';

    auto s1 = ural::cursor(is);
    auto so = std::move(s1) | ural_ex::delimited(d);
    auto s2 = std::move(so).base();

    std::string str;
    ural::copy(std::move(s2), str | ural::back_inserter);

    URAL_CHECK_EQUAL_RANGES(str, source);
}

BOOST_AUTO_TEST_CASE(chunks_rvalue_base)
{
    std::string const source("AlexanderStepanov");
    std::string const source_2(source);

    auto s1 = ural::cursor(source_2);
    auto so = ural::experimental::make_chunks_cursor(std::move(s1), 5);
    auto s2 = std::move(so).base();

    std::string str;
    ural::copy(std::move(s2), str | ural::back_inserter);

    URAL_CHECK_EQUAL_RANGES(str, source);
}

BOOST_AUTO_TEST_CASE(cursor_temporary_istream_with_spaces)
{
    std::string const source("Alex Stepanov invented STL\n");

    auto cur = ural::cursor(std::istringstream(source));

    std::string str;
    ural::copy(std::move(cur), str | ural::back_inserter);

    BOOST_CHECK_EQUAL(source, str);
}

BOOST_AUTO_TEST_CASE(cursor_temporary_ostream)
{
    std::string const source("Alex Stepanov");

    auto r = ural::copy(source, ural::cursor(std::ostringstream()))[ural::_2];

    BOOST_CHECK_EQUAL(r.stream().str(), source);
}

BOOST_AUTO_TEST_CASE(unique_cursor_forward_test)
{
    typedef std::string Type;

    std::forward_list<Type> const names = { "one", "two", "three", "four", "five"};

    std::vector<Type> v_names;
    std::unique_copy(names.begin(), names.end(), v_names | ural::back_inserter);
    std::sort(v_names.begin(), v_names.end());

    auto const us0 = names | ural_ex::uniqued;
    auto us = us0;

    BOOST_CHECK(ural::is_permutation(us, v_names));

    ural::exhaust_front(us);

    BOOST_CHECK(ural::is_permutation(us.traversed_front(), v_names));

    BOOST_CHECK(!us);
    BOOST_CHECK(us.original() == us0);
    BOOST_CHECK(us.traversed_front() == us0);
    BOOST_CHECK(ural::equal(us.traversed_front(), us0));
}

BOOST_AUTO_TEST_CASE(zip_cursor_sort)
{
    std::vector<std::string> names = { "one", "two", "three", "four", "five"};
    auto const old_names = names;

    std::vector<int> values = {1, 2, 3, 4, 5};
    auto const old_values= values;

    auto names_1 = old_names;
    std::sort(names_1.begin(), names_1.end());

    // Сортируем сначала по имени
    ural::sort(ural_ex::combine(names, values));

    BOOST_CHECK(names != old_names);
    BOOST_CHECK(values != old_values);

    BOOST_CHECK(ural::is_sorted(ural_ex::combine(names, values)));

    BOOST_CHECK(ural::is_permutation(names, old_names));
    BOOST_CHECK(ural::is_permutation(values, old_values));
    BOOST_CHECK(ural::is_permutation(ural_ex::combine(names, values),
                                     ural_ex::combine(old_names, old_values)));
    BOOST_CHECK(ural::is_permutation(ural_ex::combine(names, values) | ural_ex::reversed,
                                     ural_ex::combine(old_names, old_values)));

    URAL_CHECK_EQUAL_RANGES(names, names_1);

    // Сортируем сначала по числу
    ural::sort(ural_ex::combine(values, names));

    URAL_CHECK_EQUAL_RANGES(names, old_names);
    URAL_CHECK_EQUAL_RANGES(values, old_values);
}

BOOST_AUTO_TEST_CASE(zip_cursor_exhaust_test)
{
    std::vector<std::string> names = { "one", "two", "three", "four", "five"};
    std::vector<int> values = {1, 2, 3, 4, 5};

    auto const z0 = ural_ex::make_zip_cursor(names, values);

    auto z_front = z0;
    ural::exhaust_front(z_front);

    BOOST_CHECK(!z_front);
    BOOST_CHECK(!z_front.traversed_back());
    BOOST_CHECK(z_front.original() == z0);
    BOOST_CHECK(z_front.traversed_front() == z0);

    auto z_back = z0;
    z_back.exhaust_back();

    BOOST_CHECK(!z_back);
    BOOST_CHECK(!z_back.traversed_front());
    BOOST_CHECK(z_back.original() == z0);
    BOOST_CHECK(z_back.traversed_back() == z0);
}

// композиция pipeable
BOOST_AUTO_TEST_CASE(pipeable_combine)
{
    std::vector<int> const xs = {3, 1, 4, 1, 5};

    auto seq1 = xs | ural_ex::reversed | ural_ex::replaced(1, 0);
    auto pipe =      ural_ex::reversed | ural_ex::replaced(1, 0);
    auto seq2 = xs | pipe;

    BOOST_CHECK(seq1 == seq2);
}

BOOST_AUTO_TEST_CASE(pipeable_combine_3)
{
    std::vector<int> const xs = {3, 1, 4, 1, 5};

    auto seq1 = xs | ural_ex::reversed | ural_ex::replaced(1, 0) | ural_ex::uniqued;
    auto pipe =      ural_ex::reversed | ural_ex::replaced(1, 0) | ural_ex::uniqued;
    auto seq2 = xs | pipe;

    BOOST_CHECK(seq1 == seq2);
}

// size для контейнеров
namespace
{
    using Containers = boost::mpl::list<std::forward_list<int>, std::list<int>,
                                        std::vector<int>,
                                        ural::experimental::vector<int>>;
}

BOOST_AUTO_TEST_CASE_TEMPLATE(size_for_containers_test, Container, Containers)
{
    auto const n = 42U;

    Container const xs(n, 13);

    BOOST_CHECK_EQUAL(ural::size(xs), n);
}

BOOST_AUTO_TEST_CASE(size_for_c_array)
{
    int xs [] = {1, 2, 3, 4, 5};

    BOOST_CHECK_EQUAL(ural::size(xs), sizeof(xs) / sizeof(xs[0]));
}

BOOST_AUTO_TEST_CASE(size_for_std_array)
{
    constexpr auto const n = 13;
    std::array<int, n> xs;

    BOOST_CHECK_EQUAL(ural::size(xs), xs.size());
}

BOOST_AUTO_TEST_CASE(cursor_ostreaming_empty)
{
    std::vector<int> const xs{};

    auto cur = ural::cursor(xs);

    std::ostringstream os;
    os << cur;

    auto const expected = std::string("{}");

    BOOST_CHECK_EQUAL(os.str(), expected);
}

BOOST_AUTO_TEST_CASE(cursor_ostreaming_single)
{
    std::vector<int> const xs{1, 2, 3, 4, 5};

    auto cur = ural::cursor(xs);

    std::ostringstream os;
    os << cur;

    auto const expected = std::string("{1, 2, 3, 4, 5}");

    BOOST_CHECK_EQUAL(os.str(), expected);
}

BOOST_AUTO_TEST_CASE(cursor_ostreaming_more_than_1)
{
    std::vector<int> const xs{1, 2, 3, 4, 5};

    auto cur = ural::cursor(xs);

    std::ostringstream os;
    os << cur;

    auto const expected = std::string("{1, 2, 3, 4, 5}");

    BOOST_CHECK_EQUAL(os.str(), expected);
}
