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

#include "defs.hpp"

#include <ural/sequence/all.hpp>
#include <ural/math.hpp>
#include <ural/archetypes.hpp>
#include <ural/algorithm.hpp>

#include <forward_list>
#include <list>
#include <map>
#include <vector>

BOOST_AUTO_TEST_CASE(archetype_check)
{
    struct Type {};
    ural::archetypes::callable<bool(Type)> pred;
    ural::archetypes::callable<bool(Type, Type)> bin_pred;
    ural::archetypes::input_sequence<Type> in1;

    ural::value_consumer<bool>() = ural::all_of(in1, pred);
    ural::value_consumer<bool>() = ural::none_of(in1, pred);
    ural::value_consumer<bool>() = ural::any_of(in1, pred);

    ural::archetypes::callable<void(Type &)> action;

    ural::value_consumer<ural::tuple<decltype(in1), decltype(action)>>()
        = ural::for_each(in1, action);

    ural::value_consumer<decltype(in1)>() = ural::find(in1, Type{});
    ural::value_consumer<decltype(in1)>() = ural::find(in1, Type{}, bin_pred);
    ural::value_consumer<decltype(in1)>() = ural::find_if(in1, pred);
    ural::value_consumer<decltype(in1)>() = ural::find_if_not(in1, pred);

    ural::value_consumer<size_t>() = ural::count(in1, Type{});
    ural::value_consumer<size_t>() = ural::count_if(in1, pred);

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(has_pre_increment_test)
{
    static_assert(ural::has_pre_increment<int>() == true, "");
    static_assert(ural::has_pre_increment<std::string>() == false, "");

    static_assert(ural::has_pre_increment<std::string::iterator>() == true, "");
    static_assert(ural::has_pre_increment<int*>() == true, "");
    static_assert(ural::has_pre_increment<void*>() == false, "");

    BOOST_CHECK_EQUAL(ural::has_pre_increment<int>(), true);
    BOOST_CHECK_EQUAL(ural::has_pre_increment<std::string>(), false);

    BOOST_CHECK_EQUAL(ural::has_pre_increment<std::string::iterator>(), true);
    BOOST_CHECK_EQUAL(ural::has_pre_increment<int*>(), true);
    BOOST_CHECK_EQUAL(ural::has_pre_increment<void*>(), false);
}

BOOST_AUTO_TEST_CASE(has_pre_decrement_test)
{
    static_assert(ural::has_pre_decrement<int>() == true, "");
    static_assert(ural::has_pre_decrement<std::string>() == false, "");

    static_assert(ural::has_pre_decrement<std::string::iterator>() == true, "");
    static_assert(ural::has_pre_decrement<int*>() == true, "");
    static_assert(ural::has_pre_decrement<void*>() == false, "");

    BOOST_CHECK_EQUAL(ural::has_pre_decrement<int>(), true);
    BOOST_CHECK_EQUAL(ural::has_pre_decrement<std::string>(), false);

    BOOST_CHECK_EQUAL(ural::has_pre_decrement<std::string::iterator>(), true);
    BOOST_CHECK_EQUAL(ural::has_pre_decrement<int*>(), true);
    BOOST_CHECK_EQUAL(ural::has_pre_decrement<void*>(), false);
}

BOOST_AUTO_TEST_CASE(value_type_for_c_arrays)
{
    typedef int T;

    typedef T ArrayN[10];
    typedef T Array[];
    typedef std::unique_ptr<T> Pointer;

    static_assert(std::is_same<T, ural::ValueType<ArrayN>>::value, "");
    static_assert(std::is_same<T, ural::ValueType<Array>>::value, "");
    static_assert(std::is_same<T, ural::ValueType<Pointer>>::value, "");

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(map_keys_sequence_readable)
{
    using Key = int;
    using Mapped = std::string;

    using Pair = std::pair<Key const, Mapped>;

    ural::generator_sequence<std::function<Pair()>> in{};
    std::forward_list<Pair> const fwd{};
    std::map<Key, Mapped> const bidir{};
    std::vector<Pair> const ra{};

    auto in_key = std::move(in) | ural::map_keys;
    auto fwd_key = fwd | ural::map_keys;
    auto bidir_key = bidir | ural::map_keys;
    auto ra_key = ra | ural::map_keys;

    using namespace ural::concepts;

    BOOST_CONCEPT_ASSERT((InputSequence<decltype(in_key)>));

    BOOST_CONCEPT_ASSERT((FiniteForwardSequence<decltype(fwd_key)>));
    BOOST_CONCEPT_ASSERT((ReadableSequence<decltype(fwd_key)>));

    BOOST_CONCEPT_ASSERT((BidirectionalSequence<decltype(bidir_key)>));
    BOOST_CONCEPT_ASSERT((ReadableSequence<decltype(bidir_key)>));

    BOOST_CONCEPT_ASSERT((RandomAccessSequence<decltype(ra_key)>));
    BOOST_CONCEPT_ASSERT((ReadableSequence<decltype(ra_key)>));
}

BOOST_AUTO_TEST_CASE(removed_if_concept_checks)
{
    ural_test::istringstream_helper<int> const c_in;
    std::forward_list<int> const c_fwd;
    std::list<int> const c_bidir;
    // не может быть произвольного доступа, так как заренее не известно,
    // сколько элементов придётся выкинуть

    auto const pred = ural::is_even;

    auto s_in = c_in | removed_if(pred);
    auto s_fwd = c_fwd | removed_if(pred);
    auto s_bidir = c_bidir | removed_if(pred);

    using namespace ural::concepts;

    BOOST_CONCEPT_ASSERT((SinglePassSequence<decltype(s_in)>));
    BOOST_CONCEPT_ASSERT((Readable<decltype(s_in)>));

    BOOST_CONCEPT_ASSERT((ForwardSequence<decltype(s_fwd)>));
    BOOST_CONCEPT_ASSERT((Readable<decltype(s_fwd)>));

    BOOST_CONCEPT_ASSERT((BidirectionalSequence<decltype(s_bidir)>));
    BOOST_CONCEPT_ASSERT((Readable<decltype(s_bidir)>));

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(reversed_concept_checks)
{
    typedef int Type;

    std::list<Type> const c_bidir;
    std::vector<Type> const c_ra;

    auto s_bidir = c_bidir | ural::reversed;
    auto s_ra = c_ra | ural::reversed;

    using namespace ural::concepts;

    BOOST_CONCEPT_ASSERT((BidirectionalSequence<decltype(s_bidir)>));
    BOOST_CONCEPT_ASSERT((Readable<decltype(s_bidir)>));

    BOOST_CONCEPT_ASSERT((RandomAccessSequence<decltype(s_ra)>));
    BOOST_CONCEPT_ASSERT((Readable<decltype(s_ra)>));

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(reversed_writable_concept_checks)
{
    typedef std::pair<int, std::string> Type;

    std::list<Type> c_bidir;
    std::vector<Type> c_ra;

    auto s_bidir = c_bidir | ural::reversed;
    auto s_ra = c_ra | ural::reversed;

    using namespace ural::concepts;

    BOOST_CONCEPT_ASSERT((BidirectionalSequence<decltype(s_bidir)>));
    BOOST_CONCEPT_ASSERT((OutputSequence<decltype(s_bidir), Type>));
    BOOST_CONCEPT_ASSERT((Readable<decltype(s_bidir)>));

    BOOST_CONCEPT_ASSERT((RandomAccessSequence<decltype(s_ra)>));
    BOOST_CONCEPT_ASSERT((OutputSequence<decltype(s_ra), Type>));
    BOOST_CONCEPT_ASSERT((Readable<decltype(s_ra)>));

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(transformed_concept_checks)
{
    typedef std::string Type;

    ural_test::istringstream_helper<Type> const c_in;
    std::forward_list<Type> const c_fwd;
    std::list<Type> const c_bidir;
    std::vector<Type> const c_ra;

    auto f = &std::string::size;

    // выходной transformed быть не может - не проверяем
    auto s_in = c_in | ural::transformed(f);
    auto s_fwd = c_fwd | ural::transformed(f);
    auto s_bidir = c_bidir | ural::transformed(f);
    auto s_ra = c_ra | ural::transformed(f);

    using namespace ural::concepts;

    BOOST_CONCEPT_ASSERT((SinglePassSequence<decltype(s_in)>));
    BOOST_CONCEPT_ASSERT((Readable<decltype(s_in)>));

    BOOST_CONCEPT_ASSERT((ForwardSequence<decltype(s_fwd)>));
    BOOST_CONCEPT_ASSERT((Readable<decltype(s_fwd)>));

    BOOST_CONCEPT_ASSERT((BidirectionalSequence<decltype(s_bidir)>));
    BOOST_CONCEPT_ASSERT((Readable<decltype(s_bidir)>));

    BOOST_CONCEPT_ASSERT((RandomAccessSequence<decltype(s_ra)>));
    BOOST_CONCEPT_ASSERT((Readable<decltype(s_ra)>));

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(transformed_to_lvalue_concept_checks)
{
    typedef std::pair<int, std::string> Type;

    std::forward_list<Type> c_fwd;
    std::list<Type> c_bidir;
    std::vector<Type> c_ra;

    auto f = ural::make_callable(&Type::first);

    // входной и выходной transformed, возвращающий lvalue быть не может - не проверяем
    auto s_fwd = c_fwd | ural::transformed(f);
    auto s_bidir = c_bidir | ural::transformed(f);
    auto s_ra = c_ra | ural::transformed(f);

    using namespace ural::concepts;

    BOOST_CONCEPT_ASSERT((ForwardSequence<decltype(s_fwd)>));
    BOOST_CONCEPT_ASSERT((OutputSequence<decltype(s_fwd), int>));
    BOOST_CONCEPT_ASSERT((Readable<decltype(s_fwd)>));

    BOOST_CONCEPT_ASSERT((BidirectionalSequence<decltype(s_bidir)>));
    BOOST_CONCEPT_ASSERT((OutputSequence<decltype(s_bidir), int>));
    BOOST_CONCEPT_ASSERT((Readable<decltype(s_bidir)>));

    BOOST_CONCEPT_ASSERT((RandomAccessSequence<decltype(s_ra)>));
    BOOST_CONCEPT_ASSERT((OutputSequence<decltype(s_ra), int>));
    BOOST_CONCEPT_ASSERT((Readable<decltype(s_ra)>));

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(uniqued_concept_checks)
{
    typedef int Type;

    ural_test::istringstream_helper<Type> const c_in;
    std::forward_list<Type> const c_fwd;
    std::list<Type> const c_bidir;
    std::vector<Type> const c_ra;

    auto s_in = c_in | ural::uniqued;
    auto s_fwd = c_fwd | ural::uniqued;
    auto s_bidir = c_bidir | ural::uniqued;
    auto s_ra = c_ra | ural::uniqued;

    using namespace ural::concepts;

    BOOST_CONCEPT_ASSERT((SinglePassSequence<decltype(s_in)>));
    BOOST_CONCEPT_ASSERT((Readable<decltype(s_in)>));

    BOOST_CONCEPT_ASSERT((FiniteForwardSequence<decltype(s_fwd)>));
    BOOST_CONCEPT_ASSERT((Readable<decltype(s_fwd)>));

    BOOST_CONCEPT_ASSERT((FiniteForwardSequence<decltype(s_bidir)>));
    BOOST_CONCEPT_ASSERT((Readable<decltype(s_bidir)>));

    BOOST_CONCEPT_ASSERT((FiniteForwardSequence<decltype(s_ra)>));
    BOOST_CONCEPT_ASSERT((Readable<decltype(s_ra)>));

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(writable_uniqued_concept_checks)
{
    typedef int Type;

    std::forward_list<Type> c_fwd;
    std::list<Type> c_bidir;
    std::vector<Type> c_ra;

    auto s_fwd = c_fwd | ural::uniqued;
    auto s_bidir = c_bidir | ural::uniqued;
    auto s_ra = c_ra | ural::uniqued;

    using namespace ural::concepts;

    BOOST_CONCEPT_ASSERT((FiniteForwardSequence<decltype(s_fwd)>));
    BOOST_CONCEPT_ASSERT((Readable<decltype(s_fwd)>));
    BOOST_CONCEPT_ASSERT((Writable<decltype(s_fwd), Type>));

    BOOST_CONCEPT_ASSERT((FiniteForwardSequence<decltype(s_bidir)>));
    BOOST_CONCEPT_ASSERT((Readable<decltype(s_bidir)>));
    BOOST_CONCEPT_ASSERT((Writable<decltype(s_bidir), Type>));

    BOOST_CONCEPT_ASSERT((FiniteForwardSequence<decltype(s_ra)>));
    BOOST_CONCEPT_ASSERT((Readable<decltype(s_ra)>));
    BOOST_CONCEPT_ASSERT((Writable<decltype(s_ra), Type>));

    BOOST_CHECK(true);
}
