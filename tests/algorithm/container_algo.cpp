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

#include "../defs.hpp"
#include <boost/test/unit_test.hpp>

#include <ural/algorithm.hpp>

#include <forward_list>
#include <list>

BOOST_AUTO_TEST_CASE(push_front_range_test)
{
    ural_test::istringstream_helper<int> const in = {2, 7, 1, 8, 2, 8};

    std::forward_list<int> out_1 = {3, 1, 4, 1, 5};
    auto out_2 = out_1;

    assert(out_2 == out_1);

    ural::copy(in, out_1 | ural::front_inserter);
    auto & out_2_ref = ural::push_front(out_2, in);

    URAL_CHECK_EQUAL_RANGES(out_1, out_2);
    BOOST_CHECK_EQUAL(&out_2_ref, &out_2);
}

BOOST_AUTO_TEST_CASE(push_back_range_test)
{
    ural_test::istringstream_helper<int> const in = {2, 7, 1, 8, 2, 8};

    std::list<int> out_1 = {3, 1, 4, 1, 5};
    auto out_2 = out_1;

    assert(out_2 == out_1);

    ural::copy(in, out_1 | ural::back_inserter);
    auto & out_2_ref = ural::push_back(out_2, in);

    URAL_CHECK_EQUAL_RANGES(out_1, out_2);
    BOOST_CHECK_EQUAL(&out_2_ref, &out_2);
}

BOOST_AUTO_TEST_CASE(insert_range_test)
{
    std::vector<int> const source = {2, 7, 1, 8, 2, 8};
    ural_test::istringstream_helper<int> const in(source.begin(), source.end());

    std::list<int> out_1 = {3, 1, 4, 1, 5};
    auto out_2 = out_1;

    assert(out_2 == out_1);

    auto const pos_1 = std::next(out_1.begin(), out_1.size() / 2);
    auto const pos_2 = std::next(out_2.begin(), out_2.size() / 2);

    out_1.insert(pos_1, source.begin(), source.end());

    auto & out_2_ref = ural::insert(out_2, pos_2, in);

    URAL_CHECK_EQUAL_RANGES(out_1, out_2);
    BOOST_CHECK_EQUAL(&out_2_ref, &out_2);
}
