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

#include <ural/meta/list.hpp>
#include <ural/meta/algo.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(meta_unique_test)
{
    typedef typename ::ural::meta::make_list<int, int, char, int, char, char, double>::type
        Source;
    typedef typename ::ural::meta::make_list<int, char, int, char, double>::type
        Objective;
    typedef typename ::ural::meta::unique<Source>::type Result;

    static_assert(std::is_same<Objective, Result>::value, "");
    BOOST_CHECK(true);
}
