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

#include <ural/math/fibonacci.hpp>

#include <ural/sequence/all.hpp>
#include <ural/algorithm.hpp>
#include <ural/numeric.hpp>

#include "../../defs.hpp"

#include <boost/test/unit_test.hpp>

#include <forward_list>
#include <list>
#include <vector>

namespace
{
    namespace ural_ex = ::ural::experimental;
}
// namespace

// @todo такой же тест с переставленным filtered и taken_while
BOOST_AUTO_TEST_CASE(PE_002_fibonacci_via_pipes)
{
    using Integer = long long;
    auto const n = Integer{4'000'000};
    auto seq = ural_ex::fibonacci_sequence<Integer>()
             | ural::filtered(ural::is_even)
             | ural::taken_while([n](Integer const & x) { return x < n; });

    BOOST_CHECK_EQUAL(ural::accumulate(std::move(seq), Integer{0}), 4'613'732);
}

BOOST_AUTO_TEST_CASE(PE_002_fibonacci_via_pipes_traversed_front)
{
    using Integer = long long;
    auto const n = Integer{4'000'000};
    auto const pred = [n](Integer const & x) { return x >= n; };

    auto s1 = ural_ex::fibonacci_sequence<Integer, ural::use_default, ural::forward_cursor_tag>()
            | ural::filtered(ural::is_even);

    auto seq = ural::find_if(std::move(s1), pred).traversed_front();

    BOOST_CHECK_EQUAL(ural::accumulate(std::move(seq), Integer{0}), 4'613'732);
}

BOOST_AUTO_TEST_CASE(PE_002_fibonacci_via_pipes_forward)
{
    using Integer = long long;
    auto const n = Integer{4'000'000};

    auto seq = ural_ex::fibonacci_sequence<Integer, ural::use_default, ural::forward_cursor_tag>()
             | ural::filtered(ural::is_even)
             | ural::taken_while([n](Integer const & x) { return x < n; });

    BOOST_CHECK_EQUAL(ural::accumulate(std::move(seq), Integer{0}), 4'613'732);
}

BOOST_AUTO_TEST_CASE(taken_while_forward)
{
    std::vector<int> const xs = {2, 6, 4, 1, 8, 7};
    auto pred = ural::is_even;

    auto seq = xs | ural::taken_while(pred);

    static_assert(std::is_empty<decltype(pred)>::value, "");
    using ural::sequence;
    static_assert(sizeof(seq) == sizeof(sequence(xs)), "");

    auto xs_prefix
        = ural::find_if_not(xs, pred).traversed_front()
        | ural::to_container<std::vector>{};
    ural::sort(xs_prefix);

    BOOST_CHECK(ural::is_permutation(seq, xs_prefix));
}

BOOST_AUTO_TEST_CASE(taken_while_traversed_front)
{
    std::vector<int> const xs = {2, 6, 4, 1, 8, 7};
    auto pred = ural::is_even;

    auto s2 = xs | ural::assumed_infinite | ural::taken_while(pred);

    for(; !!s2; ++ s2)
    {}

    auto const actual = s2.traversed_front();

    auto const expected = ural::find_if_not(xs, pred).traversed_front()
                        | ural::taken_while(pred);

    BOOST_CHECK(actual == expected);
}

BOOST_AUTO_TEST_CASE(taken_while_equality)
{
    std::vector<int> const x0 = {};
    std::vector<int> const x1 = {3, 1, 4, 1, 5};

    auto const p1 = +[](int x) { return ural::is_even(x); };
    auto const p2 = +[](int x) { return ural::is_odd(x); };

    auto const s01 = x0 | ural::taken_while(p1);
    auto const s02 = x0 | ural::taken_while(p2);
    auto const s11 = x1 | ural::taken_while(p1);
    auto const s12 = x1 | ural::taken_while(p2);

    BOOST_CHECK(s01 == s01);
    BOOST_CHECK(s01 != s02);
    BOOST_CHECK(s01 != s11);
    BOOST_CHECK(s01 != s12);

    BOOST_CHECK(s02 != s01);
    BOOST_CHECK(s02 == s02);
    BOOST_CHECK(s02 != s11);
    BOOST_CHECK(s02 != s12);

    BOOST_CHECK(s11 != s01);
    BOOST_CHECK(s11 != s02);
    BOOST_CHECK(s11 == s11);
    BOOST_CHECK(s11 != s12);

    BOOST_CHECK(s12 != s01);
    BOOST_CHECK(s12 != s02);
    BOOST_CHECK(s12 != s11);
    BOOST_CHECK(s12 == s12);
}

BOOST_AUTO_TEST_CASE(taken_while_concepts_checking)
{
    ural_test::istringstream_helper<int> in;
    std::forward_list<int> fwd;
    std::list<int> bidir;
    std::vector<int> ra;

    auto const pipe = ural::taken_while(ural::is_even);
    using namespace ural::concepts;

    BOOST_CONCEPT_ASSERT((SinglePassSequence<decltype(in | pipe)>));
    BOOST_CONCEPT_ASSERT((ReadableSequence<decltype(in | pipe)>));

    BOOST_CONCEPT_ASSERT((ForwardSequence<decltype(fwd | pipe)>));
    BOOST_CONCEPT_ASSERT((ReadableSequence<decltype(fwd | pipe)>));

    BOOST_CONCEPT_ASSERT((ForwardSequence<decltype(bidir | pipe)>));
    BOOST_CONCEPT_ASSERT((ReadableSequence<decltype(bidir | pipe)>));

    BOOST_CONCEPT_ASSERT((ForwardSequence<decltype(ra | pipe)>));
    BOOST_CONCEPT_ASSERT((ReadableSequence<decltype(ra | pipe)>));
}
