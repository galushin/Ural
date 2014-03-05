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

#include <string>
#include <memory>

#include <boost/test/unit_test.hpp>

#include <ural/memory.hpp>

BOOST_AUTO_TEST_CASE(make_unique_array)
{
    auto const n = 5;
    auto p = ural::make_unique<std::string[]>(n);

    // FAILS: auto p_fail = ural::make_unique<std::string[5]>(n);

    static_assert(std::is_same<std::unique_ptr<std::string[]>, decltype(p)>::value, "");

    const std::string value {};

    for(size_t i = 0; i < n; ++ i)
    {
        BOOST_CHECK_EQUAL(value, p[i]);
    }
}
