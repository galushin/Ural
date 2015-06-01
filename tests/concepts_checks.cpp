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

#include <ural/archetypes.hpp>

#include <ural/algorithm.hpp>

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
