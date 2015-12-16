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

#include <ural/sequence/adaptors/assumed_infinite.hpp>
#include <ural/sequence/adaptors/moved.hpp>
#include <ural/algorithm.hpp>

#include <forward_list>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(moved_sequence_forward_test)
{
    // Настройки
    std::forward_list<int> const src1{1, 2, 2, 2, 3, 3, 2, 2, 1};
    auto const n = 5;

    auto s1 = src1 | ural::moved;
    auto s2 = src1 | ural::assumed_infinite | ural::moved;

    auto const s2_old = s2;

    ural::advance(s1, n);
    ural::advance(s2, n);

    // Проверка результатов
    BOOST_CHECK(s1.traversed_front() == s2.traversed_front());

    BOOST_CHECK(s2.original() == s2_old);
}
