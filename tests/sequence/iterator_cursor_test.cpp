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
#include "../defs.hpp"

#include <forward_list>
#include <list>
#include <numeric>
#include <valarray>
#include <vector>

#include <boost/concept/assert.hpp>

#include <ural/algorithm.hpp>
#include <ural/numeric.hpp>
#include <ural/sequence/all.hpp>

BOOST_AUTO_TEST_CASE(iterator_cursor_compatible_init)
{
    std::vector<int> xs = {1, 2, 3, 4};
    auto const & cr = xs;

    auto s = ural::cursor(xs);
    auto sc = ural::cursor(cr);

    auto sc1 = decltype(sc)(s);

    BOOST_CHECK(sc1 == sc);
}

BOOST_AUTO_TEST_CASE(iterator_cursor_compatible_move_init)
{
    std::vector<int> xs = {1, 2, 3, 4};
    auto const & cr = xs;

    auto s = ural::cursor(xs);
    auto sc = ural::cursor(cr);

    auto const s_old = s;
    auto sc1 = decltype(sc)(std::move(s));

    BOOST_CHECK(sc1 == s_old);
}

BOOST_AUTO_TEST_CASE(iterator_cursor_compatible_assign)
{
    std::vector<int> xs = {1, 2, 3, 4};
    auto const & cr = xs;

    auto s = ural::cursor(xs);
    auto sc = ural::cursor(cr);

    sc = s;

    BOOST_CHECK(sc == s);
}

BOOST_AUTO_TEST_CASE(iterator_cursor_compatible_move_assign)
{
    std::vector<int> xs = {1, 2, 3, 4};
    auto const & cr = xs;

    auto s = ural::cursor(xs);
    auto sc = ural::cursor(cr);

    auto const s_old = s;
    sc = std::move(s);

    BOOST_CHECK(sc == s_old);
}

BOOST_AUTO_TEST_CASE(copy_cursor_test)
{
    std::vector<int> const xs = {1, 2, 3, 4};

    std::vector<int> x1(xs.size());

    ural::copy_fn{}(ural::cursor(xs), ural::cursor(x1));

    BOOST_CHECK(ural::cursor(xs) == ural::cursor(xs));
    BOOST_CHECK(ural::cursor(x1) == ural::cursor(x1));
    BOOST_CHECK(ural::cursor(x1) != ural::cursor(xs));
    BOOST_CHECK(ural::cursor(xs) != ural::cursor(x1));

    URAL_CHECK_EQUAL_RANGES(x1, xs);
}

BOOST_AUTO_TEST_CASE(copy_sequence_test)
{
    std::vector<int> const xs = {1, 2, 3, 4};

    std::vector<int> x1(xs.size());

    auto const r = ural::copy(xs, x1);

    URAL_CHECK_EQUAL_RANGES(x1, xs);

    BOOST_CHECK(!r[ural::_1]);
    BOOST_CHECK(!r[ural::_2]);
}

BOOST_AUTO_TEST_CASE(copy_to_back_inserter)
{
    std::vector<int> const xs = {1, 2, 3, 4};

    std::vector<int> x1;

    ural::copy(xs, std::back_inserter(x1));

    URAL_CHECK_EQUAL_RANGES(xs, x1);
}

BOOST_AUTO_TEST_CASE(c_array_to_cursor)
{
    int xs [] = {1, 2, 3, 4};

    auto s = ural::cursor(xs);

    auto const sum_std = std::accumulate(xs, xs + sizeof(xs) / sizeof(xs[0]), 0);
    auto const sum_ural = ural::accumulate(s, 0);

    BOOST_CHECK_EQUAL(sum_std, sum_ural);
}

BOOST_AUTO_TEST_CASE(iteretor_cursor_plus_assign_test)
{
    std::vector<int> const xs = {1, 2, 3, 4};
    auto s = ural::cursor(xs);
    auto const n = 2;
    s += 2;

    BOOST_CHECK_EQUAL(n, s.traversed_front().size());
    BOOST_CHECK_EQUAL(n, ural::size(s.traversed_front()));
}

BOOST_AUTO_TEST_CASE(iterator_cursor_size_test)
{
    std::istringstream is;
    std::forward_list<int> fwd;
    std::list<int> bi;
    std::vector<int> ra;

    auto s_in = ::ural::make_iterator_cursor(std::istream_iterator<int>(is),
                                             std::istream_iterator<int>());
    auto s_fwd = ::ural::cursor(fwd);
    auto s_bi = ::ural::cursor(bi);
    auto s_ra = ::ural::cursor(ra);

    BOOST_CHECK_EQUAL(2*sizeof(std::istream_iterator<int>{}), sizeof(s_in));
    BOOST_CHECK_EQUAL(3*sizeof(fwd.begin()), sizeof(s_fwd));
    BOOST_CHECK_EQUAL(3*sizeof(bi.begin()) + sizeof(bi.rbegin()), sizeof(s_bi));
    BOOST_CHECK_EQUAL(3*sizeof(ra.begin()) + sizeof(ra.rbegin()), sizeof(s_ra));
}

BOOST_AUTO_TEST_CASE(valarray_to_cursor_test)
{
    std::valarray<int> x0;
    std::valarray<int> x = {1, 2, 3};

    std::valarray<int> const &r0 = x0;
    std::valarray<int> const &r = x;

    auto s0 = ural::cursor(x0);
    auto s = ural::cursor(x);
    auto sc0 = ural::cursor(r0);
    auto sc = ural::cursor(r);

    BOOST_CONCEPT_ASSERT((ural::concepts::RandomAccessCursor<decltype(s0)>));
    BOOST_CONCEPT_ASSERT((ural::concepts::RandomAccessCursor<decltype(s)>));
    BOOST_CONCEPT_ASSERT((ural::concepts::RandomAccessCursor<decltype(sc0)>));
    BOOST_CONCEPT_ASSERT((ural::concepts::RandomAccessCursor<decltype(sc)>));

    BOOST_CONCEPT_ASSERT((ural::concepts::ReadableCursor<decltype(s0)>));
    BOOST_CONCEPT_ASSERT((ural::concepts::ReadableCursor<decltype(s)>));
    BOOST_CONCEPT_ASSERT((ural::concepts::ReadableCursor<decltype(sc0)>));
    BOOST_CONCEPT_ASSERT((ural::concepts::ReadableCursor<decltype(sc)>));

    BOOST_CONCEPT_ASSERT((ural::concepts::WritableCursor<decltype(s), int>));
    BOOST_CONCEPT_ASSERT((ural::concepts::WritableCursor<decltype(s0), int>));

    BOOST_CHECK(s0.traversed_begin() == nullptr);
    BOOST_CHECK(s0.begin() == nullptr);
    BOOST_CHECK(s0.end() == nullptr);
    BOOST_CHECK(s0.traversed_end() == nullptr);

    BOOST_CHECK(sc0.traversed_begin() == nullptr);
    BOOST_CHECK(sc0.begin() == nullptr);
    BOOST_CHECK(sc0.end() == nullptr);
    BOOST_CHECK(sc0.traversed_end() == nullptr);

    BOOST_CHECK(s.traversed_begin() == &x[0]);
    BOOST_CHECK(s.begin() == &x[0]);
    BOOST_CHECK(s.end() == &x[0] + x.size());
    BOOST_CHECK(s.traversed_end() == &x[0] + x.size());

    BOOST_CHECK(sc.traversed_begin() == &x[0]);
    BOOST_CHECK(sc.begin() == &x[0]);
    BOOST_CHECK(sc.end() == &x[0] + x.size());
    BOOST_CHECK(sc.traversed_end() == &x[0] + x.size());
}
