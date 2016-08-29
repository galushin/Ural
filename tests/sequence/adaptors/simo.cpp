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
#include <ural/sequence/all.hpp>

#include <forward_list>

#include "../../defs.hpp"
#include <boost/test/unit_test.hpp>

namespace
{
    namespace ural_ex = ::ural::experimental;
}

BOOST_AUTO_TEST_CASE(multy_output_cursor_test)
{
    std::vector<int> const src = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3};

    std::vector<int> v1;
    std::vector<int> v2;

    auto out = ural_ex::simo_cursor(v1 | ural::back_inserter,
                                    v2 | ural::back_inserter);

    BOOST_CONCEPT_ASSERT((ural::concepts::OutputCursor<decltype(out), int>));

    ural::copy(src, out);

    URAL_CHECK_EQUAL_RANGES(v1, src);
    URAL_CHECK_EQUAL_RANGES(v2, src);
}

BOOST_AUTO_TEST_CASE(simo_cursor_traversed_front)
{
    std::vector<int> const src = {3, 1, 4, 1, 5, 9, 2, 6, 5, 3};

    std::forward_list<int> v1(src.size() * 2, -1);
    std::forward_list<int> v2(src.size() * 3, -2);

    auto out1 = ural_ex::simo_cursor(v1, v2);
    auto out2 = ural_ex::simo_cursor(v1 | ural_ex::assumed_infinite,
                                     v2 | ural_ex::assumed_infinite);

    out1 = ural::copy(src, out1)[ural::_2];
    out2 = ural::copy(src, out2)[ural::_2];

    BOOST_CHECK(out1.traversed_front() == out2.traversed_front());
}
