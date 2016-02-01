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

#include <ural/numeric.hpp>

#include <forward_list>
#include <forward_list>
#include <list>
#include <vector>

#include <boost/mpl/list.hpp>
#include <boost/test/unit_test.hpp>

namespace
{
    typedef boost::mpl::list<std::forward_list<int>,
                             std::list<int>,
                             std::vector<int>,
                             ural_test::istringstream_helper<int>> Sources;
}

// 25.2 Алгоритмы, не модифицирующие последовательность
// 25.2.1
BOOST_AUTO_TEST_CASE_TEMPLATE(all_of_test, Source, Sources)
{
    Source const is0{};
    Source const is1{2, 4, 6, 8, 10};
    Source const is2{2, 4, 6, 7, 10};

    BOOST_CHECK_EQUAL(ural::all_of(is0, ural::is_even), true);
    BOOST_CHECK_EQUAL(ural::all_of(is1, ural::is_even), true);
    BOOST_CHECK_EQUAL(ural::all_of(is2, ural::is_even), false);
}

// 25.2.2
BOOST_AUTO_TEST_CASE(any_of_test)
{
    typedef ural_test::istringstream_helper<int> Source;

    Source const is0{};
    Source const is1{2, 4, 6, 8, 10};
    Source const is2{2, 4, 6, 7, 10};

    BOOST_CHECK_EQUAL(ural::any_of(is0, ural::is_odd), false);
    BOOST_CHECK_EQUAL(ural::any_of(is1, ural::is_odd), false);
    BOOST_CHECK_EQUAL(ural::any_of(is2, ural::is_odd), true);
}

// 25.2.3
BOOST_AUTO_TEST_CASE(none_of_test)
{
    typedef ural_test::istringstream_helper<int> Source;

    Source const is0{};
    Source const is1{2, 4, 6, 8, 10};
    Source const is2{2, 4, 6, 7, 10};

    BOOST_CHECK_EQUAL(ural::none_of(is0, ural::is_odd), true);
    BOOST_CHECK_EQUAL(ural::none_of(is1, ural::is_odd), true);
    BOOST_CHECK_EQUAL(ural::none_of(is2, ural::is_odd), false);
}

// 25.2.4
BOOST_AUTO_TEST_CASE(for_each_test)
{
    std::vector<int> x_std = {1, 2, 3, 4, 5};
    std::forward_list<int> x_ural(x_std.begin(), x_std.end());

    auto const action = [](int & x) {x *= 2;};

    auto const r_std = std::for_each(x_std.begin(), x_std.end(), +action);
    auto const r_ural = ural::for_each(x_ural, +action);

    BOOST_CHECK(r_ural[ural::_1].original() == ural::sequence(x_ural));
    BOOST_CHECK(r_ural[ural::_1].traversed_front() == ural::sequence(x_ural));

    BOOST_CHECK_EQUAL(r_std, r_ural[ural::_2].target());

    URAL_CHECK_EQUAL_RANGES(x_std, x_ural);
}

BOOST_AUTO_TEST_CASE(for_each_input_sequence_test)
{
    std::vector<int> const src{1, 2, 3, 4, 5};
    ural_test::istringstream_helper<int> const src_ural(src);

    int sum = 0;
    ural::for_each(src_ural, [&sum](auto const & x) { sum += x; });

    BOOST_CHECK_EQUAL(sum, ural::accumulate(src, 0));
}

// 25.2.5
BOOST_AUTO_TEST_CASE(find_fail_test_istream)
{
    ural_test::istringstream_helper<int> const v{0, 1, 2, 3, 4};

    auto const value = -1;

    auto const r_ural = ural::find(v, value);

    BOOST_CHECK(!r_ural);
}

BOOST_AUTO_TEST_CASE(find_fail_test_forward_list)
{
    std::forward_list<int> const v{0, 1, 2, 3, 4};

    auto const value = -1;

    auto const r_std = std::find(v.begin(), v.end(), value);
    auto const r_ural = ural::find(v, value);

    BOOST_CHECK(r_std == v.end());
    BOOST_CHECK(!r_ural);

    BOOST_CHECK(r_ural.traversed_begin() == v.begin());
    BOOST_CHECK(r_ural.begin() == v.end());
    BOOST_CHECK(r_ural.end() == v.end());
}

BOOST_AUTO_TEST_CASE(find_fail_test_list)
{
    std::list<int> const v{0, 1, 2, 3, 4};

    auto const value = -1;

    auto const r_std = std::find(v.begin(), v.end(), value);
    auto const r_ural = ural::find(v, value);

    BOOST_CHECK(r_std == v.end());
    BOOST_CHECK(!r_ural);

    BOOST_CHECK(r_ural.traversed_begin() == v.begin());
    BOOST_CHECK(r_ural.begin() == v.end());
    BOOST_CHECK(r_ural.end() == v.end());
    BOOST_CHECK(r_ural.traversed_end() == v.end());
}

BOOST_AUTO_TEST_CASE(find_success_test)
{
    ural_test::istringstream_helper<int> const v{0, 1, 2, 3, 4};

    auto const value = 2;

    auto r_ural = ural::find(v, value);

    BOOST_CHECK(!!r_ural);
    BOOST_CHECK_EQUAL(value, *r_ural);
}

BOOST_AUTO_TEST_CASE(find_success_test_forward_list)
{
    std::forward_list<int> v{0, 1, 2, 3, 4};

    auto const value = 2;

    auto r_std = std::find(v.begin(), v.end(), value);
    auto r_ural = ural::find(v, value);

    BOOST_CHECK(!!r_ural);
    BOOST_CHECK_EQUAL(value, *r_ural);

    BOOST_CHECK(r_ural.traversed_begin() == v.begin());
    BOOST_CHECK(r_ural.begin() == r_std);
    BOOST_CHECK(r_ural.end() == v.end());
}

BOOST_AUTO_TEST_CASE(find_success_test_list)
{
    std::list<int> v{0, 1, 2, 3, 4};

    auto const value = 2;

    auto r_std = std::find(v.begin(), v.end(), value);
    auto r_ural = ural::find(v, value);

    BOOST_CHECK(!!r_ural);
    BOOST_CHECK_EQUAL(value, *r_ural);

    BOOST_CHECK(r_ural.traversed_begin() == v.begin());
    BOOST_CHECK(r_ural.begin() == r_std);
    BOOST_CHECK(r_ural.end() == v.end());
    BOOST_CHECK(r_ural.traversed_end() == v.end());
}

// 25.2.6
BOOST_AUTO_TEST_CASE(find_end_test_success)
{
    std::forward_list<int> const v{1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4};
    std::forward_list<int> const t1{1, 2, 3};

    auto r_std = std::find_end(v.begin(), v.end(), t1.begin(), t1.end());
    auto r_ural = ural::find_end(v, t1);

    BOOST_CHECK(r_ural.traversed_begin() == v.begin());
    BOOST_CHECK(r_ural.begin() == r_std);
    BOOST_CHECK(r_ural.end() == v.end());
}

BOOST_AUTO_TEST_CASE(find_end_test_fail)
{
    std::forward_list<int> const v{1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4};
    std::forward_list<int> const t2{4, 5, 6};

    auto r_std = std::find_end(v.begin(), v.end(), t2.begin(), t2.end());
    auto r_ural = ural::find_end(v, t2);

    BOOST_CHECK(r_ural.traversed_begin() == v.begin());
    BOOST_CHECK(r_ural.begin() == r_std);
    BOOST_CHECK(r_ural.end() == v.end());

    BOOST_CHECK_EQUAL(std::distance(r_std, v.end()), ::ural::size(r_ural));
}

// 25.2.7
BOOST_AUTO_TEST_CASE(find_first_of_test)
{
    std::vector<int> const v{0, 2, 3, 25, 5};
    std::vector<int> const t{3, 19, 10, 2};

    auto r_std = std::find_first_of(v.begin(), v.end(), t.begin(), t.end());
    auto r_ural = ural::find_first_of(v, t);

    BOOST_CHECK_EQUAL(std::distance(r_std, v.end()), r_ural.size());
    BOOST_CHECK(!!r_ural);
    BOOST_CHECK_EQUAL(*r_std, *r_ural);
}

// 25.2.8
BOOST_AUTO_TEST_CASE(adjacent_find_test)
{
    std::vector<int> v1{0, 1, 2, 3, 40, 40, 41, 41, 5};

    auto r_std = std::adjacent_find(v1.begin(), v1.end());
    auto r_ural = ural::adjacent_find(v1);

    BOOST_CHECK_EQUAL(std::distance(r_std, v1.end()), r_ural.size());
    BOOST_CHECK(!!r_ural);
    BOOST_CHECK_EQUAL(*r_std, *r_ural);
}

// 25.2.9
BOOST_AUTO_TEST_CASE(count_test)
{
    std::vector<int> v = { 1, 2, 3, 4, 4, 3, 7, 8, 9, 10 };

    int const target1 = 3;
    int const target2 = 5;

    int const n1_std = std::count(v.begin(), v.end(), target1);
    int const n2_std = std::count(v.begin(), v.end(), target2);

    int const n1_ural = ural::count(v, target1);
    int const n2_ural = ural::count(v, target2);

    BOOST_CHECK_EQUAL(n1_std, n1_ural);
    BOOST_CHECK_EQUAL(n2_std, n2_ural);
}

BOOST_AUTO_TEST_CASE(count_if_test)
{
    std::vector<int> const data = { 1, 2, 3, 4, 4, 3, 7, 8, 9, 10 };

    auto const pred = [](int i) {return i % 3 == 0;};

    auto const n_std
        = std::count_if(data.begin(), data.end(), pred);

    auto const n_ural = ural::count_if(data, pred);

    BOOST_CHECK_EQUAL(n_std, n_ural);
}

// 25.2.10
BOOST_AUTO_TEST_CASE(mismatch_test)
{
    std::string const x("abca");
    std::string const y("aba");

    auto const r_std = std::mismatch(x.begin(), x.end(), y.begin());
    auto const r_ural = ural::mismatch(x, y);

    BOOST_CHECK_EQUAL(std::distance(r_std.first, x.end()), r_ural[ural::_1].size());
    BOOST_CHECK_EQUAL(std::distance(r_std.second, y.end()), r_ural[ural::_2].size());
}

// 25.2.11
BOOST_AUTO_TEST_CASE(equal_test)
{
     std::string const x1("radar");
     std::string const y1("rocket");

     auto const x2 = x1;
     auto const y2 = y1;

     BOOST_CHECK(ural::equal(x1, x1) == true);
     BOOST_CHECK(ural::equal(x1, x2) == true);
     BOOST_CHECK(ural::equal(x2, x1) == true);
     BOOST_CHECK(ural::equal(x2, x2) == true);

     BOOST_CHECK(ural::equal(y1, y1) == true);
     BOOST_CHECK(ural::equal(y2, y1) == true);
     BOOST_CHECK(ural::equal(y1, y2) == true);
     BOOST_CHECK(ural::equal(y2, y2) == true);

     BOOST_CHECK(ural::equal(x1, y1) == false);
     BOOST_CHECK(ural::equal(x1, y2) == false);
     BOOST_CHECK(ural::equal(x2, y1) == false);
     BOOST_CHECK(ural::equal(x2, y2) == false);
}

// 25.2.12
BOOST_AUTO_TEST_CASE(is_permutation_test)
{
    std::vector<int> const v1{1,2,3,4,5};
    std::list<int> const v2{3,5,4,1,2};
    std::forward_list<int> const v3{3,5,4,1,1};

    BOOST_CHECK(ural::is_permutation(v1, v2));
    BOOST_CHECK(ural::is_permutation(v2, v1));

    BOOST_CHECK(!ural::is_permutation(v1, v3));
    BOOST_CHECK(!ural::is_permutation(v3, v1));
    BOOST_CHECK(!ural::is_permutation(v2, v3));

    BOOST_CHECK(!ural::is_permutation(v3, v2));
}

BOOST_AUTO_TEST_CASE(is_permutation_test_different_traversed_front)
{
    auto const s1 = ural::numbers(1, 9);
    auto const s2 = ural::numbers(0, 9);

    auto const seq1 = ural::make_cartesian_product_sequence(s1, s2);
    auto const seq2 = ural::make_cartesian_product_sequence(s2, s2);

    static_assert(!std::is_same<decltype(seq1), decltype(seq1.traversed_front())>::value, "");

    BOOST_CHECK(!ural::is_permutation(seq1, seq2));
}

BOOST_AUTO_TEST_CASE(is_permutation_regression_47)
{
    std::string const s1 = "YEAR";
    std::string const s2 = "NEARLY";

    BOOST_CHECK(!ural::is_permutation(s1, s2));
}

// 25.2.13
BOOST_AUTO_TEST_CASE(search_test)
{
    struct Inner
    {
        static bool in_quote(const std::string& cont, const std::string& s)
        {
            return std::search(cont.begin(), cont.end(), s.begin(), s.end())
                    != cont.end();
        }
    };

    std::string const str
        = "why waste time learning, when ignorance is instantaneous?";
    std::string const s1 {"lemming"};
    std::string const s2 {"learning"};
    BOOST_CHECK_EQUAL(Inner::in_quote(str, s1), !!ural::search(str, s1));
    BOOST_CHECK_EQUAL(Inner::in_quote(str, s2), !!ural::search(str, s2));
}

BOOST_AUTO_TEST_CASE(search_n_test)
{
    const std::string xs = "1001010100010101001010101";

    for(auto i : ural::numbers(0, 5))
    {
        BOOST_CHECK_EQUAL(std::search_n(xs.begin(), xs.end(), i, '0') == xs.end(),
                          !ural::search_n(xs, i, '0'));
    }
}

// find_first_not_of
BOOST_AUTO_TEST_CASE(find_first_not_of_test)
{
    std::vector<int> const v{2, 3, 25, 5, 0};
    std::vector<int> const t{3, 19, 10, 2};

    auto r_ural = ural::find_first_not_of(v, t);

    auto const n1 = r_ural.traversed_front().size();
    auto const n2 = r_ural.size();

    BOOST_CHECK_EQUAL(ural::to_signed(v.size()), n1+n2);

    for(auto i : ural::numbers(0, n1))
    {
        BOOST_CHECK(!!ural::find(t, v[i]));
    }

    BOOST_CHECK(!ural::find(t, v[n1]));
}

// @todo тест с однопроходной последовательностью
BOOST_AUTO_TEST_CASE(fused_for_each_test)
{
    using Tuple = ural::tuple<std::string, char>;
    std::vector<Tuple> xs
        = { {"Wate", 'r'}, {"Eart", 'h'}, {"Fir", 'e'}, {"Ai", 'r'}};

    std::vector<std::string> z;
    for(auto const & p : xs)
    {
        z.push_back(p[ural::_1]);
        z.back().push_back(p[ural::_2]);
    }

    auto result = ural::fused_for_each(xs, &std::string::push_back);

    BOOST_CHECK(result[ural::_1].original() == ural::sequence(xs));
    BOOST_CHECK(result[ural::_1].traversed_front() == ural::sequence(xs));

    BOOST_CHECK(result[ural::_2].target() == &std::string::push_back);

    BOOST_CHECK_EQUAL(z.size(), xs.size());

    for(auto i : ural::indices_of(z))
    {
        BOOST_CHECK_EQUAL(z[i], xs[i][ural::_1]);
    }
}
