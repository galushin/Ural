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
#include <ural/sequence/progression.hpp>

#include <boost/test/unit_test.hpp>
#include "../../defs.hpp"

namespace
{
    namespace ural_ex = ::ural::experimental;
}

BOOST_AUTO_TEST_CASE(copy_n_analog_test)
{
    std::string const src = "1234567890";
    std::string r_std;
    std::string r_ural;

    auto const n = 4;

    std::copy_n(src.begin(), n, std::back_inserter(r_std));

    ural::copy(src | ural_ex::taken(n), std::back_inserter(r_ural));

    BOOST_CHECK_EQUAL(r_std, r_ural);
}

BOOST_AUTO_TEST_CASE(taken_taken_test_shorter)
{
    std::vector<int> const src = {1, 2, 3, 4, 5};

    auto const n1 = 4;
    auto const n2 = 3;

    auto s_composite = src | ural_ex::taken(n1) | ural_ex::taken(n2);
    auto s_direct = src | ural_ex::taken(std::min(n1, n2));

    static_assert(std::is_same<decltype(s_composite), decltype(s_direct)>::value, "");

    BOOST_CHECK(s_direct == s_composite);
}

BOOST_AUTO_TEST_CASE(taken_taken_test_longer)
{
    std::vector<int> const src = {1, 2, 3, 4, 5};

    auto const n1 = 3;
    auto const n2 = 4;

    auto s_composite = src | ural_ex::taken(n1) | ural_ex::taken(n2);
    auto s_direct = src | ural_ex::taken(std::min(n1, n2));

    static_assert(std::is_same<decltype(s_composite), decltype(s_direct)>::value, "");

    BOOST_CHECK(s_direct == s_composite);
}

BOOST_AUTO_TEST_CASE(fill_n_test_via_sequence_and_copy)
{
    std::vector<int> v_std{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto v_ural = v_std;

    auto const n = v_std.size() / 2;
    auto const value = -1;

    std::fill_n(v_std.begin(), n, value);
    auto r = ural::fill(v_ural | ural_ex::taken(n), value);

    BOOST_CHECK_EQUAL(ural::to_signed(n), r.base().traversed_front().size());
    BOOST_CHECK_EQUAL(ural::to_signed(v_std.size() - n), r.base().size());

    URAL_CHECK_EQUAL_RANGES(v_std, v_ural);
}

BOOST_AUTO_TEST_CASE(generate_n_test)
{
    // Подготовка
    auto const n = int{5};

    std::vector<int> r_std;
    auto r_ural = r_std;

    // std
    auto counter = int{0};
    auto gen = [&]{ return counter++; };
    std::generate_n(r_std | ural::back_inserter, n, gen);

    // ural
    counter = 0;
    ural::copy(ural::make_generator_sequence(gen) | ural_ex::taken(n),
               r_ural | ural::back_inserter);

    // Проверка
    URAL_CHECK_EQUAL_RANGES(r_std, r_ural);
}

BOOST_AUTO_TEST_CASE(take_sequence_more_than_size)
{
    std::vector<int> const z{11, 11, 22, 33, 55};

    std::vector<int> result;

    ural::copy(z | ural_ex::taken(z.size() + 10),  result | ural::back_inserter);

    URAL_CHECK_EQUAL_RANGES(z, result);
}

BOOST_AUTO_TEST_CASE(take_sequence_traversed_front)
{
    auto const n1 = 5;
    auto const n2 = 2 * n1;

    auto s0 = ural_ex::make_arithmetic_progression(0, 1) | ural_ex::taken(n2);

    auto s1 = s0.base() | ural_ex::taken_exactly(n1);

    auto s0_n1 = ural::next(s0, n1);

    BOOST_CHECK_EQUAL(s0_n1.count(), n2 - n1);

    auto s2 = s0_n1.traversed_front();

    BOOST_CHECK(s1.base() == s2.base());
    BOOST_CHECK_EQUAL(s1.size(), s2.size());

    BOOST_CHECK(s1 == s2);
}
