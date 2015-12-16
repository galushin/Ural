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

BOOST_AUTO_TEST_CASE(map_keys_and_values_traversed_front_tests)
{
    std::forward_list<int> const x = {1, 2, 3, 4, 5};
    std::forward_list<char> const y = {'a', 'b', 'c', 'd', 'e'};

    std::map<int, char> const xy
        = ural::make_zip_sequence(x, y) | ural::to_map<std::map>{};

    auto sx = xy | ural::map_keys;
    auto sy = xy | ural::map_values;

    auto sx_inf = xy | ural::assumed_infinite | ural::map_keys;
    auto sy_inf = xy | ural::assumed_infinite| ural::map_values;

    auto const n = xy.size() / 2;

    ural::advance(sx, n);
    ural::advance(sy, n);
    ural::advance(sx_inf, n);
    ural::advance(sy_inf, n);

    BOOST_CHECK(sx.traversed_front() == sx_inf.traversed_front());
    BOOST_CHECK(sy.traversed_front() == sy_inf.traversed_front());
}
