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
#include <ural/numeric/partial_sums.hpp>
#include <ural/sequence/progression.hpp>

#include <boost/test/unit_test.hpp>
#include "../defs.hpp"

namespace
{
    namespace ural_ex = ::ural::experimental;
}

BOOST_AUTO_TEST_CASE(partial_sums_traversed_front)
{
    auto src = ural_ex::make_arithmetic_progression(1, 2);

    auto src_p = ural_ex::partial_sums(src);

    auto const n = 13;
    std::vector<ural::value_type_t<decltype(src)>> v1(n, -1);

    auto res = ural::copy(src_p, v1)[ural::_1].traversed_front();

    std::vector<ural::value_type_t<decltype(src)>> v2;

    ural::copy(std::move(res), v2 | ural::back_inserter);

    URAL_CHECK_EQUAL_RANGES(v1, v2);
}
