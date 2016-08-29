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
#include <ural/sequence/all.hpp>

#include <forward_list>
#include <string>

#include <boost/test/unit_test.hpp>

namespace
{
    namespace ural_ex = ::ural::experimental;
}

BOOST_AUTO_TEST_CASE(remove_cursor_test_make_function)
{
    std::string s_std = "Text with some   spaces";
    auto s_ural = s_std;

    auto const to_remove = ' ';

    // std
    s_std.erase(std::remove(s_std.begin(), s_std.end(), to_remove),
                s_std.end());

    // ural
    auto s = ural_ex::make_remove_cursor(s_ural, to_remove);
    auto r = ural::copy(s, s_ural)[ural::_2];
    s_ural.erase(r.begin(), r.end());

    // Сравнение
    BOOST_CHECK_EQUAL(s_std, s_ural);
}

BOOST_AUTO_TEST_CASE(remove_cursor_test)
{
    std::string s_std = "Text with some   spaces";
    auto s_ural = s_std;

    auto const to_remove = ' ';

    // std
    s_std.erase(std::remove(s_std.begin(), s_std.end(), to_remove),
                s_std.end());

    // ural
    auto r = ural::copy(s_ural | ural_ex::removed(to_remove), s_ural)[ural::_2];
    s_ural.erase(r.begin(), r.end());

    // Сравнение
    BOOST_CHECK_EQUAL(s_std, s_ural);
}

BOOST_AUTO_TEST_CASE(remove_cursor_test_cref)
{
    std::string s_std = "Text with some   spaces";
    auto s_ural = s_std;

    auto const to_remove = ' ';

    // std
    s_std.erase(std::remove(s_std.begin(), s_std.end(), to_remove),
                s_std.end());

    // ural
    auto r = ural::copy(s_ural | ural_ex::removed(std::cref(to_remove)), s_ural)[ural::_2];
    s_ural.erase(r.begin(), r.end());

    // Сравнение
    BOOST_CHECK_EQUAL(s_std, s_ural);
}

BOOST_AUTO_TEST_CASE(remove_cursor_traversed_front)
{
    std::string const str = "Text with some   spaces";
    std::forward_list<char> const src(str.begin(), str.end());
    auto const to_remove = ' ';

    auto const n = str.size() / 2;

    auto s1 = src | ural_ex::removed(to_remove);
    auto s2 = src | ural_ex::assumed_infinite | ural_ex::removed(to_remove);

    static_assert(!std::is_same<decltype(s2), ural::TraversedFrontType<decltype(s2)>>::value, "");

    ural::advance(s1, n);
    ural::advance(s2, n);

    BOOST_CHECK(s1.traversed_front() == s2.traversed_front());
}

BOOST_AUTO_TEST_CASE(remove_if_cursor_test)
{
    std::string s_std = "Text\n with\tsome \t  whitespaces\n\n";
    auto s_ural = s_std;

    auto pred = [](char x){return std::isspace(x);};

    s_std.erase(std::remove_if(s_std.begin(), s_std.end(), pred), s_std.end());

    auto s = ural_ex::make_remove_if_cursor(s_ural, pred);
    auto r = ural::copy(s, s_ural)[ural::_2];
    s_ural.erase(r.begin(), r.end());

    BOOST_CHECK_EQUAL(s_std, s_ural);
}
