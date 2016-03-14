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

#include <ural/sequence/all.hpp>
#include <ural/math.hpp>

#include <forward_list>

#include <boost/test/unit_test.hpp>

namespace
{
    namespace ural_ex = ::ural::experimental;
}

BOOST_AUTO_TEST_CASE(transformed_traversed_front)
{
    std::forward_list<int> const src = {1, 2, 3, 3, 4, 5};

    auto const f = ural::square;
    auto const n = ural::size(ural::sequence(src)) / 2;

    auto const s1 = src | ural_ex::transformed(f);
    auto const s2 = src | ural_ex::assumed_infinite | ural_ex::transformed(f);

    auto const s1_n = ural::next(s1, n);
    auto const s2_n = ural::next(s2, n);

    BOOST_CHECK(s1_n.original() == s1);
    BOOST_CHECK(s2_n.original() == s2);

    BOOST_CHECK(s1.traversed_front() == s2.traversed_front());
}
