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

#include "../defs.hpp"
#include <boost/test/unit_test.hpp>

#include <ural/container/vector.hpp>

#include <forward_list>
#include <list>
#include <vector>

BOOST_AUTO_TEST_CASE(push_front_range_test)
{
    ural_test::istringstream_helper<int> const in = {2, 7, 1, 8, 2, 8};

    std::forward_list<int> out_1 = {3, 1, 4, 1, 5};
    auto out_2 = out_1;

    assert(out_2 == out_1);

    ural::copy(in, out_1 | ural::front_inserter);
    auto & out_2_ref = ural::push_front(out_2, in);

    URAL_CHECK_EQUAL_RANGES(out_1, out_2);
    BOOST_CHECK_EQUAL(&out_2_ref, &out_2);
}

BOOST_AUTO_TEST_CASE(push_back_range_test)
{
    ural_test::istringstream_helper<int> const in = {2, 7, 1, 8, 2, 8};

    std::list<int> out_1 = {3, 1, 4, 1, 5};
    auto out_2 = out_1;

    assert(out_2 == out_1);

    ural::copy(in, out_1 | ural::back_inserter);
    auto & out_2_ref = ural::push_back(out_2, in);

    URAL_CHECK_EQUAL_RANGES(out_1, out_2);
    BOOST_CHECK_EQUAL(&out_2_ref, &out_2);
}

BOOST_AUTO_TEST_CASE(insert_range_test)
{
    std::vector<int> const source = {2, 7, 1, 8, 2, 8};
    ural_test::istringstream_helper<int> const in(source.begin(), source.end());

    std::list<int> out_1 = {3, 1, 4, 1, 5};
    auto out_2 = out_1;

    assert(out_2 == out_1);

    auto const pos_1 = std::next(out_1.begin(), out_1.size() / 2);
    auto const pos_2 = std::next(out_2.begin(), out_2.size() / 2);

    out_1.insert(pos_1, source.begin(), source.end());

    auto & out_2_ref = ural::insert(out_2, pos_2, in);

    URAL_CHECK_EQUAL_RANGES(out_1, out_2);
    BOOST_CHECK_EQUAL(&out_2_ref, &out_2);
}

// Комбинирование вызовов модифицирующих алгоритмов с erase
BOOST_AUTO_TEST_CASE(unqiue_fn_const_iterator_test)
{
    ural::vector<int> v1 = {1, 2, 3, 4, 5, 6};
    auto v2 = v1;

    auto const n = v1.size() / 2;

    v2.erase(v2.cbegin() + n, v2.cend());

    ural::erase(v1, ural::make_iterator_sequence(v1.cbegin() + n, v1.cend()));

    URAL_CHECK_EQUAL_RANGES(v1, v2);
}

BOOST_AUTO_TEST_CASE(unique_erase_combination_test)
{
    std::vector<int> v_std{1,2,3,1,2,3,3,4,5,4,5,6,7};
    std::sort(v_std.begin(), v_std.end());

    auto v_ural = v_std;

    // std
    auto const last = std::unique(v_std.begin(), v_std.end());
    v_std.erase(last, v_std.end());

    // ural
    auto const to_erase = ural::unique(v_ural);
    ural::erase(v_ural, to_erase);

    // Сравнение результатов
    URAL_CHECK_EQUAL_RANGES(v_std, v_ural);
}

BOOST_AUTO_TEST_CASE(unique_erase_test)
{
    std::vector<int> v_std{1,2,3,1,2,3,3,4,5,4,5,6,7};
    std::sort(v_std.begin(), v_std.end());

    auto v_ural = v_std;

    // std
    auto const last = std::unique(v_std.begin(), v_std.end());
    v_std.erase(last, v_std.end());

    // ural
    ural::unique_erase(v_ural);

    // Сравнение результатов
    URAL_CHECK_EQUAL_RANGES(v_std, v_ural);
}
