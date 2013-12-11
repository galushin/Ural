#include <boost/test/unit_test.hpp>

#include <ural/algorithm.hpp>

// @todo Удалить
#include <ural/sequence/transform.hpp>
#include <ural/sequence/set_operations.hpp>
#include <ural/utility/tracers.hpp>

BOOST_AUTO_TEST_CASE(all_of_test)
{
    std::vector<int> v(10, 2);
    std::partial_sum(v.cbegin(), v.cend(), v.begin());

    auto const pred = [](int i){ return i % 2 == 0; };

    BOOST_CHECK(std::all_of(v.cbegin(), v.cend(), pred));
    BOOST_CHECK(ural::all_of(v, pred));

    v[1] = 1;

    BOOST_CHECK(!std::all_of(v.cbegin(), v.cend(), pred));
    BOOST_CHECK(!ural::all_of(v, pred));
}

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
    BOOST_CHECK(std::find(v.begin(), v.end(), value) == s.front_iterator());
}

BOOST_AUTO_TEST_CASE(equal_test)
{
     std::string const x1("radar");
     std::string const y1("hello");

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

BOOST_AUTO_TEST_CASE(transform_test)
{
    std::string const s("hello");
    std::string x_std;
    std::string x_ural;

    std::transform(s.begin(), s.end(), std::back_inserter(x_std),
                   std::ptr_fun<int, int>(std::toupper));
    auto seq = ural::transform(s, std::ptr_fun<int, int>(std::toupper));
    ural::copy(seq, std::back_inserter(x_ural));

    BOOST_CHECK_EQUAL_COLLECTIONS(x_std.begin(), x_std.end(),
                                  x_ural.begin(), x_ural.end());
}

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

BOOST_AUTO_TEST_CASE(includes_test)
{
    std::vector<std::string> vs{"abcfhx", "abc", "ac", "g", "acg", {}};

    for(size_t i = 0; i != vs.size(); ++ i)
    for(size_t j = 0; j != vs.size(); ++ j)
    {
        bool const r_std = std::includes(vs[i].begin(), vs[i].end(),
                                         vs[j].begin(), vs[j].end());        bool const r_ural = ural::includes(vs[i], vs[j]);
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

    for(size_t i = 0; i != vs.size(); ++ i)
    {
        bool const r_std = std::includes(vs[i].begin(), vs[i].end(),
                                         v0.begin(), v0.end(), cmp_nocase);        bool const r_ural = ural::includes(vs[i], v0, cmp_nocase);
        BOOST_CHECK_EQUAL(r_std, r_ural);
    }
}

BOOST_AUTO_TEST_CASE(merge_test)
{
    std::vector<int> const v1{1,2,3,4,5,6,7,8};
    std::vector<int> const v2{        5,  7,  9,10};

    std::vector<int> std_merge;
    std::merge(v1.begin(), v1.end(), v2.begin(), v2.end(),
               std::back_inserter(std_merge));

    std::vector<int> ural_merge;
    ural::copy(ural::merge(v1, v2), std::back_inserter(ural_merge));

    BOOST_CHECK_EQUAL_COLLECTIONS(std_merge.begin(), std_merge.end(),
                                  ural_merge.begin(), ural_merge.end());
}

BOOST_AUTO_TEST_CASE(set_intersection_test)
{
    std::vector<int> const v1{1,2,3,4,5,6,7,8};
    std::vector<int> const v2{        5,  7,  9,10};

    std::vector<int> std_intersection;
    std::set_intersection(v1.begin(), v1.end(),
                          v2.begin(), v2.end(),
                          std::back_inserter(std_intersection));

    std::vector<int> ural_intersection;
    ural::copy(ural::set_intersection(v1, v2),
               std::back_inserter(ural_intersection));

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
    std::vector<int> ural_diff;
    ural::copy(ural::set_difference(v1, v2), std::back_inserter(ural_diff));

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

    std::vector<int> ural_intersection;
    ural::copy(ural::set_symmetric_difference(v1, v2),
               std::back_inserter(ural_intersection));

    BOOST_CHECK_EQUAL_COLLECTIONS(std_intersection.begin(), std_intersection.end(),
                                  ural_intersection.begin(), ural_intersection.end());
}

BOOST_AUTO_TEST_CASE(set_union_test)
{
    std::vector<int> v1 = {1, 2, 3, 4, 5};
    std::vector<int> v2 = {      3, 4, 5, 6, 7};

    std::vector<long> r_std;
    std::set_union(v1.begin(), v1.end(), v2.begin(), v2.end(),
                   std::back_inserter(r_std));

    std::vector<int> r_ural;
    ural::copy(ural::set_union(v1, v2), std::back_inserter(r_ural));

    BOOST_CHECK_EQUAL_COLLECTIONS(r_std.begin(), r_std.end(),
                                  r_ural.begin(), r_ural.end());
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

BOOST_AUTO_TEST_CASE(push_heap_test)
{
    std::vector<int> v { 3, 1, 4, 1, 5, 9 };

    for(size_t i = 0; i != v.size(); ++ i)
    {
        BOOST_CHECK(std::is_heap(v.begin(), v.begin() + i));
        ural::push_heap(ural::make_iterator_sequence(v.begin(), v.begin() + i+1));
    }
    BOOST_CHECK(std::is_heap(v.begin(), v.end()));
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
    std::vector<int> nums = {1, 1, 3, 4, 5, 9};

    do
    {
        auto n_std = nums.end() - std::is_sorted_until(nums.begin(), nums.end());
        auto n_ural = ural::is_sorted_until(nums).size();
        BOOST_CHECK_EQUAL(n_std, n_ural);
    }
    while(std::next_permutation(nums.begin(), nums.end()));
}

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

BOOST_AUTO_TEST_CASE(lexicographical_compare_test)
{
    BOOST_CHECK_EQUAL(false, ural::lexicographical_compare("", ""));

    BOOST_CHECK_EQUAL(true, ural::lexicographical_compare("ab", "abc"));
    BOOST_CHECK_EQUAL(false, ural::lexicographical_compare("abc", "ab"));

    BOOST_CHECK_EQUAL(true, ural::lexicographical_compare("abcd", "abed"));
    BOOST_CHECK_EQUAL(false, ural::lexicographical_compare("abed", "abcd"));
}
