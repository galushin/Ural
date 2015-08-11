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

#include <ural/container/vector.hpp>
#include <ural/numeric/numbers_sequence.hpp>
#include <ural/utility/tracers.hpp>

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

// 25.4 Сортировка и связанные с ней операции
// 25.4.1 Сортировка
BOOST_AUTO_TEST_CASE(sort_test)
{
    std::vector<int> x_std = {5, 7, 4, 2, 8, 6, 1, 9, 0, 3};
    auto x_ural = x_std;

    std::sort(x_std.begin(), x_std.end());
    auto const result = ural::sort(x_ural);

    URAL_CHECK_EQUAL_RANGES(x_std, x_ural);

    BOOST_CHECK(!result);
    BOOST_CHECK(::ural::sequence(x_ural) == result.traversed_front());
}

namespace
{
    struct Double_compared_by_integral_part
    {
        double value;

        Double_compared_by_integral_part(double x)
         : value{x}
        {}

        bool operator<(Double_compared_by_integral_part const & that) const
        {
            return int(this->value) < int(that.value);
        }

        bool operator!=(Double_compared_by_integral_part const & that) const
        {
            return this->value != that.value;
        }

    friend std::ostream & operator<<(std::ostream & os, Double_compared_by_integral_part x)
    {
        return os << x.value;
    }
    };
}

BOOST_AUTO_TEST_CASE(stable_sort_test)
{
    std::vector<Double_compared_by_integral_part> x_std
        = {3.14, 1.41, 2.72, 4.67, 1.73, 1.32, 1.62, 2.58};
    auto x_ural = x_std;

    std::stable_sort(x_std.begin(), x_std.end());
    auto const result = ural::stable_sort(x_ural);

    URAL_CHECK_EQUAL_RANGES(x_std, x_ural);

    BOOST_CHECK(!result);
    BOOST_CHECK(::ural::sequence(x_ural) == result.traversed_front());
}

BOOST_AUTO_TEST_CASE(partial_sort_test)
{
    std::array<int, 10> const xs {5, 7, 4, 2, 8, 6, 1, 9, 0, 3};
    auto ys = xs;

    auto const part = 3;

    auto const result = ural::partial_sort(ys, part);

    BOOST_CHECK(std::is_sorted(ys.begin(), ys.begin() + part));
    BOOST_CHECK(ural::is_permutation(xs, ys));
    BOOST_CHECK(std::all_of(ys.begin() + part, ys.end(),
                            [=](int x) {return x >= ys[2];}));

    BOOST_CHECK(result.begin() == ys.end());
    BOOST_CHECK(result.end()   == ys.end());
    BOOST_CHECK(result.traversed_front().begin() == ys.begin());
    BOOST_CHECK(result.traversed_front().end()   == ys.end());
}

BOOST_AUTO_TEST_CASE(partial_sort_reversed_test)
{
    std::array<int, 10> const ys {5, 7, 4, 2, 8, 6, 1, 9, 0, 3};

    auto xs = ys;

    ural::partial_sort(xs | ural::reversed, xs.size());

    BOOST_CHECK(std::is_sorted(xs.rbegin(), xs.rend()));
    BOOST_CHECK(ural::is_permutation(xs, ys));
}

BOOST_AUTO_TEST_CASE(partial_sort_copy_test)
{
    std::list<int> const v0{4, 2, 5, 1, 3};

    std::vector<int> r1_std{10, 11, 12};
    std::vector<int> r1_ural{10, 11, 12};

    auto pos_std = std::partial_sort_copy(v0.begin(), v0.end(),
                                          r1_std.begin(), r1_std.end());
    auto pos_ural = ural::partial_sort_copy(v0, r1_ural);

    BOOST_CHECK(pos_ural.original() == ural::sequence(r1_ural));
    BOOST_CHECK_EQUAL(r1_std.end() - pos_std, pos_ural.size());
    BOOST_CHECK_EQUAL(pos_std - r1_std.begin(), pos_ural.traversed_front().size());

    URAL_CHECK_EQUAL_RANGES(r1_std, r1_ural);
}

BOOST_AUTO_TEST_CASE(partial_sort_copy_test_custom_predicate_to_greater)
{
    std::list<int> const v0{4, 2, 5, 1, 3};

    std::vector<int> r2_std{10, 11, 12, 13, 14, 15, 16};
    std::vector<int> r2_ural{10, 11, 12, 13, 14, 15, 16};

    auto pos_std = std::partial_sort_copy(v0.begin(), v0.end(),
                                     r2_std.begin(), r2_std.end(),
                                     std::greater<int>());
    auto pos_ural = ural::partial_sort_copy(v0, r2_ural, ural::greater<>());

    BOOST_CHECK(pos_ural.original() == ural::sequence(r2_ural));
    BOOST_CHECK_EQUAL(r2_std.end() - pos_std, pos_ural.size());
    BOOST_CHECK_EQUAL(pos_std - r2_std.begin(), pos_ural.traversed_front().size());

    URAL_CHECK_EQUAL_RANGES(r2_std, r2_ural);
}

BOOST_AUTO_TEST_CASE(is_sorted_test)
{
    std::vector<int> digits {3, 1, 4, 1, 5};

    BOOST_CHECK_EQUAL(false, ural::is_sorted(digits));
    BOOST_CHECK_EQUAL(std::is_sorted(digits.begin(), digits.end()),
                      ural::is_sorted(digits));

    std::sort(digits.begin(), digits.end());

    BOOST_CHECK_EQUAL(true, std::is_sorted(digits.begin(), digits.end()));
    BOOST_CHECK_EQUAL(true, ural::is_sorted(digits));
}

BOOST_AUTO_TEST_CASE(is_sorted_until_test)
{
    std::vector<int> nums = {1, 3, 4, 5, 9};

    do
    {
        auto result_std = std::is_sorted_until(nums.begin(), nums.end());
        auto result_ural = ural::is_sorted_until(nums);

        BOOST_CHECK(result_ural.original() == ural::sequence(nums));
        BOOST_CHECK(result_ural.begin() == result_std);
        BOOST_CHECK(result_ural.end() == nums.end());
    }
    while(std::next_permutation(nums.begin(), nums.end()));
}

BOOST_AUTO_TEST_CASE(nth_element_test)
{
    std::vector<int> x_std{5, 6, 4, 3, 2, 6, 7, 9, 3};
    auto x_ural = x_std;

    auto const pos_1 = x_std.size() / 2;

    std::nth_element(x_std.begin(), x_std.begin() + pos_1, x_std.end());

    auto s_std = ural::sequence(x_std) + pos_1;
    auto s_ural = ural::sequence(x_ural) + pos_1;

    auto result = ural::nth_element(s_ural);

    BOOST_CHECK(ural::is_permutation(x_std, x_ural));
    BOOST_CHECK_EQUAL(x_std[pos_1], x_ural[pos_1]);

    BOOST_CHECK(ural::is_permutation(s_std, s_ural));
    BOOST_CHECK(ural::is_permutation(s_std.traversed_front(),
                                     s_ural.traversed_front()));

    BOOST_CHECK(result.begin() == x_ural.end());
    BOOST_CHECK(result.end()   == x_ural.end());
    BOOST_CHECK(result.traversed_front().begin() == x_ural.begin());
    BOOST_CHECK(result.traversed_front().end()   == x_ural.end());
}

// 25.4.3 Бинарный поиск
BOOST_AUTO_TEST_CASE(lower_bound_test)
{
    std::vector<int> const data = { 1, 1, 2, 3, 3, 3, 3, 4, 4, 4, 5, 5, 6 };
    auto const value = 4;

    auto r_std = std::lower_bound(data.begin(), data.end(), value);
    auto r_ural = ural::lower_bound(data, value);

    BOOST_CHECK(!!r_ural);
    BOOST_CHECK_EQUAL(*r_std, *r_ural);

    BOOST_CHECK(data.begin() == r_ural.traversed_begin());
    BOOST_CHECK(r_std == r_ural.begin());
    BOOST_CHECK(data.end() == r_ural.end());
    BOOST_CHECK(data.end() == r_ural.traversed_end());
}

BOOST_AUTO_TEST_CASE(upper_bound_test)
{
    std::vector<int> const data = { 1, 1, 2, 3, 3, 3, 3, 4, 4, 4, 5, 5, 6 };
    auto const value = 4;

    auto r_std = std::upper_bound(data.begin(), data.end(), value);
    auto r_ural = ural::upper_bound(data, value);

    BOOST_CHECK(!!r_ural);
    BOOST_CHECK_EQUAL(*r_std, *r_ural);

    BOOST_CHECK(data.begin() == r_ural.traversed_begin());
    BOOST_CHECK(r_std == r_ural.begin());
    BOOST_CHECK(data.end() == r_ural.end());
    BOOST_CHECK(data.end() == r_ural.traversed_end());
}

BOOST_AUTO_TEST_CASE(equal_range_test)
{
    auto const src = []()
    {
        std::vector<int> v = {10,20,30,30,20,10,10,20};
        std::sort (v.begin(), v.end());
        return v;
    }();

    auto const value = 20;

    auto r_std = std::equal_range (src.begin(), src.end(), value);
    auto r_ural = ural::equal_range(src, value);

    BOOST_CHECK(r_std.first == r_ural.begin());
    BOOST_CHECK_EQUAL(r_std.second - r_std.first,
                      r_ural.end() - r_ural.begin());
    BOOST_CHECK(src.begin() == r_ural.traversed_begin());
    BOOST_CHECK(src.end() == r_ural.traversed_end());
}

BOOST_AUTO_TEST_CASE(binary_search_test)
{
    std::vector<int> const haystack {1, 3, 4, 5, 9};
    std::vector<int> const needles {1, 2, 3, 10};

    for (auto needle : needles)
    {
        auto r_std = std::binary_search(haystack.begin(), haystack.end(), needle);
        auto r_ural = ural::binary_search(haystack, needle);

        BOOST_CHECK_EQUAL(r_std, r_ural);
    }
}

// 25.4.4 Слияние
BOOST_AUTO_TEST_CASE(merge_test)
{
    // Исходные данные
    std::vector<int> const v1{1,2,3,4,5,6,7,8};
    std::vector<int> const v2{        5,  7,  9,10};

    // std
    std::vector<int> std_merge;
    std::merge(v1.begin(), v1.end(), v2.begin(), v2.end(),
               std::back_inserter(std_merge));

    // ural
    std::vector<int> ural_merge;
    ural::merge(v1, v2, ural_merge | ural::back_inserter);

    // Проверка
    URAL_CHECK_EQUAL_RANGES(std_merge, ural_merge);
}

BOOST_AUTO_TEST_CASE(merge_test_lesser_in_1)
{
    // Исходные данные
    std::vector<int> const v1{1,2,3,4,5,6,7,8};
    std::vector<int> const v2{        5,  7,  9,10};

    BOOST_CHECK_LE(v1.back(), v2.back());

    // std
    std::vector<int> std_merge;
    std::merge(v1.begin(), v1.end(), v2.begin(), v2.end(),
               std::back_inserter(std_merge));

    // ural
    std::vector<int> ural_merge(v1.size() + v2.size() + 3, - 1);
    auto const ural_merge_old = ural_merge;

    auto const result = ural::merge(v1, v2, ural_merge);

    BOOST_CHECK(!result[ural::_1]);
    BOOST_CHECK(!result[ural::_2]);
    BOOST_CHECK(!!result[ural::_3]);

    BOOST_CHECK(result[ural::_1].original() == ural::sequence(v1));
    BOOST_CHECK(result[ural::_2].original() == ural::sequence(v2));
    BOOST_CHECK(result[ural::_3].original() == ural::sequence(ural_merge));

    BOOST_CHECK(result[ural::_3].begin() == ural_merge.begin() + std_merge.size());

    // Проверка
    BOOST_CHECK_EQUAL_COLLECTIONS(std_merge.begin(), std_merge.end(),
                                  ural_merge.begin(), result[ural::_3].begin());
    BOOST_CHECK_EQUAL_COLLECTIONS(result[ural::_3].begin(), result[ural::_3].end(),
                                  ural_merge_old.begin() + std_merge.size(),
                                  ural_merge_old.end());
}

BOOST_AUTO_TEST_CASE(merge_test_lesser_in_2)
{
    // Исходные данные
    std::vector<int> const v1{1,2,3,4,5,6,7,8,     11};
    std::vector<int> const v2{        5,  7,  9,10};

    BOOST_CHECK_GE(v1.back(), v2.back());

    // std
    std::vector<int> std_merge;
    std::merge(v1.begin(), v1.end(), v2.begin(), v2.end(),
               std::back_inserter(std_merge));

    // ural
    std::vector<int> ural_merge(v1.size() + v2.size() + 3, - 1);
    auto const ural_merge_old = ural_merge;

    auto const result = ural::merge(v1, v2, ural_merge);

    BOOST_CHECK(!result[ural::_1]);
    BOOST_CHECK(!result[ural::_2]);
    BOOST_CHECK(!!result[ural::_3]);

    BOOST_CHECK(result[ural::_1].original() == ural::sequence(v1));
    BOOST_CHECK(result[ural::_2].original() == ural::sequence(v2));
    BOOST_CHECK(result[ural::_3].original() == ural::sequence(ural_merge));

    BOOST_CHECK(result[ural::_3].begin() == ural_merge.begin() + std_merge.size());

    // Проверка
    BOOST_CHECK_EQUAL_COLLECTIONS(std_merge.begin(), std_merge.end(),
                                  ural_merge.begin(), result[ural::_3].begin());
    BOOST_CHECK_EQUAL_COLLECTIONS(result[ural::_3].begin(), result[ural::_3].end(),
                                  ural_merge_old.begin() + std_merge.size(),
                                  ural_merge_old.end());
}

BOOST_AUTO_TEST_CASE(merge_test_exhaust_out)
{
    // Исходные данные
    std::vector<int> const v1{1,2,3,4,5,6,7,8,     11};
    std::vector<int> const v2{        5,  7,  9,10};

    // ural
    std::vector<int> ural_merge((v1.size() + v2.size()) / 2, - 1);

    auto const result = ural::merge(v1, v2, ural_merge);

    BOOST_CHECK(!!result[ural::_1]);
    BOOST_CHECK(!!result[ural::_2]);
    BOOST_CHECK(!result[ural::_3]);

    BOOST_CHECK(result[ural::_1].original() == ural::sequence(v1));
    BOOST_CHECK(result[ural::_2].original() == ural::sequence(v2));
    BOOST_CHECK(result[ural::_3].original() == ural::sequence(ural_merge));

    BOOST_CHECK(!result[ural::_1].traversed_back());
    BOOST_CHECK(!result[ural::_2].traversed_back());

    // std
    std::vector<int> std_merge;
    std::merge(result[ural::_1].traversed_begin(),
               result[ural::_1].begin(),
               result[ural::_2].traversed_begin(),
               result[ural::_2].begin(),
               std::back_inserter(std_merge));

    // Проверка
    URAL_CHECK_EQUAL_RANGES(ural_merge, std_merge);
}

BOOST_AUTO_TEST_CASE(merge_test_minimalistic)
{
    // Исходные данные
    std::istringstream is1("1 2 3 4 5 6 7 8");
    std::istringstream is2("        5   7 9 10");

    std::istringstream is1_ural("1 2 3 4 5 6 7 8");
    std::istringstream is2_ural("        5   7 9 10");

    // std
    std::vector<int> std_merge;
    std::merge(std::istream_iterator<int>(is1), std::istream_iterator<int>(),
               std::istream_iterator<int>(is2), std::istream_iterator<int>(),
               std::back_inserter(std_merge));

    // ural
    std::vector<int> ural_merge;
    ural::merge(ural::make_istream_sequence<int>(is1_ural),
                ural::make_istream_sequence<int>(is2_ural),
                ural_merge | ural::back_inserter);

    // Проверка
    URAL_CHECK_EQUAL_RANGES(std_merge, ural_merge);
}

BOOST_AUTO_TEST_CASE(inplace_merge_test_empty)
{
    std::vector<int> x_std{};
    auto x_ural = x_std;

    auto pos = x_std.size() / 2;

    std::inplace_merge(x_std.begin(), x_std.begin() + pos, x_std.end());

    auto s = ::ural::sequence(x_ural);
    s += pos;
    auto result = ural::inplace_merge(s);

    URAL_CHECK_EQUAL_RANGES(x_std, x_ural);

    BOOST_CHECK(result.begin() == x_ural.end());
    BOOST_CHECK(result.end()   == x_ural.end());
    BOOST_CHECK(result.traversed_front().begin() == x_ural.begin());
    BOOST_CHECK(result.traversed_front().end()   == x_ural.end());
}

BOOST_AUTO_TEST_CASE(inplace_merge_test_1)
{
    std::vector<int> x_std{1};
    auto x_ural = x_std;

    auto pos = x_std.size() / 2;

    std::inplace_merge(x_std.begin(), x_std.begin() + pos,
                       x_std.end());
    auto s = ::ural::sequence(x_ural);
    s += pos;
    ural::inplace_merge(s);

    URAL_CHECK_EQUAL_RANGES(x_std, x_ural);
}

BOOST_AUTO_TEST_CASE(inplace_merge_test_1_2)
{
    std::vector<int> x_std{1, 2};
    auto x_ural = x_std;

    auto pos = x_std.size() / 2;

    std::inplace_merge(x_std.begin(), x_std.begin() + pos,
                       x_std.end());
    auto s = ::ural::sequence(x_ural);
    s += pos;
    ural::inplace_merge(s);

    URAL_CHECK_EQUAL_RANGES(x_std, x_ural);
}

BOOST_AUTO_TEST_CASE(inplace_merge_test_2_1)
{
    std::vector<int> x_std{2, 1};
    auto x_ural = x_std;

    auto pos = x_std.size() / 2;

    std::inplace_merge(x_std.begin(), x_std.begin() + pos,
                       x_std.end());
    auto s = ::ural::sequence(x_ural);
    s += pos;
    ural::inplace_merge(s);

    URAL_CHECK_EQUAL_RANGES(x_std, x_ural);
}

BOOST_AUTO_TEST_CASE(inplace_merge_test_2_1_3)
{
    std::vector<int> x_std{3, 1, 2};
    auto x_ural = x_std;

    auto pos = x_std.size() / 2;

    std::inplace_merge(x_std.begin(), x_std.begin() + pos,
                       x_std.end());
    auto s = ::ural::sequence(x_ural);
    s += pos;
    ural::inplace_merge(s);

    URAL_CHECK_EQUAL_RANGES(x_std, x_ural);
}

BOOST_AUTO_TEST_CASE(inplace_merge_test_4)
{
    std::vector<int> x_std{1, 2, 0, 4};
    auto x_ural = x_std;

    auto pos = x_std.size() / 2;

    std::inplace_merge(x_std.begin(), x_std.begin() + pos,
                       x_std.end());
    auto s = ::ural::sequence(x_ural);
    s += pos;
    ural::inplace_merge(s);

    URAL_CHECK_EQUAL_RANGES(x_std, x_ural);
}

BOOST_AUTO_TEST_CASE(inplace_merge_test_10)
{
    std::vector<int> x_std{1, 2, 3, 5, 8, 0, 4, 6, 7, 9};
    auto x_ural = x_std;

    auto pos = x_std.size() / 2;

    std::inplace_merge(x_std.begin(), x_std.begin() + pos,
                       x_std.end());
    auto s = ::ural::sequence(x_ural);
    s += pos;
    auto result = ural::inplace_merge(s);

    URAL_CHECK_EQUAL_RANGES(x_std, x_ural);
    BOOST_CHECK(result.original() == ural::sequence(x_ural));
    BOOST_CHECK(!result);
    BOOST_CHECK(!result.traversed_back());
}

// 25.4.5 Операции со множествами на сортированных структурах
BOOST_AUTO_TEST_CASE(includes_test)
{
    std::vector<std::string> vs{"abcfhx", "abc", "ac", "g", "acg", {}};

    for(auto const & s1 : vs)
    for(auto const & s2 : vs)
    {
        bool const r_std = std::includes(s1.begin(), s1.end(),
                                         s2.begin(), s2.end());        bool const r_ural = ural::includes(s1, s2);
        BOOST_CHECK_EQUAL(r_std, r_ural);
    }
}

BOOST_AUTO_TEST_CASE(includes_test_custom_compare)
{
    std::vector<std::string> vs{"abcfhx", "abc", "ac", "g", "acg", {}};
    std::string v0 {"ABC"};

    auto cmp_nocase = [](char a, char b) {
    return std::tolower(a) < std::tolower(b);
    };

    for(auto const & s : vs)
    {
        bool const r_std = std::includes(s.begin(), s.end(),
                                         v0.begin(), v0.end(), cmp_nocase);        std::istringstream s_stream(s);
        std::istringstream v0_stream(v0);

        bool const r_ural
            = ural::includes(ural::make_istream_sequence<char>(s_stream),
                             ural::make_istream_sequence<char>(v0_stream),
                             cmp_nocase);
        BOOST_CHECK_EQUAL(r_std, r_ural);
    }
}

BOOST_AUTO_TEST_CASE(includes_test_custom_compare_istream_auto_to_sequence)
{
    std::vector<std::string> vs{"abcfhx", "abc", "ac", "g", "acg", {}};
    std::string v0 {"ABC"};

    auto cmp_nocase = [](char a, char b) {
    return std::tolower(a) < std::tolower(b);
    };

    for(auto const & s : vs)
    {
        bool const r_std = std::includes(s.begin(), s.end(),
                                         v0.begin(), v0.end(), cmp_nocase);        std::istringstream s_stream(s);
        std::istringstream v0_stream(v0);

        bool const r_ural = ural::includes(s_stream, v0_stream, cmp_nocase);
        BOOST_CHECK_EQUAL(r_std, r_ural);
    }
}

BOOST_AUTO_TEST_CASE(set_union_test)
{
    std::istringstream is1("1 2 3 4 5");
    std::istringstream is2("    3 4 5 6 7");

    std::vector<int> const z {1, 2, 3, 4, 5, 6, 7};

    std::vector<int> r_ural;
    ural::set_union(ural::make_istream_sequence<int>(is1),
                    ural::make_istream_sequence<int>(is2),
                    r_ural | ural::back_inserter);

    URAL_CHECK_EQUAL_RANGES(z, r_ural);
}

BOOST_AUTO_TEST_CASE(set_union_test_shorter_in_1)
{
    std::vector<int> const is1{1, 2, 3, 4, 5,};
    std::vector<int> const is2{      3, 4, 5, 6, 7};
    std::vector<int> const z  {1, 2, 3, 4, 5, 6, 7};

    BOOST_CHECK_LE(is1.back(), is2.back());

    std::vector<int> r_ural;
    auto result = ural::set_union(is1, is2, r_ural | ural::back_inserter);

    URAL_CHECK_EQUAL_RANGES(z, r_ural);

    BOOST_CHECK(!result[ural::_1]);
    BOOST_CHECK(!result[ural::_2]);
    BOOST_CHECK(!!result[ural::_3]);
}

BOOST_AUTO_TEST_CASE(set_union_test_shorter_in_2)
{
    std::vector<int> const is1{1, 2, 3, 4, 5,    7};
    std::vector<int> const is2{      3, 4, 5, 6};
    std::vector<int> const z  {1, 2, 3, 4, 5, 6, 7};

    BOOST_CHECK_LE(is2.back(), is1.back());

    std::vector<int> r_ural;
    auto result = ural::set_union(is1, is2, r_ural | ural::back_inserter);

    URAL_CHECK_EQUAL_RANGES(z, r_ural);

    BOOST_CHECK(!result[ural::_1]);
    BOOST_CHECK(!result[ural::_2]);
    BOOST_CHECK(!!result[ural::_3]);
}

BOOST_AUTO_TEST_CASE(set_union_test_shorter_out)
{
    std::vector<int> const is1{1, 2, 3, 4, 5,};
    std::vector<int> const is2{      3, 4, 5, 6, 7};
    std::vector<int> const z  {1, 2, 3, 4, 5, 6, 7};

    std::vector<int> r_ural(z.size() / 2, -1);
    BOOST_CHECK_LE(r_ural.size(), z.size());
    BOOST_CHECK(r_ural.empty() == false);

    auto result = ural::set_union(is1, is2, r_ural);

    BOOST_CHECK_EQUAL_COLLECTIONS(r_ural.begin(), r_ural.end(),
                                  z.begin(), z.begin() + r_ural.size());

    BOOST_CHECK(!!result[ural::_1]);
    BOOST_CHECK(!!result[ural::_2]);
    BOOST_CHECK(!result[ural::_3]);

    BOOST_CHECK(result[ural::_1].original() == ural::sequence(is1));
    BOOST_CHECK(result[ural::_2].original() == ural::sequence(is2));
    BOOST_CHECK(result[ural::_3].original() == ural::sequence(r_ural));

    std::vector<int> r_std;
    std::set_union(result[ural::_1].traversed_begin(),
                   result[ural::_1].begin(),
                   result[ural::_2].traversed_begin(),
                   result[ural::_2].begin(),
                   std::back_inserter(r_std));

    BOOST_CHECK_EQUAL_COLLECTIONS(r_ural.begin(), r_ural.end(),
                                  r_std.begin(), r_std.end());
}

BOOST_AUTO_TEST_CASE(set_intersection_test)
{
    std::istringstream is1("1 2 3 4 5");
    std::istringstream is2("  2   4 5 6 7");

    std::vector<int> const z {2, 4, 5};

    std::vector<int> r_ural;
    ural::set_intersection(ural::make_istream_sequence<int>(is1),
                           ural::make_istream_sequence<int>(is2),
                           r_ural | ural::back_inserter);

    URAL_CHECK_EQUAL_RANGES(z, r_ural);
}

BOOST_AUTO_TEST_CASE(set_intersection_test_shorter_in_1)
{
    std::vector<int> const is1{1, 2, 3, 4, 5};
    std::vector<int> const is2{   2,    4, 5, 6, 7};
    std::vector<int> const z  {   2,    4, 5};

    BOOST_CHECK_LE(is1.size(), is2.size());
    BOOST_CHECK_LE(is1.back(), is2.back());

    std::vector<int> r_ural;
    auto result = ural::set_intersection(is1, is2, r_ural | ural::back_inserter);

    URAL_CHECK_EQUAL_RANGES(z, r_ural);

    BOOST_CHECK(!result[ural::_1]);

    BOOST_CHECK(!!result[ural::_2]);
    BOOST_CHECK_LE(z.back(), result[ural::_2].front());

    BOOST_CHECK_EQUAL(is1.back(), result[ural::_2].front());

    BOOST_CHECK(!!result[ural::_3]);
}

BOOST_AUTO_TEST_CASE(set_intersection_test_shorter_in_2)
{
    std::vector<int> const is1{   2, 3, 4, 5, 6, 7};
    std::vector<int> const is2{1, 2,    4, 5};
    std::vector<int> const z  {   2,    4, 5};

    BOOST_CHECK_LE(is2.size(), is1.size());
    BOOST_CHECK_LE(is2.back(), is1.back());

    std::vector<int> r_ural;
    auto result = ural::set_intersection(is1, is2, r_ural | ural::back_inserter);

    URAL_CHECK_EQUAL_RANGES(z, r_ural);

    BOOST_CHECK(!!result[ural::_1]);
    BOOST_CHECK(!result[ural::_2]);
    BOOST_CHECK(!!result[ural::_3]);

    BOOST_CHECK_LE(z.back(), result[ural::_1].front());

    BOOST_CHECK_EQUAL(result[ural::_1].front(),
                      ural::upper_bound(is1, is2.back()).front());
}

BOOST_AUTO_TEST_CASE(set_intersection_test_shorter_out)
{
    std::vector<int> const is1{   2, 3, 4, 5, 6, 7};
    std::vector<int> const is2{1, 2,    4, 5};
    std::vector<int> const z  {   2,    4, 5};

    std::vector<int> r_ural(z.size() / 2, -1);
    BOOST_CHECK_LE(r_ural.size(), z.size());
    BOOST_CHECK(r_ural.empty() == false);

    auto result = ural::set_intersection(is1, is2, r_ural);

    BOOST_CHECK_EQUAL_COLLECTIONS(r_ural.begin(), r_ural.end(),
                                  z.begin(), z.begin() + r_ural.size());

    BOOST_CHECK(!!result[ural::_1]);
    BOOST_CHECK(!!result[ural::_2]);
    BOOST_CHECK(!result[ural::_3]);

    BOOST_CHECK(result[ural::_1].original() == ural::sequence(is1));
    BOOST_CHECK(result[ural::_2].original() == ural::sequence(is2));
    BOOST_CHECK(result[ural::_3].original() == ural::sequence(r_ural));

    std::vector<int> r_std;
    std::set_intersection(result[ural::_1].traversed_begin(),
                          result[ural::_1].begin(),
                          result[ural::_2].traversed_begin(),
                          result[ural::_2].begin(),
                          std::back_inserter(r_std));

    URAL_CHECK_EQUAL_RANGES(r_ural, r_std);
}

BOOST_AUTO_TEST_CASE(set_difference_test)
{
    typedef ural_test::istringstream_helper<int> Source;
    Source const is1            {1, 2, 3, 4, 5,      8};
    Source const is2            {   2,    4, 5, 6, 7  };
    std::vector<int> const z    {1,    3,            8};

    std::vector<int> r_ural;
    ural::set_difference(is1, is2, r_ural | ural::back_inserter);

    URAL_CHECK_EQUAL_RANGES(z, r_ural);
}

BOOST_AUTO_TEST_CASE(set_difference_test_unexhausted_2)
{
    typedef ural_test::istringstream_helper<int> Source;
    Source const is1            {1, 2, 3, 4, 5,      8};
    Source const is2            {   2,    4, 5, 6, 7,  9};
    std::vector<int> const z    {1,    3,            8};

    std::vector<int> r_ural;
    ural::set_difference(is1, is2, r_ural | ural::back_inserter);

    URAL_CHECK_EQUAL_RANGES(z, r_ural);
}

BOOST_AUTO_TEST_CASE(set_difference_test_to_short)
{
    std::vector<int> is1     {1, 2, 3, 4, 5,       8};
    std::vector<int> is2     {   2,    4, 5, 6, 7,   9};
    std::vector<int> const z {1,    3,             8};

    std::vector<int> r_ural(z.size() / 2, -1);

    auto result = ural::set_difference(is1, is2, r_ural);

    BOOST_CHECK(result[ural::_1].original() == ural::sequence(is1));
    BOOST_CHECK(!!result[ural::_1]);
    BOOST_CHECK_LE(r_ural.back(), result[ural::_1].front());

    BOOST_CHECK(result[ural::_2].original() == ural::sequence(is2));
    BOOST_CHECK(!!result[ural::_2]);
    BOOST_CHECK_LE(r_ural.back(), result[ural::_2].front());

    BOOST_CHECK(result[ural::_3].original() == ural::sequence(r_ural));
    BOOST_CHECK(!result[ural::_3]);

    assert(r_ural.size() < z.size());
    BOOST_CHECK_EQUAL_COLLECTIONS(r_ural.begin(), r_ural.end(),
                                  z.begin(), z.begin() + r_ural.size());

    std::vector<int> r_std;
    std::set_difference(result[ural::_1].traversed_begin(),
                        result[ural::_1].begin(),
                        result[ural::_2].traversed_begin(),
                        result[ural::_2].begin(),
                        r_std | ural::back_inserter);

    URAL_CHECK_EQUAL_RANGES(r_ural, r_std);
}

BOOST_AUTO_TEST_CASE(set_symmetric_difference_test)
{
    using Source = ural_test::istringstream_helper<int>;

    Source const x1          {1, 2, 3, 4, 5, 6, 7, 8       };
    Source const x2          {            5,    7,    9, 10};
    std::vector<int> const z {1, 2, 3, 4,    6,    8, 9, 10};

    // через back_inserter
    std::vector<int> r_ural;
    ural::set_symmetric_difference(x1, x2, r_ural | ural::back_inserter);

    URAL_CHECK_EQUAL_RANGES(z, r_ural);
}

BOOST_AUTO_TEST_CASE(set_symmetric_difference_regression)
{
    std::vector<int> const x1{1, 2, 3, 4, 5,       8};
    std::vector<int> const x2{   2,    4, 5, 6, 7   };
    std::vector<int> const z {1,    3,       6, 7, 8};

    BOOST_CHECK_GE(x1.back(), x2.back());

    std::vector<int> r_ural;
    ural::set_symmetric_difference(x1, x2, r_ural | ural::back_inserter);

    URAL_CHECK_EQUAL_RANGES(z, r_ural);
}

BOOST_AUTO_TEST_CASE(set_symmetric_difference_from_istream)
{
    using Source = ural_test::istringstream_helper<int>;

    Source const x1          {1, 2, 3, 4, 5, 6, 7, 8       };
    Source const x2          {            5,    7,    9, 10};
    std::vector<int> const z {1, 2, 3, 4,    6,    8, 9, 10};

    std::vector<int> r_ural;
    ural::set_symmetric_difference(x1, x2, r_ural | ural::back_inserter);

    URAL_CHECK_EQUAL_RANGES(z, r_ural);
}

BOOST_AUTO_TEST_CASE(set_symmetric_difference_to_short)
{
    std::vector<int> const x1{1, 2, 3, 4, 5,       8};
    std::vector<int> const x2{   2,    4, 5, 6, 7   };
    std::vector<int> const z {1,    3,       6, 7, 8};

    std::vector<int> r_ural(z.size() / 2, - 1);

    assert(!r_ural.empty());
    assert(r_ural.size() < z.size());

    auto result = ural::set_symmetric_difference(x1, x2, r_ural);

    BOOST_CHECK_EQUAL_COLLECTIONS(r_ural.begin(), r_ural.end(),
                                  z.begin(), z.begin() + r_ural.size());

    BOOST_CHECK(result[ural::_1].original() == ural::sequence(x1));
    BOOST_CHECK(!!result[ural::_1]);
    BOOST_CHECK_LE(r_ural.back(), result[ural::_1].front());

    BOOST_CHECK(result[ural::_2].original() == ural::sequence(x2));
    BOOST_CHECK(!!result[ural::_2]);
    BOOST_CHECK_LE(r_ural.back(), result[ural::_2].front());

    BOOST_CHECK(result[ural::_3].original() == ural::sequence(r_ural));
    BOOST_CHECK(!result[ural::_3]);

    std::vector<int> r_std;
    std::set_symmetric_difference(result[ural::_1].traversed_begin(),
                                  result[ural::_1].begin(),
                                  result[ural::_2].traversed_begin(),
                                  result[ural::_2].begin(),
                                  std::back_inserter(r_std));
    URAL_CHECK_EQUAL_RANGES(r_ural, r_std);
}

BOOST_AUTO_TEST_CASE(regression_33_set_operations_first_base)
{
    std::list<int> const x1{1, 2, 3, 4, 5,       8};
    std::vector<int> const x2{   2,    4, 5, 6, 7   };

    std::forward_list<int> out;

    auto r_union = ural::set_union(x1, x2, out);
    BOOST_CHECK(r_union[ural::_1].original() == ural::sequence(x1));
    BOOST_CHECK(r_union[ural::_2].original() == ural::sequence(x2));

    auto r_inter = ural::set_intersection(x1, x2, out);
    BOOST_CHECK(r_inter[ural::_1].original() == ural::sequence(x1));
    BOOST_CHECK(r_inter[ural::_2].original() == ural::sequence(x2));

    auto r_diff  = ural::set_difference(x1, x2, out);
    BOOST_CHECK(r_diff[ural::_1].original() == ural::sequence(x1));
    BOOST_CHECK(r_diff[ural::_2].original() == ural::sequence(x2));

    auto r_sdiff = ural::set_symmetric_difference(x1, x2, out);
    BOOST_CHECK(r_sdiff[ural::_1].original() == ural::sequence(x1));
    BOOST_CHECK(r_sdiff[ural::_2].original() == ural::sequence(x2));
}

// 25.4.6 Операции с бинарными кучами
BOOST_AUTO_TEST_CASE(push_heap_test)
{
    std::vector<int> v { 3, 1, 4, 1, 5, 9 };

    for(auto i : ural::indices_of(v))
    {
        BOOST_CHECK(std::is_heap(v.begin(), v.begin() + i));

        auto seq = ural::make_iterator_sequence(v.begin(), v.begin()+i+1);
        auto result = ural::push_heap(seq);

        BOOST_CHECK(result.traversed_front() == seq);
        BOOST_CHECK(!result);
        BOOST_CHECK(!result.traversed_back());
    }
    BOOST_CHECK(std::is_heap(v.begin(), v.end()));
}

BOOST_AUTO_TEST_CASE(pop_heap_test)
{
    std::vector<int> v { 3, 1, 4, 1, 5, 9 };
    ural::make_heap(v);

    for(; !ural::empty(v);)
    {
        auto const old_top = v.front();
        auto result = ural::pop_heap(v);

        BOOST_CHECK(result.begin() == result.end());
        BOOST_CHECK(result.begin() == v.end());
        BOOST_CHECK(result.traversed_front().begin() == v.begin());
        BOOST_CHECK(result.traversed_front().end() == v.end());

        BOOST_CHECK_EQUAL(old_top, v.back());
        v.pop_back();
        BOOST_CHECK(std::is_heap(v.begin(), v.end()));
    }
}

BOOST_AUTO_TEST_CASE(make_heap_test)
{
    std::vector<int> v { 3, 1, 4, 1, 5, 9 };

    auto cmp = ural::callable_tracer<ural::less<int>>{};
    cmp.reset_calls();

    auto result = ural::make_heap(v, cmp);

    BOOST_CHECK(std::is_heap(v.begin(), v.end()));

    BOOST_CHECK_GE(3*v.size(), cmp.calls());

    BOOST_CHECK(result.begin() == result.end());
    BOOST_CHECK(result.begin() == v.end());
    BOOST_CHECK(result.traversed_front().begin() == v.begin());
    BOOST_CHECK(result.traversed_front().end() == v.end());
}

BOOST_AUTO_TEST_CASE(make_heap_odd_size_test)
{
    std::vector<int> v { 3, 1, 4, 1, 5, 9, 2};

    auto cmp = ural::callable_tracer<ural::less<int>>{};
    cmp.reset_calls();

    auto const result = ural::make_heap(v, cmp);

    BOOST_CHECK(result.traversed_front() == ural::sequence(v));
    BOOST_CHECK(!result);
    BOOST_CHECK(!result.traversed_back());

    BOOST_CHECK(std::is_heap(v.begin(), v.end()));

    BOOST_CHECK_GE(3*v.size(), cmp.calls());
}

BOOST_AUTO_TEST_CASE(sort_heap_test)
{
    std::vector<int> v { 3, 1, 4, 1, 5, 9 };

    ural::make_heap(v);
    auto const result = ural::sort_heap(v);

    BOOST_CHECK(std::is_sorted(v.begin(), v.end()));

    BOOST_CHECK(!result);
    BOOST_CHECK(::ural::sequence(v) == result.traversed_front());
}

BOOST_AUTO_TEST_CASE(is_heap_test)
{
    std::vector<int> v {3, 1, 4, 1, 5, 9};

    BOOST_CHECK_EQUAL(std::is_heap(v.begin(), v.end()),
                      ural::is_heap(v));
}

BOOST_AUTO_TEST_CASE(is_heap_test_all_permutations)
{
    std::vector<int> v {1, 2, 3, 4};

    do
    {
        BOOST_CHECK_EQUAL(std::is_heap(v.begin(), v.end()), ural::is_heap(v));
    }
    while(std::next_permutation(v.begin(), v.end()));
}

// 25.4.7 Минимум и максимум
BOOST_AUTO_TEST_CASE(min_max_for_values_test)
{
    constexpr auto const v1 = 5;
    constexpr auto const v2 = 17;

    static_assert(ural::min(v1, v1) == v1, "");
    static_assert(ural::min(v1, v2) == v1, "");
    static_assert(ural::min(v2, v1) == v1, "");
    static_assert(ural::min(v2, v2) == v2, "");

    static_assert(ural::max(v1, v1) == v1, "");
    static_assert(ural::max(v1, v2) == v2, "");
    static_assert(ural::max(v2, v1) == v2, "");
    static_assert(ural::max(v2, v2) == v2, "");

    typedef std::pair<int const &, int const &> Pair;

    static_assert(ural::minmax(v1, v1) == Pair(v1, v1), "");
    static_assert(ural::minmax(v1, v2) == Pair(v1, v2), "");
    static_assert(ural::minmax(v2, v1) == Pair(v1, v2), "");
    static_assert(ural::minmax(v2, v2) == Pair(v2, v2), "");

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(regression_min_max_not_converting_compare_to_function)
{
    struct Inner
    {
    bool operator==(Inner const & that) const
    {
        return this->a == that.a;
    }

    bool is_lesser(Inner const & that) const
    {
        return this->a < that.a;
    }

    public:
        int a;
    };

    auto const one = Inner{1};
    auto const two = Inner{2};

    BOOST_CHECK(ural::min(one, two, &Inner::is_lesser) == one);
    BOOST_CHECK(ural::max(one, two, &Inner::is_lesser) == two);
    BOOST_CHECK(ural::minmax(two, one, &Inner::is_lesser).first == one);
    BOOST_CHECK(ural::minmax(two, one, &Inner::is_lesser).second == two);
}

BOOST_AUTO_TEST_CASE(min_max_stability_test)
{
    auto const v1 = 'a';
    auto const v2 = 'A';

    auto cmp = [](char x, char y) { return std::toupper(x) < std::toupper(y); };

    BOOST_CHECK_EQUAL(ural::min(v1, v2, cmp), v1);
    BOOST_CHECK_EQUAL(ural::min(v2, v1, cmp), v2);

    BOOST_CHECK_EQUAL(ural::max(v1, v2, cmp), v1);
    BOOST_CHECK_EQUAL(ural::max(v2, v1, cmp), v2);

    BOOST_CHECK_EQUAL(ural::minmax(v1, v2, cmp).first, v1);
    BOOST_CHECK_EQUAL(ural::minmax(v1, v2, cmp).second, v2);

    BOOST_CHECK_EQUAL(ural::minmax(v2, v1, cmp).first, v2);
    BOOST_CHECK_EQUAL(ural::minmax(v2, v1, cmp).second, v1);
}

BOOST_AUTO_TEST_CASE(min_max_for_init_list_test)
{
    constexpr auto const r = ::ural::min({3, 1, 4, 1, 5, 9, 2});
    constexpr auto const R = ::ural::max({3, 1, 4, 1, 5, 9, 2});
    constexpr auto const rR = ::ural::minmax({3, 1, 4, 1, 5, 9, 2});

    static_assert(r == 1, "");
    static_assert(R == 9, "");
    static_assert(rR.first == 1, "");
    static_assert(rR.second == 9, "");

    std::initializer_list<int> e;

    BOOST_CHECK_THROW(::ural::min(e), std::logic_error);
    BOOST_CHECK_THROW(::ural::max(e), std::logic_error);
    BOOST_CHECK_THROW(::ural::minmax(e), std::logic_error);
}

BOOST_AUTO_TEST_CASE(min_max_for_init_list_stability)
{
     auto cmp = [](char x, char y) { return std::toupper(x) < std::toupper(y); };

    auto const r = ::ural::min({'c', 'a', 'd', 'A', 'E', 'Z', 'B'}, cmp);
    auto const R = ::ural::max({'c', 'a', 'd', 'A', 'E', 'Z', 'B'}, cmp);
    auto const rR = ::ural::minmax({'c', 'a', 'd', 'A', 'E', 'Z', 'B'}, cmp);

    BOOST_CHECK_EQUAL(r, 'a');
    BOOST_CHECK_EQUAL(R, 'Z');
    BOOST_CHECK_EQUAL(rR.first, 'a');
    BOOST_CHECK_EQUAL(rR.second, 'Z');

    std::initializer_list<char> e;

    BOOST_CHECK_THROW(::ural::min(e, cmp), std::logic_error);
    BOOST_CHECK_THROW(::ural::max(e, cmp), std::logic_error);
    BOOST_CHECK_THROW(::ural::minmax(e, cmp), std::logic_error);
}

// @todo перегрузки min/max/minmax для интервалов

BOOST_AUTO_TEST_CASE(min_element_test)
{
    std::forward_list<int> const v{3, 1, 4, 1, 5, 9, 2, 6, 5};

    auto std_result = std::min_element(std::begin(v), std::end(v));
    auto ural_result = ural::min_element(v);

    BOOST_CHECK_EQUAL(std::distance(std_result, v.end()),
                      ural::size(ural_result));
    BOOST_CHECK(!!ural_result);
    BOOST_CHECK_EQUAL(*std_result, *ural_result);
}

BOOST_AUTO_TEST_CASE(max_element_test)
{
    std::forward_list<int> const v{ 3, 1, -14, 1, 5, 9 };
    auto std_result = std::max_element(v.begin(), v.end());
    auto ural_result = ural::max_element(v);

    BOOST_CHECK_EQUAL(std::distance(std_result, v.end()),
                      ural::size(ural_result));
}

BOOST_AUTO_TEST_CASE(max_element_test_custom_compare)
{
    auto abs_compare = [](int a, int b) {return (std::abs(a) < std::abs(b));};

    std::forward_list<int> const v{ 3, 1, -14, 1, 5, 9 };
    auto std_result = std::max_element(v.begin(), v.end(), +abs_compare);
    auto ural_result = ural::max_element(v, +abs_compare);

    BOOST_CHECK_EQUAL(std::distance(std_result, v.end()),
                      ural::size(ural_result));
}

#include <ural/math.hpp>

BOOST_AUTO_TEST_CASE(max_element_using_compare_by)
{
    auto const sq_cmp = ural::compare_by(ural::square);

    static_assert(std::is_empty<decltype(sq_cmp)>::value, "Must be empty!");

    std::forward_list<int> const v{ 3, 1, -14, 1, 5, 9 };
    auto std_result = std::max_element(v.begin(), v.end(), sq_cmp);
    auto ural_result = ural::max_element(v, sq_cmp);

    BOOST_CHECK_EQUAL(std::distance(std_result, v.end()),
                      ural::size(ural_result));
}

BOOST_AUTO_TEST_CASE(minmax_element_test)
{
    std::forward_list<int> const v{ 3, 1, -14, 1, 5, 9 };
    auto std_result = std::minmax_element(v.begin(), v.end());
    auto ural_result = ural::minmax_element(v);

    BOOST_CHECK_EQUAL(std::distance(std_result.first, v.end()),
                      ural::size(ural_result[ural::_1]));
    BOOST_CHECK_EQUAL(std::distance(std_result.second, v.end()),
                      ural::size(ural_result[ural::_2]));
}

// 25.4.8 Лексикографическое сравнение
BOOST_AUTO_TEST_CASE(lexicographical_compare_test)
{
    typedef std::istringstream S;
    S is0_1("");
    S is0_2("");
    S ab("ab");
    S abc("abc");

    BOOST_CHECK_EQUAL(false, ural::lexicographical_compare(is0_1, is0_2));

    BOOST_CHECK_EQUAL(true, ural::lexicographical_compare(ab, abc));
    BOOST_CHECK_EQUAL(false, ural::lexicographical_compare("abc", "ab"));

    BOOST_CHECK_EQUAL(true, ural::lexicographical_compare("abcd", "abed"));
    BOOST_CHECK_EQUAL(false, ural::lexicographical_compare("abed", "abcd"));
}

// 25.4.9 Порождение перестановок
BOOST_AUTO_TEST_CASE(next_permutation_test)
{
    typedef std::list<int> String;
    String x {1, 2, 3, 4};
    std::vector<String> r_std;

    do
    {
        r_std.push_back(x);
    }
    while(std::next_permutation(x.begin(), x.end()));

    std::vector<String> r_ural;

    do
    {
        r_ural.push_back(x);
    }
    while(ural::next_permutation(x));

    BOOST_CHECK(r_std == r_ural);
}

BOOST_AUTO_TEST_CASE(prev_permutation_test)
{
    typedef std::list<int> String;
    String x {4, 3, 2, 1};
    std::vector<String> r_std;

    do
    {
        r_std.push_back(x);
    }
    while(std::prev_permutation(x.begin(), x.end()));

    std::vector<String> r_ural;

    do
    {
        r_ural.push_back(x);
    }
    while(ural::prev_permutation(x));

    BOOST_CHECK(r_std == r_ural);
}
