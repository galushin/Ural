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

#include <ural/algorithm.hpp>
#include <ural/sequence/make.hpp>
#include <ural/sequence/repeat_value.hpp>

#include <boost/test/unit_test.hpp>

namespace
{
    namespace ural_ex = ::ural::experimental;
}

BOOST_AUTO_TEST_CASE(repeat_value_cursor_explcit_single_pass)
{
    auto const value = 'z';
    auto const n = 17;

    std::string const expected(n, value);

    std::string actual(n, -1);

    BOOST_CHECK_EQUAL(expected.size(), actual.size());
    BOOST_CHECK_NE(expected, actual);

    auto cur = ural_ex::repeat_value_cursor<char, ural::single_pass_cursor_tag>(value);

    BOOST_CONCEPT_ASSERT((ural::concepts::ReadableCursor<decltype(cur)>));
    BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassCursor<decltype(cur)>));

    ural::copy(std::move(cur), actual);

    BOOST_CHECK_EQUAL(actual, expected);
}

BOOST_AUTO_TEST_CASE(repeat_value_cursor_explcit_forward)
{
    auto const value = 'z';
    auto const n = 17;

    std::string const expected(n, value);

    std::string actual(n, -1);

    BOOST_CHECK_EQUAL(expected.size(), actual.size());
    BOOST_CHECK_NE(expected, actual);

    using RVC = ural_ex::repeat_value_cursor<char, ural::forward_cursor_tag>;
    auto const cur = RVC(value);

    BOOST_CONCEPT_ASSERT((ural::concepts::ReadableCursor<RVC>));
    BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassCursor<RVC>));

    auto result_cur = ural::copy(std::move(cur), actual)[ural::_1];

    BOOST_CHECK_EQUAL(actual, expected);
    BOOST_CHECK(ural::equal(result_cur.traversed_front(), expected));
    BOOST_CHECK(result_cur.original() == cur);

    BOOST_CHECK(result_cur != cur);
    result_cur.shrink_front();

    BOOST_CHECK(result_cur == cur);
}

// @todo explicit random access
