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

#include "../rnd.hpp"
#include "../defs.hpp"

#include <ural/sequence/all.hpp>
#include <ural/math/rational.hpp>
#include <ural/numeric/numbers_sequence.hpp>

#include <forward_list>
#include <list>
#include <vector>

#include <boost/test/unit_test.hpp>
#include <boost/mpl/list.hpp>

namespace ural_ex = ural::experimental;

namespace
{
    typedef boost::mpl::list<std::forward_list<int>,
                             std::list<int>,
                             std::vector<int>,
                             ural_test::istringstream_helper<int>> Sources;
}

// 25.3 Модифицирующие последовательность алгоритмы
// 25.3.1 Копирование
BOOST_AUTO_TEST_CASE(copy_test)
{
    std::vector<int> const src = {1, 2, 3, 4};
    ural_test::istringstream_helper<int> const xs(src);

    std::vector<int> x1;

    ural::copy(xs, std::back_inserter(x1));

    URAL_CHECK_EQUAL_RANGES(src, x1);
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
    ural_test::istringstream_helper<char> const src(str);
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
    ural_test::istringstream_helper<Type> const src_ural(src_std);

    auto const pred = [](Type i){return !(i<0);};

    std::vector<Type> r_std;
    std::copy_if(src_std.begin(), src_std.end(), std::back_inserter(r_std) , pred);

    std::vector<Type> r_ural;
    ural::copy_if(src_ural, r_ural | ural::back_inserter, pred);

    URAL_CHECK_EQUAL_RANGES(r_std, r_ural);
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

    URAL_CHECK_EQUAL_RANGES(r_std, r_ural);
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
    URAL_CHECK_EQUAL_RANGES(r_ural, r_std);

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

    auto src = ural::make_iterator_cursor(x_ural.begin(), x_ural.end() - 1);
    ural::copy_backward(src, x_ural);

    URAL_CHECK_EQUAL_RANGES(x_std, x_ural);
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
BOOST_AUTO_TEST_CASE(move_test_minimal)
{
    std::vector<std::string> const src_old = {"Alpha", "Beta", "Gamma"};
    ural_test::istringstream_helper<std::string> const src(src_old);

    std::vector<std::string> xs;

    ural::move(src, xs | ural::back_inserter);

    URAL_CHECK_EQUAL_RANGES(xs, src_old);
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

    auto src = ural::make_iterator_cursor(x_ural.begin(), x_ural.end() - 1);
    ural::move_backward(src, x_ural);

    URAL_CHECK_EQUAL_RANGES(x_std, x_ural);
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
    std::string str("hello, world");
    std::istringstream is(str);

    auto f = std::ptr_fun<int, int>(std::toupper);

    std::string x_std;
    std::transform(str.begin(), str.end(), std::back_inserter(x_std), f);

    std::string x_ural;
    auto result = ural::transform(is, x_ural | ural::back_inserter, f);

    BOOST_CHECK(!result[ural::_1]);
    BOOST_CHECK(!!result[ural::_2]);

    URAL_CHECK_EQUAL_RANGES(x_std, x_ural);
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

BOOST_AUTO_TEST_CASE(transform_2_test_shorter_in1_minimal)
{
    std::vector<int> const src1 = {1, 20, 30, 40};
    std::vector<int> const src2 = {10, 2, 30, 4, 5};

    BOOST_CHECK_LE(src1.size(), src2.size());

    ural_test::istringstream_helper<int> const x1(src1);
    ural_test::istringstream_helper<int> const x2(src2);

    std::vector<bool> z_std;
    std::vector<bool> z_ural;

    std::less_equal<int> constexpr f_std{};
    ural::less_equal<> constexpr f_ural{};

    std::transform(src1.begin(), src1.end(), src2.begin(),
                   std::back_inserter(z_std), f_std);

    ural::transform(x1, x2, std::back_inserter(z_ural), f_ural);

    URAL_CHECK_EQUAL_RANGES(z_std, z_ural);
}

BOOST_AUTO_TEST_CASE(transform_2_test_shorter_in1)
{
    std::vector<int> const src1 = {1, 20, 30, 40};
    std::vector<int> const src2 = {10, 2, 30, 4, 5};

    BOOST_CHECK_LE(src1.size(), src2.size());

    std::vector<bool> z_std(src2.size() + src1.size() + 1, -1);
    auto z_ural = z_std;

    std::less_equal<int> constexpr f_std{};
    ural::less_equal<> constexpr f_ural{};

    // Выполнение алгоритмов
    auto const r_std = std::transform(src1.begin(), src1.end(), src2.begin(),
                                      z_std.begin(), f_std);

    auto const r_ural = ural::transform(src1, src2, z_ural, f_ural);

    // Проверки
    URAL_CHECK_EQUAL_RANGES(z_std, z_ural);

    BOOST_CHECK(r_ural[ural::_1].original() == ural::cursor(src1));
    BOOST_CHECK(r_ural[ural::_1].traversed_front() == ural::cursor(src1));

    BOOST_CHECK(r_ural[ural::_2].original() == ural::cursor(src2));
    BOOST_CHECK(r_ural[ural::_2].begin() == src2.begin() + src1.size());
    BOOST_CHECK(!r_ural[ural::_2].traversed_back());

    BOOST_CHECK(r_ural[ural::_3].original() == ural::cursor(z_ural));
    BOOST_CHECK(r_ural[ural::_3].begin()
                == z_ural.begin() + (r_std - z_std.begin()));
    BOOST_CHECK(!r_ural[ural::_3].traversed_back());
}

BOOST_AUTO_TEST_CASE(transform_2_test_shorter_in2)
{
    std::vector<int> const src1 = {1, 20, 30, 40, 50};
    std::vector<int> const src2 = {10, 2, 30, 4};

    BOOST_CHECK_GE(src1.size(), src2.size());

    std::vector<bool> z_std(src2.size() + src1.size() + 1, -1);
    auto z_ural = z_std;

    std::less_equal<int> constexpr f_std{};
    ural::less_equal<> constexpr f_ural{};

    // std ограничивает по первой последовательности
    auto const r_std = std::transform(src1.begin(),
                                      std::next(src1.begin(), src2.size()),
                                      src2.begin(), z_std.begin(), f_std);

    auto const r_ural = ural::transform(src1, src2, z_ural, f_ural);

    URAL_CHECK_EQUAL_RANGES(z_std, z_ural);

    BOOST_CHECK(r_ural[ural::_1].original() == ural::cursor(src1));
    BOOST_CHECK(r_ural[ural::_1].begin() == src1.begin() + src2.size());
    BOOST_CHECK(!r_ural[ural::_1].traversed_back());

    BOOST_CHECK(r_ural[ural::_2].original() == ural::cursor(src2));
    BOOST_CHECK(r_ural[ural::_2].traversed_front() == ural::cursor(src2));

    BOOST_CHECK(r_ural[ural::_3].original() == ural::cursor(z_ural));
    BOOST_CHECK(r_ural[ural::_3].begin()
                == z_ural.begin() + (r_std - z_std.begin()));
    BOOST_CHECK(!r_ural[ural::_3].traversed_back());
}

BOOST_AUTO_TEST_CASE(transform_2_test_shorter_out)
{
    std::vector<int> const src1 = {1, 20, 30, 40};
    std::vector<int> const src2 = {10, 2, 30, 4, 5};

    BOOST_CHECK_LE(src1.size(), src2.size());

    std::vector<int> z_std;
    std::vector<int> z_ural(std::min(src1.size(), src2.size()) - 1, -1);

    std::plus<int> constexpr f_std{};
    ural::plus<> constexpr f_ural{};

    std::transform(src1.begin(), src1.end(), src2.begin(),
                   std::back_inserter(z_std), f_std);

    auto const r_ural = ural::transform(src1, src2, z_ural, f_ural);

    BOOST_CHECK_EQUAL_COLLECTIONS(z_std.begin(), z_std.begin() + z_ural.size(),
                                  z_ural.begin(), z_ural.end());

    BOOST_CHECK(r_ural[ural::_1].original() == ural::cursor(src1));
    BOOST_CHECK(r_ural[ural::_1].begin() == src1.begin() + z_ural.size());
    BOOST_CHECK(!r_ural[ural::_1].traversed_back());

    BOOST_CHECK(r_ural[ural::_2].original() == ural::cursor(src2));
    BOOST_CHECK(r_ural[ural::_2].begin() == src2.begin() + z_ural.size());
    BOOST_CHECK(!r_ural[ural::_2].traversed_back());

    BOOST_CHECK(r_ural[ural::_3].original() == ural::cursor(z_ural));
    BOOST_CHECK(r_ural[ural::_3].traversed_front() == ural::cursor(z_ural));
}

// 25.3.5 Замена
BOOST_AUTO_TEST_CASE(replace_test_different_types)
{
    // Подготовка
    std::vector<int> s_std = {5, 7, 4, 2, 8, 6, 1, 9, 0, 3};
    std::forward_list<int> s_ural(s_std.begin(), s_std.end());

    auto const old_value = ural_ex::rational<int>(8);
    auto const new_value = 88;

    BOOST_CHECK_EQUAL(old_value.denominator(), 1);

    // Выполнение алгоритмов
    std::replace(s_std.begin(), s_std.end(), old_value.numerator(), new_value);

    auto const r_ural = ural::replace(s_ural, old_value, new_value);

    // Проверки
    BOOST_CHECK(r_ural.original() == ural::cursor(s_ural));
    BOOST_CHECK(r_ural.traversed_front() == ural::cursor(s_ural));

    URAL_CHECK_EQUAL_RANGES(s_std, s_ural);
}

BOOST_AUTO_TEST_CASE(replace_if_test)
{
    // Подготовка
    std::array<int, 10> const s{5, 7, 4, 2, 8, 6, 1, 9, 0, 3};

    auto x_std = s;
    std::forward_list<int> x_ural(s.begin(), s.end());

    auto pred = [](int x) {return x < 5;};
    auto const new_value = 55;

    // Выполнение алгоритмов
    std::replace_if(x_std.begin(), x_std.end(), pred, new_value);
    auto const r_ural = ural::replace_if(x_ural, pred, new_value);

    // Проверки
    BOOST_CHECK(r_ural.original() == ural::cursor(x_ural));
    BOOST_CHECK(r_ural.traversed_front() == ural::cursor(x_ural));

    URAL_CHECK_EQUAL_RANGES(x_std, x_ural);
}

BOOST_AUTO_TEST_CASE(replace_copy_test_minimalistic)
{
    // Исходные данные
    typedef ural_test::istringstream_helper<int> const Source;
    std::vector<int> const src_std = {5, 7, 4, 2, 8, 6, 1, 9, 0, 3};
    Source src_ural(src_std.begin(), src_std.end());

    auto const old_value = 8;
    auto const new_value = 88;

    // std
    std::vector<int> s_std;
    std::replace_copy(src_std.begin(), src_std.end(), std::back_inserter(s_std),
                      old_value, new_value);

    // ural
    std::vector<int> s_ural;
    ural::replace_copy(src_ural, s_ural | ural::back_inserter,
                       old_value, new_value);

    // Проверка
    URAL_CHECK_EQUAL_RANGES(s_std, s_ural);
}

BOOST_AUTO_TEST_CASE(replace_copy_test_to_longer)
{
    // Исходные данные
    std::vector<int> const source = {5, 7, 4, 2, 8, 6, 1, 9, 0, 3};
    auto const old_value = 8;
    auto const new_value = 88;

    // std
    std::vector<int> s_std(source.size() + 5, -1);
    auto const r_std = std::replace_copy(source.begin(), source.end(),
                                         s_std.begin(), old_value, new_value);

    // ural
    std::vector<int> s_ural(source.size() + 5, -1);
    auto const r_ural = ural::replace_copy(source, s_ural,
                                           old_value, new_value);

    // Проверка
    assert(s_ural.size() > source.size());

    URAL_CHECK_EQUAL_RANGES(s_std, s_ural);

    BOOST_CHECK(r_ural[ural::_1].original() == ural::cursor(source));
    BOOST_CHECK(r_ural[ural::_1].traversed_front() == ural::cursor(source));

    BOOST_CHECK(r_ural[ural::_2].original() == ural::cursor(s_ural));
    BOOST_CHECK(!r_ural[ural::_2].traversed_back());
    BOOST_CHECK_EQUAL(r_ural[ural::_2].size(), s_std.end() - r_std);
}

BOOST_AUTO_TEST_CASE(replace_copy_test_to_shorter)
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
    std::vector<int> s_ural(source.size() / 2, -1);
    auto const r_ural = ural::replace_copy(source, s_ural,
                                           old_value, new_value);

    // Проверка
    assert(s_ural.size() < source.size());

    BOOST_CHECK_EQUAL_COLLECTIONS(s_ural.begin(), s_ural.end(),
                                  s_std.begin(), s_std.begin() + s_ural.size());

    BOOST_CHECK(r_ural[ural::_1].original() == ural::cursor(source));
    BOOST_CHECK(r_ural[ural::_1].begin() == source.begin() + s_ural.size());
    BOOST_CHECK(!r_ural[ural::_1].traversed_back());

    BOOST_CHECK(r_ural[ural::_2].original() == ural::cursor(s_ural));
    BOOST_CHECK(r_ural[ural::_2].traversed_front() == ural::cursor(s_ural));
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
    URAL_CHECK_EQUAL_RANGES(s_std, s_ural);
}

BOOST_AUTO_TEST_CASE(replace_copy_if_test_minimalistic)
{
    // Подготовка
    std::forward_list<int> const src_std{5, 7, 4, 2, 8, 6, 1, 9, 0, 3};
    ural_test::istringstream_helper<int> const src_ural(src_std);

    auto pred = [](int x) {return x < 5;};
    auto const new_value = 55;

    // std
    std::vector<int> x_std;
    std::replace_copy_if(src_std.begin(), src_std.end(),
                         std::back_inserter(x_std), pred, new_value);

    // ural
    std::vector<int> x_ural;
    ural::replace_copy_if(src_ural, x_ural | ural::back_inserter,
                          pred, new_value);

    // Проверка
    URAL_CHECK_EQUAL_RANGES(x_std, x_ural);
}

BOOST_AUTO_TEST_CASE(replace_copy_if_test_to_longer)
{
    // Исходные данные
    std::vector<int> const source = {5, 7, 4, 2, 8, 6, 1, 9, 0, 3};
    auto pred = [](int x) {return x < 5;};
    auto const new_value = 55;

    // std
    std::vector<int> s_std(source.size() + 5, -1);
    auto const r_std = std::replace_copy_if(source.begin(), source.end(),
                                            s_std.begin(), pred, new_value);

    // ural
    std::vector<int> s_ural(source.size() + 5, -1);
    auto const r_ural = ural::replace_copy_if(source, s_ural, pred, new_value);

    // Проверка
    assert(s_ural.size() > source.size());

    URAL_CHECK_EQUAL_RANGES(s_std, s_ural);

    BOOST_CHECK(r_ural[ural::_1].original() == ural::cursor(source));
    BOOST_CHECK(r_ural[ural::_1].traversed_front() == ural::cursor(source));

    BOOST_CHECK(r_ural[ural::_2].original() == ural::cursor(s_ural));
    BOOST_CHECK(!r_ural[ural::_2].traversed_back());
    BOOST_CHECK_EQUAL(r_ural[ural::_2].size(), s_std.end() - r_std);
}

BOOST_AUTO_TEST_CASE(replace_copy_if_test_to_shorter)
{
    // Исходные данные
    std::vector<int> const source = {5, 7, 4, 2, 8, 6, 1, 9, 0, 3};
    auto pred = [](int x) {return x < 5;};
    auto const new_value = 55;

    // std
    std::vector<int> s_std;
    std::replace_copy_if(source.begin(), source.end(),
                         std::back_inserter(s_std), pred, new_value);

    // ural
    std::vector<int> s_ural(source.size() / 2, -1);
    auto const r_ural = ural::replace_copy_if(source, s_ural, pred, new_value);

    // Проверка
    assert(s_ural.size() < source.size());

    BOOST_CHECK_EQUAL_COLLECTIONS(s_ural.begin(), s_ural.end(),
                                  s_std.begin(), s_std.begin() + s_ural.size());

    BOOST_CHECK(r_ural[ural::_1].original() == ural::cursor(source));
    BOOST_CHECK(r_ural[ural::_1].begin() == source.begin() + s_ural.size());
    BOOST_CHECK(!r_ural[ural::_1].traversed_back());

    BOOST_CHECK(r_ural[ural::_2].original() == ural::cursor(s_ural));
    BOOST_CHECK(r_ural[ural::_2].traversed_front() == ural::cursor(s_ural));
}

// 25.3.6 Заполнение
BOOST_AUTO_TEST_CASE(fill_test)
{
    std::forward_list<int> x_std = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    auto x_ural = x_std;

    auto const value = -1;
    std::forward_list<int> const z(std::distance(x_std.begin(), x_std.end()),
                                   value);

    std::fill(x_std.begin(), x_std.end(), value);
    auto const r_ural = ural::fill(x_ural, value);

    URAL_CHECK_EQUAL_RANGES(x_std, x_ural);
    URAL_CHECK_EQUAL_RANGES(x_ural, z);

    BOOST_CHECK(r_ural.original() == ural::cursor(x_ural));
    BOOST_CHECK(r_ural.traversed_front() == ural::cursor(x_ural));
}

BOOST_AUTO_TEST_CASE(fill_n_test_minimalistic)
{
    std::forward_list<int> v_std{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto v_ural = v_std;

    auto const n = std::distance(v_std.begin(), v_std.end()) / 2;
    auto const value = -1;

    std::fill_n(v_std.begin(), n, value);
    ural::fill_n(v_ural, n, value);

    URAL_CHECK_EQUAL_RANGES(v_ural, v_std);
}

BOOST_AUTO_TEST_CASE(fill_n_test)
{
    std::vector<int> v_std{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto v_ural = v_std;

    auto const n = std::distance(v_std.begin(), v_std.end()) / 2;
    auto const value = -1;

    auto const r_std = std::fill_n(v_std.begin(), n, value);

    auto const r_ural = ural::fill_n(v_ural, n, value);

    URAL_CHECK_EQUAL_RANGES(v_ural, v_std);

    BOOST_CHECK_EQUAL(r_ural.begin() - v_ural.begin(), r_std - v_std.begin());
    BOOST_CHECK_EQUAL(r_ural.begin() - v_ural.begin(), n);
    BOOST_CHECK(r_ural.end() == v_ural.end());
    BOOST_CHECK(r_ural.traversed_front().begin() == v_ural.begin());
}

BOOST_AUTO_TEST_CASE(fill_n_test_negative_n)
{
    std::forward_list<int> v_std{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto v_ural = v_std;

    auto const n = -5;
    auto const value = -1;

    auto const r_ural = ural::fill_n(v_ural, n, value);

    URAL_CHECK_EQUAL_RANGES(v_ural, v_std);

    BOOST_CHECK(r_ural == ural::cursor(v_ural));
}

// 25.3.7 Порождениеs
BOOST_AUTO_TEST_CASE(generate_test)
{
    // Подготовка
    auto const n = int{5};

    std::forward_list<int> v_std(n, -1);
    auto v_ural = v_std;

    // std
    auto counter = int{0};
    auto gen = [&]{ return counter++; };
    std::generate(v_std.begin(), v_std.end(), gen);

    // ural
    counter = 0;
    ural::generate(v_ural, gen);

    // Проверки
    URAL_CHECK_EQUAL_RANGES(v_ural, v_std);
}

BOOST_AUTO_TEST_CASE(generate_test_return_value)
{
    // Подготовка
    auto const n = int{5};

    std::vector<int> v_std(n, -1);
    auto v_ural = v_std;

    // std
    auto counter = int{0};
    auto gen = [&]{ return counter++; };
    std::generate(v_std.begin(), v_std.end(), gen);

    // ural
    counter = 0;
    auto const r_ural = ural::generate(v_ural, gen);

    // Проверки
    URAL_CHECK_EQUAL_RANGES(v_std, v_ural);

    BOOST_CHECK(r_ural.original() == ural::cursor(v_ural));
    BOOST_CHECK(r_ural.traversed_front() == ural::cursor(v_ural));
}

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
    URAL_CHECK_EQUAL_RANGES(r_std, r_ural);
}

BOOST_AUTO_TEST_CASE(generate_n_return_value_negative_count_test)
{
    // Подготовка
    auto const n = int{-5};

    std::vector<int> v_ural(n + 5, -1);
    auto const v_ural_old = v_ural;

    auto counter = int{0};
    auto gen = [&]{ return counter++; };

    // ural
    auto r_ural = ural::generate_n(v_ural, n, gen);

    // Проверка
    assert(n < 0);

    BOOST_CHECK(r_ural == ural::cursor(v_ural));

    URAL_CHECK_EQUAL_RANGES(v_ural, v_ural_old);
}

BOOST_AUTO_TEST_CASE(generate_n_return_value_test)
{
    // Подготовка
    auto const n = int{5};

    std::vector<int> v_std(n + 5, -1);
    auto v_ural = v_std;

    // std
    auto counter = int{0};
    auto gen = [&]{ return counter++; };
    auto const r_std = std::generate_n(v_std.begin(), n, gen);

    // ural
    counter = 0;
    auto const r_ural = ural::generate_n(v_ural, n, gen);

    // Проверка
    BOOST_CHECK(r_ural.original() == ural::cursor(v_ural));
    BOOST_CHECK_EQUAL(r_ural.traversed_front().size(), n);
    BOOST_CHECK(!r_ural.traversed_back());

    BOOST_CHECK_EQUAL(v_ural.end() - r_ural.begin(),
                      v_std.end() - r_std);

    URAL_CHECK_EQUAL_RANGES(v_std, v_ural);
}

// 25.3.8 Удаление
BOOST_AUTO_TEST_CASE(remove_test_minimalistic)
{
    // Подготовка
    std::string const source = "Text with some   spaces";

    std::forward_list<char> v_std(source.begin(), source.end());
    auto v_ural = v_std;

    auto const to_remove = ' ';

    // Алгоритм
    auto r_std = std::remove(v_std.begin(), v_std.end(), to_remove);

    auto r_ural = ural::remove(v_ural, to_remove);

    // Проверки
    URAL_CHECK_EQUAL_RANGES(v_std, v_ural);

    BOOST_CHECK(r_ural.original() == ural::cursor(v_ural));
    BOOST_CHECK(!r_ural.traversed_back());

    BOOST_CHECK_EQUAL(ural::size(r_ural.traversed_front()),
                      std::distance(v_std.begin(), r_std));

    BOOST_CHECK_EQUAL_COLLECTIONS(r_ural.traversed_begin(), r_ural.begin(),
                                  v_std.begin(), r_std);
}

BOOST_AUTO_TEST_CASE(remove_test)
{
    // Подготовка
    std::string s_std = "Text with some   spaces";
    auto s_ural = s_std;

    auto const to_remove = ' ';

    auto const n = std::count(s_std.begin(), s_std.end(), to_remove);

    // Алгоритм
    auto r_std = std::remove(s_std.begin(), s_std.end(), to_remove);

    auto r_ural = ural::remove(s_ural, to_remove);

    // Проверки
    BOOST_CHECK_EQUAL(s_std, s_ural);

    BOOST_CHECK(r_ural.original() == ural::cursor(s_ural));
    BOOST_CHECK(!r_ural.traversed_back());

    BOOST_CHECK_EQUAL(r_ural.begin() - s_ural.begin(), r_std - s_std.begin());
    BOOST_CHECK_EQUAL(r_ural.size(), n);

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
    ural_test::istringstream_helper<char> const src_ural(source);
    std::string s_ural;
    ural::remove_copy(src_ural, s_ural | ural::back_inserter, to_remove);

    // Сравнение
    BOOST_CHECK_EQUAL(s_std, s_ural);
}

BOOST_AUTO_TEST_CASE(remove_copy_test_to_longer)
{
    // Подготовка
    auto const source = std::string("Text with some   spaces");
    auto const to_remove = ' ';

    std::string s_std(source.size() + 7, 'Z');
    auto s_ural = s_std;

    // std
    auto r_std = std::remove_copy(source.begin(), source.end(),
                                  s_std.begin(), to_remove);

    // ural
    auto r_ural = ural::remove_copy(source, s_ural, to_remove);

    // Сравнение
    assert(source.size() < s_std.size());
    assert(source.size() < s_ural.size());

    BOOST_CHECK_EQUAL(s_std, s_ural);

    BOOST_CHECK(r_ural[ural::_1].original() == ural::cursor(source));
    BOOST_CHECK(r_ural[ural::_1].traversed_front() == ural::cursor(source));

    BOOST_CHECK(r_ural[ural::_2].original() == ural::cursor(s_ural));
    BOOST_CHECK(!r_ural[ural::_2].traversed_back());

    BOOST_CHECK_EQUAL(r_ural[ural::_2].begin() - s_ural.begin(),
                      r_std - s_std.begin());
}

BOOST_AUTO_TEST_CASE(remove_copy_test_to_shorter)
{
    // Подготовка
    auto const source = std::string("Text with some   spaces");
    auto const to_remove = ' ';

    size_t const n = std::count(source.begin(), source.end(), to_remove);
    BOOST_CHECK_LE(n, source.size());

    // std
    std::string s_std;
    std::remove_copy(source.begin(), source.end(),
                     std::back_inserter(s_std), to_remove);

    // ural
    std::string s_ural((source.size() - n)/2, 'Z');
    auto r_ural = ural::remove_copy(source, s_ural, to_remove);

    // Сравнение
    auto const n_removed = ural::count(r_ural[ural::_1].traversed_front(), to_remove);

    assert(source.size() > s_std.size());
    assert(source.size() > s_ural.size());

    BOOST_CHECK_EQUAL_COLLECTIONS(s_ural.begin(), s_ural.end(),
                                  s_std.begin(),  s_std.begin() + s_ural.size());

    BOOST_CHECK(r_ural[ural::_1].original() == ural::cursor(source));
    BOOST_CHECK(!!r_ural[ural::_1]);
    BOOST_CHECK(r_ural[ural::_1].begin() == source.begin() + s_ural.size() + n_removed);
    BOOST_CHECK(!r_ural[ural::_1].traversed_back());

    BOOST_CHECK(r_ural[ural::_2].original() == ural::cursor(s_ural));
    BOOST_CHECK(!r_ural[ural::_2]);
    BOOST_CHECK(!r_ural[ural::_2].traversed_back());
}

BOOST_AUTO_TEST_CASE(remove_erase_test)
{
    std::string s_std = "Text with some   spaces";
    auto s_ural = s_std;

    auto const to_remove = ' ';

    s_std.erase(std::remove(s_std.begin(), s_std.end(), to_remove), s_std.end());

    auto & ref_ural = ural_ex::remove_erase(s_ural, to_remove);

    BOOST_CHECK_EQUAL(s_std, s_ural);
    BOOST_CHECK_EQUAL(&ref_ural, &s_ural);
}

BOOST_AUTO_TEST_CASE(remove_if_test_minimalistic)
{
    // Подготовка
    std::string const source = "Text\n with\tsome \t  whitespaces\n\n";

    std::forward_list<char> s_std(source.begin(), source.end());
    auto s_ural = s_std;

    auto const pred = [](char x){return std::isspace(x);};

    // Выполнение операций
    std::remove_if(s_std.begin(), s_std.end(), pred);
    ural::remove_if(s_ural, pred);

    //Проверка
    URAL_CHECK_EQUAL_RANGES(s_ural, s_std);
}

BOOST_AUTO_TEST_CASE(remove_if_test)
{
    std::string s_std = "Text\n with\tsome \t  whitespaces\n\n";
    auto s_ural = s_std;

    auto const pred = [](char x){return std::isspace(x);};

    s_std.erase(std::remove_if(s_std.begin(), s_std.end(), pred), s_std.end());

    auto s = ural::remove_if(s_ural, pred);

    BOOST_CHECK(s.original() == ural::cursor(s_ural));
    BOOST_CHECK(!s.traversed_back());

    auto & ref_ural = ural_ex::erase(s_ural, s);

    BOOST_CHECK_EQUAL(s_std, s_ural);
    BOOST_CHECK_EQUAL(&ref_ural, &s_ural);
}

BOOST_AUTO_TEST_CASE(remove_if_erase_test)
{
    std::string s_std = "Text\n with\tsome \t  whitespaces\n\n";
    auto s_ural = s_std;

    auto const pred = [](char x){return std::isspace(x);};

    s_std.erase(std::remove_if(s_std.begin(), s_std.end(), pred), s_std.end());

    auto & ref_ural = ural_ex::remove_if_erase(s_ural, pred);

    BOOST_CHECK_EQUAL(s_std, s_ural);
    BOOST_CHECK_EQUAL(&ref_ural, &s_ural);
}

// для согласования с boost.range
BOOST_AUTO_TEST_CASE(remove_erase_if_test)
{
    std::string s_std = "Text\n with\tsome \t  whitespaces\n\n";
    auto s_ural = s_std;

    auto const pred = [](char x){return std::isspace(x);};

    s_std.erase(std::remove_if(s_std.begin(), s_std.end(), pred), s_std.end());

    auto & ref_ural = ural_ex::remove_erase_if(s_ural, pred);

    BOOST_CHECK_EQUAL(s_std, s_ural);
    BOOST_CHECK_EQUAL(&ref_ural, &s_ural);
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
    std::istringstream src_ural(source);
    std::string s_ural;
    ural::remove_copy_if(src_ural, s_ural | ural::back_inserter, pred);

    // Проверка
    BOOST_CHECK_EQUAL(s_std, s_ural);
}

BOOST_AUTO_TEST_CASE(remove_copy_if_to_longer_test)
{
    // Подготовка
    std::string const src = "Text\n with\tsome \t  whitespaces\n\n";
    auto pred = [](char x){return std::isspace(x);};

    // std
    std::string s_std(src.size() + 1, 'Z');
    auto const r_std = std::remove_copy_if(src.begin(), src.end(),
                                           s_std.begin(), pred);

    // ural
    std::string s_ural(src.size() + 1, 'Z');
    auto const r_ural = ural::remove_copy_if(src, s_ural, pred);

    // Проверка
    BOOST_CHECK_EQUAL(s_std, s_ural);

    assert(src.size() < s_ural.size());

    BOOST_CHECK(r_ural[ural::_1].original() == ural::cursor(src));
    BOOST_CHECK(r_ural[ural::_1].traversed_front() == ural::cursor(src));
    BOOST_CHECK(!r_ural[ural::_1]);
    BOOST_CHECK(!r_ural[ural::_1].traversed_back());

    BOOST_CHECK_EQUAL(r_ural[ural::_2].begin() - s_ural.begin(),
                      r_std - s_std.begin());

    BOOST_CHECK(r_ural[ural::_2].original() == ural::cursor(s_ural));
    BOOST_CHECK(!!r_ural[ural::_2]);
    BOOST_CHECK(!r_ural[ural::_2].traversed_back());
}

BOOST_AUTO_TEST_CASE(remove_copy_if_to_shorter_test)
{
    // Подготовка
    std::string const src = "Text\n with\tsome \t  whitespaces\n\n";
    auto pred = [](char x){return std::isspace(x);};

    // std
    std::string s_std;
    std::remove_copy_if(src.begin(), src.end(),
                        std::back_inserter(s_std), pred);

    // ural
    std::string s_ural(src.size() / 2, 'Z');
    auto const r_ural = ural::remove_copy_if(src, s_ural, pred);

    // Проверка
    auto const n_removed = ural::count_if(r_ural[ural::_1].traversed_front(), pred);

    assert(src.size() > s_ural.size());

    BOOST_CHECK_EQUAL_COLLECTIONS(s_ural.begin(), s_ural.end(),
                                  s_std.begin(),  s_std.begin() + s_ural.size());

    BOOST_CHECK(r_ural[ural::_1].original() == ural::cursor(src));
    BOOST_CHECK(!!r_ural[ural::_1]);
    BOOST_CHECK(r_ural[ural::_1].begin() == src.begin() + s_ural.size() + n_removed);
    BOOST_CHECK(!r_ural[ural::_1].traversed_back());

    BOOST_CHECK(r_ural[ural::_2].original() == ural::cursor(s_ural));
    BOOST_CHECK(!r_ural[ural::_2]);
    BOOST_CHECK(!r_ural[ural::_2].traversed_back());
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

BOOST_AUTO_TEST_CASE(unique_test_return_value)
{
    std::vector<int> s_std{1, 2, 2, 2, 3, 3, 2, 2, 1};
    auto s_ural = s_std;

    auto const r_std = std::unique(s_std.begin(), s_std.end());
    auto const r_ural = ural::unique(s_ural);

    BOOST_CHECK_EQUAL_COLLECTIONS(s_std.begin(), r_std,
                                  r_ural.traversed_begin(), r_ural.begin());
    BOOST_CHECK(r_ural.original() == ural::cursor(s_ural));
    BOOST_CHECK(!r_ural.traversed_back());
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

    ural::unique_copy(ural::experimental::make_istream_cursor<char>(is_ural),
                      ural::experimental::make_ostream_cursor(os_ural),
                      pred);

    // Проверка результатов
    BOOST_CHECK_EQUAL(os_std.str(), os_ural.str());
}

BOOST_AUTO_TEST_CASE(unique_copy_from_istream_to_ostream)
{
    std::list<int> const v1{1, 2, 2, 2, 3, 3, 2, 2, 1};

    std::ostringstream src;
    ural::copy(v1, ural::experimental::make_ostream_cursor(src, ' '));

    auto v2 = v1;
    ural_ex::unique_erase(v2);

    std::ostringstream z;
    ural::copy(v2, ural::experimental::make_ostream_cursor(z, ' '));

    // Сам алгоритм
    ural_test::istringstream_helper<int> const is(v1);
    std::ostringstream os;

    ural::unique_copy(is, ural::experimental::make_ostream_cursor(os, ' '));

    BOOST_CHECK_EQUAL(z.str(), os.str());
}

BOOST_AUTO_TEST_CASE(unqiue_copy_to_shorter)
{
    // Подготовка
    std::vector<int> const src{1, 2, 2, 2, 3, 3, 2, 2, 1};

    std::vector<int> v_ural(src.size() /2, -1);

    // Выполнение алгоритма
    std::vector<int> v_std;
    std::unique_copy(src.begin(), src.end(), std::back_inserter(v_std));

    auto r_ural = ural::unique_copy(src, v_ural);

    // Проверки
    assert(src.size() > v_ural.size());

    BOOST_CHECK_EQUAL_COLLECTIONS(v_ural.begin(), v_ural.end(),
                                  v_std.begin(),  v_std.begin() + v_ural.size());

    BOOST_CHECK(r_ural[ural::_1].original() == ural::cursor(src));
    BOOST_CHECK(!!r_ural[ural::_1]);
    BOOST_CHECK(r_ural[ural::_1].begin() >= src.begin() + v_ural.size());
    BOOST_CHECK(!r_ural[ural::_1].traversed_back());

    BOOST_CHECK(r_ural[ural::_2].original() == ural::cursor(v_ural));
    BOOST_CHECK(!r_ural[ural::_2]);
    BOOST_CHECK(!r_ural[ural::_2].traversed_back());
}

BOOST_AUTO_TEST_CASE(unqiue_copy_to_longer)
{
    // Подготовка
    std::vector<int> const src{1, 2, 2, 2, 3, 3, 2, 2, 1};

    std::vector<int> v_std(src.size() + 5, -1);
    auto v_ural = v_std;

    // Выполнение алгоритма
    auto r_std = std::unique_copy(src.begin(), src.end(), v_std.begin());
    auto r_ural = ural::unique_copy(src, v_ural);

    // Проверки
    BOOST_CHECK_EQUAL_COLLECTIONS(v_ural.begin(), v_ural.end(),
                                  v_std.begin(),  v_std.end());

    assert(src.size() < v_ural.size());

    BOOST_CHECK(r_ural[ural::_1].original() == ural::cursor(src));
    BOOST_CHECK(r_ural[ural::_1].traversed_front() == ural::cursor(src));
    BOOST_CHECK(!r_ural[ural::_1]);
    BOOST_CHECK(!r_ural[ural::_1].traversed_back());

    BOOST_CHECK_EQUAL(r_ural[ural::_2].begin() - v_ural.begin(),
                      r_std - v_std.begin());

    BOOST_CHECK(r_ural[ural::_2].original() == ural::cursor(v_ural));
    BOOST_CHECK(!!r_ural[ural::_2]);
    BOOST_CHECK(!r_ural[ural::_2].traversed_back());
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

        URAL_CHECK_EQUAL_RANGES(c_std, c_ural);

        BOOST_CHECK(result.original() == ural::cursor(c_ural));
        BOOST_CHECK(result.traversed_front() == ural::cursor(c_ural));
        BOOST_CHECK(!result);
        BOOST_CHECK(!result.traversed_back());
    }
}
BOOST_AUTO_TEST_CASE(reverse_test)
{
    std::list<int> const src = {1, 2, 3, 4, 5, 6};

    auto x_std = src;
    auto x_ural = src;

    std::reverse(x_std.begin(), x_std.end());
    auto const result = ural::reverse(x_ural);

    URAL_CHECK_EQUAL_RANGES(x_std, x_ural);

    BOOST_CHECK(result.original() == ural::cursor(x_ural));
    BOOST_CHECK(result.traversed_front() == ural::cursor(x_ural));
    BOOST_CHECK(!result);
    BOOST_CHECK(!result.traversed_back());
}

BOOST_AUTO_TEST_CASE(reverse_pre_bidirectional_test)
{
    std::list<int> const src = {1, 2, 3, 4, 6, 7};

    auto pred = +[](int const & x) { return x < 5; };

    static_assert(ural::concepts::CopyAssignable<decltype(pred)>(), "");

    // Первый способ
    auto x1 = src;
    auto const r1 = ural::reverse(ural::find_if_not(x1, pred).traversed_front());

    // Второй способ
    auto x2 = src;
    auto const r2 = ural::reverse(x2 | ::ural::experimental::taken_while(pred));

    // Сравнение
    URAL_CHECK_EQUAL_RANGES(x1, x2);

    URAL_CHECK_EQUAL_RANGES(r1.traversed_front(), r2.base().traversed_front());
}

BOOST_AUTO_TEST_CASE(reversed_reverse_test)
{
    std::list<int> const src = {1, 2, 3, 4, 5, 6};

    auto x_std = src;
    auto x_ural = src;

    std::reverse(x_std.begin(), x_std.end());
    ural::reverse(x_ural | ural_ex::reversed);

    URAL_CHECK_EQUAL_RANGES(x_std, x_ural);
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
    URAL_CHECK_EQUAL_RANGES(r_std, r_ural);
}

BOOST_AUTO_TEST_CASE(reverse_copy_test_to_longer)
{
    // Исходные данные
    std::vector<int> const src = {1, 2, 3, 4, 5, 6};

    // std
    std::vector<int> r_std(src.size() + 5, -1);
    auto const result_std = std::reverse_copy(src.begin(), src.end(), r_std.begin());

    // ural
    std::vector<int> r_ural(src.size() + 5, -1);
    auto const result_ural = ural::reverse_copy(src, r_ural);

    // Проверка
    assert(r_ural.size() > src.size());
    URAL_CHECK_EQUAL_RANGES(r_std, r_ural);

    BOOST_CHECK(result_ural[ural::_1].original() == ural::cursor(src));
    BOOST_CHECK(result_ural[ural::_1].traversed_back() == ural::cursor(src));

    BOOST_CHECK(result_ural[ural::_2].original() == ural::cursor(r_ural));
    BOOST_CHECK(!result_ural[ural::_2].traversed_back());
    BOOST_CHECK(result_ural[ural::_2].begin() ==
                r_ural.begin() + (result_std - r_std.begin()));
    BOOST_CHECK(result_ural[ural::_2].begin() ==
                r_ural.begin() + src.size());
}

// 25.3.11 Вращение
BOOST_AUTO_TEST_CASE(rotate_test_minimalistic)
{
    std::forward_list<int> const v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    for(auto i : ural::numbers(0, std::distance(v.begin(), v.end())))
    {
        auto v_std = v;
        auto v_ural = v;

        std::rotate(v_std.begin(), std::next(v_std.begin(), i), v_std.end());

        ural::rotate(ural::next(ural::cursor(v_ural), i));

        URAL_CHECK_EQUAL_RANGES(v_std, v_ural);
    }
}

BOOST_AUTO_TEST_CASE(rotate_test)
{
    std::vector<int> const v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    for(auto i : ural::indices_of(v))
    {
        auto v_std = v;
        auto v_ural = v;

        std::rotate(v_std.begin(), v_std.begin() + i, v_std.end());

        auto r_ural = ural::rotate(ural::cursor(v_ural) + i);

        URAL_CHECK_EQUAL_RANGES(v_std, v_ural);

        BOOST_CHECK(r_ural.original() == ural::cursor(v_ural));

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

        URAL_CHECK_EQUAL_RANGES(v_std, v_ural);

        ural::rotate(r_ural);

        URAL_CHECK_EQUAL_RANGES(v, v_ural);
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

        auto s = ural::cursor(src);
        s += i;

        ural::rotate_copy(s, r_ural | ural::front_inserter);

        URAL_CHECK_EQUAL_RANGES(r_std, r_ural);
    }
}

BOOST_AUTO_TEST_CASE(rotate_copy_different_traversed_front)
{
    auto const s1 = ural::numbers(1, 9);
    auto const s2 = ural::numbers(0, 9);

    auto const seq = ural_ex::make_cartesian_product_cursor(s1, s2);

    using Value = ural::ValueType<decltype(seq)>;

    auto const n = s1.size() * s2.size() / 2;

    // Первый способ
    std::vector<Value> v1;
    ural::copy(seq, v1 | ural::back_inserter);
    std::rotate(v1.begin(), v1.begin() + n, v1.end());

    // Второй способ
    std::vector<Value> v2;

    ural::rotate_copy(ural::next(seq, n), v2 | ural::back_inserter);

    BOOST_CHECK(ural::equal(v1, v2));
}

BOOST_AUTO_TEST_CASE(rotate_copy_return_test)
{
    std::vector<int> const src{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    for(auto i : ural::indices_of(src))
    {
        // Подготовка
        std::vector<int> d_std(src.size() + 5, -1);
        std::vector<int> d_ural(src.size() + 5, -1);

        // Выполнение операций
        std::rotate_copy(src.begin(), src.begin() + i, src.end(), d_std.begin());

        auto r_ural = ural::rotate_copy(ural::cursor(src) + i, d_ural);

        // Проверки
        BOOST_CHECK(r_ural[ural::_1].original() == ural::cursor(src));
        BOOST_CHECK(r_ural[ural::_1].traversed_front() == ural::cursor(src));

        BOOST_CHECK(r_ural[ural::_2].original() == ural::cursor(d_ural));
        BOOST_CHECK_EQUAL(ural::to_signed(src.size()),
                          r_ural[ural::_2].traversed_front().size());
        BOOST_CHECK_EQUAL(ural::to_signed(d_ural.size() - src.size()),
                          r_ural[ural::_2].size());

        URAL_CHECK_EQUAL_RANGES(d_std, d_ural);
    }
}

BOOST_AUTO_TEST_CASE(rotate_copy_to_shorter)
{
    std::vector<int> const src{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    for(auto i : ural::indices_of(src))
    {
        // Подготовка
        std::vector<int> d_std;
        std::vector<int> d_ural(src.size() / 2);

        // Выполнение операций
        std::rotate_copy(src.begin(), src.begin() + i, src.end(),
                         std::back_inserter(d_std));

        ural::rotate_copy(ural::cursor(src) + i, d_ural);

        auto z = d_ural;
        ural::copy(d_std, z);

        // Проверки
        URAL_CHECK_EQUAL_RANGES(d_ural, z);
    }
}

// 25.3.12 Тусовка
BOOST_AUTO_TEST_CASE(shuffle_test)
{
    auto v = ural::numbers(1, 10) | ural_ex::to_container<std::vector>{};

    auto const v_old = v;

    auto const result = ural::shuffle(v, ural_test::random_engine());

    BOOST_CHECK(result.traversed_front().begin() == v.begin());
    BOOST_CHECK(result.traversed_front().end() == v.end());
    BOOST_CHECK(result.begin() == v.end());
    BOOST_CHECK(result.end() == v.end());

    BOOST_CHECK_EQUAL(v.size(), v_old.size());

    BOOST_CHECK(std::is_permutation(v.begin(), v.end(), v_old.begin()));

    BOOST_CHECK(ural::is_permutation(v, v_old));
}

BOOST_AUTO_TEST_CASE(random_shuffle_test)
{
    auto v = ural::numbers(1, 10) | ural_ex::to_container<std::vector>{};

    auto const v_old = v;

    auto const result = ural_ex::random_shuffle(v);

    BOOST_CHECK(result.traversed_front().begin() == v.begin());
    BOOST_CHECK(result.traversed_front().end() == v.end());
    BOOST_CHECK(result.begin() == v.end());
    BOOST_CHECK(result.end() == v.end());

    BOOST_CHECK_EQUAL(v.size(), v_old.size());

    BOOST_CHECK(std::is_permutation(v.begin(), v.end(), v_old.begin()));

    BOOST_CHECK(ural::is_permutation(v, v_old));
}

// 25.3.13 Разделение
BOOST_AUTO_TEST_CASE(is_partitioned_test)
{
    std::vector<int> v = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    ural_test::istringstream_helper<int> const v_ural(v);

    BOOST_CHECK_EQUAL(std::is_partitioned(v.begin(), v.end(), ural::is_even),
                      ural::is_partitioned(v, ural::is_even));
    BOOST_CHECK_EQUAL(false, ural::is_partitioned(v_ural, ural::is_even));

    std::partition(v.begin(), v.end(), ural::is_even);

    BOOST_CHECK_EQUAL(std::is_partitioned(v.begin(), v.end(), ural::is_even),
                      ural::is_partitioned(v, ural::is_even));
    BOOST_CHECK_EQUAL(true, ural::is_partitioned(v, ural::is_even));

    std::reverse(v.begin(), v.end());

    BOOST_CHECK_EQUAL(std::is_partitioned(v.begin(), v.end(), ural::is_even),
                      ural::is_partitioned(v, ural::is_even));
    BOOST_CHECK_EQUAL(false, ural::is_partitioned(v, ural::is_even));
}

BOOST_AUTO_TEST_CASE(partition_test)
{
    // Подготовка
    typedef std::forward_list<int> Container;
    Container const xs = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto ys = xs;

    using Element = ::ural::ValueType<Container>;

    static_assert(std::is_same<Element, int>::value, "");

    // Выполнение операции
    auto r_ural = ural::partition(ys, ural::is_even);

    // Проверка
    BOOST_CHECK(r_ural.original() == ural::cursor(ys));

    BOOST_CHECK(ural::is_permutation(ys, xs));
    BOOST_CHECK(ural::is_partitioned(ys, ural::is_even));

    BOOST_CHECK(::ural::all_of(r_ural.traversed_front(), ural::is_even));
    BOOST_CHECK(std::all_of(r_ural.traversed_begin(), r_ural.begin(),
                            ural::is_even));

    BOOST_CHECK(::ural::none_of(r_ural, ural::is_even));
    BOOST_CHECK(std::none_of(r_ural.begin(), r_ural.end(), ural::is_even));
}

BOOST_AUTO_TEST_CASE(stable_partition_test_empty)
{
    std::vector<int> const src {};
    auto v_std = src;
    auto v_ural = src;

    auto const pred = [](int n){return n % 2 == 0;};

    std::stable_partition(v_std.begin(), v_std.end(), pred);
    auto r_ural = ural::stable_partition(v_ural, pred);

    URAL_CHECK_EQUAL_RANGES(v_std, v_ural);

    BOOST_CHECK(::ural::all_of(r_ural.traversed_front(), pred));
    BOOST_CHECK(std::all_of(r_ural.traversed_begin(), r_ural.begin(), pred));

    BOOST_CHECK(::ural::none_of(r_ural, pred));
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

    URAL_CHECK_EQUAL_RANGES(v_std, v_ural);

    BOOST_CHECK(::ural::all_of(r_ural.traversed_front(), pred));
    BOOST_CHECK(std::all_of(r_ural.traversed_begin(), r_ural.begin(), pred));

    BOOST_CHECK(::ural::none_of(r_ural, pred));
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

    URAL_CHECK_EQUAL_RANGES(v_std, v_ural);

    BOOST_CHECK(::ural::all_of(r_ural.traversed_front(), pred));
    BOOST_CHECK(std::all_of(r_ural.traversed_begin(), r_ural.begin(), pred));

    BOOST_CHECK(::ural::none_of(r_ural, pred));
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

    URAL_CHECK_EQUAL_RANGES(v_std, v_ural);

    BOOST_CHECK(::ural::all_of(r_ural.traversed_front(), pred));
    BOOST_CHECK(std::all_of(r_ural.traversed_begin(), r_ural.begin(), pred));

    BOOST_CHECK(::ural::none_of(r_ural, pred));
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

    URAL_CHECK_EQUAL_RANGES(v_std, v_ural);

    BOOST_CHECK(::ural::all_of(r_ural.traversed_front(), pred));
    BOOST_CHECK(std::all_of(r_ural.traversed_begin(), r_ural.begin(), pred));

    BOOST_CHECK(::ural::none_of(r_ural, pred));
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

    BOOST_CHECK(r_ural.original() == ural::cursor(v_ural));
    BOOST_CHECK(!r_ural.traversed_back());

    BOOST_CHECK(ural::is_permutation(v_ural, src));

    URAL_CHECK_EQUAL_RANGES(v_std, v_ural);

    BOOST_CHECK(::ural::all_of(r_ural.traversed_front(), pred));
    BOOST_CHECK(std::all_of(r_ural.traversed_begin(), r_ural.begin(), pred));

    BOOST_CHECK(::ural::none_of(r_ural, pred));
    BOOST_CHECK(std::none_of(r_ural.begin(), r_ural.end(), pred));
    BOOST_CHECK(std::none_of(r_ural.begin(), r_ural.traversed_end(), pred));
}

BOOST_AUTO_TEST_CASE(partition_copy_test)
{
    // Подготовка
    std::vector<int> xs = {1,2,3,4,5,6,7,8,9,10};
    ural_test::istringstream_helper<int> const src(xs);

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
