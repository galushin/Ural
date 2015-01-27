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

#include <boost/test/unit_test.hpp>

#include <forward_list>
#include <list>
#include <vector>

#include <boost/mpl/list.hpp>

#include <ural/numeric/numbers_sequence.hpp>
#include <ural/algorithm.hpp>
#include <ural/memory.hpp>
#include <ural/sequence/all.hpp>
#include <ural/utility/tracers.hpp>

typedef boost::mpl::list<std::forward_list<int>,
                         std::list<int>,
                         std::vector<int>> Sources;

// 25.2 Алгоритмы, не модифицирующие последовательность
// 25.2.1
BOOST_AUTO_TEST_CASE_TEMPLATE(all_of_test, Source, Sources)
{
    typedef typename Source::value_type Element;

    Source v(10, 2);
    std::partial_sum(v.cbegin(), v.cend(), v.begin());

    auto const pred = [](Element i){ return i % 2 == 0; };

    BOOST_CHECK(std::all_of(v.cbegin(), v.cend(), pred));
    BOOST_CHECK(ural::all_of(v, pred));

    v.front() = 1;

    BOOST_CHECK(!std::all_of(v.cbegin(), v.cend(), pred));
    BOOST_CHECK(!ural::all_of(v, pred));
}

// 25.2.2
BOOST_AUTO_TEST_CASE(any_of_test)
{
    std::vector<int> v(10, 2);
    std::partial_sum(v.cbegin(), v.cend(), v.begin());

    auto const pred = [](int i){ return i % 7 == 0; };

    BOOST_CHECK(std::any_of(v.cbegin(), v.cend(), pred));
    BOOST_CHECK(ural::any_of(v, pred));

    v[6] = 13;

    BOOST_CHECK(!std::any_of(v.cbegin(), v.cend(), pred));
    BOOST_CHECK(!ural::any_of(v, pred));
}

// 25.2.3
BOOST_AUTO_TEST_CASE(none_of_test)
{
    std::vector<int> v(10, 2);
    std::partial_sum(v.cbegin(), v.cend(), v.begin());

    auto const pred = [](int i){ return i % 7 == 0; };

    BOOST_CHECK(!std::none_of(v.cbegin(), v.cend(), pred));
    BOOST_CHECK(!ural::none_of(v, pred));

    v[6] = 13;

    BOOST_CHECK(std::none_of(v.cbegin(), v.cend(), pred));
    BOOST_CHECK(ural::none_of(v, pred));
}

// 25.2.4
BOOST_AUTO_TEST_CASE(for_each_test)
{
    std::vector<int> x_std = {1, 2, 3, 4, 5};
    std::vector<int> x_ural = x_std;

    auto const action = [](int & x) {x *= 2;};

    auto const r_std = std::for_each(x_std.begin(), x_std.end(), +action);
    auto const r_ural = ural::for_each(x_ural, +action);

    BOOST_CHECK_EQUAL(r_std, r_ural.target());

    BOOST_CHECK_EQUAL_COLLECTIONS(x_std.begin(), x_std.end(),
                                  x_ural.begin(), x_ural.end());
}

// 25.2.5
BOOST_AUTO_TEST_CASE(find_fail_test)
{
    std::vector<int> v{0, 1, 2, 3, 4};

    auto const value = -1;

    auto s = ural::find(v, value);

    BOOST_CHECK(std::find(v.begin(), v.end(), value) == v.end());
    BOOST_CHECK(!s);
}

BOOST_AUTO_TEST_CASE(find_success_test)
{
    std::vector<int> v{0, 1, 2, 3, 4};

    auto const value = 2;

    auto s = ural::find(v, value);

    BOOST_CHECK(!!s);
    BOOST_CHECK_EQUAL(value, *s);
    BOOST_CHECK(std::find(v.begin(), v.end(), value) == s.begin());
}

// 25.2.6
BOOST_AUTO_TEST_CASE(find_end_test_success)
{
    std::vector<int> const v{1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4};
    std::list<int> const t1{1, 2, 3};

    auto r_std = std::find_end(v.begin(), v.end(), t1.begin(), t1.end());
    auto r_ural = ural::find_end(v, t1);

    BOOST_CHECK_EQUAL(std::distance(r_std, v.end()), r_ural.size());
}

BOOST_AUTO_TEST_CASE(find_end_test_fail)
{
    std::vector<int> const v{1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4};
    std::list<int> const t2{4, 5, 6};

    auto r_std = std::find_end(v.begin(), v.end(), t2.begin(), t2.end());
    auto r_ural = ural::find_end(v, t2);

    BOOST_CHECK_EQUAL(std::distance(r_std, v.end()), r_ural.size());
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
    std::vector<int> const xs = {1, 2, 3, 4};

    std::vector<int> x1;

    ural::copy(xs, std::back_inserter(x1));

    BOOST_CHECK_EQUAL_COLLECTIONS(xs.begin(), xs.end(), x1.begin(), x1.end());
}

BOOST_AUTO_TEST_CASE(copy_test_different_sizes)
{
    std::vector<int> const xs = {1, 2, 3, 4};

    std::vector<int> x1(xs.size() - 2, 0);
    std::vector<int> x2(xs.size() + 2, 0);

    auto const r1 = ural::copy(xs, x1);
    auto const r2 = ural::copy(xs, x2);

    BOOST_CHECK(!!r1[ural::_1]);
    BOOST_CHECK(!r1[ural::_2]);

    BOOST_CHECK(!r2[ural::_1]);
    BOOST_CHECK(!!r2[ural::_2]);
}

BOOST_AUTO_TEST_CASE(copy_n_test)
{
    std::string const src = "1234567890";
    std::string r_std;
    std::string r_ural;

    auto const n = 4;

    std::copy_n(src.begin(), n, std::back_inserter(r_std));

    ural::copy(src | ural::taken(n), std::back_inserter(r_ural));

    BOOST_CHECK_EQUAL(r_std, r_ural);
}

BOOST_AUTO_TEST_CASE(filtered_test)
{
    typedef int Type;
    std::vector<Type> const xs = {25, -15, 5, -5, 15};
    auto const pred = [](Type i){return !(i<0);};
    typedef decltype(ural::sequence(xs)) Sequence;

    static_assert(std::is_empty<decltype(pred)>::value, "");
    BOOST_CHECK_EQUAL(sizeof(Sequence), sizeof(boost::compressed_pair<Sequence, decltype(pred)>));

    std::vector<Type> r_std;

    std::copy_if (xs.begin(), xs.end(), std::back_inserter(r_std) , pred);

    auto const r_ural
        = xs | ural::filtered(pred) | ural::to_container<std::vector>{};

    BOOST_CHECK_EQUAL_COLLECTIONS(r_std.begin(), r_std.end(),
                                  r_ural.begin(), r_ural.end());
}

BOOST_AUTO_TEST_CASE(copy_if_test)
{
    typedef int Type;
    std::vector<Type> const xs = {25, -15, 5, -5, 15};
    auto const pred = [](Type i){return !(i<0);};

    std::vector<Type> r_std;

    std::copy_if (xs.begin(), xs.end(), std::back_inserter(r_std) , pred);

    std::vector<Type> r_ural;
    ural::copy_if(xs, r_ural | ural::back_inserter, pred);

    BOOST_CHECK_EQUAL_COLLECTIONS(r_std.begin(), r_std.end(),
                                  r_ural.begin(), r_ural.end());
}

BOOST_AUTO_TEST_CASE(filtered_getters_test)
{
    typedef int Type;
    std::vector<Type> const xs = {25, -15, 5, -5, 15};
    std::vector<Type> const xs_1 = xs;

    auto const pred_1 = +[](Type i){return !(i<0);};
    auto const pred_2 = +[](Type i){return !(i<=0);};

    auto s1 = xs | ural::filtered(pred_1);
    auto s11 = xs | ural::filtered(pred_1);
    auto s2 = xs | ural::filtered(pred_2);
    auto s3 = xs_1 | ural::filtered(pred_1);

    BOOST_CHECK(pred_1 == s1.predicate());
    BOOST_CHECK(pred_2 == s2.predicate());

    BOOST_CHECK(s1 == s1);
    BOOST_CHECK(s1 == s11);
    BOOST_CHECK(s2 == s2);
    BOOST_CHECK(s1 != s2);
    BOOST_CHECK(s1 != s3);
}

BOOST_AUTO_TEST_CASE(copy_backward_test)
{
    std::vector<int> x_std = {1, 2, 3, 4, 5};
    std::vector<int> x_ural = x_std;

    std::copy_backward(x_std.begin(), x_std.end() - 1, x_std.end());

    auto src = ural::make_iterator_sequence(x_ural.begin(), x_ural.end() - 1);
    ural::copy_backward(src, x_ural);

    BOOST_CHECK_EQUAL_COLLECTIONS(x_std.begin(), x_std.end(),
                                  x_ural.begin(), x_ural.end());
}

// 25.3.2 Перемещение
BOOST_AUTO_TEST_CASE(moved_test)
{
    typedef std::unique_ptr<int> Type;

    std::vector<int> const ys = {25, -15, 5, -5, 15};
    std::vector<Type> xs1;
    std::vector<Type> xs2;

    for(auto & y : ys)
    {
        auto constexpr f = ural::to_unique_ptr;

        xs1.emplace_back(f(y));
        xs2.emplace_back(f(y));
    }

    std::vector<Type> r_std;
    std::vector<Type> r_ural;

    std::move(xs1.begin(), xs1.end(), std::back_inserter(r_std));

    ural::copy(xs2 | ural::moved, r_ural | ural::back_inserter);

    BOOST_CHECK_EQUAL(r_std.size(), r_ural.size());
    BOOST_CHECK(std::none_of(r_ural.begin(), r_ural.end(),
                             [](Type const & x) {return !x;}));

    BOOST_CHECK(ural::equal(r_std, r_ural, ural::equal_by(ural::dereference<>{})));
}

// move
BOOST_AUTO_TEST_CASE(eager_move_test)
{
    std::vector<std::string> src = {"Alpha", "Beta", "Gamma"};

    auto const src_old = src;

    std::vector<std::string> xs;

    ural::move(src, xs | ural::back_inserter);

    BOOST_CHECK_EQUAL_COLLECTIONS(src_old.begin(), src_old.end(),
                                  xs.begin(), xs.end());

    for(auto const & s : src)
    {
        BOOST_CHECK(s.empty());
    }
}

// @todo move_backward

// 25.3.3 Обмен интервалов
BOOST_AUTO_TEST_CASE(swap_ranges_test)
{
    std::vector<int> const x1 = {1, 2, 3, 4, 5};
    std::list<int> const x2   = {-1, -2, -3, -4, -5};

    auto y1 = x1;
    auto y2 = x2;

    auto r = ural::swap_ranges(y1, y2);

    BOOST_CHECK(!r[ural::_1] || !r[ural::_2]);
    BOOST_CHECK_EQUAL(ural::size(r[ural::_1]), ural::size(r[ural::_2]));

    BOOST_CHECK_EQUAL_COLLECTIONS(y1.begin(), y1.end(), x2.begin(), x2.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(y2.begin(), y2.end(), x1.begin(), x1.end());
}

// @todo Аналог iter_swap

// 25.3.4 Преобразование
BOOST_AUTO_TEST_CASE(transform_test)
{
    std::string const s("hello");
    std::string x_std;

    auto f = std::ptr_fun<int, int>(std::toupper);

    std::transform(s.begin(), s.end(), std::back_inserter(x_std), f);

    auto const x_ural
        = ural::make_transform_sequence(f, s)
        | ural::to_container<std::basic_string>{};

    BOOST_CHECK_EQUAL_COLLECTIONS(x_std.begin(), x_std.end(),
                                  x_ural.begin(), x_ural.end());
}

BOOST_AUTO_TEST_CASE(eager_transform_test)
{
    std::string const s("hello");
    std::string x_std;
    std::string x_ural;

    auto f = std::ptr_fun<int, int>(std::toupper);

    std::transform(s.begin(), s.end(), std::back_inserter(x_std), f);

    // @todo Проверка возвращаемого значения
    ural::transform(s, x_ural | ural::back_inserter, f);

    BOOST_CHECK_EQUAL_COLLECTIONS(x_std.begin(), x_std.end(),
                                  x_ural.begin(), x_ural.end());
}

BOOST_AUTO_TEST_CASE(transform_2_test)
{
    std::vector<int> const x1 = {1, 20, 30, 40, 50};
    std::vector<int> const x2 = {10, 2, 30, 4, 5};

    std::vector<bool> z_std;
    std::vector<bool> z_ural;

    std::less_equal<int> constexpr f_std{};
    ural::less_equal<> constexpr f_ural{};

    std::transform(x1.begin(), x1.end(), x2.begin(),
                   std::back_inserter(z_std), f_std);

    auto seq = ural::make_transform_sequence(f_ural, x1, x2);
    ural::copy(std::move(seq), std::back_inserter(z_ural));

    BOOST_CHECK_EQUAL_COLLECTIONS(z_std.begin(), z_std.end(),
                                  z_ural.begin(), z_ural.end());
}

BOOST_AUTO_TEST_CASE(transform_2_test_eager)
{
    std::forward_list<int> const x1 = {1, 20, 30, 40, 50};
    std::list<int> const x2 = {10, 2, 30, 4, 5};

    std::vector<bool> z_std;
    std::vector<bool> z_ural;

    std::less_equal<int> constexpr f_std{};
    ural::less_equal<> constexpr f_ural{};

    std::transform(x1.begin(), x1.end(), x2.begin(),
                   std::back_inserter(z_std), f_std);

    ural::transform(x1, x2, std::back_inserter(z_ural), f_ural);

    BOOST_CHECK_EQUAL_COLLECTIONS(z_std.begin(), z_std.end(),
                                  z_ural.begin(), z_ural.end());
}

// 25.3.5 Замена
BOOST_AUTO_TEST_CASE(replace_test)
{
    std::vector<int> s_std = {5, 7, 4, 2, 8, 6, 1, 9, 0, 3};
    std::vector<int> s_ural = s_std;

    auto const old_value = 8;
    auto const new_value = 88;

    std::replace(s_std.begin(), s_std.end(), old_value, new_value);
    ural::replace(s_ural, old_value, new_value);

    BOOST_CHECK_EQUAL_COLLECTIONS(s_std.begin(), s_std.end(),
                                  s_ural.begin(), s_ural.end());
}

BOOST_AUTO_TEST_CASE(replace_if_test)
{
    std::array<int, 10> const s{5, 7, 4, 2, 8, 6, 1, 9, 0, 3};

    auto x_std = s;
    auto x_ural = s;

    auto pred = [](int x) {return x < 5;};
    auto const new_value = 55;

    std::replace_if(x_std.begin(), x_std.end(), pred, new_value);
    ural::replace_if(x_ural, pred, new_value);

    BOOST_CHECK_EQUAL_COLLECTIONS(x_std.begin(), x_std.end(),
                                  x_ural.begin(), x_ural.end());
}

BOOST_AUTO_TEST_CASE(replace_sequence_test)
{
    std::vector<int> s_std = {5, 7, 4, 2, 8, 6, 1, 9, 0, 3};
    std::vector<int> s_ural = s_std;

    auto const old_value = 8;
    auto const new_value = 88;

    std::replace(s_std.begin(), s_std.end(), old_value, new_value);
    ural::copy(ural::make_replace_sequence(s_ural, old_value, new_value), s_ural);

    BOOST_CHECK_EQUAL_COLLECTIONS(s_std.begin(), s_std.end(),
                                  s_ural.begin(), s_ural.end());
}

BOOST_AUTO_TEST_CASE(replace_sequence_if_test)
{
    std::array<int, 10> const s{5, 7, 4, 2, 8, 6, 1, 9, 0, 3};

    auto x_std = s;
    std::vector<int> x_ural;

    auto pred = [](int x) {return x < 5;};
    auto const new_value = 55;

    std::replace_if(x_std.begin(), x_std.end(), pred, new_value);
    ural::copy(ural::make_replace_if_sequence(s, pred, new_value),
               x_ural | ural::back_inserter);

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

    std::fill_n(v_std.begin(), n, value);
    auto r = ural::fill(v_ural | ural::taken(n), value);

    BOOST_CHECK_EQUAL(n, r.base().traversed_front().size());
    BOOST_CHECK_EQUAL(v_std.size() - n, r.base().size());

    BOOST_CHECK_EQUAL_COLLECTIONS(v_std.begin(), v_std.end(),
                                  v_ural.begin(), v_ural.end());
}

// 25.3.7 Порождение
BOOST_AUTO_TEST_CASE(generate_test)
{
    std::vector<int> r_std(5);
    auto r_ural = r_std;

    auto counter = int{0};
    auto gen = [&]{ return counter++; };
    std::generate(r_std.begin(), r_std.end(), gen);

    counter = 0;

    ural::copy(ural::make_generator_sequence(gen), r_ural);

    BOOST_CHECK_EQUAL_COLLECTIONS(r_std.begin(), r_std.end(),
                                  r_ural.begin(), r_ural.end());
}

BOOST_AUTO_TEST_CASE(generate_n_test)
{
    auto const n = int{5};

    std::vector<int> r_std;
    auto r_ural = r_std;

    auto counter = int{0};
    auto gen = [&]{ return counter++; };
    std::generate_n(r_std | ural::back_inserter, n, gen);

    counter = 0;
    ural::copy(ural::make_generator_sequence(gen) | ural::taken(n),
               r_ural | ural::back_inserter);

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
    BOOST_CHECK_EQUAL(r_ural.end() - s_ural.begin(), s_ural.size());

    BOOST_CHECK_EQUAL_COLLECTIONS(r_ural.traversed_begin(), r_ural.begin(),
                                  s_std.begin(), r_std);
}

BOOST_AUTO_TEST_CASE(remove_sequence_test)
{
    std::string s_std = "Text with some   spaces";
    auto s_ural = s_std;

    auto const to_remove = ' ';

    s_std.erase(std::remove(s_std.begin(), s_std.end(), to_remove),
                s_std.end());

    auto s = ural::make_remove_sequence(s_ural, to_remove);
    auto r = ural::copy(s, s_ural)[ural::_2];
    s_ural.erase(r.begin(), r.end());

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

BOOST_AUTO_TEST_CASE(remove_if_sequence_test)
{
    std::string s_std = "Text\n with\tsome \t  whitespaces\n\n";
    auto s_ural = s_std;

    auto pred = [](char x){return std::isspace(x);};

    s_std.erase(std::remove_if(s_std.begin(), s_std.end(), pred), s_std.end());

    auto s = ural::make_remove_if_sequence(s_ural, pred);
    auto r = ural::copy(s, s_ural)[ural::_2];
    s_ural.erase(r.begin(), r.end());

    BOOST_CHECK_EQUAL(s_std, s_ural);
}

// 25.3.9 Удаление повторов
BOOST_AUTO_TEST_CASE(eager_unique_test)
{
    std::forward_list<int> s_std{1, 2, 2, 2, 3, 3, 2, 2, 1};
    auto s_ural = s_std;

    auto const r_std = std::unique(s_std.begin(), s_std.end());
    auto const r_ural = ural::unique(s_ural);

    BOOST_CHECK_EQUAL_COLLECTIONS(s_std.begin(), r_std,
                                  r_ural.traversed_begin(), r_ural.begin());
}

BOOST_AUTO_TEST_CASE(unique_test)
{
    std::forward_list<int> v1{1, 2, 2, 2, 3, 3, 2, 2, 1};
    auto v2 = v1;

    auto const last = std::unique(v1.begin(), v1.end());
    std::forward_list<int> r_std(v1.begin(), last);

    auto const r_ural
        = v2 | ural::uniqued | ural::to_container<std::forward_list>{};

    BOOST_CHECK_EQUAL_COLLECTIONS(r_std.begin(), r_std.end(), r_ural.begin(),
                                  r_ural.end());
}

BOOST_AUTO_TEST_CASE(unique_test_custom_predicate)
{
    std::string const src = "The      string    with many       spaces!";

    auto const pred = [](char c1, char c2){ return c1 == ' ' && c2 == ' '; };

    std::string s_std;
    std::unique_copy(src.begin(), src.end(), std::back_inserter(s_std), pred);

    auto const s_ural
        = src | ural::uniqued(pred) | ural::to_container<std::basic_string>{};

    BOOST_CHECK_EQUAL(s_std, s_ural);
}

BOOST_AUTO_TEST_CASE(unique_sequence_move_only)
{
    std::forward_list<int> src{1, 2, 2, 2, 3, 3, 2, 2, 1};

    typedef std::unique_ptr<int> Pointer;

    std::vector<Pointer> v1;
    std::vector<Pointer> v2;

    for(auto & y : src)
    {
        auto constexpr f = ural::to_unique_ptr;

        v1.emplace_back(f(y));
        v2.emplace_back(f(y));
    }

    auto const eq = [](Pointer const & x, Pointer const & y)
    {
        return (!!x && !!y) ? (*x == *y) : (!x && !y);
    };

    auto const last = std::unique(v1.begin(), v1.end(), eq);

    auto const r_ural
        = v2 | ural::uniqued(eq) | ural::moved | ural::to_container<std::vector>{};

    BOOST_CHECK_EQUAL(last - v1.begin(), r_ural.end() - r_ural.begin());

    for(auto i : ural::numbers(0, last - v1.begin()))
    {
        BOOST_CHECK_EQUAL(*v1[i], *r_ural[i]);
    }
}

// 25.3.10 Обращение
BOOST_AUTO_TEST_CASE(reverse_test)
{
    std::list<int> const src = {1, 2, 3, 4, 5, 6};

    auto x_std = src;
    auto x_ural = src;

    std::reverse(x_std.begin(), x_std.end());
    ural::reverse(x_ural);

    BOOST_CHECK(x_std == x_ural);

    BOOST_CHECK_EQUAL_COLLECTIONS(x_std.begin(), x_std.end(),
                                  x_ural.begin(), x_ural.end());
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

BOOST_AUTO_TEST_CASE(reversed_copy_test)
{
    std::list<int> const src = {1, 2, 3, 4, 5, 6};

    std::list<int> r_std;

    std::reverse_copy(src.begin(), src.end(), std::back_inserter(r_std));

    auto const r_ural
        = src | ural::reversed | ural::to_container<std::list>{};

    BOOST_CHECK_EQUAL_COLLECTIONS(r_std.begin(), r_std.end(),
                                  r_ural.begin(), r_ural.end());
}

// 25.3.11 Вращение
BOOST_AUTO_TEST_CASE(rotate_test)
{
    std::vector<int> const v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    for(auto i : ural::numbers(0, v.size()))
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
            BOOST_CHECK_EQUAL(i, ural::size(r_ural));
            BOOST_CHECK_EQUAL(v.size() - i, ural::size(r_ural.traversed_front()));
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

    for(auto i : ural::numbers(0, src.size()))
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

    for(auto i : ural::numbers(0, src.size()))
    {
        std::vector<int> d_std(src.size() + 5);
        std::vector<int> d_ural(src.size() + 5);

        std::rotate_copy(src.begin(), src.begin() + i, src.end(), d_std.begin());

        auto s = ural::sequence(src);
        s += i;

        auto r_ural = ural::rotate_copy(s, d_ural);

        BOOST_CHECK_EQUAL(src.size(), r_ural[ural::_2].traversed_front().size());
        BOOST_CHECK_EQUAL(d_ural.size() - src.size(), r_ural[ural::_2].size());

        BOOST_CHECK_EQUAL(s.size(), r_ural[ural::_1].traversed_front().size());
        BOOST_CHECK_EQUAL(s.traversed_front().size(), r_ural[ural::_1].size());

        BOOST_CHECK_EQUAL_COLLECTIONS(d_std.begin(), d_std.end(),
                                      d_ural.begin(), d_ural.end());
    }
}

// @todo 25.3.12 Тусовка

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
    typedef std::forward_list<int> Container;
    Container const xs = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    auto ys = xs;

    typedef Container::value_type Element;

    auto const is_even = [](Element x) { return x % 2 == 0;};

    auto r_ural = ural::partition(ys, is_even);

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
    std::vector<int> const src {1, 2, 3, 4, 5, 6, 7, 8, 9};
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

BOOST_AUTO_TEST_CASE(partition_copy_test)
{
    std::array<int, 10> const src = {1,2,3,4,5,6,7,8,9,10};
    std::list<int> true_sink;
    std::forward_list<int> false_sink;

    auto const pred = [] (int x) {return x % 2 == 0;};

    ural::partition_copy(src, true_sink | ural::back_inserter,
                         std::front_inserter(false_sink), pred);

    BOOST_CHECK(ural::all_of(true_sink, pred));
    BOOST_CHECK(ural::none_of(false_sink, pred));

    for(auto const & x : src)
    {
        BOOST_CHECK(!!ural::find(true_sink, x) || !!ural::find(false_sink, x));
    }
}

BOOST_AUTO_TEST_CASE(partition_copy_return_value_test)
{
    std::array<int, 10> const src = {1,2,3,4,5,6,7,8,9,10};
    std::vector<int> true_sink(src.size(), -1);
    std::vector<int> false_sink(src.size(), -1);

    auto const pred = [] (int x) {return x % 2 == 0;};

    auto r = ural::partition_copy(src, true_sink, false_sink, pred);

    BOOST_CHECK(!r[ural::_1]);
    BOOST_CHECK_EQUAL(src.size(), r[ural::_2].traversed_front().size()
                                  + r[ural::_3].traversed_front().size());

    BOOST_CHECK(ural::all_of(r[ural::_2].traversed_front(), pred));
    BOOST_CHECK(ural::none_of(r[ural::_3].traversed_front(), pred));
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
    ural::sort(x_ural);

    BOOST_CHECK_EQUAL_COLLECTIONS(x_std.begin(), x_std.end(),
                                  x_ural.begin(), x_ural.end());
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
    ural::stable_sort(x_ural);

    BOOST_CHECK_EQUAL_COLLECTIONS(x_std.begin(), x_std.end(),
                                  x_ural.begin(), x_ural.end());
}

BOOST_AUTO_TEST_CASE(partial_sort_test)
{
    std::array<int, 10> const xs {5, 7, 4, 2, 8, 6, 1, 9, 0, 3};
    auto ys = xs;

    ural::partial_sort(ys, 3);

    BOOST_CHECK(std::is_sorted(ys.begin(), ys.begin() + 3));
    BOOST_CHECK(ural::is_permutation(xs, ys));
    BOOST_CHECK(std::all_of(ys.begin() + 3, ys.end(),
                            [=](int x) {return x >= ys[2];}));
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

    std::vector<int> r2_std{10, 11, 12, 13, 14, 15, 16};
    std::vector<int> r2_ural{10, 11, 12, 13, 14, 15, 16};

    auto pos_std = std::partial_sort_copy(v0.begin(), v0.end(),
                                          r1_std.begin(), r1_std.end());
    auto pos_ural = ural::partial_sort_copy(v0, r1_ural);

    BOOST_CHECK_EQUAL(r1_std.end() - pos_std, pos_ural.size());
    BOOST_CHECK_EQUAL(pos_std - r1_std.begin(), pos_ural.traversed_front().size());

    BOOST_CHECK_EQUAL_COLLECTIONS(r1_std.begin(), r1_std.end(),
                                  r1_ural.begin(), r1_ural.end());

    pos_std = std::partial_sort_copy(v0.begin(), v0.end(),
                                     r2_std.begin(), r2_std.end(),
                                     std::greater<int>());
    pos_ural = ural::partial_sort_copy(v0, r2_ural, ural::greater<>());

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
        auto n_std = nums.end() - std::is_sorted_until(nums.begin(), nums.end());
        auto n_ural = ural::is_sorted_until(nums).size();
        BOOST_CHECK_EQUAL(n_std, n_ural);
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

    ural::nth_element(s_ural);

    BOOST_CHECK(ural::is_permutation(x_std, x_ural));
    BOOST_CHECK_EQUAL(x_std[pos_1], x_ural[pos_1]);

    BOOST_CHECK(ural::is_permutation(s_std, s_ural));
    BOOST_CHECK(ural::is_permutation(s_std.traversed_front(),
                                     s_ural.traversed_front()));
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
    std::vector<int> const v1{1,2,3,4,5,6,7,8};
    std::vector<int> const v2{        5,  7,  9,10};

    std::vector<int> std_merge;
    std::merge(v1.begin(), v1.end(), v2.begin(), v2.end(),
               std::back_inserter(std_merge));

    auto const ural_merge
        = ural::merge(v1, v2) | ural::to_container<std::vector>{};

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
    ural::inplace_merge(s);

    BOOST_CHECK_EQUAL_COLLECTIONS(x_std.begin(), x_std.end(),
                                  x_ural.begin(), x_ural.end());
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
    ural::inplace_merge(s);

    BOOST_CHECK_EQUAL_COLLECTIONS(x_std.begin(), x_std.end(),
                                  x_ural.begin(), x_ural.end());
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
                                         v0.begin(), v0.end(), cmp_nocase);        bool const r_ural = ural::includes(s, v0, cmp_nocase);
        BOOST_CHECK_EQUAL(r_std, r_ural);
    }
}

BOOST_AUTO_TEST_CASE(set_union_test)
{
    std::vector<int> v1 = {1, 2, 3, 4, 5};
    std::vector<int> v2 = {      3, 4, 5, 6, 7};

    std::vector<long> r_std;
    std::set_union(v1.begin(), v1.end(), v2.begin(), v2.end(),
                   std::back_inserter(r_std));

    auto const r_ural
        = ural::set_union(v1, v2) | ural::to_container<std::vector>{};

    BOOST_CHECK_EQUAL_COLLECTIONS(r_std.begin(), r_std.end(),
                                  r_ural.begin(), r_ural.end());
}

BOOST_AUTO_TEST_CASE(set_intersection_test)
{
    std::vector<int> const v1{1,2,3,4,5,6,7,8};
    std::vector<int> const v2{        5,  7,  9,10};

    std::vector<int> std_intersection;
    std::set_intersection(v1.begin(), v1.end(),
                          v2.begin(), v2.end(),
                          std::back_inserter(std_intersection));

    auto const ural_intersection
        = ural::set_intersection(v1, v2) | ural::to_container<std::vector>{};

    BOOST_CHECK_EQUAL_COLLECTIONS(std_intersection.begin(),
                                  std_intersection.end(),
                                  ural_intersection.begin(),
                                  ural_intersection.end());
}

BOOST_AUTO_TEST_CASE(set_difference_test)
{
    std::vector<int> v1 {1, 2, 5, 5, 5, 9};
    std::vector<int> v2 {2, 5, 7};

    std::vector<int> std_diff;
    std::set_difference(v1.begin(), v1.end(), v2.begin(), v2.end(),
                        std::back_inserter(std_diff));
    auto const ural_diff
        = ural::set_difference(v1, v2) | ural::to_container<std::vector>{};

    BOOST_CHECK_EQUAL_COLLECTIONS(std_diff.begin(), std_diff.end(),
                                  ural_diff.begin(), ural_diff.end());
}

BOOST_AUTO_TEST_CASE(set_symmetric_difference_test)
{
    std::vector<int> const v1{1,2,3,4,5,6,7,8     };
    std::vector<int> const v2{        5,  7,  9,10};

    std::vector<int> std_intersection;
    std::set_symmetric_difference(v1.begin(), v1.end(), v2.begin(), v2.end(),
                                  std::back_inserter(std_intersection));

    auto const ural_intersection
        = ural::set_symmetric_difference(v1, v2) | ural::to_container<std::vector>{};

    BOOST_CHECK_EQUAL_COLLECTIONS(std_intersection.begin(), std_intersection.end(),
                                  ural_intersection.begin(), ural_intersection.end());
}

// 25.4.6 Операции с бинарными кучами
BOOST_AUTO_TEST_CASE(push_heap_test)
{
    std::vector<int> v { 3, 1, 4, 1, 5, 9 };

    for(auto i : ural::numbers(0, v.size()))
    {
        BOOST_CHECK(std::is_heap(v.begin(), v.begin() + i));
        ural::push_heap(ural::make_iterator_sequence(v.begin(), v.begin()+i+1));
    }
    BOOST_CHECK(std::is_heap(v.begin(), v.end()));
}

BOOST_AUTO_TEST_CASE(pop_heap_test)
{
    std::vector<int> v { 3, 1, 4, 1, 5, 9 };
    ural::make_heap(v);

    while(v.empty() == false)
    {
        auto const old_top = v.front();
        ural::pop_heap(v);

        BOOST_CHECK_EQUAL(old_top, v.back());
        v.pop_back();
        BOOST_CHECK(std::is_heap(v.begin(), v.end()));
    }
}

BOOST_AUTO_TEST_CASE(make_heap_test)
{
    std::vector<int> v { 3, 1, 4, 1, 5, 9 };

    auto cmp = ural::functor_tracer<ural::less<decltype(v.front())>>{};
    cmp.reset_calls();

    ural::make_heap(v, cmp);

    BOOST_CHECK(std::is_heap(v.begin(), v.end()));

    BOOST_CHECK_GE(3*v.size(), cmp.calls());
}

BOOST_AUTO_TEST_CASE(make_heap_odd_size_test)
{
    std::vector<int> v { 3, 1, 4, 1, 5, 9, 2};

    auto cmp = ural::functor_tracer<ural::less<decltype(v.front())>>{};
    cmp.reset_calls();

    ural::make_heap(v, cmp);

    BOOST_CHECK(std::is_heap(v.begin(), v.end()));

    BOOST_CHECK_GE(3*v.size(), cmp.calls());
}

BOOST_AUTO_TEST_CASE(sort_heap_test)
{
    std::vector<int> v { 3, 1, 4, 1, 5, 9 };

    ural::make_heap(v);
    ural::sort_heap(v);

    BOOST_CHECK(std::is_sorted(v.begin(), v.end()));
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
BOOST_AUTO_TEST_CASE(min_element_test)
{
    std::vector<int> const v{3, 1, 4, 1, 5, 9, 2, 6, 5};

    auto std_result = std::min_element(std::begin(v), std::end(v));
    auto ural_result = ural::min_element(v);

    BOOST_CHECK_EQUAL(std::distance(std_result, v.end()),
                      ural_result.size());
    BOOST_CHECK(!!ural_result);
    BOOST_CHECK_EQUAL(*std_result, *ural_result);
}

BOOST_AUTO_TEST_CASE(max_element_test)
{
    std::vector<int> const v{ 3, 1, -14, 1, 5, 9 };
    auto std_result = std::max_element(v.begin(), v.end());
    auto ural_result = ural::max_element(v);

    BOOST_CHECK_EQUAL(std::distance(std_result, v.end()), ural_result.size());
}

BOOST_AUTO_TEST_CASE(max_element_test_custom_compare)
{
    auto abs_compare = [](int a, int b) {return (std::abs(a) < std::abs(b));};

    std::vector<int> const v{ 3, 1, -14, 1, 5, 9 };
    auto std_result = std::max_element(v.begin(), v.end(), +abs_compare);
    auto ural_result = ural::max_element(v, +abs_compare);

    BOOST_CHECK_EQUAL(std::distance(std_result, v.end()), ural_result.size());
}

#include <ural/math.hpp>

BOOST_AUTO_TEST_CASE(max_element_using_compare_by)
{
    auto const sq_cmp = ural::compare_by(ural::square);

    static_assert(std::is_empty<decltype(sq_cmp)>::value, "Must be empty!");

    std::vector<int> const v{ 3, 1, -14, 1, 5, 9 };
    auto std_result = std::max_element(v.begin(), v.end(), sq_cmp);
    auto ural_result = ural::max_element(v, sq_cmp);

    BOOST_CHECK_EQUAL(std::distance(std_result, v.end()), ural_result.size());
}

BOOST_AUTO_TEST_CASE(minmax_element_test)
{
    std::vector<int> const v{ 3, 1, -14, 1, 5, 9 };
    auto std_result = std::minmax_element(v.begin(), v.end());
    auto ural_result = ural::minmax_element(v);

    BOOST_CHECK_EQUAL(std::distance(std_result.first, v.end()),
                      ural_result[ural::_1].size());
    BOOST_CHECK_EQUAL(std::distance(std_result.second, v.end()),
                      ural_result[ural::_2].size());
}

// 25.4.8 Лексикографическое сравнение
BOOST_AUTO_TEST_CASE(lexicographical_compare_test)
{
    BOOST_CHECK_EQUAL(false, ural::lexicographical_compare("", ""));

    BOOST_CHECK_EQUAL(true, ural::lexicographical_compare("ab", "abc"));
    BOOST_CHECK_EQUAL(false, ural::lexicographical_compare("abc", "ab"));

    BOOST_CHECK_EQUAL(true, ural::lexicographical_compare("abcd", "abed"));
    BOOST_CHECK_EQUAL(false, ural::lexicographical_compare("abed", "abcd"));
}

// 25.4.9 Порождение перестановок
BOOST_AUTO_TEST_CASE(next_permutation_test)
{
    typedef std::string String;
    String x {"1234"};
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

    BOOST_CHECK_EQUAL_COLLECTIONS(r_std.begin(), r_std.end(),
                                  r_ural.begin(), r_ural.end());
}

BOOST_AUTO_TEST_CASE(prev_permutation_test)
{
    typedef std::string String;
    String x {"4321"};
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

    BOOST_CHECK_EQUAL_COLLECTIONS(r_std.begin(), r_std.end(),
                                  r_ural.begin(), r_ural.end());
}

// Комбинирование вызовов модифицирующих алгоритмов с erase
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
