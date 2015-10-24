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

#include <ural/sequence/base.hpp>

// @todo fibonacci_sequence в ural, возможно обобщив
namespace
{
    template <class Integer>
    class fibonacci_sequence
     : public ural::sequence_base<fibonacci_sequence<Integer>>
    {
    public:
        using traversal_tag = ural::single_pass_traversal_tag;
        using value_type = Integer;
        using reference = value_type const &;
        using pointer = value_type const *;
        using distance_type = Integer;

        fibonacci_sequence()
         : prev_(0)
         , cur_(1)
        {}

        bool operator!() const
        {
            return false;
        }

        reference front() const
        {
            return this->cur_;
        }

        void pop_front()
        {
            auto new_value = prev_ + cur_;
            prev_ = std::move(cur_);
            cur_ = std::move(new_value);
        }

    private:
        Integer prev_;
        Integer cur_;
    };
}

#include <ural/sequence/all.hpp>
#include <ural/algorithm.hpp>
#include <ural/numeric.hpp>

#include "../defs.hpp"

#include <boost/test/unit_test.hpp>

#include <forward_list>
#include <list>
#include <vector>

// @todo такой же тест с переставленным filtered и taken_while
BOOST_AUTO_TEST_CASE(PE_002_fibonacci_via_pipes)
{
    using Integer = long long;
    auto const n = Integer{4'000'000};
    auto seq = fibonacci_sequence<Integer>()
             | ural::filtered(ural::is_even)
             | ural::taken_while([n](Integer const & x) { return x < n; });

    BOOST_CHECK_EQUAL(ural::accumulate(std::move(seq), Integer{0}), 4'613'732);
}

// @todo тест прямой последовательности taken_while
BOOST_AUTO_TEST_CASE(taken_while_forward)
{
    std::vector<int> const xs = {2, 6, 4, 1, 8, 7};

    auto seq = xs | ural::taken_while(ural::is_even);

    auto xs_prefix
        = ural::find_if_not(xs, ural::is_even).traversed_front()
        | ural::to_container<std::vector>{};
    ural::sort(xs_prefix);

    BOOST_CHECK(ural::is_permutation(seq, xs_prefix));
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
