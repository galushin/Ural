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

#include "rnd.hpp"
#include "defs.hpp"

#include <ural/container/vector.hpp>

#include <boost/test/unit_test.hpp>

#include <ural/numeric.hpp>
#include <ural/math/rational.hpp>
#include <ural/sequence/transform.hpp>
#include <ural/numeric/numbers_sequence.hpp>
#include <ural/algorithm.hpp>
#include <ural/memory.hpp>
#include <ural/sequence/all.hpp>
#include <ural/utility/tracers.hpp>
#include <ural/concepts.hpp>

#include <forward_list>
#include <forward_list>
#include <list>
#include <vector>

#include <boost/mpl/list.hpp>

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
    Source is0{};
    Source is1{2, 4, 6, 8, 10};
    Source is2{2, 4, 6, 7, 10};

    auto const is_even = [](typename Source::const_reference i)
                         { return i % 2 == 0; };

    BOOST_CHECK_EQUAL(ural::all_of(is0, is_even), true);
    BOOST_CHECK_EQUAL(ural::all_of(is1, is_even), true);
    BOOST_CHECK_EQUAL(ural::all_of(is2, is_even), false);
}

// 25.2.2
BOOST_AUTO_TEST_CASE(any_of_test)
{
    typedef ural_test::istringstream_helper<int> Source;

    Source is0{};
    Source is1{2, 4, 6, 8, 10};
    Source is2{2, 4, 6, 7, 10};

    auto const is_odd = [](typename Source::const_reference i)
                         { return i % 2 == 1; };

    BOOST_CHECK_EQUAL(ural::any_of(is0, is_odd), false);
    BOOST_CHECK_EQUAL(ural::any_of(is1, is_odd), false);
    BOOST_CHECK_EQUAL(ural::any_of(is2, is_odd), true);
}

// 25.2.3
BOOST_AUTO_TEST_CASE(none_of_test)
{
    typedef ural_test::istringstream_helper<int> Source;

    Source is0{};
    Source is1{2, 4, 6, 8, 10};
    Source is2{2, 4, 6, 7, 10};

    auto const is_odd = [](typename Source::const_reference i)
                         { return i % 2 == 1; };

    BOOST_CHECK_EQUAL(ural::none_of(is0, is_odd), true);
    BOOST_CHECK_EQUAL(ural::none_of(is1, is_odd), true);
    BOOST_CHECK_EQUAL(ural::none_of(is2, is_odd), false);
}

// 25.2.4
BOOST_AUTO_TEST_CASE(for_each_test)
{
    std::vector<int> x_std = {1, 2, 3, 4, 5};
    std::forward_list<int> x_ural(x_std.begin(), x_std.end());

    auto const action = [](int & x) {x *= 2;};

    auto const r_std = std::for_each(x_std.begin(), x_std.end(), +action);
    auto const r_ural = ural::for_each(x_ural, +action);

    BOOST_CHECK(r_ural[ural::_1].traversed_front().begin() == x_ural.begin());
    BOOST_CHECK(r_ural[ural::_1].traversed_front().end() == x_ural.end());
    BOOST_CHECK(r_ural[ural::_1].begin() == x_ural.end());
    BOOST_CHECK(r_ural[ural::_1].end() == x_ural.end());

    BOOST_CHECK_EQUAL(r_std, r_ural[ural::_2].target());

    BOOST_CHECK_EQUAL_COLLECTIONS(x_std.begin(), x_std.end(),
                                  x_ural.begin(), x_ural.end());
}

BOOST_AUTO_TEST_CASE(for_each_input_sequence_test)
{
    std::vector<int> const x{1, 2, 3, 4, 5};
    ural_test::istringstream_helper<int> x_ural(x.begin(), x.end());

    int sum = 0;
    auto acc = [&sum](auto x) { sum += x; };

    ural::for_each(x_ural, acc);

    BOOST_CHECK_EQUAL(sum, ural::accumulate(x, 0));
}

// 25.2.5
BOOST_AUTO_TEST_CASE(find_fail_test_istream)
{
    ural_test::istringstream_helper<int> v{0, 1, 2, 3, 4};

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
    ural_test::istringstream_helper<int> v{0, 1, 2, 3, 4};

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

// 25.3 Модифицирующие последовательность алгоритмы
// 25.3.1 Копирование
BOOST_AUTO_TEST_CASE(copy_test)
{
    std::vector<int> const src = {1, 2, 3, 4};
    ural_test::istringstream_helper<int> xs(src.begin(), src.end());

    std::vector<int> x1;

    ural::copy(xs, std::back_inserter(x1));

    BOOST_CHECK_EQUAL_COLLECTIONS(src.begin(), src.end(), x1.begin(), x1.end());
}

BOOST_AUTO_TEST_CASE(copy_to_shorter_test)
{
    std::vector<int> const xs = {1, 2, 3, 4};

    std::vector<int> x1(xs.size() - 2, 0);

    BOOST_CHECK_LE(x1.size(), xs.size());

    auto const r1 = ural::copy(xs, x1);

    BOOST_CHECK_EQUAL_COLLECTIONS(x1.begin(), x1.end(),
                                  xs.begin(), xs.begin() + x1.size());

    BOOST_CHECK(!!r1[ural::_1]);
    BOOST_CHECK(!r1[ural::_2]);

    BOOST_CHECK(r1[ural::_1].traversed_begin() == xs.begin());
    BOOST_CHECK(r1[ural::_1].begin() == xs.begin() + (xs.size() - x1.size()));
    BOOST_CHECK(r1[ural::_1].end() == xs.end());
    BOOST_CHECK(r1[ural::_1].traversed_end() == xs.end());

    BOOST_CHECK(r1[ural::_2].traversed_begin() == x1.begin());
    BOOST_CHECK(r1[ural::_2].begin() == x1.end());
    BOOST_CHECK(r1[ural::_2].end() == x1.end());
    BOOST_CHECK(r1[ural::_2].traversed_end() == x1.end());
}

BOOST_AUTO_TEST_CASE(copy_to_longer_test)
{
    std::vector<int> const xs = {1, 2, 3, 4};

    std::vector<int> x2(xs.size() + 2, 0);
    auto const x2_old = x2;

    BOOST_CHECK_GE(x2.size(), xs.size());

    auto const r2 = ural::copy(xs, x2);

    BOOST_CHECK_EQUAL_COLLECTIONS(x2.begin(), x2.begin() + xs.size(),
                                  xs.begin(), xs.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(x2.begin() + xs.size(), x2.end(),
                                  x2_old.begin() + xs.size(), x2_old.end());

    BOOST_CHECK(!r2[ural::_1]);
    BOOST_CHECK(!!r2[ural::_2]);

    BOOST_CHECK(r2[ural::_1].traversed_begin() == xs.begin());
    BOOST_CHECK(r2[ural::_1].begin() == xs.end());
    BOOST_CHECK(r2[ural::_1].end() == xs.end());
    BOOST_CHECK(r2[ural::_1].traversed_end() == xs.end());

    BOOST_CHECK(r2[ural::_2].traversed_begin() == x2.begin());
    BOOST_CHECK(r2[ural::_2].begin() == x2.begin() + xs.size());
    BOOST_CHECK(r2[ural::_2].end() == x2.end());
    BOOST_CHECK(r2[ural::_2].traversed_end() == x2.end());
}

BOOST_AUTO_TEST_CASE(copy_to_ostream_test)
{
    std::string const src = "1234567890";

    std::string r_std;
    std::copy(src.begin(), src.end(), std::back_inserter(r_std));

    std::ostringstream os_ural;
    ural::copy(src, os_ural);

    BOOST_CHECK_EQUAL(r_std, os_ural.str());
}

BOOST_AUTO_TEST_CASE(copy_n_test)
{
    std::string const str = "1234567890";
    ural_test::istringstream_helper<char> src(str.begin(), str.end());
    std::string r_std;
    std::string r_ural;

    auto const n = 4;

    std::copy_n(str.begin(), n, std::back_inserter(r_std));

    ural::copy_n(src, n, std::back_inserter(r_ural));

    BOOST_CHECK_EQUAL(r_std, r_ural);
}

BOOST_AUTO_TEST_CASE(copy_n_test_to_longer_container)
{
    std::string const src = "1234567890";
    auto const n = 4;

    std::string r_std(src, 0, n + 2);
    std::string r_ural = r_std;
    BOOST_CHECK_EQUAL(r_std, r_ural);

    auto const result_std  = std::copy_n(src.begin(), n, r_std.begin());
    auto const result_ural = ural::copy_n(src, n, r_ural);

    BOOST_CHECK_EQUAL(r_std, r_ural);

    BOOST_CHECK(result_ural[ural::_1].begin() == src.begin() + n);
    BOOST_CHECK(result_ural[ural::_1].end() == src.end());

    BOOST_CHECK_EQUAL(result_ural[ural::_2].size(), r_std.end() - result_std);
    BOOST_CHECK(result_ural[ural::_2].end() == r_ural.end());
}

BOOST_AUTO_TEST_CASE(copy_n_test_to_shorter_container)
{
    std::string const src = "1234567890";
    auto const n = 4;

    std::string r_std(src, 0, n - 2);
    std::string r_ural = r_std;
    BOOST_CHECK_EQUAL(r_std, r_ural);

    auto const n0 = std::min<size_t>(n, r_std.size());

    auto const result_std  = std::copy_n(src.begin(), n0, r_std.begin());
    auto const result_ural = ural::copy_n(src, n, r_ural);

    BOOST_CHECK_EQUAL(r_std, r_ural);

    BOOST_CHECK(result_ural[ural::_1].begin() == src.begin() + n0);
    BOOST_CHECK(result_ural[ural::_1].end() == src.end());

    BOOST_CHECK_EQUAL(result_ural[ural::_2].size(), r_std.end() - result_std);
    BOOST_CHECK(result_ural[ural::_2].end() == r_ural.end());
}

BOOST_AUTO_TEST_CASE(copy_if_test)
{
    typedef int Type;
    std::vector<Type> const src_std = {25, -15, 5, -5, 15};
    ural_test::istringstream_helper<Type> src_ural(src_std.begin(), src_std.end());

    auto const pred = [](Type i){return !(i<0);};

    std::vector<Type> r_std;
    std::copy_if(src_std.begin(), src_std.end(), std::back_inserter(r_std) , pred);

    std::vector<Type> r_ural;
    ural::copy_if(src_ural, r_ural | ural::back_inserter, pred);

    BOOST_CHECK_EQUAL_COLLECTIONS(r_std.begin(), r_std.end(),
                                  r_ural.begin(), r_ural.end());
}

BOOST_AUTO_TEST_CASE(copy_if_test_to_longer_container)
{
    typedef int Type;
    std::vector<Type> const xs = {25, -15, 5, -5, 15};
    auto const pred = [](Type i){return !(i<0);};

    std::vector<Type> r_std(xs.size() + 2, 42);
    std::vector<Type> r_ural(r_std);

    auto const result_std = std::copy_if(xs.begin(), xs.end(), r_std.begin(), pred);
    auto const result_ural = ural::copy_if(xs, r_ural, pred);

    BOOST_CHECK(result_ural[ural::_1].begin() == xs.end());
    BOOST_CHECK(result_ural[ural::_1].end() == xs.end());

    BOOST_CHECK_EQUAL(result_ural[ural::_2].begin() - r_ural.begin(),
                      result_std - r_std.begin());
    BOOST_CHECK(result_ural[ural::_2].end() == r_ural.end());

    BOOST_CHECK_EQUAL_COLLECTIONS(r_std.begin(), r_std.end(),
                                  r_ural.begin(), r_ural.end());
}

BOOST_AUTO_TEST_CASE(copy_if_test_to_shorter_container)
{
    // Подготовка
    typedef int Type;
    std::vector<Type> const xs = {25, -15, 5, -5, 15, -13, -42, 18};
    auto const pred = [](Type i){return !(i<0);};

    // ural
    std::vector<Type> r_ural(::ural::count_if(xs, pred)/2, 42);

    auto const result_ural = ural::copy_if(xs, r_ural, pred);

    // std
    std::vector<Type> r_std;
    std::copy_if(xs.begin(), xs.end(), r_std | ural::back_inserter, pred);
    r_std.resize(r_ural.size());

    // Проверки
    BOOST_CHECK_EQUAL_COLLECTIONS(r_ural.begin(), r_ural.end(),
                                  r_std.begin(), r_std.end());

    BOOST_CHECK_EQUAL(::ural::count_if(result_ural[ural::_1].traversed_front(),
                                       pred),
                      ural::to_signed(r_ural.size()));
    BOOST_CHECK(result_ural[ural::_1].end() == xs.end());

    BOOST_CHECK(result_ural[ural::_2].begin() == r_ural.end());
    BOOST_CHECK(result_ural[ural::_2].end() == r_ural.end());
}

BOOST_AUTO_TEST_CASE(copy_backward_test)
{
    std::vector<int> x_std = {1, 2, 3, 4, 5};
    std::vector<int> x_ural(x_std.begin(), x_std.end());

    std::copy_backward(x_std.begin(), x_std.end() - 1, x_std.end());

    auto src = ural::make_iterator_sequence(x_ural.begin(), x_ural.end() - 1);
    ural::copy_backward(src, x_ural);

    BOOST_CHECK_EQUAL_COLLECTIONS(x_std.begin(), x_std.end(),
                                  x_ural.begin(), x_ural.end());
}

BOOST_AUTO_TEST_CASE(copy_backward_to_shorter)
{
    std::list<int> const src = {1, 2, 3, 4, 5};
    std::list<int> out(src.size() / 2, -1);

    BOOST_CHECK(out.empty() == false);
    BOOST_CHECK_LE(out.size(), src.size());

    auto const result = ::ural::copy_backward(src, out);

    auto const copied_begin = std::next(src.begin(), src.size() - out.size());

    BOOST_CHECK_EQUAL_COLLECTIONS(copied_begin, src.end(),
                                  out.begin(), out.end());

    BOOST_CHECK(result[ural::_1].traversed_begin() == src.begin());
    BOOST_CHECK(result[ural::_1].begin() == src.begin());
    BOOST_CHECK(result[ural::_1].end() == copied_begin);
    BOOST_CHECK(result[ural::_1].traversed_end() == src.end());

    BOOST_CHECK(result[ural::_2].traversed_begin() == out.begin());
    BOOST_CHECK(result[ural::_2].begin() == out.begin());
    BOOST_CHECK(result[ural::_2].end() == out.begin());
    BOOST_CHECK(result[ural::_2].traversed_end() == out.end());
}

BOOST_AUTO_TEST_CASE(copy_backward_to_longer)
{
    std::list<int> const src = {1, 2, 3, 4, 5};
    std::list<int> out(src.size() * 2, -1);
    auto const out_old = out;

    BOOST_CHECK(out.empty() == false);
    BOOST_CHECK_GE(out.size(), src.size());

    auto const result = ::ural::copy_backward(src, out);

    auto const dn = out.size() - src.size();

    auto const writed_begin = std::next(out.begin(), dn);

    BOOST_CHECK_EQUAL_COLLECTIONS(out.begin(), writed_begin,
                                  out_old.begin(), std::next(out_old.begin(), dn));

    BOOST_CHECK_EQUAL_COLLECTIONS(src.begin(), src.end(),
                                  writed_begin, out.end());

    BOOST_CHECK(result[ural::_1].traversed_begin() == src.begin());
    BOOST_CHECK(result[ural::_1].begin() == src.begin());
    BOOST_CHECK(result[ural::_1].end() == src.begin());
    BOOST_CHECK(result[ural::_1].traversed_end() == src.end());

    BOOST_CHECK(result[ural::_2].traversed_begin() == out.begin());
    BOOST_CHECK(result[ural::_2].begin() == out.begin());
    BOOST_CHECK(result[ural::_2].end() == writed_begin);
    BOOST_CHECK(result[ural::_2].traversed_end() == out.end());
}

// 25.3.2 Перемещение
// move
// @todo move Минимизация требований к последовательностям
BOOST_AUTO_TEST_CASE(move_test)
{
    std::vector<std::string> src = {"Alpha", "Beta", "Gamma"};

    auto const src_old = src;

    std::vector<std::string> xs;

    ural::move(src, xs | ural::back_inserter);

    BOOST_CHECK_EQUAL_COLLECTIONS(src_old.begin(), src_old.end(),
                                  xs.begin(), xs.end());

    for(auto const & s : src)
    {
        BOOST_CHECK(ural::empty(s));
    }
}

BOOST_AUTO_TEST_CASE(move_to_shorter_test)
{
    std::vector<std::string> xs = {"Alpha", "Beta", "Gamma", "Delta"};
    auto const xs_old = xs;

    std::vector<std::string> x1(xs.size() - 2, "Omega");

    BOOST_CHECK_LE(x1.size(), xs.size());

    auto const r1 = ural::move(xs, x1);

    BOOST_CHECK_EQUAL_COLLECTIONS(x1.begin(), x1.end(),
                                  xs_old.begin(), xs_old.begin() + x1.size());

    for(auto i : ural::indices_of(x1))
    {
        BOOST_CHECK_NE(xs[i], xs_old[i]);
    }
    BOOST_CHECK_EQUAL_COLLECTIONS(xs.begin() + x1.size(), xs.end(),
                                  xs_old.begin() + x1.size(), xs_old.end());

    BOOST_CHECK(!!r1[ural::_1]);
    BOOST_CHECK(!r1[ural::_2]);

    BOOST_CHECK(r1[ural::_1].traversed_begin() == xs.begin());
    BOOST_CHECK(r1[ural::_1].begin() == xs.begin() + (xs.size() - x1.size()));
    BOOST_CHECK(r1[ural::_1].end() == xs.end());
    BOOST_CHECK(r1[ural::_1].traversed_end() == xs.end());

    BOOST_CHECK(r1[ural::_2].traversed_begin() == x1.begin());
    BOOST_CHECK(r1[ural::_2].begin() == x1.end());
    BOOST_CHECK(r1[ural::_2].end() == x1.end());
    BOOST_CHECK(r1[ural::_2].traversed_end() == x1.end());
}

BOOST_AUTO_TEST_CASE(move_to_longer_test)
{
    std::vector<std::string> xs = {"Alpha", "Beta", "Gamma", "Delta"};
    auto const xs_old = xs;

    std::vector<std::string> x2(xs.size() + 2, "Omega");
    auto const x2_old = x2;

    BOOST_CHECK_GE(x2.size(), xs.size());

    auto const r2 = ural::move(xs, x2);

    BOOST_CHECK_EQUAL_COLLECTIONS(x2.begin(), x2.begin() + xs.size(),
                                  xs_old.begin(), xs_old.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(x2.begin() + xs.size(), x2.end(),
                                  x2_old.begin() + xs.size(), x2_old.end());

    for(auto i : ural::indices_of(xs))
    {
        BOOST_CHECK_NE(xs[i], xs_old[i]);
    }

    BOOST_CHECK(!r2[ural::_1]);
    BOOST_CHECK(!!r2[ural::_2]);

    BOOST_CHECK(r2[ural::_1].traversed_begin() == xs.begin());
    BOOST_CHECK(r2[ural::_1].begin() == xs.end());
    BOOST_CHECK(r2[ural::_1].end() == xs.end());
    BOOST_CHECK(r2[ural::_1].traversed_end() == xs.end());

    BOOST_CHECK(r2[ural::_2].traversed_begin() == x2.begin());
    BOOST_CHECK(r2[ural::_2].begin() == x2.begin() + xs.size());
    BOOST_CHECK(r2[ural::_2].end() == x2.end());
    BOOST_CHECK(r2[ural::_2].traversed_end() == x2.end());
}

BOOST_AUTO_TEST_CASE(move_backward_test)
{
    std::vector<std::string> x_std = {"one", "two", "three", "four", "five"};
    auto x_ural = x_std;

    std::move_backward(x_std.begin(), x_std.end() - 1, x_std.end());

    auto src = ural::make_iterator_sequence(x_ural.begin(), x_ural.end() - 1);
    ural::move_backward(src, x_ural);

    BOOST_CHECK_EQUAL_COLLECTIONS(x_std.begin(), x_std.end(),
                                  x_ural.begin(), x_ural.end());
}

BOOST_AUTO_TEST_CASE(move_backward_to_shorter_test)
{
    std::vector<std::string> xs = {"Alpha", "Beta", "Gamma", "Delta"};
    auto const xs_old = xs;

    std::vector<std::string> x1(xs.size() - 2, "Omega");

    BOOST_CHECK_LE(x1.size(), xs.size());

    auto const r1 = ural::move_backward(xs, x1);

    BOOST_CHECK_EQUAL_COLLECTIONS(x1.begin(), x1.end(),
                                  xs_old.end() - x1.size(), xs_old.end());

    for(size_t i = xs_old.size() - x1.size(); i != xs_old.size(); ++ i)
    {
        BOOST_CHECK_NE(xs[i], xs_old[i]);
    }
    BOOST_CHECK_EQUAL_COLLECTIONS(xs.begin(), xs.end() -  x1.size(),
                                  xs_old.begin(), xs_old.end() - x1.size());

    BOOST_CHECK(r1[ural::_1].traversed_begin() == xs.begin());
    BOOST_CHECK(r1[ural::_1].begin() == xs.begin());
    BOOST_CHECK(r1[ural::_1].end() == xs.end() - x1.size());
    BOOST_CHECK(r1[ural::_1].traversed_end() == xs.end());

    BOOST_CHECK(r1[ural::_2].traversed_begin() == x1.begin());
    BOOST_CHECK(r1[ural::_2].begin() == x1.begin());
    BOOST_CHECK(r1[ural::_2].end() == x1.begin());
    BOOST_CHECK(r1[ural::_2].traversed_end() == x1.end());
}

BOOST_AUTO_TEST_CASE(move_backward_to_longer_test)
{
    std::vector<std::string> xs = {"Alpha", "Beta", "Gamma", "Delta"};
    auto const xs_old = xs;

    std::vector<std::string> x2(xs.size() + 2, "Omega");
    auto const x2_old = x2;

    BOOST_CHECK_GE(x2.size(), xs.size());

    auto const r2 = ural::move_backward(xs, x2);

    BOOST_CHECK_EQUAL_COLLECTIONS(x2.begin(), x2.end() - xs.size(),
                                  x2_old.begin(), x2_old.end() - xs.size());
    BOOST_CHECK_EQUAL_COLLECTIONS(x2.end() - xs.size(), x2.end(),
                                  xs_old.begin(), xs_old.end());

    for(auto i : ural::indices_of(xs))
    {
        BOOST_CHECK_NE(xs[i], xs_old[i]);
    }

    BOOST_CHECK(!r2[ural::_1]);
    BOOST_CHECK(!!r2[ural::_2]);

    BOOST_CHECK(r2[ural::_1].traversed_begin() == xs.begin());
    BOOST_CHECK(r2[ural::_1].begin() == xs.begin());
    BOOST_CHECK(r2[ural::_1].end() == xs.begin());
    BOOST_CHECK(r2[ural::_1].traversed_end() == xs.end());

    BOOST_CHECK(r2[ural::_2].traversed_begin() == x2.begin());
    BOOST_CHECK(r2[ural::_2].begin() == x2.begin());
    BOOST_CHECK(r2[ural::_2].end() == x2.end() - xs.size());
    BOOST_CHECK(r2[ural::_2].traversed_end() == x2.end());
}

// 25.3.3 Обмен интервалов
BOOST_AUTO_TEST_CASE(swap_ranges_test_shorter_1)
{
    std::forward_list<int> const x1 = {1, 2, 3, 4};
    std::forward_list<int> const x2   = {-1, -2, -3, -4, -5};

     BOOST_CHECK_LE(std::distance(x1.begin(), x1.end()),
                   std::distance(x2.begin(), x2.end()));

    auto y1 = x1;
    auto y2 = x2;

    auto r = ural::swap_ranges(y1, y2);

    BOOST_CHECK(!r[ural::_1] || !r[ural::_2]);
    BOOST_CHECK_EQUAL(ural::size(r[ural::_1].traversed_front()),
                      ural::size(r[ural::_2].traversed_front()));

    auto const n = std::min(std::distance(x1.begin(), x1.end()),
                            std::distance(x2.begin(), x2.end()));

    BOOST_CHECK_EQUAL_COLLECTIONS(y1.begin(), y1.end(),
                                  x2.begin(), std::next(x2.begin(), n));

    BOOST_CHECK_EQUAL_COLLECTIONS(y2.begin(), std::next(y2.begin(), n),
                                  x1.begin(), x1.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(std::next(y2.begin(), n), y2.end(),
                                  std::next(x2.begin(), n), x2.end());
}

BOOST_AUTO_TEST_CASE(swap_ranges_test_shorter_2)
{
    std::forward_list<int> const x1 = {1, 2, 3, 4, 5};
    std::forward_list<int> const x2   = {-1, -2, -3, -4};

    BOOST_CHECK_GE(std::distance(x1.begin(), x1.end()),
                   std::distance(x2.begin(), x2.end()));

    auto y1 = x1;
    auto y2 = x2;

    auto r = ural::swap_ranges(y1, y2);

    BOOST_CHECK(!r[ural::_1] || !r[ural::_2]);
    BOOST_CHECK_EQUAL(ural::size(r[ural::_1].traversed_front()),
                      ural::size(r[ural::_2].traversed_front()));

    auto const n = std::min(std::distance(x1.begin(), x1.end()),
                            std::distance(x2.begin(), x2.end()));

    BOOST_CHECK_EQUAL_COLLECTIONS(y1.begin(), std::next(y1.begin(), n),
                                  x2.begin(), x2.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(std::next(y1.begin(), n), y1.end(),
                                  std::next(x1.begin(), n), x1.end());

    BOOST_CHECK_EQUAL_COLLECTIONS(y2.begin(), y2.end(),
                                  x1.begin(), std::next(x1.begin(), n));
}

// 25.3.4 Преобразование
BOOST_AUTO_TEST_CASE(transform_test)
{
    std::string str("hello");
    std::istringstream is(str);

    auto f = std::ptr_fun<int, int>(std::toupper);

    std::string x_std;
    std::transform(str.begin(), str.end(), std::back_inserter(x_std), f);

    std::string x_ural;
    auto result = ural::transform(is, x_ural | ural::back_inserter, f);

    BOOST_CHECK(!result[ural::_1]);
    BOOST_CHECK(!!result[ural::_2]);

    BOOST_CHECK_EQUAL_COLLECTIONS(x_std.begin(), x_std.end(),
                                  x_ural.begin(), x_ural.end());
}

BOOST_AUTO_TEST_CASE(transform_test_return_value)
{
    std::string const s("hello");
    std::string x_ural(s.size() / 2, '?');

    auto f = std::ptr_fun<int, int>(std::toupper);

    auto result = ural::transform(s, x_ural, f);

    std::string x_std;
    std::transform(s.begin(), s.begin() + x_ural.size(),
                   std::back_inserter(x_std), f);

    BOOST_CHECK_EQUAL(x_ural, x_std);

    BOOST_CHECK_LE(x_ural.size(), s.size());

    BOOST_CHECK(result[ural::_1].traversed_begin() == s.begin());
    BOOST_CHECK(result[ural::_1].begin() == s.begin() + x_ural.size());
    BOOST_CHECK(result[ural::_1].end() == s.end());
    BOOST_CHECK(result[ural::_1].traversed_end() == s.end());

    BOOST_CHECK(result[ural::_2].traversed_begin() == x_ural.begin());
    BOOST_CHECK(result[ural::_2].begin() == x_ural.end());
    BOOST_CHECK(result[ural::_2].end() == x_ural.end());
    BOOST_CHECK(result[ural::_2].traversed_end() == x_ural.end());

    BOOST_CHECK(!!result[ural::_1]);
    BOOST_CHECK(!result[ural::_2]);
}

// @todo тесты возвращаемых значений transform
BOOST_AUTO_TEST_CASE(transform_2_test_shorter_in1)
{
    std::vector<int> const src1 = {1, 20, 30, 40};
    std::vector<int> const src2 = {10, 2, 30, 4, 5};

    BOOST_CHECK_LE(src1.size(), src2.size());

    ural_test::istringstream_helper<int> x1(src1.begin(), src1.end());
    ural_test::istringstream_helper<int> x2(src2.begin(), src2.end());

    std::vector<bool> z_std;
    std::vector<bool> z_ural;

    std::less_equal<int> constexpr f_std{};
    ural::less_equal<> constexpr f_ural{};

    std::transform(src1.begin(), src1.end(), src2.begin(),
                   std::back_inserter(z_std), f_std);

    ural::transform(x1, x2, std::back_inserter(z_ural), f_ural);

    BOOST_CHECK_EQUAL_COLLECTIONS(z_std.begin(), z_std.end(),
                                  z_ural.begin(), z_ural.end());
}

BOOST_AUTO_TEST_CASE(transform_2_test_shorter_in2)
{
    std::vector<int> const src1 = {1, 20, 30, 40, 50};
    std::vector<int> const src2 = {10, 2, 30, 4};

    BOOST_CHECK_GE(src1.size(), src2.size());

    ural_test::istringstream_helper<int> x1(src1.begin(), src1.end());
    ural_test::istringstream_helper<int> x2(src2.begin(), src2.end());

    std::vector<bool> z_std;
    std::vector<bool> z_ural;

    std::less_equal<int> constexpr f_std{};
    ural::less_equal<> constexpr f_ural{};

    // std ограничивает по первой последовательности
    std::transform(src1.begin(), std::next(src1.begin(), src2.size()),
                   src2.begin(),
                   std::back_inserter(z_std), f_std);

    ural::transform(x1, x2, std::back_inserter(z_ural), f_ural);

    BOOST_CHECK_EQUAL_COLLECTIONS(z_std.begin(), z_std.end(),
                                  z_ural.begin(), z_ural.end());
}

BOOST_AUTO_TEST_CASE(transform_2_test_shorter_out)
{
    std::vector<int> const src1 = {1, 20, 30, 40};
    std::vector<int> const src2 = {10, 2, 30, 4, 5};

    BOOST_CHECK_LE(src1.size(), src2.size());

    ural_test::istringstream_helper<int> x1(src1.begin(), src1.end());
    ural_test::istringstream_helper<int> x2(src2.begin(), src2.end());

    std::vector<int> z_std;
    std::vector<int> z_ural(std::min(src1.size(), src2.size()) - 1, -1);

    std::plus<int> constexpr f_std{};
    ural::plus<> constexpr f_ural{};

    std::transform(src1.begin(), src1.end(), src2.begin(),
                   std::back_inserter(z_std), f_std);

    ural::transform(x1, x2, z_ural, f_ural);

    BOOST_CHECK_EQUAL_COLLECTIONS(z_std.begin(), z_std.begin() + z_ural.size(),
                                  z_ural.begin(), z_ural.end());
}

// 25.3.5 Замена
BOOST_AUTO_TEST_CASE(replace_test_different_types)
{
    std::vector<int> s_std = {5, 7, 4, 2, 8, 6, 1, 9, 0, 3};
    std::forward_list<int> s_ural(s_std.begin(), s_std.end());

    auto const old_value = ural::rational<int>(8);
    auto const new_value = 88;

    BOOST_CHECK_EQUAL(old_value.denominator(), 1);

    std::replace(s_std.begin(), s_std.end(), old_value.numerator(), new_value);

    auto const r_ural = ural::replace(s_ural, old_value, new_value);

    BOOST_CHECK(r_ural.traversed_front().begin() == s_ural.begin());
    BOOST_CHECK(r_ural.traversed_front().end() == s_ural.end());
    BOOST_CHECK(r_ural.begin() == s_ural.end());
    BOOST_CHECK(r_ural.end() == s_ural.end());

    BOOST_CHECK_EQUAL_COLLECTIONS(s_std.begin(), s_std.end(),
                                  s_ural.begin(), s_ural.end());
}

BOOST_AUTO_TEST_CASE(replace_if_test)
{
    std::array<int, 10> const s{5, 7, 4, 2, 8, 6, 1, 9, 0, 3};

    auto x_std = s;
    std::forward_list<int> x_ural(s.begin(), s.end());

    auto pred = [](int x) {return x < 5;};
    auto const new_value = 55;

    std::replace_if(x_std.begin(), x_std.end(), pred, new_value);
    auto const r_ural = ural::replace_if(x_ural, pred, new_value);

    BOOST_CHECK(r_ural.traversed_front().begin() == x_ural.begin());
    BOOST_CHECK(r_ural.traversed_front().end() == x_ural.end());
    BOOST_CHECK(r_ural.begin() == x_ural.end());
    BOOST_CHECK(r_ural.end() == x_ural.end());

    BOOST_CHECK_EQUAL_COLLECTIONS(x_std.begin(), x_std.end(),
                                  x_ural.begin(), x_ural.end());
}

BOOST_AUTO_TEST_CASE(replace_copy_test)
{
    // Исходные данные
    std::vector<int> const source = {5, 7, 4, 2, 8, 6, 1, 9, 0, 3};
    auto const old_value = 8;
    auto const new_value = 88;

    // std
    std::vector<int> s_std;
    std::replace_copy(source.begin(), source.end(), std::back_inserter(s_std),
                      old_value, new_value);

    // ural
    std::vector<int> s_ural;
    ural::replace_copy(source, s_ural | ural::back_inserter,
                       old_value, new_value);

    // Проверка
    BOOST_CHECK_EQUAL_COLLECTIONS(s_std.begin(), s_std.end(),
                                  s_ural.begin(), s_ural.end());
}

BOOST_AUTO_TEST_CASE(replace_copy_with_pred_regression)
{
    // Исходные данные
    std::vector<int> const source = {5, 7, 4, 2, 8, 6, 1, 9, 0, 3};
    auto const old_value = 5;
    auto const new_value = 55;

    auto pred = [=](int x) {return x < old_value;};
    auto bin_pred = ural::less<>{};

    // std
    std::vector<int> s_std;
    std::replace_copy_if(source.begin(), source.end(),
                         std::back_inserter(s_std), pred, new_value);

    // ural
    std::vector<int> s_ural;
    ural::replace_copy(source, s_ural | ural::back_inserter,
                       old_value, new_value, bin_pred);

    // Проверка
    BOOST_CHECK_EQUAL_COLLECTIONS(s_std.begin(), s_std.end(),
                                  s_ural.begin(), s_ural.end());
}

BOOST_AUTO_TEST_CASE(replace_copy_if_test)
{
    // Подготовка
    std::forward_list<int> const source{5, 7, 4, 2, 8, 6, 1, 9, 0, 3};
    auto pred = [](int x) {return x < 5;};
    auto const new_value = 55;

    // std
    std::vector<int> x_std;
    std::replace_copy_if(source.begin(), source.end(),
                         std::back_inserter(x_std), pred, new_value);

    // ural
    std::vector<int> x_ural;
    ural::replace_copy_if(source, x_ural | ural::back_inserter,
                          pred, new_value);

    // Проверка
    BOOST_CHECK_EQUAL_COLLECTIONS(x_std.begin(), x_std.end(),
                                  x_ural.begin(), x_ural.end());
}

// 25.3.6 Заполнение
BOOST_AUTO_TEST_CASE(fill_test)
{
    std::vector<int> x_std = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    auto x_ural = x_std;

    auto const value = -1;
    std::vector<int> const z(x_std.size(), value);

    std::fill(x_std.begin(), x_std.end(), value);
    ural::fill(x_ural, value);

    BOOST_CHECK_EQUAL_COLLECTIONS(x_std.begin(), x_std.end(),
                                  x_ural.begin(), x_ural.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(x_ural.begin(), x_ural.end(),
                                  z.begin(), z.end());
}

BOOST_AUTO_TEST_CASE(fill_n_test)
{
    std::vector<int> v_std{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto v_ural = v_std;

    auto const n = v_std.size() / 2;
    auto const value = -1;

    auto const r_std = std::fill_n(v_std.begin(), n, value);

    auto const r_ural = ural::fill_n(v_ural, n, value);

    BOOST_CHECK_EQUAL_COLLECTIONS(v_std.begin(), v_std.end(),
                                  v_ural.begin(), v_ural.end());

    BOOST_CHECK_EQUAL(r_ural.begin() - v_ural.begin(), r_std - v_std.begin());
    BOOST_CHECK(r_ural.end() == v_ural.end());
    BOOST_CHECK(r_ural.traversed_front().begin() == v_ural.begin());
}

// 25.3.7 Порождение
// @todo тест generate

BOOST_AUTO_TEST_CASE(generate_n_terse_test)
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
    ural::generate_n(r_ural | ural::back_inserter, n, gen);

    // Проверка
    BOOST_CHECK_EQUAL_COLLECTIONS(r_std.begin(), r_std.end(),
                                  r_ural.begin(), r_ural.end());
}

// 25.3.8 Удаление
BOOST_AUTO_TEST_CASE(remove_test)
{
    std::string s_std = "Text with some   spaces";
    auto s_ural = s_std;

    auto const to_remove = ' ';

    auto r_std = std::remove(s_std.begin(), s_std.end(), to_remove);

    auto r_ural = ural::remove(s_ural, to_remove);

    BOOST_CHECK_EQUAL(s_std, s_ural);

    BOOST_CHECK_EQUAL(r_ural.begin() - s_ural.begin(), r_std - s_std.begin());
    BOOST_CHECK_EQUAL(r_ural.traversed_begin() - s_ural.begin(), 0);
    BOOST_CHECK_EQUAL(ural::to_unsigned(r_ural.end() - s_ural.begin()), s_ural.size());

    BOOST_CHECK_EQUAL_COLLECTIONS(r_ural.traversed_begin(), r_ural.begin(),
                                  s_std.begin(), r_std);
}

BOOST_AUTO_TEST_CASE(remove_copy_test)
{
    // Подготовка
    auto const source = std::string("Text with some   spaces");
    auto const to_remove = ' ';

    // std
    std::string s_std;
    std::remove_copy(source.begin(), source.end(),
                     std::back_inserter(s_std), to_remove);

    // ural
    std::string s_ural;
    ural::remove_copy(source, s_ural | ural::back_inserter, to_remove);

    // Сравнение
    BOOST_CHECK_EQUAL(s_std, s_ural);
}

BOOST_AUTO_TEST_CASE(remove_erase_test)
{
    std::string s_std = "Text with some   spaces";
    auto s_ural = s_std;

    auto const to_remove = ' ';

    s_std.erase(std::remove(s_std.begin(), s_std.end(), to_remove), s_std.end());

    ural::remove_erase(s_ural, to_remove);

    BOOST_CHECK_EQUAL(s_std, s_ural);
}

BOOST_AUTO_TEST_CASE(inplace_remove_if_test)
{
    std::string s_std = "Text\n with\tsome \t  whitespaces\n\n";
    auto s_ural = s_std;

    auto const pred = [](char x){return std::isspace(x);};

    s_std.erase(std::remove_if(s_std.begin(), s_std.end(), pred), s_std.end());

    auto s = ural::remove_if(s_ural, pred);
    ural::erase(s_ural, s);

    BOOST_CHECK_EQUAL(s_std, s_ural);
}

BOOST_AUTO_TEST_CASE(remove_if_erase_test)
{
    std::string s_std = "Text\n with\tsome \t  whitespaces\n\n";
    auto s_ural = s_std;

    auto const pred = [](char x){return std::isspace(x);};

    s_std.erase(std::remove_if(s_std.begin(), s_std.end(), pred), s_std.end());

    ural::remove_if_erase(s_ural, pred);

    BOOST_CHECK_EQUAL(s_std, s_ural);
}

BOOST_AUTO_TEST_CASE(remove_copy_if_test)
{
    // Подготовка
    std::string const source = "Text\n with\tsome \t  whitespaces\n\n";
    auto pred = [](char x){return std::isspace(x);};

    // std
    std::string s_std;
    std::remove_copy_if(source.begin(), source.end(),
                        std::back_inserter(s_std), pred);

    // ural
    std::string s_ural;
    ural::remove_copy_if(source, s_ural | ural::back_inserter, pred);

    // Проверка
    BOOST_CHECK_EQUAL(s_std, s_ural);
}

// 25.3.9 Удаление повторов
BOOST_AUTO_TEST_CASE(unique_test)
{
    std::forward_list<int> s_std{1, 2, 2, 2, 3, 3, 2, 2, 1};
    auto s_ural = s_std;

    auto const r_std = std::unique(s_std.begin(), s_std.end());
    auto const r_ural = ural::unique(s_ural);

    BOOST_CHECK_EQUAL_COLLECTIONS(s_std.begin(), r_std,
                                  r_ural.traversed_begin(), r_ural.begin());
}

BOOST_AUTO_TEST_CASE(unique_copy_test_custom_predicate)
{
    // Подготовка
    std::string const source = "The      string    with many       spaces!";

    auto const pred = [](char c1, char c2){ return c1 == ' ' && c2 == ' '; };

    // std
    std::istringstream is_std(source);
    std::ostringstream os_std;

    std::unique_copy(std::istream_iterator<char>(is_std),
                     std::istream_iterator<char>(),
                     std::ostream_iterator<char>(os_std),
                     pred);

    // ural
    std::istringstream is_ural(source);
    std::ostringstream os_ural;

    ural::unique_copy(ural::make_istream_sequence<char>(is_ural),
                      ural::make_ostream_sequence(os_ural),
                      pred);

    // Проверка результатов
    BOOST_CHECK_EQUAL(os_std.str(), os_ural.str());
}

BOOST_AUTO_TEST_CASE(unique_copy_from_istream_to_ostream)
{
    std::list<int> const v1{1, 2, 2, 2, 3, 3, 2, 2, 1};

    std::ostringstream src;
    ural::copy(v1, ural::make_ostream_sequence(src, ' '));

    auto v2 = v1;
    ural::unique_erase(v2);

    std::ostringstream z;
    ural::copy(v2, ural::make_ostream_sequence(z, ' '));

    // Сам алгоритм
    std::istringstream is(src.str());
    std::ostringstream os;

    ural::unique_copy(ural::make_istream_sequence<int>(is),
                      ural::make_ostream_sequence(os, ' '));

    BOOST_CHECK_EQUAL(z.str(), os.str());
}

// 25.3.10 Обращение
BOOST_AUTO_TEST_CASE(reverse_forward_test)
{
    std::vector<int> const src = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};

    for(auto n : ural::numbers(0, src.size() + 1))
    {
        BOOST_CHECK_LE(n, src.size());

        std::list<int> c_std(src.begin(), src.begin() + n);
        std::forward_list<int> c_ural(c_std.begin(), c_std.end());

        std::reverse(c_std.begin(), c_std.end());

        auto const result = ural::reverse(c_ural);

        BOOST_CHECK_EQUAL_COLLECTIONS(c_std.begin(), c_std.end(),
                                      c_ural.begin(), c_ural.end());

        BOOST_CHECK(result.traversed_begin() == c_ural.begin());
        BOOST_CHECK(result.begin() == c_ural.end());
        BOOST_CHECK(result.end() == c_ural.end());
    }
}
BOOST_AUTO_TEST_CASE(reverse_test)
{
    std::list<int> const src = {1, 2, 3, 4, 5, 6};

    auto x_std = src;
    auto x_ural = src;

    std::reverse(x_std.begin(), x_std.end());
    auto const result = ural::reverse(x_ural);

    BOOST_CHECK_EQUAL_COLLECTIONS(x_std.begin(), x_std.end(),
                                  x_ural.begin(), x_ural.end());

    BOOST_CHECK(result.traversed_begin() == x_ural.begin());
    BOOST_CHECK(result.begin() == x_ural.end());
    BOOST_CHECK(result.end() == x_ural.end());
    BOOST_CHECK(result.traversed_end() == x_ural.end());
}

BOOST_AUTO_TEST_CASE(reversed_reverse_test)
{
    std::list<int> const src = {1, 2, 3, 4, 5, 6};

    auto x_std = src;
    auto x_ural = src;

    std::reverse(x_std.begin(), x_std.end());
    ural::reverse(x_ural | ural::reversed);

    BOOST_CHECK_EQUAL_COLLECTIONS(x_std.begin(), x_std.end(),
                                  x_ural.begin(), x_ural.end());
}

BOOST_AUTO_TEST_CASE(reverse_copy_test)
{
    // Исходные данные
    std::list<int> const src = {1, 2, 3, 4, 5, 6};

    // std
    std::list<int> r_std;
    std::reverse_copy(src.begin(), src.end(), std::back_inserter(r_std));

    // ural
    std::list<int> r_ural;
    ural::reverse_copy(src, r_ural | ural::back_inserter);

    // Проверка
    BOOST_CHECK_EQUAL_COLLECTIONS(r_std.begin(), r_std.end(),
                                  r_ural.begin(), r_ural.end());
}

// 25.3.11 Вращение
BOOST_AUTO_TEST_CASE(rotate_test)
{
    std::vector<int> const v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    for(auto i : ural::indices_of(v))
    {
        auto v_std = v;
        auto v_ural = v;

        std::rotate(v_std.begin(), v_std.begin() + i, v_std.end());

        auto s = ural::sequence(v_ural);
        s += i;

        auto r_ural = ural::rotate(s);

        if(i == 0)
        {
            size_t const n = ural::size(r_ural);
            BOOST_CHECK(0U == n || v.size() == n);
            BOOST_CHECK_EQUAL(v.size(), n + ural::size(r_ural.traversed_front()));
        }
        else
        {
            BOOST_CHECK_EQUAL(ural::to_signed(i), ural::size(r_ural));

            BOOST_CHECK_EQUAL(ural::to_signed(v.size() - i),
                              ural::size(r_ural.traversed_front()));
        }

        BOOST_CHECK_EQUAL_COLLECTIONS(v_std.begin(), v_std.end(),
                                      v_ural.begin(), v_ural.end());
        ural::rotate(r_ural);

        BOOST_CHECK_EQUAL_COLLECTIONS(v.begin(), v.end(),
                                      v_ural.begin(), v_ural.end());
    }
}

BOOST_AUTO_TEST_CASE(rotate_copy_test)
{
    std::vector<int> const src{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    for(auto i : ural::indices_of(src))
    {
        std::forward_list<int> r_std;
        std::forward_list<int> r_ural;

        std::rotate_copy(src.begin(), src.begin() + i, src.end(),
                         r_std | ural::front_inserter);

        auto s = ural::sequence(src);
        s += i;

        ural::rotate_copy(s, r_ural | ural::front_inserter);

        BOOST_CHECK_EQUAL_COLLECTIONS(r_std.begin(), r_std.end(),
                                      r_ural.begin(), r_ural.end());
    }
}

BOOST_AUTO_TEST_CASE(rotate_copy_return_test)
{
    std::vector<int> const src{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    for(auto i : ural::indices_of(src))
    {
        std::vector<int> d_std(src.size() + 5);
        std::vector<int> d_ural(src.size() + 5);

        std::rotate_copy(src.begin(), src.begin() + i, src.end(), d_std.begin());

        auto s = ural::sequence(src);
        s += i;

        auto r_ural = ural::rotate_copy(s, d_ural);

        BOOST_CHECK_EQUAL(ural::to_signed(src.size()),
                          r_ural[ural::_2].traversed_front().size());
        BOOST_CHECK_EQUAL(ural::to_signed(d_ural.size() - src.size()),
                          r_ural[ural::_2].size());

        BOOST_CHECK_EQUAL(s.size(), r_ural[ural::_1].traversed_front().size());
        BOOST_CHECK_EQUAL(s.traversed_front().size(), r_ural[ural::_1].size());

        BOOST_CHECK_EQUAL_COLLECTIONS(d_std.begin(), d_std.end(),
                                      d_ural.begin(), d_ural.end());
    }
}

// 25.3.12 Тусовка
BOOST_AUTO_TEST_CASE(shuffle_test)
{
    auto v = ural::numbers(1, 10) | ural::to_container<std::vector>{};

    auto const v_old = v;

    auto const result = ural::shuffle(v, ural_test::random_engine());

    BOOST_CHECK(result.traversed_front().begin() == v.begin());
    BOOST_CHECK(result.traversed_front().end() == v.end());
    BOOST_CHECK(result.begin() == v.end());
    BOOST_CHECK(result.end() == v.end());

    BOOST_CHECK_EQUAL(v.size(), v_old.size());

    BOOST_CHECK(std::is_permutation(v.begin(), v.end(), v_old.begin()));

    BOOST_CHECK(ural::is_permutation(v, v_old));

    // @todo тест того, что перестановки случайные
}

BOOST_AUTO_TEST_CASE(random_shuffle_test)
{
    auto v = ural::numbers(1, 10) | ural::to_container<std::vector>{};

    auto const v_old = v;

    auto const result = ural::random_shuffle(v);

    BOOST_CHECK(result.traversed_front().begin() == v.begin());
    BOOST_CHECK(result.traversed_front().end() == v.end());
    BOOST_CHECK(result.begin() == v.end());
    BOOST_CHECK(result.end() == v.end());

    BOOST_CHECK_EQUAL(v.size(), v_old.size());

    BOOST_CHECK(std::is_permutation(v.begin(), v.end(), v_old.begin()));

    BOOST_CHECK(ural::is_permutation(v, v_old));

    // @todo тест того, что перестановки случайные
}

// 25.3.13 Разделение
BOOST_AUTO_TEST_CASE(is_partitioned_test)
{
    std::vector<int> v = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    auto is_even = [](int i){ return i % 2 == 0; };

    BOOST_CHECK_EQUAL(std::is_partitioned(v.begin(), v.end(), is_even),
                      ural::is_partitioned(v, is_even));
    BOOST_CHECK_EQUAL(false, ural::is_partitioned(v, is_even));

    std::partition(v.begin(), v.end(), is_even);

    BOOST_CHECK_EQUAL(std::is_partitioned(v.begin(), v.end(), is_even),
                      ural::is_partitioned(v, is_even));
    BOOST_CHECK_EQUAL(true, ural::is_partitioned(v, is_even));

    std::reverse(v.begin(), v.end());

    BOOST_CHECK_EQUAL(std::is_partitioned(v.begin(), v.end(), is_even),
                      ural::is_partitioned(v, is_even));
    BOOST_CHECK_EQUAL(false, ural::is_partitioned(v, is_even));
}

BOOST_AUTO_TEST_CASE(partition_test)
{
    // Подготовка
    typedef std::forward_list<int> Container;
    Container const xs = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto ys = xs;

    typedef ::ural::ValueType<Container> Element;

    auto const is_even = [](Element x) { return x % 2 == 0;};

    // Выполнение операции
    auto r_ural = ural::partition(ys, is_even);

    // Проверка
    BOOST_CHECK(r_ural.original() == ural::sequence(ys));

    BOOST_CHECK(ural::is_permutation(ys, xs));
    BOOST_CHECK(ural::is_partitioned(ys, is_even));

    BOOST_CHECK(::ural::all_of(r_ural.traversed_front(), is_even));
    BOOST_CHECK(std::all_of(r_ural.traversed_begin(), r_ural.begin(), is_even));

    BOOST_CHECK(::ural::none_of(ural::shrink_front(r_ural), is_even));
    BOOST_CHECK(std::none_of(r_ural.begin(), r_ural.end(), is_even));
}

BOOST_AUTO_TEST_CASE(stable_partition_test_empty)
{
    std::vector<int> const src {};
    auto v_std = src;
    auto v_ural = src;

    auto const pred = [](int n){return n % 2 == 0;};

    std::stable_partition(v_std.begin(), v_std.end(), pred);
    auto r_ural = ural::stable_partition(v_ural, pred);

    BOOST_CHECK_EQUAL_COLLECTIONS(v_std.begin(), v_std.end(),
                                  v_ural.begin(), v_ural.end());

    BOOST_CHECK(::ural::all_of(r_ural.traversed_front(), pred));
    BOOST_CHECK(std::all_of(r_ural.traversed_begin(), r_ural.begin(), pred));

    BOOST_CHECK(::ural::none_of(ural::shrink_front(r_ural), pred));
    BOOST_CHECK(std::none_of(r_ural.begin(), r_ural.end(), pred));
    BOOST_CHECK(std::none_of(r_ural.begin(), r_ural.traversed_end(), pred));
}

BOOST_AUTO_TEST_CASE(stable_partition_test_1_2)
{
    std::vector<int> const src {1, 2};
    auto v_std = src;
    auto v_ural = src;

    auto const pred = [](int n){return n % 2 == 0;};

    std::stable_partition(v_std.begin(), v_std.end(), pred);
    auto r_ural = ural::stable_partition(v_ural, pred);

    BOOST_CHECK_EQUAL_COLLECTIONS(v_std.begin(), v_std.end(),
                                  v_ural.begin(), v_ural.end());

    BOOST_CHECK(::ural::all_of(r_ural.traversed_front(), pred));
    BOOST_CHECK(std::all_of(r_ural.traversed_begin(), r_ural.begin(), pred));

    BOOST_CHECK(::ural::none_of(ural::shrink_front(r_ural), pred));
    BOOST_CHECK(std::none_of(r_ural.begin(), r_ural.end(), pred));
    BOOST_CHECK(std::none_of(r_ural.begin(), r_ural.traversed_end(), pred));
}

BOOST_AUTO_TEST_CASE(stable_partition_test_2_1)
{
    std::vector<int> const src {2, 1};
    auto v_std = src;
    auto v_ural = src;

    auto const pred = [](int n){return n % 2 == 0;};

    std::stable_partition(v_std.begin(), v_std.end(), pred);
    auto r_ural = ural::stable_partition(v_ural, pred);

    BOOST_CHECK_EQUAL_COLLECTIONS(v_std.begin(), v_std.end(),
                                  v_ural.begin(), v_ural.end());

    BOOST_CHECK(::ural::all_of(r_ural.traversed_front(), pred));
    BOOST_CHECK(std::all_of(r_ural.traversed_begin(), r_ural.begin(), pred));

    BOOST_CHECK(::ural::none_of(ural::shrink_front(r_ural), pred));
    BOOST_CHECK(std::none_of(r_ural.begin(), r_ural.end(), pred));
    BOOST_CHECK(std::none_of(r_ural.begin(), r_ural.traversed_end(), pred));
}

BOOST_AUTO_TEST_CASE(stable_partition_test_3)
{
    std::vector<int> const src {1, 2, 3};
    auto v_std = src;
    auto v_ural = src;

    auto const pred = [](int n){return n % 2 == 0;};

    std::stable_partition(v_std.begin(), v_std.end(), pred);
    auto r_ural = ural::stable_partition(v_ural, pred);

    BOOST_CHECK_EQUAL_COLLECTIONS(v_std.begin(), v_std.end(),
                                  v_ural.begin(), v_ural.end());

    BOOST_CHECK(::ural::all_of(r_ural.traversed_front(), pred));
    BOOST_CHECK(std::all_of(r_ural.traversed_begin(), r_ural.begin(), pred));

    BOOST_CHECK(::ural::none_of(ural::shrink_front(r_ural), pred));
    BOOST_CHECK(std::none_of(r_ural.begin(), r_ural.end(), pred));
    BOOST_CHECK(std::none_of(r_ural.begin(), r_ural.traversed_end(), pred));
}

BOOST_AUTO_TEST_CASE(stable_partition_test_4)
{
    std::vector<int> const src {1, 2, 3, 4};
    auto v_std = src;
    auto v_ural = src;

    auto const pred = [](int n){return n % 2 == 0;};

    std::stable_partition(v_std.begin(), v_std.end(), pred);
    auto r_ural = ural::stable_partition(v_ural, pred);

    BOOST_CHECK_EQUAL_COLLECTIONS(v_std.begin(), v_std.end(),
                                  v_ural.begin(), v_ural.end());

    BOOST_CHECK(::ural::all_of(r_ural.traversed_front(), pred));
    BOOST_CHECK(std::all_of(r_ural.traversed_begin(), r_ural.begin(), pred));

    BOOST_CHECK(::ural::none_of(ural::shrink_front(r_ural), pred));
    BOOST_CHECK(std::none_of(r_ural.begin(), r_ural.end(), pred));
    BOOST_CHECK(std::none_of(r_ural.begin(), r_ural.traversed_end(), pred));
}

BOOST_AUTO_TEST_CASE(stable_partition_test_9)
{
    std::list<int> const src {1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto v_std = src;
    auto v_ural = src;

    auto const pred = [](int n){return n % 2 == 0;};

    std::stable_partition(v_std.begin(), v_std.end(), pred);
    auto r_ural = ural::stable_partition(v_ural, pred);

    BOOST_CHECK(r_ural.original() == ural::sequence(v_ural));
    BOOST_CHECK(!r_ural.traversed_back());

    BOOST_CHECK(ural::is_permutation(v_ural, src));

    BOOST_CHECK_EQUAL_COLLECTIONS(v_std.begin(), v_std.end(),
                                  v_ural.begin(), v_ural.end());

    BOOST_CHECK(::ural::all_of(r_ural.traversed_front(), pred));
    BOOST_CHECK(std::all_of(r_ural.traversed_begin(), r_ural.begin(), pred));

    BOOST_CHECK(::ural::none_of(ural::shrink_front(r_ural), pred));
    BOOST_CHECK(std::none_of(r_ural.begin(), r_ural.end(), pred));
    BOOST_CHECK(std::none_of(r_ural.begin(), r_ural.traversed_end(), pred));
}

BOOST_AUTO_TEST_CASE(partition_copy_test)
{
    // Подготовка
    std::vector<int> xs = {1,2,3,4,5,6,7,8,9,10};
    ural_test::istringstream_helper<int> src(xs.begin(), xs.end());
    std::list<int> true_sink;
    std::forward_list<int> false_sink;

    auto const pred = [] (int x) {return x % 2 == 0;};

    // Выполнение операции
    ural::partition_copy(src, true_sink | ural::back_inserter,
                         std::front_inserter(false_sink), pred);

    // Проверка
    BOOST_CHECK(ural::all_of(true_sink, pred));
    BOOST_CHECK(ural::none_of(false_sink, pred));

    for(auto const & x : xs)
    {
        BOOST_CHECK(!!ural::find(true_sink, x) || !!ural::find(false_sink, x));
    }
}

BOOST_AUTO_TEST_CASE(partition_copy_return_value_test)
{
    // Подготовка
    std::array<int, 10> const src = {1,2,3,4,5,6,7,8,9,10};

    std::vector<int> true_sink(src.size(), -1);
    auto const true_sink_old = true_sink;

    std::vector<int> false_sink(src.size(), -1);
    auto const false_sink_old = false_sink;

    auto const pred = [] (int x) {return x % 2 == 0;};

    // Выполнение операции
    auto r = ural::partition_copy(src, true_sink, false_sink, pred);

    // Проверки
    assert(src.size() < true_sink.size() + false_sink.size());
    BOOST_CHECK(!r[ural::_1]);
    BOOST_CHECK_EQUAL(ural::to_signed(src.size()),
                      r[ural::_2].traversed_front().size()
                      + r[ural::_3].traversed_front().size());

    BOOST_CHECK(ural::all_of(r[ural::_2].traversed_front(), pred));
    BOOST_CHECK(ural::none_of(r[ural::_3].traversed_front(), pred));

    std::vector<int> y;
    ural::copy(r[ural::_2].traversed_front(), y | ural::back_inserter);
    ural::copy(r[ural::_3].traversed_front(), y | ural::back_inserter);

    BOOST_CHECK(ural::is_permutation(y, src));

    BOOST_CHECK_EQUAL_COLLECTIONS(true_sink.end() - r[ural::_2].size(),
                                  true_sink.end(),
                                  true_sink_old.end() - r[ural::_2].size(),
                                  true_sink_old.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(false_sink.end() - r[ural::_2].size(),
                                  false_sink.end(),
                                  false_sink.end() - r[ural::_2].size(),
                                  false_sink.end());
}

BOOST_AUTO_TEST_CASE(partition_point_test)
{
    typedef int Value;

    auto pred = [](Value x) { return x < 5; };

    std::vector<Value> const z = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    assert(std::is_partitioned(z.begin(), z.end(), pred));

    auto r_std = std::partition_point(z.begin(), z.end(), pred);
    auto r_ural = ural::partition_point(z, pred);

    BOOST_CHECK(z.begin() == r_ural.traversed_begin());
    BOOST_CHECK(z.end() == r_ural.end());
    BOOST_CHECK(z.end() == r_ural.traversed_end());
    BOOST_CHECK(r_std == r_ural.begin());
}

// 25.4 Сортировка и связанные с ней операции
// 25.4.1 Сортировка
BOOST_AUTO_TEST_CASE(sort_test)
{
    std::vector<int> x_std = {5, 7, 4, 2, 8, 6, 1, 9, 0, 3};
    auto x_ural = x_std;

    std::sort(x_std.begin(), x_std.end());
    auto const result = ural::sort(x_ural);

    BOOST_CHECK_EQUAL_COLLECTIONS(x_std.begin(), x_std.end(),
                                  x_ural.begin(), x_ural.end());

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

    BOOST_CHECK_EQUAL_COLLECTIONS(x_std.begin(), x_std.end(),
                                  x_ural.begin(), x_ural.end());

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

    BOOST_CHECK_EQUAL_COLLECTIONS(r1_std.begin(), r1_std.end(),
                                  r1_ural.begin(), r1_ural.end());
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

    BOOST_CHECK_EQUAL_COLLECTIONS(r2_std.begin(), r2_std.end(),
                                  r2_ural.begin(), r2_ural.end());
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
    BOOST_CHECK_EQUAL_COLLECTIONS(std_merge.begin(), std_merge.end(),
                                  ural_merge.begin(), ural_merge.end());
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
    BOOST_CHECK_EQUAL_COLLECTIONS(ural_merge.begin(), ural_merge.end(),
                                  std_merge.begin(), std_merge.end());
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
    BOOST_CHECK_EQUAL_COLLECTIONS(std_merge.begin(), std_merge.end(),
                                  ural_merge.begin(), ural_merge.end());
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

    BOOST_CHECK_EQUAL_COLLECTIONS(x_std.begin(), x_std.end(),
                                  x_ural.begin(), x_ural.end());

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

    BOOST_CHECK_EQUAL_COLLECTIONS(x_std.begin(), x_std.end(),
                                  x_ural.begin(), x_ural.end());
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

    BOOST_CHECK_EQUAL_COLLECTIONS(x_std.begin(), x_std.end(),
                                  x_ural.begin(), x_ural.end());
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

    BOOST_CHECK_EQUAL_COLLECTIONS(x_std.begin(), x_std.end(),
                                  x_ural.begin(), x_ural.end());
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

    BOOST_CHECK_EQUAL_COLLECTIONS(x_std.begin(), x_std.end(),
                                  x_ural.begin(), x_ural.end());
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

    BOOST_CHECK_EQUAL_COLLECTIONS(x_std.begin(), x_std.end(),
                                  x_ural.begin(), x_ural.end());
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

    BOOST_CHECK_EQUAL_COLLECTIONS(x_std.begin(), x_std.end(),
                                  x_ural.begin(), x_ural.end());
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

    BOOST_CHECK_EQUAL_COLLECTIONS(z.begin(), z.end(),
                                  r_ural.begin(), r_ural.end());
}

BOOST_AUTO_TEST_CASE(set_union_test_shorter_in_1)
{
    std::vector<int> const is1{1, 2, 3, 4, 5,};
    std::vector<int> const is2{      3, 4, 5, 6, 7};
    std::vector<int> const z  {1, 2, 3, 4, 5, 6, 7};

    BOOST_CHECK_LE(is1.back(), is2.back());

    std::vector<int> r_ural;
    auto result = ural::set_union(is1, is2, r_ural | ural::back_inserter);

    BOOST_CHECK_EQUAL_COLLECTIONS(z.begin(), z.end(),
                                  r_ural.begin(), r_ural.end());

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

    BOOST_CHECK_EQUAL_COLLECTIONS(z.begin(), z.end(),
                                  r_ural.begin(), r_ural.end());

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

    BOOST_CHECK_EQUAL_COLLECTIONS(z.begin(), z.end(),
                                  r_ural.begin(), r_ural.end());
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

    BOOST_CHECK_EQUAL_COLLECTIONS(z.begin(), z.end(),
                                  r_ural.begin(), r_ural.end());

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

    BOOST_CHECK_EQUAL_COLLECTIONS(z.begin(), z.end(),
                                  r_ural.begin(), r_ural.end());

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

    BOOST_CHECK_EQUAL_COLLECTIONS(r_ural.begin(), r_ural.end(),
                                  r_std.begin(), r_std.end());
}

BOOST_AUTO_TEST_CASE(set_difference_test)
{
    ural_test::istringstream_helper<int> is1{1, 2, 3, 4, 5,      8};
    ural_test::istringstream_helper<int> is2{   2,    4, 5, 6, 7  };
    std::vector<int> const z                {1,    3,            8};

    std::vector<int> r_ural;
    ural::set_difference(is1, is2, r_ural | ural::back_inserter);

    BOOST_CHECK_EQUAL_COLLECTIONS(z.begin(), z.end(),
                                  r_ural.begin(), r_ural.end());
}

BOOST_AUTO_TEST_CASE(set_difference_test_unexhausted_2)
{
    ural_test::istringstream_helper<int> is1{1, 2, 3, 4, 5,      8};
    ural_test::istringstream_helper<int> is2{   2,    4, 5, 6, 7,  9};
    std::vector<int> const z                {1,    3,            8};

    std::vector<int> r_ural;
    ural::set_difference(is1, is2, r_ural | ural::back_inserter);

    BOOST_CHECK_EQUAL_COLLECTIONS(z.begin(), z.end(),
                                  r_ural.begin(), r_ural.end());
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
    BOOST_CHECK_EQUAL_COLLECTIONS(r_ural.begin(), r_ural.end(),
                                  r_std.begin(), r_std.end());
}

BOOST_AUTO_TEST_CASE(set_symmetric_difference_test)
{
    using Source = ural_test::istringstream_helper<int>;

    Source x1          {1, 2, 3, 4, 5, 6, 7, 8       };
    Source x2          {            5,    7,    9, 10};
    std::vector<int> const z {1, 2, 3, 4,    6,    8, 9, 10};

    // через back_inserter
    std::vector<int> r_ural;
    ural::set_symmetric_difference(x1, x2, r_ural | ural::back_inserter);

    BOOST_CHECK_EQUAL_COLLECTIONS(z.begin(), z.end(),
                                  r_ural.begin(), r_ural.end());
}

BOOST_AUTO_TEST_CASE(set_symmetric_difference_regression)
{
    std::vector<int> const x1{1, 2, 3, 4, 5,       8};
    std::vector<int> const x2{   2,    4, 5, 6, 7   };
    std::vector<int> const z {1,    3,       6, 7, 8};

    BOOST_CHECK_GE(x1.back(), x2.back());

    std::vector<int> r_ural;
    ural::set_symmetric_difference(x1, x2, r_ural | ural::back_inserter);

    BOOST_CHECK_EQUAL_COLLECTIONS(z.begin(), z.end(),
                                  r_ural.begin(), r_ural.end());
}

BOOST_AUTO_TEST_CASE(set_symmetric_difference_from_istream)
{
    using Source = ural_test::istringstream_helper<int>;

    Source x1                {1, 2, 3, 4, 5, 6, 7, 8       };
    Source x2                {            5,    7,    9, 10};
    std::vector<int> const z {1, 2, 3, 4,    6,    8, 9, 10};

    std::vector<int> r_ural;
    ural::set_symmetric_difference(x1, x2, r_ural | ural::back_inserter);

    BOOST_CHECK_EQUAL_COLLECTIONS(z.begin(), z.end(),
                                  r_ural.begin(), r_ural.end());
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
    BOOST_CHECK_EQUAL_COLLECTIONS(r_ural.begin(), r_ural.end(),
                                  r_std.begin(), r_std.end());
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

    while(!ural::empty(v))
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

// Комбинирование вызовов модифицирующих алгоритмов с erase
BOOST_AUTO_TEST_CASE(unqiue_fn_const_iterator_test)
{
    ural::vector<int> v1 = {1, 2, 3, 4, 5, 6};
    auto v2 = v1;

    auto const n = v1.size() / 2;

    v2.erase(v2.cbegin() + n, v2.cend());

    ural::erase(v1, ural::make_iterator_sequence(v1.cbegin() + n, v1.cend()));

    BOOST_CHECK_EQUAL_COLLECTIONS(v1.begin(), v1.end(), v2.begin(), v2.end());
}

BOOST_AUTO_TEST_CASE(unique_erase_combination_test)
{
    std::vector<int> v_std{1,2,3,1,2,3,3,4,5,4,5,6,7};
    std::sort(v_std.begin(), v_std.end());

    auto v_ural = v_std;

    // std
    auto const last = std::unique(v_std.begin(), v_std.end());
    v_std.erase(last, v_std.end());

    // ural
    auto const to_erase = ural::unique(v_ural);
    ural::erase(v_ural, to_erase);

    // Сравнение результатов
    BOOST_CHECK_EQUAL_COLLECTIONS(v_std.begin(), v_std.end(),
                                  v_ural.begin(), v_ural.end());
}

BOOST_AUTO_TEST_CASE(unique_erase_test)
{
    std::vector<int> v_std{1,2,3,1,2,3,3,4,5,4,5,6,7};
    std::sort(v_std.begin(), v_std.end());

    auto v_ural = v_std;

    // std
    auto const last = std::unique(v_std.begin(), v_std.end());
    v_std.erase(last, v_std.end());

    // ural
    ural::unique_erase(v_ural);

    // Сравнение результатов
    BOOST_CHECK_EQUAL_COLLECTIONS(v_std.begin(), v_std.end(),
                                  v_ural.begin(), v_ural.end());
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
