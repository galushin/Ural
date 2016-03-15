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

#include "../../defs.hpp"
#include <boost/test/unit_test.hpp>

namespace
{
    namespace ural_ex = ::ural::experimental;
}

BOOST_AUTO_TEST_CASE(replace_sequence_if_test)
{
    std::array<int, 10> const s{5, 7, 4, 2, 8, 6, 1, 9, 0, 3};

    auto x_std = s;
    std::vector<int> x_ural;

    auto pred = +[](int x) {return x < 5;};
    auto const new_value = 55;

    // std
    std::replace_if(x_std.begin(), x_std.end(), pred, new_value);

    // ural
    auto seq = s | ural_ex::replaced_if(pred, new_value);
    ural::copy(seq, x_ural | ural::back_inserter);

    BOOST_CHECK(seq == seq);

    URAL_CHECK_EQUAL_RANGES(x_std, x_ural);
    BOOST_CHECK_EQUAL(seq.new_value(), new_value);
    BOOST_CHECK_EQUAL(seq.predicate(), pred);
}

BOOST_AUTO_TEST_CASE(replace_sequence_if_regression_pass_by_cref)
{
    // Подготовка
    std::array<int, 10> const s{5, 7, 4, 2, 8, 6, 1, 9, 0, 3};
    auto pred = [](int x) {return x < 5;};
    auto const new_value = 55;

    // std
    auto x_std = s;
    std::replace_if(x_std.begin(), x_std.end(), pred, new_value);

    // ural
    std::vector<int> x_ural;
    ural::copy(ural_ex::make_replace_if_sequence(s, pred, std::cref(new_value)),
               x_ural | ural::back_inserter);

    // Сравнение
    URAL_CHECK_EQUAL_RANGES(x_std, x_ural);
}

BOOST_AUTO_TEST_CASE(replace_if_sequence_traversed_front)
{
    std::forward_list<int> const src = {1, 2, 3, 4, 5, 6, 7, 9, 11};

    auto const new_value = -1;
    auto const n = ural::size(ural::cursor(src));

    auto s1 = src | ural_ex::replaced_if(ural::is_even, new_value);
    auto s2 = src | ural_ex::assumed_infinite | ural_ex::replaced_if(ural::is_even, new_value);

    static_assert(!std::is_same<decltype(s2), decltype(s2.traversed_front())>::value, "");

    ural::advance(s1, n);
    ural::advance(s2, n);

    BOOST_CHECK(s1.traversed_front() == s2.traversed_front());
}
