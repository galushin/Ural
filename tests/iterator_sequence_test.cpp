#include <boost/test/unit_test.hpp>

#include <forward_list>
#include <list>
#include <vector>

#include <boost/concept/assert.hpp>

#include <ural/algorithm.hpp>
#include <ural/numeric.hpp>
#include <ural/sequence/all.hpp>

// @todo Тест возвращаемого значения для copy при копировании разной длины

BOOST_AUTO_TEST_CASE(copy_sequence_test_via_details)
{
    std::vector<int> const xs = {1, 2, 3, 4};

    std::vector<int> x1(xs.size());

    ural::details::copy(ural::sequence(xs), ural::sequence(x1));

    BOOST_CHECK(ural::sequence(xs) == ural::sequence(xs));
    BOOST_CHECK(ural::sequence(x1) == ural::sequence(x1));
    BOOST_CHECK(ural::sequence(x1) != ural::sequence(xs));
    BOOST_CHECK(ural::sequence(xs) != ural::sequence(x1));

    BOOST_CHECK_EQUAL_COLLECTIONS(x1.begin(), x1.end(), xs.begin(), xs.end());
}

BOOST_AUTO_TEST_CASE(copy_sequence_test)
{
    std::vector<int> const xs = {1, 2, 3, 4};

    std::vector<int> x1(xs.size());

    auto const r = ural::copy(xs, x1);

    BOOST_CHECK_EQUAL_COLLECTIONS(x1.begin(), x1.end(), xs.begin(), xs.end());

    BOOST_CHECK(!r[ural::_1]);
    BOOST_CHECK(!r[ural::_2]);
}

BOOST_AUTO_TEST_CASE(copy_to_back_inserter)
{
    std::vector<int> const xs = {1, 2, 3, 4};

    std::vector<int> x1;

    ural::copy(xs, std::back_inserter(x1));

    BOOST_CHECK_EQUAL_COLLECTIONS(xs.begin(), xs.end(), x1.begin(), x1.end());
}

BOOST_AUTO_TEST_CASE(c_array_to_sequence)
{
    int xs [] = {1, 2, 3, 4};

    auto s = ural::sequence(xs);

    auto const sum_std = std::accumulate(xs, xs + sizeof(xs) / sizeof(xs[0]), 0.0);
    auto const sum_ural = ural::accumulate(s, 0.0);

    BOOST_CHECK_EQUAL(sum_std, sum_ural);
}

BOOST_AUTO_TEST_CASE(iteretor_sequence_plus_assign_test)
{
    std::vector<int> const xs = {1, 2, 3, 4};
    auto s = ural::sequence(xs);
    auto const n = 2;
    s += 2;

    BOOST_CHECK_EQUAL(n, s.traversed_front().size());
    BOOST_CHECK_EQUAL(n, ural::size(s.traversed_front()));
}

BOOST_AUTO_TEST_CASE(iterator_sequence_size_test)
{
    // @todo Добавить проверку для итераторов ввода и вывода
    std::istringstream is;
    std::forward_list<int> fwd;
    std::list<int> bi;
    std::vector<int> ra;

    auto s_in = ::ural::make_iterator_sequence(std::istream_iterator<int>(is),
                                                std::istream_iterator<int>());
    auto s_fwd = ::ural::sequence(fwd);
    auto s_bi = ::ural::sequence(bi);
    auto s_ra = ::ural::sequence(ra);

    BOOST_CHECK_EQUAL(2*sizeof(std::istream_iterator<int>{}), sizeof(s_in));
    BOOST_CHECK_EQUAL(3*sizeof(fwd.begin()), sizeof(s_fwd));
    BOOST_CHECK_EQUAL(5*sizeof(bi.begin()), sizeof(s_bi));
    BOOST_CHECK_EQUAL(5*sizeof(ra.begin()), sizeof(s_ra));
}
