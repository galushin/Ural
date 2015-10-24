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

#include "../defs.hpp"

#include <ural/algorithm.hpp>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(balanced_parens_tests)
{
    // Тесты взяты с
    // http://dlang.org/phobos/std_algorithm_searching.html#balancedParens
    using ural::balanced_parens;
    using std::string;

    string const s1 = "1 + (2 * (3 + 1 / 2)";
    BOOST_CHECK(!balanced_parens(s1, '(', ')'));

    string s = "1 + (2 * (3 + 1) / 2)";
    BOOST_CHECK( balanced_parens(s, '(', ')'));

    s = "1 + (2 * (3 + 1)) / 2) + 13";
    BOOST_CHECK(!balanced_parens(s, '(', ')'));

    s = "1 + (2 * (3 + 1) / 2)";
    BOOST_CHECK(!balanced_parens(s, '(', ']'));

    s = "1 + (2 * (3 + 1] / 2]";
    BOOST_CHECK( balanced_parens(s, '(', ']'));

    s = "1 + (2 * (3 + 1) / 2)";
    BOOST_CHECK(!balanced_parens(s, '(', ')', 0));

    s = "1 + (2 * 3 + 1) / (2 - 5)";
    BOOST_CHECK( balanced_parens(s, '(', ')', 0));
}

BOOST_AUTO_TEST_CASE(min_count_test)
{
    // Первые два теста взяты из
    // http://dlang.org/phobos/std_algorithm_searching.html#minCount
    std::vector<int> const a = {2, 3, 4, 1, 2, 4, 1, 1, 2};

    // Minimum is 1 and occurs 3 times
    BOOST_CHECK_EQUAL(ural::min_count(a)[ural::_1], 1);
    BOOST_CHECK_EQUAL(ural::min_count(a)[ural::_2], 3);
    BOOST_CHECK(ural::min_count(a) == std::make_tuple(1, 3));

    // Maximum is 4 and occurs 2 times
    BOOST_CHECK(ural::min_count(a, ural::greater<>{}) == std::make_tuple(4, 2));
}

BOOST_AUTO_TEST_CASE(skip_over_test)
{
    // Тесты взяты из
    // http://dlang.org/phobos/std_algorithm_searching.html#skipOver
    auto const str1 = std::string("Hello world");
    auto s1 = ural::sequence(str1);

    auto const ha = std::string("Ha");
    BOOST_CHECK(!ural::skip_over(s1, ha));
    URAL_CHECK_EQUAL_RANGES(s1, str1);

    auto const hell = std::string("Hell");
    auto const o_world = std::string("o world");
    BOOST_CHECK( ural::skip_over(s1, hell));
    URAL_CHECK_EQUAL_RANGES(s1, o_world);
}
