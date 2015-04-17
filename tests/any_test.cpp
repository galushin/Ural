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

#include <ural/utility/any.hpp>
#include <ural/utility/tracers.hpp>

#include <boost/mpl/list.hpp>
#include <boost/test/unit_test.hpp>

/*
любой тип       boost::any                  any
концепцци       boost::type_erasure::any    basic_any
список типов    boost::variant              variant
*/

namespace
{
    typedef boost::mpl::list<int, std::string> any_test_types;
}

BOOST_AUTO_TEST_CASE(any_default_ctor_test)
{
    ural::any a;

    bool is_empty = a.empty();

    BOOST_CHECK(is_empty);
}

BOOST_AUTO_TEST_CASE(any_value_ctor_saves)
{
    typedef ural::regular_tracer<int> Type;
    auto const old_active = Type::active_objects();
    auto const old_constructed = Type::constructed_objects();
    auto const old_move_ctor_count = Type::move_ctor_count();
    auto const old_destroyed = Type::destroyed_objects();

    {
        ural::any const a(Type(42));

        BOOST_CHECK(!a.empty());

        BOOST_CHECK(a.type() == typeid(Type));
        BOOST_CHECK_EQUAL(a.type().name(), typeid(Type).name());
    }

    BOOST_CHECK_EQUAL(Type::active_objects(), old_active);
    BOOST_CHECK_EQUAL(Type::constructed_objects(), old_constructed+2);
    BOOST_CHECK_EQUAL(Type::move_ctor_count(), old_move_ctor_count+1);
    BOOST_CHECK_EQUAL(Type::destroyed_objects(), old_destroyed + 2);
}

BOOST_AUTO_TEST_CASE(any_value_ctor_from_lvalue)
{
    typedef ural::regular_tracer<int> Type;

    auto const old_active = Type::active_objects();
    auto const old_constructed = Type::constructed_objects();
    auto const old_copy_ctor_count = Type::copy_ctor_count();
    auto const old_destroyed = Type::destroyed_objects();

    {
        auto const value = Type(42);
        ural::any const a(value);

        BOOST_CHECK(!a.empty());

        BOOST_CHECK(a.type() == typeid(Type));
        BOOST_CHECK_EQUAL(a.type().name(), typeid(Type).name());
    }

    BOOST_CHECK_EQUAL(Type::active_objects(), old_active);
    BOOST_CHECK_EQUAL(Type::constructed_objects(), old_constructed+2);
    BOOST_CHECK_EQUAL(Type::copy_ctor_count(), old_copy_ctor_count+1);
    BOOST_CHECK_EQUAL(Type::destroyed_objects(), old_destroyed + 2);
}

BOOST_AUTO_TEST_CASE(any_move_ctor)
{
    typedef ural::regular_tracer<std::string> Type;

    auto const old_active = Type::active_objects();

    {
        // Пустой
        ural::any a0;
        auto const a0_m = std::move(a0);

        BOOST_CHECK(a0.empty());
        BOOST_CHECK(a0_m.empty());

        // Не пустой
        auto const value = Type("42");
        ural::any a1(value);
        auto const a1_m = std::move(a1);

        BOOST_CHECK(a1.empty());
        BOOST_CHECK(!a1_m.empty());
        BOOST_CHECK(a1_m.get_pointer<Type>() != nullptr);
        BOOST_CHECK_EQUAL(*a1_m.get_pointer<Type>(), value);
    }

    BOOST_CHECK_EQUAL(Type::active_objects(), old_active);
}

BOOST_AUTO_TEST_CASE(any_swap_member_empty_and_empty)
{
    typedef ural::regular_tracer<std::string> Type;

    auto const old_active = Type::active_objects();

    {
        ural::any x;
        ural::any y;

        x.swap(y);

        BOOST_CHECK(x.empty());
        BOOST_CHECK(y.empty());
    }

    BOOST_CHECK_EQUAL(Type::active_objects(), old_active);
}

BOOST_AUTO_TEST_CASE(any_swap_member_empty_and_value)
{
    typedef ural::regular_tracer<std::string> Type;

    auto const old_active = Type::active_objects();

    {
        ural::any x;

        Type const y_value("42");
        ural::any y(y_value);

        x.swap(y);

        BOOST_CHECK(!x.empty());
        BOOST_CHECK(x.get_pointer<Type>() != nullptr);
        BOOST_CHECK_EQUAL(*x.get_pointer<Type>(), y_value);

        BOOST_CHECK(y.empty());
    }

    BOOST_CHECK_EQUAL(Type::active_objects(), old_active);
}

BOOST_AUTO_TEST_CASE(any_swap_member_value_and_empty)
{
    typedef ural::regular_tracer<std::string> Type;

    auto const old_active = Type::active_objects();

    {
        ural::any x;

        Type const y_value("42");
        ural::any y(y_value);

        y.swap(x);

        BOOST_CHECK(!x.empty());
        BOOST_CHECK(x.get_pointer<Type>() != nullptr);
        BOOST_CHECK_EQUAL(*x.get_pointer<Type>(), y_value);

        BOOST_CHECK(y.empty());
    }

    BOOST_CHECK_EQUAL(Type::active_objects(), old_active);
}

BOOST_AUTO_TEST_CASE(any_swap_member_value_and_value)
{
    typedef ural::regular_tracer<int> Tx;
    typedef ural::regular_tracer<std::string> Ty;

    auto const Tx_old_active = Tx::active_objects();
    auto const Ty_old_active = Ty::active_objects();

    {
        Tx const x_value(42);
        ural::any x(x_value);

        Ty const y_value("42");
        ural::any y(y_value);

        x.swap(y);

        BOOST_CHECK(!x.empty());
        BOOST_CHECK(x.get_pointer<Ty>() != nullptr);
        BOOST_CHECK_EQUAL(*x.get_pointer<Ty>(), y_value);

        BOOST_CHECK(!y.empty());
        BOOST_CHECK(y.get_pointer<Tx>() != nullptr);
        BOOST_CHECK_EQUAL(*y.get_pointer<Tx>(), x_value);
    }

    BOOST_CHECK_EQUAL(Tx::active_objects(), Tx_old_active);
    BOOST_CHECK_EQUAL(Ty::active_objects(), Ty_old_active);
}

BOOST_AUTO_TEST_CASE(any_move_assign_empty_and_empty)
{
    typedef ural::regular_tracer<std::string> Type;

    auto const old_active = Type::active_objects();

    {
        ural::any x;
        ural::any y;

        x = std::move(y);

        BOOST_CHECK(x.empty());
        BOOST_CHECK(y.empty());
    }

    BOOST_CHECK_EQUAL(Type::active_objects(), old_active);
}

BOOST_AUTO_TEST_CASE(any_move_assign_empty_and_value)
{
    typedef ural::regular_tracer<std::string> Type;

    auto const old_active = Type::active_objects();

    {
        ural::any x;

        Type const y_value("42");
        ural::any y(y_value);

        x = std::move(y);

        BOOST_CHECK(!x.empty());
        BOOST_CHECK(x.get_pointer<Type>() != nullptr);
        BOOST_CHECK_EQUAL(*x.get_pointer<Type>(), y_value);

        BOOST_CHECK(y.empty());
    }

    BOOST_CHECK_EQUAL(Type::active_objects(), old_active);
}

BOOST_AUTO_TEST_CASE(any_move_assign_value_and_empty)
{
    typedef ural::regular_tracer<std::string> Type;

    auto const old_active = Type::active_objects();

    {
        ural::any x;

        Type const y_value("42");
        ural::any y(y_value);

        x = std::move(y);

        BOOST_CHECK(!x.empty());
        BOOST_CHECK(x.get_pointer<Type>() != nullptr);
        BOOST_CHECK_EQUAL(*x.get_pointer<Type>(), y_value);

        BOOST_CHECK(y.empty());
    }

    BOOST_CHECK_EQUAL(Type::active_objects(), old_active);
}

BOOST_AUTO_TEST_CASE(any_move_assign_value_and_value)
{
    typedef ural::regular_tracer<int> Tx;
    typedef ural::regular_tracer<std::string> Ty;

    auto const Tx_old_active = Tx::active_objects();
    auto const Ty_old_active = Ty::active_objects();

    {
        Tx const x_value(42);
        ural::any x(x_value);

        Ty const y_value("42");
        ural::any y(y_value);

       x = std::move(y);

        BOOST_CHECK(!x.empty());
        BOOST_CHECK(x.get_pointer<Ty>() != nullptr);
        BOOST_CHECK_EQUAL(*x.get_pointer<Ty>(), y_value);

        BOOST_CHECK(!y.empty());
        BOOST_CHECK(y.get_pointer<Tx>() != nullptr);
        BOOST_CHECK_EQUAL(*y.get_pointer<Tx>(), x_value);
    }

    BOOST_CHECK_EQUAL(Tx::active_objects(), Tx_old_active);
    BOOST_CHECK_EQUAL(Ty::active_objects(), Ty_old_active);
}

BOOST_AUTO_TEST_CASE(any_copy_ctor_from_empty_test)
{
    ural::any x;
    ural::any y(x);

    BOOST_CHECK(x.empty());
    BOOST_CHECK(y.empty());
}

BOOST_AUTO_TEST_CASE(any_copy_ctor_from_value_test)
{
    typedef std::string Type;

    Type const x_value("ABC");
    ural::any x(x_value);

    ural::any y(x);

    BOOST_CHECK(!x.empty());
    BOOST_CHECK(!y.empty());
    BOOST_CHECK_EQUAL(*y.get_pointer<Type>(), x_value);
}

BOOST_AUTO_TEST_CASE(any_get_const_pointer_test)
{
    typedef std::string Type;

    // Пустой
    ural::any const a0;

    BOOST_CHECK(a0.get_pointer<Type>() == nullptr);
    BOOST_CHECK(a0.get_pointer<int>() == nullptr);

    // Не пустой
    auto const value = Type("42");
    ural::any const a1(value);

    BOOST_REQUIRE(a1.get_pointer<Type>() != nullptr);
    BOOST_CHECK(a1.get_pointer<int>()  == nullptr);

    BOOST_CHECK_EQUAL(*a1.get_pointer<Type>(), value);
    BOOST_CHECK_EQUAL(*a1.get_pointer<Type const>(), value);
}

BOOST_AUTO_TEST_CASE(any_get_pointer_test)
{
    typedef std::string Type;

    // Пустой
    ural::any a0;

    BOOST_CHECK(a0.get_pointer<Type>() == nullptr);
    BOOST_CHECK(a0.get_pointer<int>() == nullptr);

    // Не пустой
    auto const value = Type("42");
    ural::any a1(value);

    BOOST_CHECK(a1.get_pointer<int>()  == nullptr);
    BOOST_REQUIRE(a1.get_pointer<Type>() != nullptr);

    auto const new_value = Type("ABC");
    *a1.get_pointer<Type>() = new_value;

    BOOST_REQUIRE(a1.get_pointer<Type>() != nullptr);
    BOOST_CHECK_EQUAL(*ural::as_const(a1).get_pointer<Type>(), new_value);
}
