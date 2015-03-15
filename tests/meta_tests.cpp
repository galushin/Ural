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
    typedef ::ural::typelist<int, int, char, int, char, char, double> Source;
    typedef ::ural::typelist<int, char, int, char, double> Objective;
    typedef ::ural::meta::unique<Source>::type Result;

    Objective obj = Result{};

    static_assert(std::is_same<decltype(obj), Result>::value, "");
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(meta_flatten_test)
{
    using ural::typelist;

    typedef typelist<char, typelist<bool, typelist<void, int*>, double>> Source;
    typedef typelist<char, bool, void, int*, double> Objective;
    typedef typename ::ural::meta::flatten<Source>::type Result;

    static_assert(std::is_same<Objective, Result>::value, "");
    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(meta_remove_first_test)
{
    typedef ::ural::typelist<int, int, char, int, char, char, double> Source;
    typedef typename ::ural::meta::remove_first<Source, char>::type Result;

    typedef ::ural::typelist<int, int, int, char, char, double> Objective;

    static_assert(std::is_same<Objective, Result>::value, "");

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(meta_remove_all_test)
{
    typedef ::ural::typelist<int, int, char, int, char, char, double> Source;
    typedef typename ::ural::meta::remove_all<Source, char>::type Result;

    typedef ::ural::typelist<int, int, int, double> Objective;

    static_assert(std::is_same<Objective, Result>::value, "");

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(meta_count_test)
{
    typedef ::ural::typelist<int, int, char, int, char, char, double> Source;
    typedef std::integral_constant<size_t, 3> Objective;
    typedef ::ural::meta::count<Source, int> Result;

    static_assert(Objective::value == Result::value, "");

    BOOST_CHECK_EQUAL(Objective::value, Result::value);
}
