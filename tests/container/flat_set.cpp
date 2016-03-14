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
#include <ural/concepts.hpp>

#include <boost/test/unit_test.hpp>

namespace ural_ex = ural::experimental;

BOOST_AUTO_TEST_CASE(flat_set_default_template_parameters)
{
    typedef int T;
    typedef ural_ex::flat_set<T> Set;

    static_assert(std::is_same<::ural::ValueType<Set>, T>::value, "");

    static_assert(std::is_same<Set::key_compare, std::less<T>>::value, "");
    static_assert(std::is_same<Set::value_compare, Set::key_compare>::value, "");
    static_assert(std::is_same<Set::allocator_type, std::allocator<T>>::value, "");
    static_assert(std::is_same<Set::policy_type, ural_ex::vector<T>::policy_type>::value, "");

    typedef std::greater<T> Greater;
    typedef ural_ex::flat_set<T, Greater> Set_greater;

    static_assert(std::is_same<Set_greater::key_compare, Greater>::value, "");

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(flat_set_with_empty_compare_sizeof_test)
{
    typedef int T;
    typedef ural_ex::vector<T> Vector;
    typedef ural_ex::flat_set<T> Set;

    static_assert(std::is_empty<Set::key_compare>::value, "");

    static_assert(sizeof(Vector) == sizeof(Set), "Set is too fat!!");

    BOOST_CHECK(true);
}
