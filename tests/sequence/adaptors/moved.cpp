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

#include <ural/sequence/adaptors/moved.hpp>
#include <ural/sequence/adaptors/delimit.hpp>
#include <ural/algorithm.hpp>

#include <forward_list>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(moved_delimited_sequence_test)
{
    // Настройки
    std::forward_list<int> const src1{1, 2, 2, 2, 3, 3, 2, 2, 1};
    auto const guard = 3;

    auto const s1 = ural::find(src1 | ural::moved, guard).traversed_front();

    auto const sdu = src1 | ural::delimited(guard) | ural::moved;

    std::vector<int> r_ural;
    auto const s2 = ural::copy(sdu, r_ural | ural::back_inserter)[ural::_1];

    // Проверка результатов
    BOOST_CHECK(s1 == s2.traversed_front());

    BOOST_CHECK(s2.original() == sdu);
}
