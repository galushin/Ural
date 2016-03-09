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
#include <ural/numeric.hpp>
#include <ural/numeric/numbers_sequence.hpp>
#include <ural/sequence/chunks.hpp>
#include <ural/sequence/to.hpp>

#include <forward_list>

#include <boost/test/unit_test.hpp>
#include "../defs.hpp"

namespace
{
    namespace ural_ex = ::ural::experimental;
}

BOOST_AUTO_TEST_CASE(chunks_sequence_test)
{
    typedef std::forward_list<int> Source;
    auto const seq = ural::numbers(1, 8);
    Source src(begin(seq), end(seq));

    typedef ural::ValueType<Source> Value;
    std::vector<std::vector<Value>> expected { {1, 2, 3}, {4, 5, 6}, {7}};

    auto ch = ::ural::experimental::make_chunks_sequence(src, 3);

    BOOST_CONCEPT_ASSERT((::ural::concepts::ForwardSequence<decltype(ch)>));

    for(auto const & r : expected)
    {
        std::vector<Value> a;
        ural::copy(*ch, a | ural::back_inserter);

        BOOST_CHECK(!!ch);
        BOOST_CHECK(::ural::equal(r, *ch));
        URAL_CHECK_EQUAL_RANGES(r, a);

        ++ ch;
    }

    BOOST_CHECK(!ch);
}

BOOST_AUTO_TEST_CASE(chunks_sequence_equality_test)
{
    std::vector<int> const src0;
    std::vector<int> const src1 = {1, 2, 3};

    auto const n1 = 3;
    auto const n2 = n1 + 1;

    auto const s01 = src0 | ural::experimental::chunked(n1);
    auto const s11 = src1 | ural::experimental::chunked(n1);
    auto const s12 = src1 | ural::experimental::chunked(n2);

    BOOST_CHECK(s01 == s01);
    BOOST_CHECK(s11 == s11);
    BOOST_CHECK(s12 == s12);

    BOOST_CHECK(s01 != s11);
    BOOST_CHECK(s01 != s12);
    BOOST_CHECK(s11 != s12);
}

BOOST_AUTO_TEST_CASE(chunks_sequence_original_test)
{
    auto const seq = ural::numbers(1, 22);

    auto cs = seq | ural::experimental::chunked(3);

    auto const n = ural::size(cs);

    auto cs_2 = ural::next(cs, n / 2);

    BOOST_CHECK(!!cs_2.traversed_front());
    BOOST_CHECK(cs_2 != cs);
    BOOST_CHECK(cs_2.original() == cs);

    ural::exhaust_front(cs);

    BOOST_CHECK(cs.traversed_front() == cs_2.original());
}

BOOST_AUTO_TEST_CASE(chunks_sequence_traversed_front_test)
{
    auto const seq = ural_ex::make_arithmetic_progression(1, 3);

    auto cs = seq | ural::experimental::chunked(3);

    std::vector<int> v1(13, -1);

    auto f = [](auto const & s) { return ural::accumulate(s, 0); };

    auto res = ural::transform(cs, v1, f)[ural::_1].traversed_front();

    std::vector<int> v2;
    ural::transform(res, v2 | ural::back_inserter, f);

    URAL_CHECK_EQUAL_RANGES(v1, v2);
}

BOOST_AUTO_TEST_CASE(chunks_sequence_random_access)
{
    auto const xs = ural::numbers(1, 23) | ural_ex::to_container<std::vector>{};
    auto seq = xs | ural_ex::chunked(3);

    BOOST_CONCEPT_ASSERT((ural::concepts::RandomAccessSequence<decltype(seq)>));

    BOOST_CHECK(seq[0] == seq.front());
    BOOST_CHECK(ural::equal(seq[0], ural::numbers(1, 4)));
    BOOST_CHECK(ural::equal(seq[1], ural::numbers(4, 7)));

    BOOST_CHECK_EQUAL(static_cast<size_t>(seq.size()), xs.size() / seq.chunk_size() + 1);
    BOOST_CHECK_EQUAL(static_cast<size_t>((xs | ural::experimental::chunked(2)).size()),
                      xs.size() / 2);

    auto s2 = seq + 2;

    BOOST_CHECK(s2.front() == seq[2]);
    BOOST_CHECK(s2.original() == seq.original());
}

