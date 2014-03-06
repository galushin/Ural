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

#include <string>
#include <memory>

#include <boost/test/unit_test.hpp>

#include <ural/memory.hpp>

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

// Тесты умного указателя с семантикой копирования
namespace
{
    class Base
    {
    public:
        std::unique_ptr<Base> clone() const;

        virtual ~ Base(){};

    private:
        virtual Base * clone_impl() const = 0;
    };
}

BOOST_AUTO_TEST_CASE(copy_ptr_types)
{
    typedef int Type;
    typedef ural::copy_ptr<Type> Copy_ptr;
    typedef std::unique_ptr<Type> Unique_ptr;

    static_assert(std::is_same<typename Copy_ptr::pointer,
                               typename Unique_ptr::pointer>::value, "");
    static_assert(std::is_same<typename Copy_ptr::element_type,
                               typename Unique_ptr::element_type>::value, "");
    static_assert(std::is_same<typename Copy_ptr::deleter_type,
                               typename Unique_ptr::deleter_type>::value, "");

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(copy_ptr_default_ctor_test)
{
    typedef int Type;

    ural::copy_ptr<Type> const p{};

    static_assert(noexcept(ural::copy_ptr<Type>{}),
                  "default ctor must be noexcept");

    BOOST_CHECK(static_cast<bool>(p) == false);
    BOOST_CHECK(!p);
    BOOST_CHECK(nullptr == p.get());
}

BOOST_AUTO_TEST_CASE(copy_ptr_ctor_test)
{
    typedef int Type;
    Type const value = 42;

    ural::copy_ptr<Type> const p(new Type{value});

    static_assert(noexcept(ural::copy_ptr<Type>{nullptr}),
                  "ctor must be noexcept");

    BOOST_CHECK(static_cast<bool>(p) == true);
    BOOST_CHECK(!p == false);
    BOOST_CHECK(nullptr != p.get());

    BOOST_CHECK_EQUAL(value, *p);
}

// @todo Тест конструктора copy_ptr(pointer, d1)
// @todo Тест конструктора copy_ptr(pointer, d2)

BOOST_AUTO_TEST_CASE(copy_ptr_move_ctor_test)
{
    typedef int Type;

    auto const value = Type{42};

    ural::copy_ptr<Type> p1(new Type{value});

    auto * old_ptr = p1.get();

    ural::copy_ptr<Type> p2(std::move(p1));

    static_assert(noexcept(ural::copy_ptr<Type>{ural::copy_ptr<Type>{}}),
                  "move ctor must be noexcept");

    BOOST_CHECK_EQUAL(old_ptr, p2.get());
    BOOST_CHECK_EQUAL(value, *p2);
    BOOST_CHECK_EQUAL(static_cast<Type*>(nullptr), p1.get());
}

// @todo Конструктор на основе nullptr
// @todo Конструктор на основе copy_ptr<U>

BOOST_AUTO_TEST_CASE(copy_ptr_ctor_from_unique_ptr)
{
    typedef int Type;
    auto const value = Type{42};
    auto p_u = ural::make_unique<Type>(value);

    auto const ptr_old = p_u.get();

    ural::copy_ptr<Type> p(std::move(p_u));

    BOOST_CHECK(p_u == nullptr);
    BOOST_CHECK_EQUAL(ptr_old, p.get());
    BOOST_CHECK_EQUAL(value, *p);
}

// @todo Конструктор на основе unique_ptr с совместимым типом
// @todo Конструктор на основе shared_ptr и auto_ptr

BOOST_AUTO_TEST_CASE(copy_ptr_copy_ctor_test)
{
    typedef int Type;

    ural::copy_ptr<Type> const p1(new Type{42});
    auto p2 = p1;

    BOOST_CHECK(*p1 == *p2);
    BOOST_CHECK(p1 != p2);
}

BOOST_AUTO_TEST_CASE(copy_ptr_copy_asign_test)
{
    typedef int Type;

    ural::copy_ptr<Type> p1(new Type{42});
    ural::copy_ptr<Type> p2;
    p2 = p1;

    BOOST_CHECK(p1.get() != p2.get());
    BOOST_CHECK_EQUAL(*p1, *p2);
}

BOOST_AUTO_TEST_CASE(copy_ptr_move_assign_test)
{
    typedef int Type;

    ural::copy_ptr<Type> p1(new Type{42});
    ural::copy_ptr<Type> p2(new Type{13});

    auto * old_p2 = p2.get();

    p1 = std::move(p2);

    BOOST_CHECK_EQUAL(old_p2, p1.get());
    BOOST_CHECK(nullptr == p2.get());
}

BOOST_AUTO_TEST_CASE(copy_ptr_release_test)
{
    typedef int Type;

    ural::copy_ptr<Type> p(new Type{42});

    auto const ptr_old = p.get();

    std::unique_ptr<Type> p_u(p.release());

    BOOST_CHECK_EQUAL(p_u.get(), ptr_old);
    BOOST_CHECK(p.get() == nullptr);
}

BOOST_AUTO_TEST_CASE(copy_ptr_equality_test)
{
    typedef int Type;
    ural::copy_ptr<Type> const p0{};
    ural::copy_ptr<Type> const pn(nullptr);
    ural::copy_ptr<Type> const p(new Type{42});

    BOOST_CHECK(p0 == p0);
    BOOST_CHECK(pn == pn);
    BOOST_CHECK(p0 == pn);
    BOOST_CHECK(pn == p0);

    BOOST_CHECK(p != p0);
    BOOST_CHECK(p != pn);
}
