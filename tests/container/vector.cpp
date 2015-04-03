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

// Общие требования (23.2)

#include <ural/utility/tracers.hpp>
#include <ural/container/vector.hpp>
#include <ural/numeric/numbers_sequence.hpp>

#include <boost/test/unit_test.hpp>

#include <forward_list>

// @todo 23.2.1
// Таблица 96
BOOST_AUTO_TEST_CASE(vector_types_test)
{
    typedef int T;
    typedef ural::vector<T> Vector;

    // строка 1
    static_assert(std::is_same<Vector::value_type, T>::value, "");

    // строка 2
    static_assert(std::is_same<Vector::reference, T&>::value, "");

    // строка 3
    static_assert(std::is_same<Vector::const_reference, T const&>::value, "");

    // итераторы
    typedef Vector::iterator Iterator;
    typedef std::iterator_traits<Iterator> Iterator_traits;

    typedef Vector::const_iterator Const_iterator;
    typedef std::iterator_traits<Const_iterator> Const_iterator_traits;

    // строка 4
    static_assert(std::is_convertible<Iterator_traits::iterator_category,
                                      std::forward_iterator_tag>::value, "");
    static_assert(std::is_same<Iterator_traits::value_type, T>::value, "");
    static_assert(std::is_convertible<Iterator, Const_iterator>::value, "");

    // строка 5
    static_assert(std::is_convertible<Const_iterator_traits::iterator_category,
                                      std::forward_iterator_tag>::value, "");
    static_assert(std::is_same<Const_iterator_traits::value_type, T>::value, "");

    // строка 6
    typedef Vector::difference_type Difference;

    static_assert(std::is_signed<Difference>::value, "");
    static_assert(std::is_same<Difference, Iterator_traits::difference_type>::value, "");
    static_assert(std::is_same<Difference, Const_iterator_traits::difference_type>::value, "");

    // строка 7
    typedef Vector::size_type Size;

    // @todo Может быть отказаться от этого, раз Страуструп считает это ошибкой?
    static_assert(std::is_unsigned<Size>::value, "");

    static_assert(std::is_same<Size, std::make_unsigned<Difference>::type>::value, "");


    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(vector_construct_empty)
{
    // Регрессия: при вызове конструктора без параметров память не распределяется
    typedef int T;
    typedef ural::tracing_allocator<T> Alloc;
    typedef ural::vector<T, Alloc> Vector;

    // Строка 8
    Alloc::reset_allocations_count();

    Vector u;
    BOOST_CHECK(u.empty());
    BOOST_CHECK_EQUAL(Alloc::allocations_count(), 0U);

    // Строка 9
    BOOST_CHECK(Vector().empty());
}

BOOST_AUTO_TEST_CASE(vector_copy_constructor)
{
    typedef int T;
    typedef ural::vector<T> Vector;

    auto const n = Vector::size_type(5);
    auto const t = Vector::value_type(42);

    Vector x(n, t);

    // Строка 10
    BOOST_CHECK(x == Vector(x));

    // Строка 11
    Vector u(x);

    BOOST_CHECK_NE(u.begin(), x.begin());
    BOOST_CHECK_NE(u.end(), x.end());

    BOOST_CHECK(u == x);

    BOOST_CHECK_EQUAL_COLLECTIONS(x.begin(), x.end(),
                                  u.begin(), u.end());
}

BOOST_AUTO_TEST_CASE(vector_move_constructor)
{
    typedef int T;
    typedef ural::vector<T> Vector;

    auto const n = Vector::size_type(5);
    auto const t = Vector::value_type(42);

    Vector x(n, t);

    auto const x_old = x;
    auto const old_data = x.data();

    // Строка 11
    Vector const x1(std::move(x));

    BOOST_CHECK_EQUAL(old_data, x1.data());

    BOOST_CHECK_EQUAL_COLLECTIONS(x_old.cbegin(), x_old.cend(),
                                  x1.begin(), x1.end());
    BOOST_CHECK(x.empty());
}

BOOST_AUTO_TEST_CASE(vector_move_assign)
{
    typedef int T;
    typedef ural::vector<T> Vector;

    auto x = Vector{1, 2, 3, 4, 5};
    auto const x_old = x;

    auto y = Vector(5, 2);
    auto const y_old = y;

    auto const x_data_old = x.data();
    auto const y_data_old = y.data();

    // Строка 12
    y = std::move(x);

    typedef decltype(y = std::move(x)) Result_type;

    static_assert(std::is_same<Result_type, Vector&>::value,
                  "must be reference to vector");

    BOOST_CHECK_EQUAL_COLLECTIONS(x.begin(), x.end(), y_old.begin(), y_old.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(y.begin(), y.end(), x_old.begin(), x_old.end());
    BOOST_CHECK_EQUAL(x.data(), y_data_old);
    BOOST_CHECK_EQUAL(y.data(), x_data_old);
}

// @todo Тест строки 12 для случая, когда вектор не может передать владение

// Таблица 96 строка 13 требует наличия деструктора, это тривиально

BOOST_AUTO_TEST_CASE(vector_member_swap_test)
{
    typedef int T;
    typedef ural::tracing_allocator<T> Alloc;
    typedef ural::vector<T, Alloc> Vector;

    Vector v1({1, 3, 5, 7}, Alloc(1));
    Vector v2({2, 4, 6, 8, 10}, Alloc(2));

    auto v1_data_old = v1.data();
    auto v2_data_old = v2.data();
    auto const v1_allocator_old = v1.get_allocator();
    auto const v2_allocator_old = v2.get_allocator();

    v1.swap(v2);

    BOOST_CHECK_EQUAL(v1.data(), v2_data_old);
    BOOST_CHECK_EQUAL(v2.data(), v1_data_old);
    BOOST_CHECK(v1.get_allocator() == v2_allocator_old);
    BOOST_CHECK(v2.get_allocator() == v1_allocator_old);
}

// @todo тест swap для случая, когда обменивать распределители нельзя

BOOST_AUTO_TEST_CASE(vector_non_member_swap_test)
{
    typedef int T;
    typedef ural::tracing_allocator<T> Alloc;
    typedef ural::vector<T, Alloc> Vector;

    Vector v1({1, 3, 5, 7}, Alloc(1));
    Vector v2({2, 4, 6, 8, 10}, Alloc(2));

    auto v1_data_old = v1.data();
    auto v2_data_old = v2.data();
    auto const v1_allocator_old = v1.get_allocator();
    auto const v2_allocator_old = v2.get_allocator();

    using ural::swap;
    swap(v1, v2);

    BOOST_CHECK_EQUAL(v1.data(), v2_data_old);
    BOOST_CHECK_EQUAL(v2.data(), v1_data_old);
    BOOST_CHECK(v1.get_allocator() == v2_allocator_old);
    BOOST_CHECK(v2.get_allocator() == v1_allocator_old);
}


// Таблица 97
BOOST_AUTO_TEST_CASE(vector_table_97)
{
    typedef int T;
    typedef ural::vector<T> Vector;

    typedef Vector::iterator Iterator;
    typedef Vector::const_iterator CIterator;

    // Строка 1
    typedef Vector::reverse_iterator RIterator;
    static_assert(std::is_same<RIterator, std::reverse_iterator<Iterator>>::value, "");

    // Строка 2
    typedef Vector::const_reverse_iterator CRIterator;
    static_assert(std::is_same<CRIterator, std::reverse_iterator<CIterator>>::value, "");

    // Строки 3-6
    auto const n = Vector::size_type(5);
    auto const t = Vector::value_type(42);

    Vector x(n, t);

    BOOST_CHECK(x.rbegin() == RIterator(x.end()));
    BOOST_CHECK(x.rend() == RIterator(x.begin()));

    BOOST_CHECK(x.crbegin() == CRIterator(x.cend()));
    BOOST_CHECK(x.crend() == CRIterator(x.cbegin()));
}

// Таблица 98
BOOST_AUTO_TEST_CASE(vector_table_98_less_comparable)
{
    typedef int T;
    typedef ural::vector<T> Vector;

    Vector const v1 = {1, 2, 3};
    Vector const v2 = {1, 2, 3, 4};
    Vector const v3 = {1, 4, 3, 4};

    // Строка 1 <
    BOOST_CHECK(v1 < v2);
    BOOST_CHECK(v2 < v3);
    BOOST_CHECK(v1 < v3);

    BOOST_CHECK(!(v1 < v1));
    BOOST_CHECK(!(v2 < v2));
    BOOST_CHECK(!(v3 < v3));

    // Строка 2 >
    BOOST_CHECK(v2 > v1);
    BOOST_CHECK(v3 > v2);
    BOOST_CHECK(v3 > v1);

    BOOST_CHECK(!(v1 > v1));
    BOOST_CHECK(!(v2 > v2));
    BOOST_CHECK(!(v3 > v3));

    // Строка 3 <=
    BOOST_CHECK(v1 <= v2);
    BOOST_CHECK(v2 <= v3);
    BOOST_CHECK(v1 <= v3);

    BOOST_CHECK(v1 <= v1);
    BOOST_CHECK(v2 <= v2);
    BOOST_CHECK(v3 <= v3);

    // Строка 4 >=
    BOOST_CHECK(v2 >= v1);
    BOOST_CHECK(v3 >= v2);
    BOOST_CHECK(v3 >= v1);

    BOOST_CHECK(v1 >= v1);
    BOOST_CHECK(v2 >= v2);
    BOOST_CHECK(v3 >= v3);
}

// Таблица 99
BOOST_AUTO_TEST_CASE(vector_allocator_constructor)
{
    typedef int T;
    typedef ural::tracing_allocator<T> Alloc;
    typedef ural::vector<T, Alloc> Vector;

    // Строка 1
    static_assert(std::is_same<Vector::allocator_type::value_type,
                               Vector::value_type>::value, "Allocator for wrong type!");

    // Строка 2
    static_assert(std::is_same<Alloc, decltype(Vector().get_allocator())>::value, "");

    // Строка 3
    auto const u_0 = Vector();

    BOOST_CHECK(u_0.empty());
    BOOST_CHECK(u_0.get_allocator() == Alloc());

    // Строка 4
    Alloc alloc(42);
    auto const u_1 = Vector(alloc);

    BOOST_CHECK(u_1.empty());
    BOOST_CHECK(u_1.get_allocator() == alloc);
}

BOOST_AUTO_TEST_CASE(vector_copy_with_other_allocator)
{
    typedef int T;
    typedef ural::tracing_allocator<T> Alloc;
    typedef ural::vector<T, Alloc> Vector;

    // Строка 5
    Vector const t = {1, 2, 3, 4, 5};
    Alloc alloc(42);

    Vector const u(t, alloc);

    BOOST_CHECK_EQUAL_COLLECTIONS(t.begin(), t.end(), u.begin(), u.end());
    BOOST_CHECK_EQUAL(alloc.id(), u.get_allocator().id());
}

BOOST_AUTO_TEST_CASE(vector_move_constructor_table_99)
{
    typedef int T;
    typedef ural::tracing_allocator<T> Alloc;
    typedef ural::vector<T, Alloc> Vector;

    // Строка 6
    Vector t = {1, 2, 3, 4, 5};

    Vector const t_old = t;
    auto const t_old_data = t.data();

    Vector const u(std::move(t));

    BOOST_CHECK_EQUAL_COLLECTIONS(t_old.begin(), t_old.end(), u.begin(), u.end());
    BOOST_CHECK_EQUAL(t_old_data, u.data());
    BOOST_CHECK_EQUAL(t_old.get_allocator().id(), u.get_allocator().id());
    BOOST_CHECK(t_old.get_allocator() == u.get_allocator());
}

BOOST_AUTO_TEST_CASE(vector_move_with_different_allocator)
{
    typedef std::string T;
    typedef ural::tracing_allocator<T> Alloc;
    typedef ural::vector<T, Alloc> Vector;

    // Строка 7
    Vector t = {"one", "two", "three", "four", "five"};
    auto const t_old = t;
    Vector::allocator_type alloc(42);

    Vector const u(std::move(t), alloc);

    BOOST_CHECK_EQUAL_COLLECTIONS(t_old.begin(), t_old.end(),
                                  u.begin(), u.end());
    BOOST_CHECK_EQUAL(alloc.id(), u.get_allocator().id());

    for(auto const & s : t)
    {
        BOOST_CHECK(s.empty());
    }
}

BOOST_AUTO_TEST_CASE(vector_move_with_same_allocator)
{
    typedef int T;
    typedef ural::vector<T> Vector;

    // Строка 7
    Vector t = {1, 2, 3, 4, 5};
    auto const t_old = t;
    Vector::allocator_type alloc;

    auto const t_data_old = t.data();

    Vector const u(std::move(t), alloc);

    BOOST_CHECK_EQUAL(t_data_old, u.data());

    BOOST_CHECK(t.empty());

    BOOST_CHECK_EQUAL_COLLECTIONS(t_old.begin(), t_old.end(),
                                  u.begin(), u.end());
}

// @todo Таблица 99 строки 8,9 и 10

// @todo 23.2.3

BOOST_AUTO_TEST_CASE(vector_n_copies_of_t)
{
    typedef int T;
    typedef ural::vector<T> Vector;

    auto const n = Vector::size_type(5);
    auto const t = Vector::value_type(42);

    Vector x(n, t);

    BOOST_CHECK_EQUAL(n, x.size());

    for(auto i : ural::indices_of(x))
    {
        BOOST_CHECK_EQUAL(x[i], t);
    }
}

BOOST_AUTO_TEST_CASE(vector_construct_from_input_iterators)
{
    typedef int T;
    typedef ural::vector<T> Vector;

    std::istringstream is("1 2 3 4 5");
    Vector const z{1, 2, 3, 4, 5};

    auto first = std::istream_iterator<T>(is);
    auto last = std::istream_iterator<T>();

    Vector const x(first, last);

    // @todo Число перераспределений памяти должно быть логарифмическим

    BOOST_CHECK_EQUAL_COLLECTIONS(x.begin(), x.end(),
                                  z.begin(), z.end());
}

BOOST_AUTO_TEST_CASE(vector_construct_from_forward_iterators)
{
    typedef int T;
    typedef ural::tracing_allocator<T> Alloc;
    typedef ural::vector<T, Alloc> Vector;

    std::forward_list<T> const z(501, 42);

    Alloc::reset_allocations_count();
    BOOST_CHECK_EQUAL(Alloc::allocations_count(), 0U);

    Vector const x(z.begin(), z.end());

    BOOST_CHECK_EQUAL(Alloc::allocations_count(), 1U);

    BOOST_CHECK_EQUAL_COLLECTIONS(x.begin(), x.end(),
                                  z.begin(), z.end());
}

BOOST_AUTO_TEST_CASE(vector_construct_from_init_list)
{
    typedef int T;
    typedef ural::vector<T> Vector;

    std::vector<T> const x_std = {1, 2, 3, 4, 5};
    Vector const x_ural = {1, 2, 3, 4, 5};

    BOOST_CHECK_EQUAL_COLLECTIONS(x_ural.begin(), x_ural.end(),
                                  x_std.begin(), x_std.end());
}

BOOST_AUTO_TEST_CASE(vector_clear_test)
{
    typedef int T;
    typedef ural::vector<T> Vector;

    Vector x = {1, 3, 5, 7, 9};

    BOOST_CHECK(!x.empty());

    auto const old_data = x.data();
    auto const old_capacity = x.capacity();

    x.clear();

    BOOST_CHECK(x.empty());
    BOOST_CHECK_EQUAL(old_data, x.data());
    BOOST_CHECK_EQUAL(old_capacity, x.capacity());
}

BOOST_AUTO_TEST_CASE(vector_assign_operator_init_list_shrink)
{
    typedef int T;
    typedef ural::vector<T> Vector;

    Vector const z = {1, 3, 5, 7, 9};
    Vector x(z.size() * 2 + 13, 42);

    BOOST_CHECK_LE(z.size(), x.size());

    x = {1, 3, 5, 7, 9};

    BOOST_CHECK_EQUAL_COLLECTIONS(x.begin(), x.end(), z.begin(), z.end());
}

BOOST_AUTO_TEST_CASE(vector_assign_operator_init_list_grow)
{
    typedef int T;
    typedef ural::vector<T> Vector;

    Vector const z = {1, 3, 5, 7, 9};
    Vector x(z.size() / 2, 42);

    BOOST_CHECK_GE(z.size(), x.size());

    x = {1, 3, 5, 7, 9};

    BOOST_CHECK_EQUAL_COLLECTIONS(x.begin(), x.end(), z.begin(), z.end());
}

BOOST_AUTO_TEST_CASE(vector_assign_operator_init_list_exact)
{
    typedef int T;
    typedef ural::vector<T> Vector;

    Vector const z = {1, 3, 5, 7, 9};
    Vector x(z.size(), 42);

    BOOST_CHECK_GE(z.size(), x.size());

    x = {1, 3, 5, 7, 9};

    BOOST_CHECK_EQUAL_COLLECTIONS(x.begin(), x.end(), z.begin(), z.end());
}

BOOST_AUTO_TEST_CASE(vector_assign_input_iterators)
{
    typedef int T;
    typedef ural::vector<T> Vector;

    std::istringstream is("1 2 3 4 5");
    Vector const z{1, 2, 3, 4, 5};

    auto first = std::istream_iterator<T>(is);
    auto last = std::istream_iterator<T>();

    Vector x(3, 42);

    BOOST_CHECK(x != z);
    BOOST_CHECK_LE(x.size(), z.size());

    x.assign(first, last);

    BOOST_CHECK_EQUAL_COLLECTIONS(x.begin(), x.end(),
                                  z.begin(), z.end());
}

BOOST_AUTO_TEST_CASE(vector_assign_n_shrink)
{
    typedef int T;
    typedef ural::vector<T> Vector;

    Vector x = {1, 3, 5, 7, 9};

    auto const n1 = x.size() / 2;
    auto const v1 = T{42};

    x.assign(n1, v1);

    auto const z = Vector(n1, v1);

    BOOST_CHECK_EQUAL_COLLECTIONS(x.begin(), x.end(), z.begin(), z.end());
}

BOOST_AUTO_TEST_CASE(vector_assign_n_grow)
{
    typedef int T;
    typedef ural::vector<T> Vector;

    Vector x = {1, 3, 5, 7, 9};

    auto const n1 = x.size() * 2;
    auto const v1 = T{42};

    x.assign(n1, v1);

    auto const z = Vector(n1, v1);

    BOOST_CHECK_EQUAL_COLLECTIONS(x.begin(), x.end(), z.begin(), z.end());
}

BOOST_AUTO_TEST_CASE(vector_assign_n_exact)
{
    typedef int T;
    typedef ural::vector<T> Vector;

    Vector x = {1, 3, 5, 7, 9};

    auto const n1 = x.size();
    auto const v1 = T{42};

    x.assign(n1, v1);

    auto const z = Vector(n1, v1);

    BOOST_CHECK_EQUAL_COLLECTIONS(x.begin(), x.end(), z.begin(), z.end());
}

BOOST_AUTO_TEST_CASE(vector_insert_n)
{
    typedef int T;
    typedef ural::vector<T> Vector;

    Vector x = {1, 3, 5, 7};

    auto const index = x.size() / 2;

    auto const result = x.insert(x.cbegin() + index, 2, 42);

    BOOST_CHECK(result == x.begin() + index);

    Vector const z = {1, 3, 42, 42, 5, 7};

    BOOST_CHECK_EQUAL_COLLECTIONS(x.begin(), x.end(), z.begin(), z.end());
}

BOOST_AUTO_TEST_CASE(vector_front_test)
{
    typedef int T;
    typedef ural::vector<T> Vector;

    Vector xs = {1, 2, 3, 4, 5};

    Vector const & c_xs = xs;

    BOOST_CHECK_EQUAL(xs[0], xs.front());
    BOOST_CHECK_EQUAL(c_xs[0], c_xs.front());
}

BOOST_AUTO_TEST_CASE(vector_back_test)
{
    typedef int T;
    typedef ural::vector<T> Vector;

    Vector xs = {1, 2, 3, 4, 5};

    Vector const & c_xs = xs;

    BOOST_CHECK_EQUAL(xs[xs.size() - 1], xs.back());
    BOOST_CHECK_EQUAL(c_xs[c_xs.size() - 1], c_xs.back());
}

BOOST_AUTO_TEST_CASE(vector_index_access_exception)
{
    typedef ural::vector<int, ural::use_default, ural::container_checking_throw_policy>
        Vector;

    Vector const x(13, 42);

    BOOST_CHECK_THROW(x[x.size()], std::out_of_range);
}

BOOST_AUTO_TEST_CASE(vector_at_test)
{
    typedef int T;
    typedef ural::vector<T> Vector;

    Vector xs = {1, 2, 3, 4, 5};

    Vector const & c_xs = xs;

    BOOST_CHECK_THROW(xs.at(xs.size()), std::out_of_range);
    BOOST_CHECK_THROW(c_xs.at(xs.size()), std::out_of_range);

    for(auto i : ural::indices_of(xs))
    {
        BOOST_CHECK_EQUAL(xs[i], xs.at(i));
        BOOST_CHECK_EQUAL(c_xs[i], c_xs.at(i));
    }
}

// 23.3.6.4 Доступ к данным
BOOST_AUTO_TEST_CASE(vector_const_data_test)
{
    typedef int T;
    typedef ural::vector<T> Vector;

    Vector const xs = {1, 2, 3, 4, 5};

    BOOST_CHECK_EQUAL_COLLECTIONS(xs.begin(), xs.end(),
                                  xs.data(), xs.data() + xs.size());
}

BOOST_AUTO_TEST_CASE(vector_data_test)
{
    typedef int T;
    typedef ural::vector<T> Vector;

    std::vector<T> const src = {3, 1, 4, 1, 5};

    Vector xs = {1, 2, 3, 4, 5};

    BOOST_CHECK_GE(xs.size(), src.size());

    std::copy(src.begin(), src.end(), xs.data());

    BOOST_CHECK_EQUAL_COLLECTIONS(src.begin(), src.end(),
                                  xs.data(), xs.data() + xs.size());
}

// 23.3.6.5 Модификаторы
BOOST_AUTO_TEST_CASE(vector_push_back_rvalue)
{
    typedef std::string T;
    typedef ural::vector<T> Vector;

    Vector xs;
    auto value = T("abc 42");
    auto const old_value = value;
    auto const value_old_data = value.data();

    xs.push_back(std::move(value));

    BOOST_CHECK_EQUAL(xs.back(), old_value);
    BOOST_CHECK_EQUAL(xs.back().data(), value_old_data);
    BOOST_CHECK(value.empty());
}

BOOST_AUTO_TEST_CASE(vector_pop_back_test)
{
    typedef int T;
    typedef ural::vector<T> Vector;

    Vector const src = {3, 1, 4, 1, 5};

    auto xs = src;
    xs.push_back(9);
    auto const old_data = xs.data();

    BOOST_CHECK_EQUAL(xs.size(), src.size() + 1);

    xs.pop_back();

    BOOST_CHECK_EQUAL(old_data, xs.data());
    BOOST_CHECK_EQUAL_COLLECTIONS(src.begin(), src.end(),
                                  xs.begin(), xs.end());
}

BOOST_AUTO_TEST_CASE(vector_insert_middle_range)
{
    typedef int T;
    typedef ural::vector<T> Vector;

    std::vector<T> to_insert = {2, 3};

    Vector v_ural = {1, 4, 5};

    std::vector<T> v_std(v_ural.begin(), v_ural.end());

    auto const pos = v_ural.size() / 2;

    auto r_std = v_std.insert(v_std.begin() + pos, to_insert.begin(), to_insert.end());
    auto r_ural = v_ural.insert(v_ural.cbegin() + pos, to_insert.begin(), to_insert.end());

    BOOST_CHECK_EQUAL(r_std - v_std.begin(), ural::to_signed(pos));
    BOOST_CHECK_EQUAL(r_ural - v_ural.cbegin(), ural::to_signed(pos));

    BOOST_CHECK_EQUAL_COLLECTIONS(v_std.cbegin(), v_std.cend(),
                                  v_ural.cbegin(), v_ural.cend());
}

BOOST_AUTO_TEST_CASE(vector_insert_middle)
{
    typedef int T;
    typedef ural::vector<T> Vector;

    auto const pos = 2;
    auto const new_value = T{3};

    Vector x = {1, 2, 4, 5};
    Vector const z = {1, 2, new_value, 4, 5};


    auto const result = x.insert(x.cbegin() + pos, new_value);

    BOOST_CHECK_EQUAL(result - x.begin(), pos);
    BOOST_CHECK_EQUAL(*result, new_value);

    BOOST_CHECK_EQUAL_COLLECTIONS(x.begin(), x.end(),
                                  z.begin(), z.end());
}

BOOST_AUTO_TEST_CASE(vector_insert_middle_with_move)
{
    typedef std::string T;
    typedef ural::vector<T> Vector;

    auto const pos = 2;
    auto const new_value = T{"three"};
    auto obj = new_value;

    void const * old_obj_data = obj.data();

    Vector x = {"one", "two", "four", "five"};
    Vector const z = {"one", "two", new_value, "four", "five"};

    auto const result = x.insert(x.cbegin() + pos, std::move(obj));

    BOOST_CHECK_EQUAL(result - x.begin(), pos);
    BOOST_CHECK_EQUAL(*result, new_value);
    BOOST_CHECK_EQUAL(old_obj_data, result->data());

    BOOST_CHECK_EQUAL_COLLECTIONS(x.begin(), x.end(),
                                  z.begin(), z.end());
}

BOOST_AUTO_TEST_CASE(vector_insert_middle_init_list)
{
    typedef int T;
    typedef ural::vector<T> Vector;

    Vector v_ural = {1, 4, 5};

    std::vector<T> v_std(v_ural.begin(), v_ural.end());

    auto const pos = v_ural.size() / 2;

    auto r_std = v_std.insert(v_std.begin() + pos, {2, 3});
    auto r_ural = v_ural.insert(v_ural.cbegin() + pos, {2, 3});

    BOOST_CHECK_EQUAL(r_std - v_std.begin(), ural::to_signed(pos));
    BOOST_CHECK_EQUAL(r_ural - v_ural.cbegin(), ural::to_signed(pos));

    BOOST_CHECK_EQUAL_COLLECTIONS(v_std.cbegin(), v_std.cend(),
                                  v_ural.cbegin(), v_ural.cend());
}

BOOST_AUTO_TEST_CASE(vector_erase_one)
{
    ural::vector<int> x = {1, 2, -3, 3, 4, 5};

    auto const index = 2;
    auto const pos = x.cbegin() + index;

    BOOST_CHECK_EQUAL(*pos, -3);

    auto result = x.erase(pos);

    BOOST_CHECK_EQUAL(result - x.cbegin(), index);

    ural::vector<int> const z = {1, 2, 3, 4, 5};
    BOOST_CHECK_EQUAL_COLLECTIONS(x.cbegin(), x.cend(), z.begin(), z.end());
}

// 23.3.6
BOOST_AUTO_TEST_CASE(vector_construct_size_and_allocator)
{
    typedef std::string T;
    typedef ural::tracing_allocator<T> Alloc;
    typedef ural::vector<T, Alloc> Vector;

    Alloc alloc(42);

    auto const n = 15U;

    Vector const z(n, alloc);

    BOOST_CHECK_EQUAL(n, z.size());
    BOOST_CHECK_EQUAL(alloc.id(), z.get_allocator().id());

    auto const empty_str = T{};
    for(auto const & s : z)
    {
        BOOST_CHECK_EQUAL(s, empty_str);
    }
}

BOOST_AUTO_TEST_CASE(vector_resize_grow)
{
    typedef std::string T;
    typedef ural::vector<T> Vector;

    auto const n0 = 5;
    auto const s0 = T("tree");
    auto const dn = 7;

    Vector x(n0, s0);

    x.resize(x.size() + dn, s0);

    Vector const z(x.size(), s0);

    BOOST_CHECK_EQUAL_COLLECTIONS(x.begin(), x.end(), z.begin(), z.end());
}

BOOST_AUTO_TEST_CASE(vector_resize_shrink)
{
    typedef std::string T;
    typedef ural::vector<T> Vector;

    auto const s0 = T("tree");

    Vector x = {"one", "two", "three", "four", "five"};

    auto const new_size = x.size() / 2;

    BOOST_CHECK_LE(new_size, x.size());

    auto const z = Vector(x.begin(), x.begin() + new_size);

    x.resize(new_size, s0);

    BOOST_CHECK_EQUAL_COLLECTIONS(x.begin(), x.end(), z.begin(), z.end());
}

BOOST_AUTO_TEST_CASE(vector_resize_same_size)
{
    typedef std::string T;
    typedef ural::vector<T> Vector;

    auto const s0 = T("tree");

    Vector x = {"one", "two", "three", "four", "five"};
    auto const x_old = x;

    x.resize(x.size(), s0);

    BOOST_CHECK_EQUAL_COLLECTIONS(x.begin(), x.end(), x_old.begin(), x_old.end());
}

BOOST_AUTO_TEST_CASE(vector_resize_grow_default)
{
    typedef std::string T;
    typedef ural::vector<T> Vector;

    auto const n0 = 5;
    auto const s0 = T{};
    auto const dn = 7;

    Vector x(n0, s0);

    x.resize(x.size() + dn);

    Vector const z(x.size(), s0);

    BOOST_CHECK_EQUAL_COLLECTIONS(x.begin(), x.end(), z.begin(), z.end());
}

BOOST_AUTO_TEST_CASE(vector_resize_shrink_default)
{
    typedef std::string T;
    typedef ural::vector<T> Vector;

    Vector x = {"one", "two", "three", "four", "five"};

    auto const new_size = x.size() / 2;

    BOOST_CHECK_LE(new_size, x.size());

    auto const z = Vector(x.begin(), x.begin() + new_size);

    x.resize(new_size);

    BOOST_CHECK_EQUAL_COLLECTIONS(x.begin(), x.end(), z.begin(), z.end());
}

BOOST_AUTO_TEST_CASE(vector_resize_same_size_default)
{
    typedef std::string T;
    typedef ural::vector<T> Vector;

    Vector x = {"one", "two", "three", "four", "five"};
    auto const x_old = x;

    x.resize(x.size());

    BOOST_CHECK_EQUAL_COLLECTIONS(x.begin(), x.end(), x_old.begin(), x_old.end());
}

// Качество реализации
BOOST_AUTO_TEST_CASE(vector_optimize_empty_allocator)
{
    typedef std::string T;
    typedef std::allocator<T> Alloc;
    typedef ural::vector<T, Alloc> Vector;

    static_assert(std::is_empty<Alloc>::value, "");

    static_assert(sizeof(Vector) == 3 * sizeof(T*), "");
    BOOST_CHECK_EQUAL(sizeof(Vector), 3 * sizeof(T*));

    BOOST_CHECK(true);
}
