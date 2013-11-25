#include <boost/test/unit_test.hpp>

#include <ural/algorithm.hpp>

// @todo Удалить
#include <ural/sequence/transform.hpp>
#include <ural/sequence/set_operations.hpp>
#include <ural/utility/tracers.hpp>

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
        std::ostringstream os;
        std::copy(v.begin(), v.end(), std::ostream_iterator<int>(os));
        BOOST_CHECK_EQUAL(std::is_heap(v.begin(), v.end()), ural::is_heap(v));
    }
    while(std::next_permutation(v.begin(), v.end()));
}
