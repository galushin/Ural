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

#include <ural/format.hpp>

#include <ural/numeric/numbers_sequence.hpp>
#include <ural/sequence/to.hpp>
#include <ural/sequence/by_line.hpp>
#include <ural/sequence/make.hpp>
#include <ural/algorithm.hpp>

#include <boost/test/unit_test.hpp>

#include <forward_list>
#include <sstream>

#include "defs.hpp"

BOOST_AUTO_TEST_CASE(to_string_test)
{
    for(auto n : ural::numbers(-100, 101))
    {
        std::ostringstream os;
        os << n;

        auto const s = ural::to_string(n);

        BOOST_CHECK_EQUAL(s, os.str());
    }
}

BOOST_AUTO_TEST_CASE(to_wstring_test)
{
    for(auto n : ural::numbers(-100, 101))
    {
        std::wostringstream os;
        os << n;

        auto const s = ural::to_wstring(n);

        BOOST_CHECK(s == os.str());
    }
}

BOOST_AUTO_TEST_CASE(by_line_test)
{
    std::vector<std::string> const z = {"Occupation", "Carpenter", "Blacksmith"};

    std::ostringstream os;
    std::copy(z.begin(), z.end(), std::ostream_iterator<std::string>(os, "\n"));

    std::istringstream is(os.str());

    auto const x = ural::by_line(is) | ural::to_container<std::vector>{};

    URAL_CHECK_EQUAL_RANGES(z, x);
}

BOOST_AUTO_TEST_CASE(by_line_test_keep_delimiter)
{
    std::vector<std::string> const z = {"Occupation\n", "Carpenter\n", "Blacksmith\n"};

    std::ostringstream os;
    std::copy(z.begin(), z.end(), std::ostream_iterator<std::string>(os, ""));

    std::istringstream is(os.str());

    auto const x = ural::by_line(is, '\n', ural::keep_delimiter::yes)
                   | ural::to_container<std::forward_list>{};

    URAL_CHECK_EQUAL_RANGES(z, x);
}

BOOST_AUTO_TEST_CASE(by_line_test_keep_delimiter_unexpected_eof)
{
    std::vector<std::string> const z = {"Occupation\n", "Carpenter\n", "Blacksmith"};

    std::ostringstream os;
    std::copy(z.begin(), z.end(), std::ostream_iterator<std::string>(os, ""));

    std::istringstream is(os.str());

    auto const x = ural::by_line(is, '\n', ural::keep_delimiter::yes)
                   | ural::to_container<std::forward_list>{};

    URAL_CHECK_EQUAL_RANGES(z, x);
}

BOOST_AUTO_TEST_CASE(by_line_test_custom_separator)
{
    std::vector<std::string> const z = {"Occupation", "Carpenter", "Blacksmith"};
    auto const separator = '\t';

    std::ostringstream os;
    std::copy(z.begin(), z.end(), std::ostream_iterator<std::string>(os, "\t"));

    std::istringstream is(os.str());

    auto const x = ural::by_line(is, separator) | ural::to_container<std::list>{};

    URAL_CHECK_EQUAL_RANGES(z, x);
}

#include <ural/numeric/numbers_sequence.hpp>

BOOST_AUTO_TEST_CASE(table_io_test)
{
    typedef double Type;

    std::vector<std::vector<Type>> const data_src =
    {
        {1, 1.5, 2},
        {3, 4, 4.5},
        {5.5, 6, 6.5},
        {-1, 0, 1}
    };

    std::ostringstream os;
    ural::write_table(os, data_src);

    // текстовые редакторы иногда добавляют пустую строку в конец файла
    os << "\n";

    std::istringstream is(os.str());
    auto data = ural::read_table<Type>(is);

    BOOST_CHECK_EQUAL(data_src.size(), data.size());

    for(auto i : ural::indices_of(data))
    {
        BOOST_CHECK_EQUAL(data_src[i].size(), data[i].size());
        BOOST_CHECK(data_src[i] == data[i]);
    }
}

BOOST_AUTO_TEST_CASE(table_io_test_temporary_stream)
{
    typedef double Type;

    std::vector<std::vector<Type>> const data_src =
    {
        {1, 1.5, 2},
        {3, 4, 4.5},
        {5.5, 6, 6.5},
        {-1, 0, 1}
    };

    auto & os = ural::write_table(std::ostringstream{}, data_src);

    // текстовые редакторы иногда добавляют пустую строку в конец файла
    os << "\n";

    auto data = ural::read_table<Type>(std::istringstream(os.str()));

    BOOST_CHECK_EQUAL(data_src.size(), data.size());

    for(auto i = 0U; i != data.size(); ++ i)
    {
        BOOST_CHECK_EQUAL(data_src[i].size(), data[i].size());
        BOOST_CHECK(data_src[i] == data[i]);
    }
}

BOOST_AUTO_TEST_CASE(ostream_delimited_test)
{
    std::vector<int> const xs = {1, 2, 3, 4, 5};

    std::ostringstream os_1;
    ural::write_separated(os_1, xs, ", ") << "\n";

    std::ostringstream os_2;
    os_2 << ural::separated(xs, ", ") << "\n";

    BOOST_CHECK_EQUAL(os_1.str(), os_2.str());
}
