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
#include <ural/sequence/make.hpp>
#include <ural/sequence/constant.hpp>

#include <boost/test/unit_test.hpp>

namespace
{
    namespace ural_ex = ::ural::experimental;
}

BOOST_AUTO_TEST_CASE(constant_sequence_explcit_single_pass)
{
    auto const value = 'z';
    auto const n = 17;

    std::string const expected(n, value);

    std::string actual(n, -1);

    BOOST_CHECK_EQUAL(expected.size(), actual.size());
    BOOST_CHECK_NE(expected, actual);

    ural_ex::constant_sequence<char, ural::single_pass_cursor_tag> seq(value);

    BOOST_CONCEPT_ASSERT((ural::concepts::ReadableSequence<decltype(seq)>));
    BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassSequence<decltype(seq)>));

    ural::copy(std::move(seq), actual);

    BOOST_CHECK_EQUAL(actual, expected);
}

BOOST_AUTO_TEST_CASE(constant_sequence_explcit_forward)
{
    auto const value = 'z';
    auto const n = 17;

    std::string const expected(n, value);

    std::string actual(n, -1);

    BOOST_CHECK_EQUAL(expected.size(), actual.size());
    BOOST_CHECK_NE(expected, actual);

    using CS = ural_ex::constant_sequence<char, ural::forward_cursor_tag>;
    auto const seq = CS(value);

    BOOST_CONCEPT_ASSERT((ural::concepts::ReadableSequence<CS>));
    BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassSequence<CS>));

    auto r_seq = ural::copy(std::move(seq), actual)[ural::_1];

    BOOST_CHECK_EQUAL(actual, expected);
    BOOST_CHECK(ural::equal(r_seq.traversed_front(), expected));
    BOOST_CHECK(r_seq.original() == seq);

    BOOST_CHECK(r_seq != seq);
    r_seq.shrink_front();

    BOOST_CHECK(r_seq == seq);
}

// @todo explicit random access
