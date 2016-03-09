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

#include <boost/mpl/list.hpp>
#include <boost/test/unit_test.hpp>
#include "../../defs.hpp"

namespace
{
    namespace ural_ex = ::ural::experimental;
}

BOOST_AUTO_TEST_CASE(set_union_sequence_test)
{
    std::vector<int> v1 = {1, 2, 3, 4, 5};
    std::vector<int> v2 = {      3, 4, 5, 6, 7};

    std::vector<long> r_std;
    std::set_union(v1.begin(), v1.end(), v2.begin(), v2.end(),
                   std::back_inserter(r_std));

    auto const r_ural = ural_ex::make_set_union_sequence(v1, v2)
                      | ural_ex::to_container<std::vector>{};

    URAL_CHECK_EQUAL_RANGES(r_std, r_ural);
}

BOOST_AUTO_TEST_CASE(set_intersection_sequence_test)
{
    std::vector<int> const v1{1,2,3,4,5,6,7,8};
    std::vector<int> const v2{        5,  7,  9,10};

    std::vector<int> std_intersection;
    std::set_intersection(v1.begin(), v1.end(),
                          v2.begin(), v2.end(),
                          std::back_inserter(std_intersection));

    auto const ural_intersection
        = ural_ex::make_set_intersection_sequence(v1, v2)
        | ural_ex::to_container<std::vector>{};

    URAL_CHECK_EQUAL_RANGES(std_intersection, ural_intersection);
}

BOOST_AUTO_TEST_CASE(set_difference_sequence_test)
{
    std::vector<int> v1 {1, 2, 5, 5, 5, 9};
    std::vector<int> v2 {   2, 5,       7};

    std::vector<int> std_diff;
    std::set_difference(v1.begin(), v1.end(), v2.begin(), v2.end(),
                        std::back_inserter(std_diff));
    auto const ural_diff = ural_ex::make_set_difference_sequence(v1, v2)
                         | ural_ex::to_container<std::vector>{};

    URAL_CHECK_EQUAL_RANGES(std_diff, ural_diff);
}

BOOST_AUTO_TEST_CASE(set_symmetric_difference_sequence_test)
{
    std::vector<int> const v1{1,2,3,4,5,6,7,8     };
    std::vector<int> const v2{        5,  7,  9,10};

    std::vector<int> r_std;
    std::set_symmetric_difference(v1.begin(), v1.end(), v2.begin(), v2.end(),
                                  std::back_inserter(r_std));

    auto const r_ural
        = ural_ex::make_set_symmetric_difference_sequence(v1, v2)
        | ural_ex::to_container<std::vector>{};

    URAL_CHECK_EQUAL_RANGES(r_ural, r_std);
}

namespace
{
    template <template <class...> class SO>
    struct set_op_sequence_maker
    {
        template <class S1, class S2>
        SO<ural::SequenceType<S1>, ural::SequenceType<S2>>
        operator()(S1 && s1, S2 && s2) const
        {
            using Result = SO<ural::SequenceType<S1>, ural::SequenceType<S2>>;
            using ural::sequence;
            return Result(sequence(s1), sequence(s2));
        }
    };

    using SetOperationMakers
        = boost::mpl::list<set_op_sequence_maker<ural_ex::merge_sequence>,
                           set_op_sequence_maker<ural_ex::set_union_sequence>,
                           set_op_sequence_maker<ural_ex::set_difference_sequence>,
                           set_op_sequence_maker<ural_ex::set_intersection_sequence>,
                           set_op_sequence_maker<ural_ex::set_symmetric_difference_sequence>>;
}

BOOST_AUTO_TEST_CASE_TEMPLATE(set_operations_traversed_front, Maker, SetOperationMakers)
{
    Maker maker;

    std::forward_list<int> const v1{1,2,3,4,5,6,7,8     };
    std::forward_list<int> const v2{    3,  5,  7,  9,10};

    auto const n = 2;

    auto s1 = maker(v1, v2);
    auto s2 = maker(v1 | ural_ex::assumed_infinite, v2 | ural_ex::assumed_infinite);

    static_assert(!std::is_same<decltype(s2), decltype(s2.traversed_front())>::value, "");

    ural::advance(s1, n);
    ural::advance(s2, n);

    auto rs1 = s1.traversed_front();
    auto rs2 = s2.traversed_front();
    BOOST_CHECK(rs1 == rs2);
}
