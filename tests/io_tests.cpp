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

#include <ural/sequence/by_line.hpp>
#include <ural/algorithm.hpp>

#include <boost/test/unit_test.hpp>

#include <sstream>

BOOST_AUTO_TEST_CASE(by_line_test)
{
    std::vector<std::string> const z = {"Occupation", "Carpenter", "Blacksmith"};

    std::ostringstream os;
    std::copy(z.begin(), z.end(), std::ostream_iterator<std::string>(os, "\n"));

    std::istringstream is(os.str());

    auto seq = ural::by_line(is);

    std::vector<std::string> x;

    ural::copy(seq, x | ural::back_inserter);

    BOOST_CHECK_EQUAL_COLLECTIONS(z.begin(), z.end(), x.begin(), x.end());
}

BOOST_AUTO_TEST_CASE(by_line_test_keep_delimeter)
{
    std::vector<std::string> const z = {"Occupation\n", "Carpenter\n", "Blacksmith\n"};

    std::ostringstream os;
    std::copy(z.begin(), z.end(), std::ostream_iterator<std::string>(os, ""));

    std::istringstream is(os.str());

    auto seq = ural::by_line(is, '\n', ural::keep_delimeter::yes);

    std::vector<std::string> x;

    ural::copy(seq, x | ural::back_inserter);

    BOOST_CHECK_EQUAL_COLLECTIONS(z.begin(), z.end(), x.begin(), x.end());
}

BOOST_AUTO_TEST_CASE(by_line_test_keep_delimeter_nexpected_eof)
{
    std::vector<std::string> const z = {"Occupation\n", "Carpenter\n", "Blacksmith"};

    std::ostringstream os;
    std::copy(z.begin(), z.end(), std::ostream_iterator<std::string>(os, ""));

    std::istringstream is(os.str());

    auto seq = ural::by_line(is, '\n', ural::keep_delimeter::yes);

    std::vector<std::string> x;

    ural::copy(seq, x | ural::back_inserter);

    BOOST_CHECK_EQUAL_COLLECTIONS(z.begin(), z.end(), x.begin(), x.end());
}

BOOST_AUTO_TEST_CASE(by_line_test_custom_separator)
{
    std::vector<std::string> const z = {"Occupation", "Carpenter", "Blacksmith"};
    auto const separator = '\t';

    std::ostringstream os;
    std::copy(z.begin(), z.end(), std::ostream_iterator<std::string>(os, "\t"));

    std::istringstream is(os.str());

    auto seq = ural::by_line(is, separator);

    std::vector<std::string> x;

    ural::copy(seq, x | ural::back_inserter);

    BOOST_CHECK_EQUAL_COLLECTIONS(z.begin(), z.end(), x.begin(), x.end());
}
