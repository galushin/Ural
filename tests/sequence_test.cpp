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

#include <ural/sequence/chunks.hpp>
#include <ural/sequence/cargo.hpp>
#include <ural/sequence/sink.hpp>
#include <ural/container/flat_set.hpp>
#include <ural/algorithm.hpp>
#include <ural/sequence/all.hpp>
#include <ural/sequence/zip.hpp>
#include <ural/sequence/map.hpp>
#include <ural/sequence/progression.hpp>

#include <boost/mpl/list.hpp>
#include <boost/test/unit_test.hpp>

#include <iterator>
#include <set>
#include <map>

BOOST_AUTO_TEST_CASE(sequence_for_lvalue_container)
{
    std::vector<int> const z{11, 11, 22, 33, 55};

    auto seq = ::ural::sequence(z);

    typedef ural::iterator_sequence<decltype(z.begin())> IS;

    static_assert(std::is_same<decltype(seq), IS>::value, "");
}

BOOST_AUTO_TEST_CASE(sequence_for_rvalue_container)
{
    std::vector<int> const z{11, 11, 22, 33, 55};
    auto x = z;
    auto const old_x_data = x.data();

    auto seq = ural::sequence(std::move(x));

    BOOST_CHECK_EQUAL(seq.cargo().data(), old_x_data);

    BOOST_CHECK_EQUAL_COLLECTIONS(seq.cargo().begin(),
                                  seq.cargo().end(),
                                  z.begin(), z.end());

    BOOST_CHECK(ural::equal(seq, z) == true);
}

BOOST_AUTO_TEST_CASE(rvalue_container_sort_test)
{
    std::vector<int> const z{3, 1, 4, 1, 5};

    auto x1 = z;
    ural::sort(x1);

    auto x2 = z;
    auto seq = ::ural::sequence(std::move(x2));
    ural::sort(seq);

    BOOST_CHECK_EQUAL_COLLECTIONS(x1.begin(), x1.end(),
                                  seq.cargo().begin(), seq.cargo().end());

    BOOST_CHECK(ural::equal(x1, seq));
}

BOOST_AUTO_TEST_CASE(rvalue_container_reverse_test)
{
    std::vector<int> const z{3, 1, 4, 1, 5};

    auto x1 = z;
    ural::reverse(x1);

    auto x2 = z;
    auto seq = ::ural::sequence(std::move(x2));
    ural::reverse(seq);

    BOOST_CHECK_EQUAL_COLLECTIONS(x1.begin(), x1.end(),
                                  seq.cargo().begin(), seq.cargo().end());

    BOOST_CHECK(ural::equal(x1, seq));
}

BOOST_AUTO_TEST_CASE(rvalue_container_partial_sort_test)
{
    std::vector<int> const z{3, 1, 4, 1, 5};
    auto const part = z.size() / 2;

    auto x1 = z;
    ural::partial_sort(x1, part);

    auto x2 = z;
    auto seq = ::ural::sequence(std::move(x2));
    ural::partial_sort(seq, part);

    BOOST_CHECK_EQUAL_COLLECTIONS(x1.begin(), x1.end(),
                                  seq.cargo().begin(), seq.cargo().end());

    BOOST_CHECK(ural::equal(x1, seq));
}

BOOST_AUTO_TEST_CASE(istream_sequence_test)
{
    std::istringstream str1("0.1 0.2 0.3 0.4");
    std::istringstream str2("0.1 0.2 0.3 0.4");

    std::vector<double> r_std;
    std::copy(std::istream_iterator<double>(str1),
              std::istream_iterator<double>(),
              r_std | ural::back_inserter);

    std::vector<double> r_ural;
    ural::copy(ural::make_istream_sequence<double>(str2),
               r_ural | ural::back_inserter);

    BOOST_CHECK_EQUAL_COLLECTIONS(r_std.begin(), r_std.end(),
                                  r_ural.begin(), r_ural.end());
}

BOOST_AUTO_TEST_CASE(istream_sequence_regression_1)
{
    std::istringstream str1("0.1 0.2 0.3 0.4");
    std::istringstream str2("0.1 0.2 0.3 0.4");

    std::vector<double> r_std;
    std::copy(std::istream_iterator<double>(str1),
              std::istream_iterator<double>(),
              r_std | ural::back_inserter);

    std::vector<double> r_ural;

    // @todo можно ли это заменить на for для интервалаов?
    for(auto s = ural::make_istream_sequence<double>(str2); !!s; ++ s)
    {
        r_ural.push_back(*s);
    }

    BOOST_CHECK_EQUAL_COLLECTIONS(r_std.begin(), r_std.end(),
                                  r_ural.begin(), r_ural.end());
}

BOOST_AUTO_TEST_CASE(ostream_sequence_test)
{
    typedef int Value;
    std::vector<Value> const xs = {1, 2, 3, 4, 5};

    std::ostringstream os_std;
    std::ostringstream os_ural;

    std::copy(xs.begin(), xs.end(), std::ostream_iterator<Value>(os_std, " "));

    auto seq = ural::make_ostream_sequence<Value>(os_ural, " ");
    std::copy(xs.begin(), xs.end(), std::move(seq));

    BOOST_CHECK_EQUAL(os_std.str(), os_ural.str());

    typedef decltype(seq) Sequence;

    BOOST_CONCEPT_ASSERT((boost::OutputIterator<Sequence, Value>));
}

BOOST_AUTO_TEST_CASE(ostream_sequence_test_auto)
{
    typedef int Value;
    std::vector<Value> const xs = {1, 2, 3, 4, 5};

    std::ostringstream os_std;
    std::ostringstream os_ural;

    std::copy(xs.begin(), xs.end(), std::ostream_iterator<Value>(os_std, " "));

    auto seq = ural::make_ostream_sequence(os_ural, " ");
    ural::copy(xs, std::move(seq));

    BOOST_CHECK_EQUAL(os_std.str(), os_ural.str());

    typedef decltype(seq) Sequence;

    BOOST_CONCEPT_ASSERT((boost::OutputIterator<Sequence, Value>));
}

BOOST_AUTO_TEST_CASE(ostream_sequence_test_no_delim)
{
    std::vector<int> const xs = {1, 2, 3, 4, 5};

    std::ostringstream os_std;
    std::ostringstream os_ural;

    std::copy(xs.begin(), xs.end(), std::ostream_iterator<int>(os_std));
    ural::copy(xs, ural::make_ostream_sequence<int>(os_ural));

    BOOST_CHECK_EQUAL(os_std.str(), os_ural.str());
}

BOOST_AUTO_TEST_CASE(ostream_sequence_test_auto_no_delim)
{
    std::vector<int> const xs = {1, 2, 3, 4, 5};

    std::ostringstream os_std;
    std::ostringstream os_ural;

    std::copy(xs.begin(), xs.end(), std::ostream_iterator<int>(os_std));
    ural::copy(xs, ural::make_ostream_sequence(os_ural));

    BOOST_CHECK_EQUAL(os_std.str(), os_ural.str());
}

BOOST_AUTO_TEST_CASE(move_iterators_to_sequence_test)
{
    std::vector<int> xs = {1, 2, 3, 4, 5};
    auto m_begin = std::make_move_iterator(xs.begin());
    auto m_end = std::make_move_iterator(xs.end());

    auto ms = ural::make_iterator_sequence(m_begin, m_end);

    typedef ural::move_sequence<ural::iterator_sequence<decltype(xs.begin()),
                                                        decltype(xs.end())>>
        MSequence;

    static_assert(std::is_same<decltype(ms), MSequence>::value, "");

    BOOST_CHECK(m_begin.base() == ms.base().begin());
    BOOST_CHECK(m_end.base() == ms.base().end());
}

BOOST_AUTO_TEST_CASE(ural_move_iterators_to_sequence_test)
{
    std::vector<int> xs = {1, 2, 3, 4, 5};
    auto m_begin = ural::make_move_iterator(xs.begin());
    auto m_end = ural::make_move_iterator(xs.end());

    auto ms = ural::make_iterator_sequence(m_begin, m_end);

    typedef ural::move_sequence<ural::iterator_sequence<decltype(xs.begin()),
                                                        decltype(xs.end())>>
        MSequence;

    static_assert(std::is_same<decltype(ms), MSequence>::value, "");

    BOOST_CHECK(m_begin.base() == ms.base().begin());
    BOOST_CHECK(m_end.base() == ms.base().end());
}

BOOST_AUTO_TEST_CASE(sink_output_sequence_test_auto)
{
    ural::sink_sequence<> sink {};

    static_assert(std::is_empty<decltype(sink)>::value, "too big");
    BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassSequence<decltype(sink)>));
    BOOST_CONCEPT_ASSERT((ural::concepts::WritableSequence<decltype(sink), int>));
    BOOST_CONCEPT_ASSERT((ural::concepts::WritableSequence<decltype(sink), std::string>));

    *sink = 42;
    *sink = std::string("42");

    typedef decltype(sink) Sequence;

     BOOST_CONCEPT_ASSERT((boost::OutputIterator<Sequence, int>));
     BOOST_CONCEPT_ASSERT((boost::OutputIterator<Sequence, std::string>));
}

 BOOST_AUTO_TEST_CASE(sink_output_sequence_test)
{
    typedef int Value;
    ural::sink_sequence<Value> sink;

    static_assert(std::is_empty<decltype(sink)>::value, "too big");
    BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassSequence<decltype(sink)>));
    BOOST_CONCEPT_ASSERT((ural::concepts::WritableSequence<decltype(sink), Value>));

    *sink = 42;

    typedef decltype(sink) Sequence;

     BOOST_CONCEPT_ASSERT((boost::OutputIterator<Sequence, Value>));
}

BOOST_AUTO_TEST_CASE(istream_sequence_no_default_ctor_test)
{
    int var = 13;

    std::istringstream is("42");

    ural::istream_sequence<std::istream, std::reference_wrapper<int>>
        seq(is, std::ref(var));

    typedef decltype(seq) Sequence;

    BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassSequence<Sequence>));
    BOOST_CONCEPT_ASSERT((ural::concepts::ReadableSequence<Sequence>));

    std::ref(var) = *seq;

    BOOST_CHECK_EQUAL(42, var);
}

BOOST_AUTO_TEST_CASE(ostream_sequence_default_test)
{
    typedef std::ostream OStream;

    typedef ural::ostream_sequence<OStream, int> S1;

    static_assert(std::is_same<std::string, S1::delimeter_type>::value, "");

    typedef ural::ostream_sequence<OStream> S2;

    static_assert(std::is_same<std::string, S2::delimeter_type>::value, "");

    typedef ural::ostream_sequence<> S3;

    static_assert(std::is_same<std::ostream, S3::ostream_type>::value, "");
    static_assert(std::is_same<std::string, S3::delimeter_type>::value, "");
}

BOOST_AUTO_TEST_CASE(arithmetic_progression_size_test)
{
    auto s = ural::make_arithmetic_progression(0, 1);

    BOOST_CHECK_LE(sizeof(s), sizeof(0) * 3);
}

BOOST_AUTO_TEST_CASE(cartesian_product_sequence_test)
{
    auto digits = ural::make_arithmetic_progression(0, 1) | ural::taken(10);
    auto s2 = ural::make_cartesian_product_sequence(digits, digits);

    std::set<int> r2;

    for(; !!s2; ++ s2)
    {
        auto t = *s2;
        r2.insert(t[ural::_1] * 10 + t[ural::_2]);
    }

    BOOST_CHECK_EQUAL(100U, r2.size());
    BOOST_CHECK_EQUAL(0,  *r2.begin());
    BOOST_CHECK_EQUAL(99, *r2.rbegin());
}

BOOST_AUTO_TEST_CASE(geometric_progression_test)
{
    std::vector<int> const zs = {1, 2, 4, 8, 16, 32, 64};

    auto const xs = ural::make_arithmetic_progression(1, 2, ural::multiplies<>{})
                  | ural::taken(zs.size())
                  | ural::to_container<std::vector>{};

    BOOST_CHECK_EQUAL_COLLECTIONS(zs.begin(), zs.end(),
                                  xs.begin(), xs.end());
}

BOOST_AUTO_TEST_CASE(arithmetic_progression_concept_check)
{
    using namespace ural;
    typedef arithmetic_progression<int, use_default, single_pass_traversal_tag> SP;
    typedef arithmetic_progression<int, use_default, forward_traversal_tag> Fw;
    typedef arithmetic_progression<int, use_default, random_access_traversal_tag> RA;

    BOOST_CHECK_LE(sizeof(SP), 2 * sizeof(int));
    BOOST_CHECK_LE(sizeof(Fw), 3 * sizeof(int));
    BOOST_CHECK_LE(sizeof(RA), 3 * sizeof(int));

    BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassSequence<SP>));
    BOOST_CONCEPT_ASSERT((ural::concepts::ForwardSequence<Fw>));
    BOOST_CONCEPT_ASSERT((ural::concepts::ForwardSequence<RA>));
    // @todo BOOST_CONCEPT_ASSERT((ural::concepts::RandomAccessSequence<RA>));
}

BOOST_AUTO_TEST_CASE(arithmetic_progression_equality_test)
{
    struct Inner
    {
        static int add_1(int x, int y)
        {
            return x+y;
        }

        static int add_2(int x, int y)
        {
            return x*y;
        }
    };

    auto a1 = ural::make_arithmetic_progression(2, 2, &Inner::add_1);
    auto a1c = a1;
    auto a2 = ural::make_arithmetic_progression(1, 2, &Inner::add_1);
    auto a3 = ural::make_arithmetic_progression(2, 1, &Inner::add_1);
    auto a4 = ural::make_arithmetic_progression(2, 2, &Inner::add_2);

    BOOST_CHECK(a1 == a1);
    BOOST_CHECK(a1 == a1c);

    BOOST_CHECK(a1 != a2);
    BOOST_CHECK(a1 != a3);
    BOOST_CHECK(a1 != a4);

    BOOST_CHECK(a2 != a3);
    BOOST_CHECK(a2 != a4);

    BOOST_CHECK(a3 != a4);
}

BOOST_AUTO_TEST_CASE(cartesian_product_sequence_is_sorted_test)
{
    auto digits = ural::make_arithmetic_progression(0, 1) | ural::taken(10);
    auto s2 = ural::make_cartesian_product_sequence(digits, digits);

    std::vector<decltype(s2)::value_type> r2;

    ural::copy(s2, r2 | ural::back_inserter);

    BOOST_CHECK_EQUAL(100U, r2.size());
    BOOST_CHECK(std::is_sorted(r2.begin(), r2.end()));
}

BOOST_AUTO_TEST_CASE(cartesian_product_sequence_regression_ctor)
{
    std::vector<int> const x_std = {1, 2, 3, 4, 5};

    auto seq = ural::sequence(x_std);
    ++ seq;

    auto ts = ural::make_cartesian_product_sequence(seq, seq);

    for(; !!ts; ++ ts)
    {
        BOOST_CHECK_NE(1, (*ts)[ural::_1]);
        BOOST_CHECK_NE(1, (*ts)[ural::_2]);
    }
}

BOOST_AUTO_TEST_CASE(sequence_for_each_test)
{
    std::vector<int> x_std = {1, 2, 3, 4, 5};
    auto x_ural = x_std;

    for(auto & x : x_std)
    {
        x *= 2;
    }

    for(auto & x : ural::sequence(x_ural))
    {
        x *= 2;
    }

    BOOST_CHECK_EQUAL_COLLECTIONS(x_std.begin(), x_std.end(),
                                  x_ural.begin(), x_ural.end());
}

BOOST_AUTO_TEST_CASE(filtered_sequence_for_each)
{
    std::vector<int> x = {1, 2, 3, 4, 5, 6, 7, 8};
    auto s = x | ural::filtered([](int const & x) { return x % 3 == 0;});

    std::vector<int> r;
    std::vector<int> const z  = {3, 6};

    // Цикл вместо алгоритма используется специально
    for(auto & x : s)
    {
        r.push_back(x);
    }

    BOOST_CHECK_EQUAL_COLLECTIONS(z.begin(), z.end(), r.begin(), r.end());
}

BOOST_AUTO_TEST_CASE(zip_sequence_bases_access)
{
    std::vector<int> const x = {1, 2, 3, 4, 5};
    std::vector<char> const y = {'a', 'b', 'c', 'd', 'e'};

    auto xy_zip = ural::make_zip_sequence(x, y);

    BOOST_CHECK(xy_zip.bases()[ural::_1].begin() == x.begin());
    BOOST_CHECK(xy_zip.bases()[ural::_1].end() == x.end());
    BOOST_CHECK(xy_zip.bases()[ural::_2].begin() == y.begin());
    BOOST_CHECK(xy_zip.bases()[ural::_2].end() == y.end());

    BOOST_CHECK_EQUAL_COLLECTIONS(xy_zip.bases()[ural::_1].begin(),
                                  xy_zip.bases()[ural::_1].end(),
                                  x.begin(), x.end());

    BOOST_CHECK_EQUAL_COLLECTIONS(xy_zip.bases()[ural::_2].begin(),
                                  xy_zip.bases()[ural::_2].end(),
                                  y.begin(), y.end());
}

BOOST_AUTO_TEST_CASE(zip_sequence_test)
{
    std::vector<int> const x = {1, 2, 3, 4, 5};
    std::vector<char> const y = {'a', 'b', 'c', 'd', 'e'};

    assert(x.size() == y.size());

    // std
    std::vector<std::tuple<int, char>> r_std;

    for(size_t i = 0; i < x.size(); ++ i)
    {
        r_std.emplace_back(x[i], y[i]);
    }

    // ural
    auto const r_ural = ural::make_zip_sequence(x, y)
                      | ural::to_container<std::vector>{};

    // проверка
    BOOST_CHECK_EQUAL(x.size(), r_ural.size());
    BOOST_CHECK_EQUAL(r_std.size(), r_ural.size());

    for(size_t i = 0; i < x.size(); ++ i)
    {
        BOOST_CHECK(r_std[i] == r_ural[i]);
    }
}

BOOST_AUTO_TEST_CASE(map_keys_and_values_test)
{
    std::vector<int> const x = {1, 2, 3, 4, 5};
    std::vector<char> const y = {'a', 'b', 'c', 'd', 'e'};

    assert(x.size() == y.size());

    std::map<int, char> const xy
        = ural::make_zip_sequence(x, y) | ural::to_map<std::map>{};

    BOOST_CHECK(ural::equal(xy | ural::map_keys, x));
    BOOST_CHECK(ural::equal(xy | ural::map_values, y));
}

BOOST_AUTO_TEST_CASE(set_inserter_container_access)
{
    std::set<int> s;

    auto seq = s | ural::set_inserter;

    BOOST_CHECK_EQUAL(&s, &seq.container());
}

namespace
{
    typedef boost::mpl::list<std::set<int>, ural::flat_set<int>> Int_set_types;
}

BOOST_AUTO_TEST_CASE_TEMPLATE(set_inserter_test, Set, Int_set_types)
{
    std::vector<int> const xs = {1, 2, 3, 4, 5, 1, 3, 5, 2, 4, 6};

    Set const z(xs.begin(), xs.end());

    Set z_ural;
    ural::copy(xs, z_ural | ural::set_inserter);

    BOOST_CHECK_EQUAL_COLLECTIONS(z.begin(), z.end(),
                                  z_ural.begin(), z_ural.end());
}

#include <ural/numeric/numbers_sequence.hpp>

BOOST_AUTO_TEST_CASE(numbers_range_test)
{
    std::vector<int> const z = {0, 1, 2, 3, 4, 5, 6};

    std::vector<int> x;

    for(auto i : ural::numbers(0, 7))
    {
        x.push_back(i);
    }

    BOOST_CHECK_EQUAL_COLLECTIONS(z.begin(), z.end(), x.begin(), x.end());
}

BOOST_AUTO_TEST_CASE(numbers_size_test)
{
    auto const ns = ::ural::numbers(2, 7, 3);

    BOOST_CHECK_EQUAL(ns.size(), (7-2)/3);
}

BOOST_AUTO_TEST_CASE(numbers_stride_range_test)
{
    std::vector<int> const z = {0, 2, 4, 6};

    std::vector<int> x;

    for(auto i : ural::numbers(0, 7, 2))
    {
        x.push_back(i);
    }

    BOOST_CHECK_EQUAL_COLLECTIONS(z.begin(), z.end(), x.begin(), x.end());
}

// @todo Интервал чисел с отрицательным шагом?


// Итераторы последовательностей
#include <ural/sequence/make.hpp>
BOOST_AUTO_TEST_CASE(iterator_sequence_iterators)
{
    typedef std::vector<int> Container;
    Container v1 = {0, 2, 4, 6};
    auto const v2 = v1;

    auto const s1 = ural::sequence(v1);
    auto const s2 = ural::sequence(v2);

    static_assert(std::is_convertible<decltype(s1.begin()), Container::iterator>::value, "");
    static_assert(std::is_convertible<decltype(s2.begin()), Container::const_iterator>::value, "");

    BOOST_CHECK(s1.begin() == v1.begin());
    BOOST_CHECK(s1.end() == v1.end());

    BOOST_CHECK(s2.begin() == v2.begin());
    BOOST_CHECK(s2.end() == v2.end());
}

BOOST_AUTO_TEST_CASE(reversed_iterator_sequence_iterators)
{
    typedef std::vector<int> Container;
    Container v1 = {0, 2, 4, 6};
    auto const v2 = v1;

    auto const rs1 = ural::sequence(v1) | ural::reversed;
    auto const rs2 = ural::sequence(v2) | ural::reversed;

    static_assert(std::is_same<decltype(begin(rs1)), Container::reverse_iterator>::value, "");
    static_assert(std::is_same<decltype(begin(rs2)), Container::const_reverse_iterator>::value, "");

    BOOST_CHECK(begin(rs1) == v1.rbegin());
    BOOST_CHECK(end(rs1) == v1.rend());

    BOOST_CHECK(begin(rs2) == v2.rbegin());
    BOOST_CHECK(end(rs2) == v2.rend());
}

BOOST_AUTO_TEST_CASE(moved_from_value_cpp_17_test)
{
    std::string const s("hello");
    std::string x_std;
    auto x_ural = x_std;

    auto f = std::ptr_fun<int, int>(std::toupper);

    std::transform(s.begin(), s.end(), std::back_inserter(x_std), f);

    auto seq = ural::make_transform_sequence(f, s)
             | ural::moved;

    using Sequence = decltype(seq);

    static_assert(std::is_same<Sequence::reference, int>::value, "");

    ural::copy(seq, x_ural | ural::back_inserter);

    BOOST_CHECK_EQUAL_COLLECTIONS(x_std.begin(), x_std.end(),
                                  x_ural.begin(), x_ural.end());
}

BOOST_AUTO_TEST_CASE(move_iterator_cpp_11_moving_from_return_by_value_regression)
{
    std::string const s("hello");
    std::string x_std;

    auto f = std::ptr_fun<int, int>(std::toupper);

    std::transform(s.begin(), s.end(), std::back_inserter(x_std), f);

    auto seq = ural::make_transform_sequence(f, s)
             | ural::moved;

    using Sequence = decltype(seq);

    static_assert(std::is_same<Sequence::reference, int>::value, "");

    auto x_ural = seq | ural::to_container<std::basic_string>{};

    BOOST_CHECK_EQUAL_COLLECTIONS(x_std.begin(), x_std.end(),
                                  x_ural.begin(), x_ural.end());
}

BOOST_AUTO_TEST_CASE(moved_iterator_sequence_iterators)
{
    typedef std::vector<int> Container;
    Container v1 = {0, 2, 4, 6};
    auto const v2 = v1;

    auto const rs1 = ural::sequence(v1) | ural::moved;
    auto const rs2 = ural::sequence(v2) | ural::moved;

    static_assert(std::is_same<decltype(begin(rs1)), ural::move_iterator<Container::iterator>>::value, "");
    static_assert(std::is_same<decltype(begin(rs2)), ural::move_iterator<Container::const_iterator>>::value, "");

    BOOST_CHECK(begin(rs1) == ural::make_move_iterator(v1.begin()));
    BOOST_CHECK(end(rs1) == ural::make_move_iterator(v1.end()));

    BOOST_CHECK(begin(rs2) == ural::make_move_iterator(v2.begin()));
    BOOST_CHECK(end(rs2) == ural::make_move_iterator(v2.end()));
}

BOOST_AUTO_TEST_CASE(function_output_sequence_as_iterator)
{
    int result = 0;

    std::function<void(int)> acc = [&result](int x) { result += x; };

    auto out = ural::make_function_output_sequence(acc);

    // Концепция
    BOOST_CONCEPT_ASSERT((boost::OutputIterator<decltype(out), int>));

    // Пример использования
    std::vector<int> const xs = {1, 2, 3, 4, 5};
    std::copy(xs.begin(), xs.end(), out);

    BOOST_CHECK_EQUAL(15, result);
}

#include <boost/iterator/transform_iterator.hpp>
BOOST_AUTO_TEST_CASE(transform_sequence_iterators)
{
    std::string const s("hello");

    auto f = ural::function_ptr_wrapper<int(int)>(std::toupper);

    auto const seq = ural::make_transform_sequence(f, s);

    typedef boost::transform_iterator<decltype(f), decltype(s.begin())>
        Iterator;

    Iterator bf = begin(seq);
    Iterator ef = end(seq);

    BOOST_CHECK(bf.base() == s.begin());
    BOOST_CHECK(ef.base() == s.end());

    BOOST_CHECK(bf.functor() == f);
    BOOST_CHECK(ef.functor() == f);
}

BOOST_AUTO_TEST_CASE(to_container_additional_parameters)
{
    typedef ural::greater<> Compare;

    auto const ns = ural::numbers(1, 10)
                  | ural::to_container<std::set, Compare>{};

    static_assert(std::is_same<Compare, decltype(ns)::key_compare>::value, "");

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(to_map_additional_parameters)
{
    typedef ural::greater<> Compare;

    std::map<int, char> f;

    auto f1 = ural::sequence(f) | ural::to_map<std::map, Compare>{};

    static_assert(std::is_same<Compare, decltype(f1)::key_compare>::value, "");

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(iterator_sequence_for_istream_iterator_regression)
{
    std::vector<int> const z{11, 11, 22, 33, 55};

    std::ostringstream os;
    ural::copy(z, ural::make_ostream_sequence(os, " "));

    std::istringstream is(os.str());

    auto first = std::istream_iterator<int>(is);
    auto last = std::istream_iterator<int>();

    auto seq = ural::make_iterator_sequence(first, last);

    std::vector<int> x;
    ural::copy(seq, x | ural::back_inserter);

    BOOST_CHECK_EQUAL_COLLECTIONS(x.begin(), x.end(), z.begin(), z.end());
}

// @todo преобразовать в шаблон
BOOST_AUTO_TEST_CASE(chunks_sequence_test)
{
    typedef std::vector<int> Source;

    auto const seq = ural::numbers(1, 8);
    Source src(begin(seq), end(seq));

    typedef ural::ValueType<Source> Value;
    std::vector<std::vector<Value>> expected { {1, 2, 3}, {4, 5, 6}, {7}};

    auto ch = ::ural::make_chunks_sequence(src, 3);
    for(auto const & r : expected)
    {
        BOOST_CHECK(!!ch);
        BOOST_CHECK(::ural::equal(r, *ch));
        BOOST_CHECK_EQUAL_COLLECTIONS(r.begin(), r.end(),
                                      begin(*ch), end(*ch));

        ++ ch;
    }

    BOOST_CHECK(!ch);
}
