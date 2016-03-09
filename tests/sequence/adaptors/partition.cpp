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
#include <ural/numeric/numbers_sequence.hpp>

#include <boost/test/unit_test.hpp>

namespace
{
    namespace ural_ex = ::ural::experimental;
}

BOOST_AUTO_TEST_CASE(partition_sequence_regeression_90_shorter_1)
{
    auto const src = ural::numbers(1, 10);

    auto const pred = ural::is_even;

    auto const n_true = ural::count_if(src, pred);
    auto const n_false = ural::count_if(src, ural::not_fn(pred));

    BOOST_CHECK_GT(n_true, 0);

    std::vector<int> sink_true(n_true - 1, -1);
    std::vector<int> sink_false(n_false + 1, -1);

    auto sink = ural_ex::make_partition_sequence(sink_true, sink_false, pred);

    auto const result = ural::copy(src, sink)[ural::_2];

    BOOST_CHECK(!result.true_sequence());
    BOOST_CHECK(!!result.false_sequence());
}

BOOST_AUTO_TEST_CASE(partition_sequence_regeression_90_shorter_2)
{
    auto const src = ural::numbers(1, 10);

    auto const pred = ural::is_even;

    auto const n_true = ural::count_if(src, pred);
    auto const n_false = ural::count_if(src, ural::not_fn(pred));

    BOOST_CHECK_GT(n_true, 0);

    std::vector<int> sink_true(n_true + 1, -1);
    std::vector<int> sink_false(n_false - 1, -1);

    auto sink = ural_ex::make_partition_sequence(sink_true, sink_false, pred);

    auto const result = ural::copy(src, sink)[ural::_2];

    BOOST_CHECK(!!result.true_sequence());
    BOOST_CHECK(!result.false_sequence());
}
