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

#include <ural/container/flat_set.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(flat_set_default_template_parameters)
{
    typedef int T;
    typedef ural::flat_set<T> Set;

    static_assert(std::is_same<Set::value_type, T>::value, "");

    static_assert(std::is_same<Set::key_compare, std::less<T>>::value, "");
    static_assert(std::is_same<Set::value_compare, Set::key_compare>::value, "");
    static_assert(std::is_same<Set::allocator_type, std::allocator<T>>::value, "");
    static_assert(std::is_same<Set::policy_type, ural::vector<T>::policy_type>::value, "");

    BOOST_CHECK(true);
}
