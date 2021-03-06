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

#include <ural/memory.hpp>
#include <ural/utility/tracers.hpp>

#include <string>
#include <memory>

#include <boost/test/unit_test.hpp>

namespace
{
    namespace ural_ex = ::ural::experimental;
}

BOOST_AUTO_TEST_CASE(make_unique_array)
{
    auto const n = 5;
    auto p = ural::make_unique<std::string[]>(n);

    // FAILS: auto p_fail = ural::make_unique<std::string[5]>(n);

    static_assert(std::is_same<std::unique_ptr<std::string[]>, decltype(p)>::value, "");

    const std::string value {};

    for(size_t i = 0; i < n; ++ i)
    {
        BOOST_CHECK_EQUAL(value, p[i]);
    }
}

// ����� ������ ��������� � ���������� �����������
namespace
{
    class Base
    {
    public:
        std::unique_ptr<Base> clone() const
        {
            return std::unique_ptr<Base>{this->clone_impl()};
        }

        virtual ~ Base(){};

    private:
        virtual Base * clone_impl() const = 0;
    };

    class Derived
     : public Base
    {
    public:
        Derived(int val)
         : value{val}
        {}

        int value;

    private:
        virtual Base * clone_impl() const override
        {
            return ural_ex::make_copy_new(*this).release();
        }
    };

    class MoreDerived
     : public Derived
    {
    public:
        MoreDerived(int val)
         : Derived{val}
        {}

    private:
    };
}

BOOST_AUTO_TEST_CASE(copy_ptr_types)
{
    typedef int Type;
    typedef ural_ex::copy_ptr<Type> Copy_ptr;
    typedef std::unique_ptr<Type> Unique_ptr;

    static_assert(std::is_same<typename Copy_ptr::pointer,
                               typename Unique_ptr::pointer>::value, "");
    static_assert(std::is_same<typename Copy_ptr::element_type,
                               typename Unique_ptr::element_type>::value, "");
    static_assert(std::is_same<typename Copy_ptr::deleter_type,
                               typename Unique_ptr::deleter_type>::value, "");
    static_assert(std::is_same<typename Copy_ptr::cloner_type,
                               ural_ex::default_copy<Type>>::value, "");
    static_assert(std::is_same<typename Copy_ptr::checker_type,
                               ural_ex::default_ptr_checker<Type*>>::value, "");

    static_assert(sizeof(Copy_ptr) == sizeof(Unique_ptr),
                  "Copy ptr with default policies must be lean!");

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(copy_ptr_default_ctor_test)
{
    typedef int Type;

    ural_ex::copy_ptr<Type> const p{};

    static_assert(noexcept(ural_ex::copy_ptr<Type>{}),
                  "default ctor must be noexcept");

    BOOST_CHECK(static_cast<bool>(p) == false);
    BOOST_CHECK(!p);
    BOOST_CHECK(nullptr == p.get());
}

BOOST_AUTO_TEST_CASE(copy_ptr_dereference_check_test)
{
    typedef int Type;
    typedef ural_ex::copy_ptr<Type, ural::use_default, ural::use_default,
                                ural_ex::throwing_ptr_checker<Type*>> Pointer;

    Pointer const p0{};
    Pointer const p1{ural::make_unique<int>(42)};

    static_assert(noexcept(ural_ex::copy_ptr<Pointer>{}),
                  "default ctor must be noexcept");

    BOOST_CHECK(static_cast<bool>(p0) == false);
    BOOST_CHECK_THROW(*p0, std::logic_error);

    BOOST_CHECK(static_cast<bool>(p1) == true);
    BOOST_CHECK_EQUAL(42, *p1);
}

BOOST_AUTO_TEST_CASE(copy_ptr_ctor_test)
{
    typedef int Type;
    Type const value = 42;

    ural_ex::copy_ptr<Type> const p(new Type{value});

    static_assert(noexcept(ural_ex::copy_ptr<Type>{nullptr}),
                  "ctor must be noexcept");

    BOOST_CHECK(static_cast<bool>(p) == true);
    BOOST_CHECK(!p == false);
    BOOST_CHECK(nullptr != p.get());

    BOOST_CHECK_EQUAL(value, *p);
}

namespace
{
    template <class T>
    class deleter_with_id
    {
    public:
        deleter_with_id(int id)
         : id_(id)
        {}

        int id() const
        {
            return id_;
        }

        void set_id(int new_id)
        {
            id_ = new_id;
        }

        void operator()(T * p) const
        {
            delete p;
        }

    private:
        int id_;
    };
}

BOOST_AUTO_TEST_CASE(copy_ptr_copy_init_deleter_test)
{
    deleter_with_id<int> d(13);

    ural_ex::copy_ptr<int, ural::use_default, deleter_with_id<int>>
        p(new int{42}, d);

    BOOST_CHECK_EQUAL(d.id(), p.get_deleter().id());
}

BOOST_AUTO_TEST_CASE(copy_ptr_copy_init_deleter_cref_test)
{
    deleter_with_id<int> d(13);

    ural_ex::copy_ptr<int, ural::use_default, deleter_with_id<int> const &>
        p(new int{42}, d);

    BOOST_CHECK_EQUAL(d.id(), p.get_deleter().id());
    BOOST_CHECK_EQUAL(&d, &p.get_deleter());
}

BOOST_AUTO_TEST_CASE(copy_ptr_copy_init_deleter_ref_test)
{
    deleter_with_id<int> d(13);

    ural_ex::copy_ptr<int, ural::use_default, deleter_with_id<int> const &>
        p(new int{42}, d);

    BOOST_CHECK_EQUAL(d.id(), p.get_deleter().id());
    BOOST_CHECK_EQUAL(&d, &p.get_deleter());

    d.set_id(0);
    BOOST_CHECK_EQUAL(d.id(), p.get_deleter().id());
}

BOOST_AUTO_TEST_CASE(copy_ptr_move_init_deleter_test)
{
    const auto id = 13;
    ural_ex::copy_ptr<int, ural::use_default, deleter_with_id<int>>
        p(new int{42}, deleter_with_id<int>(id));

    BOOST_CHECK_EQUAL(id, p.get_deleter().id());
}

BOOST_AUTO_TEST_CASE(copy_ptr_move_ctor_test)
{
    typedef ural_ex::regular_tracer<int> Type;

    auto const old_active = Type::active_objects();

    {
        auto const value = Type{42};

        ural_ex::copy_ptr<Type> p1(ural::make_unique<Type>(value));

        auto * old_ptr = p1.get();

        ural_ex::copy_ptr<Type> p2(std::move(p1));

        static_assert(noexcept(ural_ex::copy_ptr<Type>{ural_ex::copy_ptr<Type>{}}),
                      "move ctor must be noexcept");

        BOOST_CHECK_EQUAL(old_ptr, p2.get());
        BOOST_CHECK_EQUAL(value, *p2);
        BOOST_CHECK_EQUAL(static_cast<Type*>(nullptr), p1.get());
    }

    auto const new_active = Type::active_objects();

    BOOST_CHECK_EQUAL(old_active, new_active);
}

BOOST_AUTO_TEST_CASE(copy_ptr_nullptr_ctor_test)
{
    typedef int Type;

    ural_ex::copy_ptr<Type> const p{nullptr};

    static_assert(noexcept(ural_ex::copy_ptr<Type>{nullptr}),
                  "default ctor must be noexcept");

    BOOST_CHECK(static_cast<bool>(p) == false);
    BOOST_CHECK(!p);
    BOOST_CHECK(nullptr == p.get());
}

BOOST_AUTO_TEST_CASE(copy_ptr_ctor_from_unique_ptr)
{
    typedef int Type;
    auto const value = Type{42};
    auto p_u = ural::make_unique<Type>(value);

    auto const ptr_old = p_u.get();

    ural_ex::copy_ptr<Type> p(std::move(p_u));

    BOOST_CHECK(p_u == nullptr);
    BOOST_CHECK_EQUAL(ptr_old, p.get());
    BOOST_CHECK_EQUAL(value, *p);
}

BOOST_AUTO_TEST_CASE(copy_ptr_ctor_unique_ptr_U_test)
{
    auto const value = 42;

    auto pu = ural::make_unique<MoreDerived>(value);
    ural_ex::copy_ptr<Derived> pc(std::move(pu));

    BOOST_CHECK(!!pc);
    BOOST_CHECK_EQUAL(value, pc->value);

    pc = std::unique_ptr<MoreDerived>{};

    BOOST_CHECK(!pc);
}

BOOST_AUTO_TEST_CASE(copy_ptr_copy_ctor_test)
{
    typedef int Type;

    ural_ex::copy_ptr<Type> const p1(ural::make_unique<Type>(42));
    auto p2 = p1;

    BOOST_CHECK(*p1 == *p2);
    BOOST_CHECK(p1 != p2);
}

BOOST_AUTO_TEST_CASE(copy_ptr_nullptr_assign_test)
{
    typedef int Type;

    ural_ex::copy_ptr<Type> p{ural::make_unique<Type>(42)};

    p = nullptr;

    BOOST_CHECK(static_cast<bool>(p) == false);
    BOOST_CHECK(!p);
    BOOST_CHECK(nullptr == p.get());
}

BOOST_AUTO_TEST_CASE(copy_ptr_copy_assign_test)
{
    typedef int Type;

    ural_ex::copy_ptr<Type> p1(ural::make_unique<Type>(42));
    ural_ex::copy_ptr<Type> p2;
    p2 = p1;

    BOOST_CHECK(p1.get() != p2.get());
    BOOST_CHECK_EQUAL(*p1, *p2);
}

BOOST_AUTO_TEST_CASE(copy_ptr_compatible_copy_test)
{
    ural_ex::copy_ptr<MoreDerived> p1{ural::make_unique<MoreDerived>(42)};
    ural_ex::copy_ptr<Derived> p2{p1};

    BOOST_CHECK(p1.get() != p2.get());
    BOOST_CHECK_EQUAL(p1->value, p2->value);

    BOOST_CHECK(typeid(*p1) == typeid(MoreDerived));
    BOOST_CHECK(typeid(*p2) == typeid(MoreDerived));
    BOOST_CHECK(typeid(*p1) == typeid(*p2));
    BOOST_CHECK_EQUAL(typeid(*p1).name(), typeid(*p2).name());
}

BOOST_AUTO_TEST_CASE(copy_ptr_copy_polymorhic_test)
{
    typedef ural_ex::copy_ptr<Base, ural_ex::member_function_copy<Base>> Pointer;

    Pointer p1{ural::make_unique<Derived>(42)};
    Pointer p2{p1};

    BOOST_CHECK(p1.get() != p2.get());

    BOOST_CHECK(typeid(*p1) == typeid(Derived));
    BOOST_CHECK(typeid(*p2) == typeid(Derived));
    BOOST_CHECK(typeid(*p1) == typeid(*p2));
    BOOST_CHECK_EQUAL(typeid(*p1).name(), typeid(*p2).name());

    auto & r1 = dynamic_cast<Derived&>(*p1);
    auto & r2 = dynamic_cast<Derived&>(*p2);

    BOOST_CHECK(r1.value == r2.value);
}

BOOST_AUTO_TEST_CASE(copy_ptr_move_compatible_ctor_test)
{
    auto const value = 42;

    ural_ex::copy_ptr<Derived> p1{ural::make_unique<Derived>(value)};
    auto const ptr_old = p1.get();

    ural_ex::copy_ptr<Base> p2{std::move(p1)};

    BOOST_CHECK(!p1);
    BOOST_CHECK(!!p2);

    BOOST_CHECK(typeid(*p2) == typeid(Derived));
    BOOST_CHECK_EQUAL(ptr_old, p2.get());
}

BOOST_AUTO_TEST_CASE(copy_ptr_move_compatible_assign_test)
{
    auto const value = 42;

    ural_ex::copy_ptr<Derived> p1{ural::make_unique<Derived>(value)};
    auto const ptr_old = p1.get();

    ural_ex::copy_ptr<Base> p2;
    p2 = std::move(p1);

    BOOST_CHECK(!p1);
    BOOST_CHECK(!!p2);

    BOOST_CHECK(typeid(*p2) == typeid(Derived));
    BOOST_CHECK_EQUAL(ptr_old, p2.get());
}

BOOST_AUTO_TEST_CASE(copy_ptr_copy_compatible_ctor_test)
{
    ural_ex::copy_ptr<Derived, ural_ex::member_function_copy<Base>>
        p1{ural::make_unique<Derived>(42)};
    ural_ex::copy_ptr<Base, ural_ex::member_function_copy<Base>>
        p2{p1};

    BOOST_CHECK(p1.get() != p2.get());

    BOOST_CHECK(typeid(*p1) == typeid(Derived));
    BOOST_CHECK(typeid(*p2) == typeid(Derived));
    BOOST_CHECK(typeid(*p1) == typeid(*p2));
    BOOST_CHECK_EQUAL(typeid(*p1).name(), typeid(*p2).name());

    auto & r1 = dynamic_cast<Derived&>(*p1);
    auto & r2 = dynamic_cast<Derived&>(*p2);

    BOOST_CHECK(r1.value == r2.value);
}

BOOST_AUTO_TEST_CASE(copy_ptr_assign_polymorhic_test)
{
    typedef ural_ex::copy_ptr<Base, ural_ex::member_function_copy<Base>> Pointer;

    Pointer p1{ural::make_unique<Derived>(42)};
    Pointer p2{p1};
    p2 = p1;

    BOOST_CHECK(p1.get() != p2.get());

    BOOST_CHECK(typeid(*p1) == typeid(Derived));
    BOOST_CHECK(typeid(*p2) == typeid(Derived));
    BOOST_CHECK(typeid(*p1) == typeid(*p2));
    BOOST_CHECK_EQUAL(typeid(*p1).name(), typeid(*p2).name());
}

BOOST_AUTO_TEST_CASE(copy_ptr_compatible_copy_assign)
{
    ural_ex::copy_ptr<MoreDerived> p1{ural::make_unique<MoreDerived>(42)};
    ural_ex::copy_ptr<Derived> p2;
    p2 = p1;

    BOOST_CHECK(p1.get() != p2.get());
    BOOST_CHECK_EQUAL(p1->value, p2->value);

    BOOST_CHECK(typeid(*p1) == typeid(MoreDerived));
    BOOST_CHECK(typeid(*p2) == typeid(MoreDerived));
    BOOST_CHECK(typeid(*p1) == typeid(*p2));
    BOOST_CHECK_EQUAL(typeid(*p1).name(), typeid(*p2).name());
}

BOOST_AUTO_TEST_CASE(copy_ptr_move_assign_test)
{
    typedef int Type;

    ural_ex::copy_ptr<Type> p1(ural::make_unique<Type>(42));
    ural_ex::copy_ptr<Type> p2(ural::make_unique<Type>(13));

    auto * old_p2 = p2.get();

    p1 = std::move(p2);

    BOOST_CHECK_EQUAL(old_p2, p1.get());
    BOOST_CHECK(nullptr == p2.get());
}

BOOST_AUTO_TEST_CASE(copy_ptr_member_access_test)
{
    typedef Derived Type;

    auto const value = 42;

    ural_ex::copy_ptr<Derived const> p_c(ural::make_unique<Type>(value));

    BOOST_CHECK_EQUAL(value, p_c->value);
}

BOOST_AUTO_TEST_CASE(copy_ptr_release_test)
{
    typedef int Type;

    auto const value = 42;

    ural_ex::copy_ptr<Type> p(ural::make_unique<Type>(value));

    auto const ptr_old = p.get();

    std::unique_ptr<Type> p_u(p.release());

    BOOST_CHECK_EQUAL(p_u.get(), ptr_old);
    BOOST_CHECK(p.get() == nullptr);
}

BOOST_AUTO_TEST_CASE(copy_ptr_swap_test)
{
    typedef int Type;
    typedef ural_ex::copy_ptr<Type> Pointer;

    Pointer p1(ural::make_unique<Type>(42));
    Pointer p2(ural::make_unique<Type>(13));

    auto p1_old = p1.get();
    auto p2_old = p2.get();

    p1.swap(p2);
    static_assert(noexcept(p1.swap(p2)), "swap must be noexcept");

    BOOST_CHECK_EQUAL(p1_old, p2.get());
    BOOST_CHECK_EQUAL(p2_old, p1.get());

    void(*swap_ptr)(Pointer &, Pointer &) = &ural::experimental::swap;

    swap_ptr(p1, p2);
    static_assert(noexcept(::ural::experimental::swap(p1, p2)),
                  "swap must be noexcept");

    BOOST_CHECK_EQUAL(p1_old, p1.get());
    BOOST_CHECK_EQUAL(p2_old, p2.get());
}

BOOST_AUTO_TEST_CASE(copy_ptr_equality_test)
{
    typedef int Type;
    ural_ex::copy_ptr<Type> const p0{};
    ural_ex::copy_ptr<Type> const pn(nullptr);
    ural_ex::copy_ptr<Type> const p(ural::make_unique<Type>(42));

    BOOST_CHECK(p0 == p0);
    BOOST_CHECK(pn == pn);
    BOOST_CHECK(p0 == pn);
    BOOST_CHECK(pn == p0);

    BOOST_CHECK(p != p0);
    BOOST_CHECK(p != pn);

    BOOST_CHECK(p0 == nullptr);
    BOOST_CHECK(nullptr == p0);

    BOOST_CHECK(pn == nullptr);
    BOOST_CHECK(nullptr == pn);

    BOOST_CHECK(p != nullptr);
    BOOST_CHECK(nullptr != p);

    auto * const ptr = p.get();

    BOOST_CHECK(p0 != ptr);
    BOOST_CHECK(ptr != p0);

    BOOST_CHECK(pn != ptr);
    BOOST_CHECK(ptr != pn);

    BOOST_CHECK(p == ptr);
    BOOST_CHECK(ptr == p);

    ural_ex::copy_ptr<long> const p0_long{};
    ural_ex::copy_ptr<long> const p1_long{ural::make_unique<long>(42)};

    BOOST_CHECK(p0_long == p0);
    BOOST_CHECK(p1_long != p);
}

BOOST_AUTO_TEST_CASE(make_copy_ptr_test)
{
    auto const value = 42;
    ural_ex::copy_ptr<Derived> p = ural_ex::make_copy_ptr<Derived>(value);
    BOOST_CHECK(!!p);
    BOOST_CHECK_EQUAL(value, p->value);
}
