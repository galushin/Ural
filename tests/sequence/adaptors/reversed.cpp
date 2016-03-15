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

#include <boost/test/unit_test.hpp>
#include "../../defs.hpp"

#include <ural/sequence/adaptors/reversed.hpp>
#include <ural/sequence/all.hpp>

#include <ural/abi.hpp>

namespace
{
    namespace ural_ex = ::ural::experimental;
}

BOOST_AUTO_TEST_CASE(reversed_reversed_test)
{
    std::vector<int> const xs = {1, 2, 3, 4, 5};
    auto s = ural::cursor(xs);
    auto rr = s | ural_ex::reversed | ural_ex::reversed;

    BOOST_CHECK_EQUAL(ural_ex::abi::demangle_name(typeid(s).name()),
                      ural_ex::abi::demangle_name(typeid(rr).name()));
    BOOST_CHECK(typeid(s).name() == typeid(rr).name());
    BOOST_CHECK(typeid(s) == typeid(rr));
}

#include <ural/algorithm.hpp>

BOOST_AUTO_TEST_CASE(reversed_iterators_to_sequence_test)
{
    std::vector<int> const xs = {1, 2, 3, 4, 5};
    auto r_begin = xs.rbegin();
    auto r_end = xs.rend();

    auto rs = ural::experimental::make_iterator_cursor(r_begin, r_end);

    using RSequence = ural_ex::reverse_sequence<ural::iterator_cursor<decltype(xs.begin())>>;

    static_assert(std::is_same<decltype(rs), RSequence>::value, "");

    BOOST_CHECK(r_begin.base() == rs.base().end());
    BOOST_CHECK(r_end.base() == rs.base().begin());

    std::vector<int> result;
    ural::copy(rs, std::back_inserter(result));

    BOOST_CHECK_EQUAL_COLLECTIONS(xs.begin(), xs.end(),
                                  result.rbegin(), result.rend());
}

BOOST_AUTO_TEST_CASE(reversed_exhaust_test)
{
    std::vector<int> const xs = {1, 2, 3, 4, 5};

    auto const xs_reversed = xs | ural_ex::reversed;

    BOOST_CHECK(ural::is_permutation(xs, xs_reversed));

    auto s1 = xs_reversed;
    ural::exhaust_front(s1);

    BOOST_CHECK(!s1);
    BOOST_CHECK(!s1.traversed_back());
    BOOST_CHECK(s1.original() == xs_reversed);
    BOOST_CHECK(s1.traversed_front() == xs_reversed);

    auto s2 = xs_reversed;
    s2.exhaust_back();

    BOOST_CHECK(!s2);
    BOOST_CHECK(!s2.traversed_front());
    BOOST_CHECK(s2.original() == xs_reversed);
    BOOST_CHECK(s2.traversed_back() == xs_reversed);
}

BOOST_AUTO_TEST_CASE(reversed_pop_back_n_test)
{
    auto const xs = ural_ex::make_arithmetic_progression(0, 1)
                  | ural_ex::taken(10) | ural_ex::to_container<std::vector>{};

    auto s = ural::cursor(xs);
    auto s_r = s | ural_ex::reversed;

    auto const n = xs.size() / 3;

    s += n;
    s_r.pop_back(n);

    BOOST_CHECK(s == s_r.base());

    BOOST_CHECK_EQUAL_COLLECTIONS(s.begin(), s.end(),
                                  s_r.base().begin(), s_r.base().end());

    s.pop_back(n);
    s_r += n;

    BOOST_CHECK(s == s_r.base());

    URAL_CHECK_EQUAL_RANGES(s, s_r.base());

    auto b   = s.traversed_front();
    auto b_r = s_r.traversed_back();

    BOOST_CHECK(b == b_r.base());

    s.shrink_front();
    s_r.shrink_back();

    BOOST_CHECK(s == s_r.base());
}

BOOST_AUTO_TEST_CASE(copy_reversed_to_reversed_vs_copy_backward)
{
    std::vector<int> x_std = {1, 2, 3, 4, 5};
    std::vector<int> x_ural = x_std;

    std::copy_backward(x_std.begin(), x_std.end() - 1, x_std.end());

    auto src = ural::make_iterator_cursor(x_ural.begin(), x_ural.end() - 1);

    ural::copy(src | ural_ex::reversed, x_ural | ural_ex::reversed);

    URAL_CHECK_EQUAL_RANGES(x_std, x_ural);
}

#include <ural/memory.hpp>

BOOST_AUTO_TEST_CASE(moved_backward_test_unique_ptr)
{
    typedef std::unique_ptr<int> Type;

    std::vector<int> const ys = {25, -15, 5, -5, 15};
    std::vector<Type> xs1;
    std::vector<Type> xs2;

    for(auto & y : ys)
    {
        xs1.emplace_back(ural::make_unique<int>(y));
        xs2.emplace_back(ural::make_unique<int>(y));
    }

    std::move_backward(xs1.begin(), xs1.end() - 1, xs1.end());

    auto src = ural::make_iterator_cursor(xs2.begin(), xs2.end() - 1);
    ural::copy(src | ural_ex::reversed | ural_ex::moved, xs2 | ural_ex::reversed);

    for(size_t i = 0; i < xs1.size(); ++ i)
    {
        BOOST_CHECK(!xs1.at(i) == !xs2.at(i));

        if(!!xs1.at(i))
        {
            BOOST_CHECK_EQUAL(*xs1.at(i), *xs2.at(i));
        }
    }
}

BOOST_AUTO_TEST_CASE(reversed_iterator_sequence_iterators)
{
    typedef std::vector<int> Container;
    Container v1 = {0, 2, 4, 6};
    auto const v2 = v1;

    auto const rs1 = ural::cursor(v1) | ural_ex::reversed;
    auto const rs2 = ural::cursor(v2) | ural_ex::reversed;

    static_assert(std::is_same<decltype(begin(rs1)), Container::reverse_iterator>::value, "");
    static_assert(std::is_same<decltype(begin(rs2)), Container::const_reverse_iterator>::value, "");

    BOOST_CHECK(begin(rs1) == v1.rbegin());
    BOOST_CHECK(end(rs1) == v1.rend());

    BOOST_CHECK(begin(rs2) == v2.rbegin());
    BOOST_CHECK(end(rs2) == v2.rend());
}

BOOST_AUTO_TEST_CASE(reversed_copy_test)
{
    // Исходные данные
    std::list<int> const src = {1, 2, 3, 4, 5, 6};

    // std
    std::list<int> r_std;
    std::reverse_copy(src.begin(), src.end(), std::back_inserter(r_std));

    // ural
    auto const r_ural
        = src | ural_ex::reversed | ural_ex::to_container<std::list>{};

    // Проверка
    URAL_CHECK_EQUAL_RANGES(r_std, r_ural);
}
