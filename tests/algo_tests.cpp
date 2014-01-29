#include <boost/test/unit_test.hpp>
#include <boost/mpl/list.hpp>

#include <ural/algorithm.hpp>
#include <ural/memory.hpp>

// @todo ������ ����������� ������ <ural/algorithms.hpp>
#include <ural/sequence/all.hpp>
#include <ural/utility/tracers.hpp>

#include <forward_list>
#include <list>
#include <vector>

typedef boost::mpl::list<std::forward_list<int>,
                         std::list<int>,
                         std::vector<int>> Sources;

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

BOOST_AUTO_TEST_CASE(count_if_test)
{
    std::vector<int> const data = { 1, 2, 3, 4, 4, 3, 7, 8, 9, 10 };

    auto const pred = [](int i) {return i % 3 == 0;};

    auto const n_std
        = std::count_if(data.begin(), data.end(), pred);

    auto const n_ural = ural::count_if(data, pred);

    BOOST_CHECK_EQUAL(n_std, n_ural);
}

BOOST_AUTO_TEST_CASE(mismatch_test)
{
    std::string const x("abca");
    std::string const y("aba");

    auto const r_std = std::mismatch(x.begin(), x.end(), y.begin());
    auto const r_ural = ural::mismatch(x, y);

    BOOST_CHECK_EQUAL(std::distance(r_std.first, x.end()), r_ural[ural::_1].size());
    BOOST_CHECK_EQUAL(std::distance(r_std.second, y.end()), r_ural[ural::_2].size());
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

BOOST_AUTO_TEST_CASE(adjacent_find_test)
{
    std::vector<int> v1{0, 1, 2, 3, 40, 40, 41, 41, 5};

    auto r_std = std::adjacent_find(v1.begin(), v1.end());
    auto r_ural = ural::adjacent_find(v1);

    BOOST_CHECK_EQUAL(std::distance(r_std, v1.end()), r_ural.size());
    BOOST_CHECK(!!r_ural);
    BOOST_CHECK_EQUAL(*r_std, *r_ural);
}

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

// @todo ����� �� (� ������������� ��) search_n �������� ����� search �
// ����-������������������
BOOST_AUTO_TEST_CASE(search_n_test)
{
    const std::string xs = "1001010100010101001010101";

    for(size_t i = 0; i < 5; ++ i)
    {
        BOOST_CHECK_EQUAL(std::search_n(xs.begin(), xs.end(), i, '0') == xs.end(),
                          !ural::search_n(xs, i, '0'));
    }
}

// �������������� ������������������ ���������
BOOST_AUTO_TEST_CASE(copy_test)
{
    std::vector<int> const xs = {1, 2, 3, 4};

    std::vector<int> x1;

    ural::copy(xs, std::back_inserter(x1));

    BOOST_CHECK_EQUAL_COLLECTIONS(xs.begin(), xs.end(), x1.begin(), x1.end());
}

BOOST_AUTO_TEST_CASE(copy_if_test)
{
    typedef int Type;
    std::vector<Type> const xs = {25, -15, 5, -5, 15};
    auto const pred = [](Type i){return !(i<0);};

    std::vector<Type> r_std;
    std::vector<Type> r_ural;

    std::copy_if (xs.begin(), xs.end(), std::back_inserter(r_std) , pred);

    ural::copy(xs | ural::filtered(pred), r_ural | ural::back_inserter);

    BOOST_CHECK_EQUAL_COLLECTIONS(r_std.begin(), r_std.end(),
                                  r_ural.begin(), r_ural.end());
}

// @copy ������ copy_backward

BOOST_AUTO_TEST_CASE(moved_test)
{
    typedef std::unique_ptr<int> Type;

    std::vector<int> const ys = {25, -15, 5, -5, 15};
    std::vector<Type> xs1;
    std::vector<Type> xs2;

    for(auto & y : ys)
    {
        xs1.emplace_back(ural::make_unique<int>(y));
        xs2.emplace_back(ural::make_unique<int>(y));
    }

    std::vector<Type> r_std;
    std::vector<Type> r_ural;

    std::move(xs1.begin(), xs1.end(), std::back_inserter(r_std));

    ural::copy(xs2 | ural::moved, r_ural | ural::back_inserter);

    BOOST_CHECK_EQUAL(r_std.size(), r_ural.size());
    BOOST_CHECK(std::none_of(r_ural.begin(), r_ural.end(),
                             [](Type const & x) {return !x;}));

    BOOST_CHECK(ural::equal(r_std, r_ural,
                           [](Type const & x, Type const & y)
                                { return *x == *y;}));
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

BOOST_AUTO_TEST_CASE(fill_n_test)
{
    std::vector<int> v_std{0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto v_ural = v_std;

    auto const n = v_std.size() / 2;
    auto const value = -1;

    // @todo ����� ������������ ��������
    std::fill_n(v_std.begin(), n, value);
    ural::fill(v_ural | ural::taken(n), value);

    BOOST_CHECK_EQUAL_COLLECTIONS(v_std.begin(), v_std.end(),
                                  v_ural.begin(), v_ural.end());
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

// @todo ������ generate
// @todo ������ generate_n

// @todo ������ remove
// @todo ������ remove_if

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

// @todo ������ remove_copy_if

BOOST_AUTO_TEST_CASE(replace_test)
{
    std::vector<int> s_std = {5, 7, 4, 2, 8, 6, 1, 9, 0, 3};
    std::vector<int> s_ural = s_std;

    auto const old_value = 8;
    auto const new_value = 88;

    std::replace(s_std.begin(), s_std.end(), old_value, new_value);
    ural::copy(ural::replace(s_ural, old_value, new_value), s_ural);

    BOOST_CHECK_EQUAL_COLLECTIONS(s_std.begin(), s_std.end(),
                                  s_ural.begin(), s_ural.end());
}

BOOST_AUTO_TEST_CASE(replace_if_test)
{
    std::array<int, 10> const s{5, 7, 4, 2, 8, 6, 1, 9, 0, 3};

    auto x_std = s;
    std::vector<int> x_ural;

    auto pred = [](int x) {return x < 5;};
    auto const new_value = 55;

    std::replace_if(x_std.begin(), x_std.end(), pred, new_value);
    ural::copy(ural::replace_if(s, pred, new_value), x_ural | ural::back_inserter);

    BOOST_CHECK_EQUAL_COLLECTIONS(x_std.begin(), x_std.end(),
                                  x_ural.begin(), x_ural.end());
}

BOOST_AUTO_TEST_CASE(swap_ranges_test)
{
    std::vector<int> const x1 = {1, 2, 3, 4, 5};
    std::list<int> const x2   = {-1, -2, -3, -4, -5};

    auto y1 = x1;
    auto y2 = x2;

    // @todo ���� ������������ ��������
    ural::swap_ranges(y1, y2);

    BOOST_CHECK_EQUAL_COLLECTIONS(y1.begin(), y1.end(), x2.begin(), x2.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(y2.begin(), y2.end(), x1.begin(), x1.end());
}

BOOST_AUTO_TEST_CASE(reverse_test)
{
    std::list<int> const src = {1, 2, 3, 4, 5, 6};

    auto x_std = src;
    auto x_ural = src;

    std::reverse(x_std.begin(), x_std.end());
    ural::reverse(x_ural);

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
    std::list<int> r_ural;

    std::reverse_copy(src.begin(), src.end(), std::back_inserter(r_std));
    ural::copy(src | ural::reversed, r_ural | ural::back_inserter);

    BOOST_CHECK_EQUAL_COLLECTIONS(r_std.begin(), r_std.end(),
                                  r_ural.begin(), r_ural.end());
}

BOOST_AUTO_TEST_CASE(rotate_test)
{
    std::vector<int> const v{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    for(size_t i = 0; i != v.size(); ++ i)
    {
        auto v_std = v;
        auto v_ural = v;

        std::rotate(v_std.begin(), v_std.begin() + i, v_std.end());

        auto s = ural::sequence(v_ural);
        s += i;
        // @todo ��������� ������������ ��������
        ural::rotate(s);

        BOOST_CHECK_EQUAL_COLLECTIONS(v_std.begin(), v_std.end(),
                                      v_ural.begin(), v_ural.end());
    }
}

BOOST_AUTO_TEST_CASE(rotate_copy_test)
{
    std::vector<int> const src{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    for(size_t i = 0; i != src.size(); ++ i)
    {
        std::forward_list<int> r_std;
        std::forward_list<int> r_ural;

        std::rotate_copy(src.begin(), src.begin() + i, src.end(),
                         r_std | ural::front_inserter);

        auto s = ural::sequence(src);
        s += i;

        // @todo ��������� ������������ ��������
        ural::rotate_copy(s, r_ural | ural::front_inserter);

        BOOST_CHECK_EQUAL_COLLECTIONS(r_std.begin(), r_std.end(),
                                      r_ural.begin(), r_ural.end());
    }
}

// @todo ������ random_shuffle
// @todo ������ shuffle

BOOST_AUTO_TEST_CASE(unique_test)
{
    std::forward_list<int> v1{1, 2, 2, 2, 3, 3, 2, 2, 1};
    auto v2 = v1;

    auto const last = std::unique(v1.begin(), v1.end());
    std::forward_list<int> r_std(v1.begin(), last);

    // @todo �������� �� forward_list
    // @todo � ���� ����������
    std::list<int> r_ural;
    ural::copy(v2 | ural::uniqued, r_ural | ural::back_inserter);

    BOOST_CHECK_EQUAL_COLLECTIONS(r_std.begin(), r_std.end(), r_ural.begin(),
                                  r_ural.end());
}

BOOST_AUTO_TEST_CASE(unique_test_custom_predicate)
{
    std::string const src = "The      string    with many       spaces!";

    auto const pred = [](char c1, char c2){ return c1 == ' ' && c2 == ' '; };

    std::string s_std;
    std::unique_copy(src.begin(), src.end(), std::back_inserter(s_std), pred);

    std::string s_ural;
    ural::copy(ural::make_unique_sequence(src, pred),
               s_ural | ural::back_inserter);

    BOOST_CHECK_EQUAL(s_std, s_ural);
}

// ����������
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
    BOOST_CHECK(std::none_of(r_ural.begin(), r_ural.traversed_end(), is_even));
}

BOOST_AUTO_TEST_CASE(partition_copy_test)
{
    std::array<int, 10> const src = {1,2,3,4,5,6,7,8,9,10};
    std::list<int> true_sink;
    std::forward_list<int> false_sink;

    auto const pred = [] (int x) {return x % 2 == 0;};

    // @todo ���� ������������� ��������
    ural::partition_copy(src, true_sink | ural::back_inserter,
                         std::front_inserter(false_sink), pred);

    BOOST_CHECK(ural::all_of(true_sink, pred));
    BOOST_CHECK(ural::none_of(false_sink, pred));

    for(auto const & x : src)
    {
        BOOST_CHECK(!!ural::find(true_sink, x) || !!ural::find(false_sink, x));
    }
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

BOOST_AUTO_TEST_CASE(partition_point_test)
{
    typedef int Value;

    // @todo ������� ����� ��������
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

// ����������
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

// @todo ������ sort

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

BOOST_AUTO_TEST_CASE(partial_sort_copy_test)
{
    std::list<int> const v0{4, 2, 5, 1, 3};

    std::vector<int> r1_std{10, 11, 12};
    std::vector<int> r1_ural{10, 11, 12};

    std::vector<int> r2_std{10, 11, 12, 13, 14, 15, 16};
    std::vector<int> r2_ural{10, 11, 12, 13, 14, 15, 16};

    // @todo ���� ������������ ��������
    std::partial_sort_copy(v0.begin(), v0.end(), r1_std.begin(), r1_std.end());
    ural::partial_sort_copy(v0, r1_ural);

    BOOST_CHECK_EQUAL_COLLECTIONS(r1_std.begin(), r1_std.end(),
                                  r1_ural.begin(), r1_ural.end());

    std::partial_sort_copy(v0.begin(), v0.end(), r2_std.begin(), r2_std.end(),
                           std::greater<int>());
    ural::partial_sort_copy(v0, r2_ural, ural::greater<>());

    BOOST_CHECK_EQUAL_COLLECTIONS(r2_std.begin(), r2_std.end(),
                                  r2_ural.begin(), r2_ural.end());
}

// @todo ������ stable_sort
// @todo ������ nth_element

// �������� �����
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

// @todo ������ binary_search
// @todo ������ equal_range

// �������� �� �����������
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

// �������� � ��������� ������
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
        ural::push_heap(ural::make_iterator_sequence(v.begin(), v.begin()+i+1));
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

// ������� � ��������
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

    // @todo ��������� ���������� ��������

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

// @todo ������ next_permutation
// @todo ������ prev_permutation
