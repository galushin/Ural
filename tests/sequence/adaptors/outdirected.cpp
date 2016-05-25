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

#include <ural/sequence/adaptors/outdirected.hpp>
#include <ural/sequence/adaptors/assumed_infinite.hpp>
#include <ural/algorithm.hpp>

#include <forward_list>

#include <boost/test/unit_test.hpp>

namespace
{
    namespace ural_ex = ::ural::experimental;
}

BOOST_AUTO_TEST_CASE(outdirected_cursor_forward_test)
{
    // Настройки
    std::forward_list<int> const src{1, 2, 2, 2, 3, 3, 2, 2, 1};

    auto const n = 3;

    auto s = ural::cursor(src);
    auto s_inf = s | ural_ex::assumed_infinite;

    auto s_out = s | ural_ex::outdirected;
    auto s_inf_out = s_inf | ural_ex::outdirected;
    auto const s_inf_out_old = s_inf_out;

    ural::advance(s_out, n);
    ural::advance(s_inf_out, n);

    // Проверка результатов
    BOOST_CHECK(s_out.traversed_front() == s_inf_out.traversed_front());

    BOOST_CHECK(s_inf_out.original() == s_inf_out_old);
}
