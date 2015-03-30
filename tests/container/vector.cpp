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

// @todo тесты для разных типов элементов

// @todo 23.2.1
// Таблица 96
BOOST_AUTO_TEST_CASE(vector_types_test)
{
    typedef int T;
    typedef ural::vector<T> Vector;

    // строка 1
    static_assert(std::is_same<Vector::value_type, T>::value, "");
    // @todo проверить, что T является Eraseble из Vector (см. 23.2.1)

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
    Vector u(x);

    BOOST_CHECK_NE(u.begin(), x.begin());
    BOOST_CHECK_NE(u.end(), x.end());

    // @todo Проверить, что T является CopyInsertable для Vector

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

    Vector const x1(std::move(x));

    BOOST_CHECK_EQUAL(old_data, x1.data());

    BOOST_CHECK_EQUAL_COLLECTIONS(x_old.cbegin(), x_old.cend(),
                                  x1.begin(), x1.end());
    BOOST_CHECK(x.empty());
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

    // @todo строки 3-6
    auto const n = Vector::size_type(5);
    auto const t = Vector::value_type(42);

    Vector x(n, t);

    BOOST_CHECK(x.rbegin() == RIterator(x.end()));
    BOOST_CHECK(x.rend() == RIterator(x.begin()));

    BOOST_CHECK(x.crbegin() == CRIterator(x.cend()));
    BOOST_CHECK(x.crend() == CRIterator(x.cbegin()));
}

// @todo Таблица 98
// @todo Таблица 99

// @todo 23.2.3

BOOST_AUTO_TEST_CASE(vector_n_copies_of_t)
{
    typedef int T;
    typedef ural::vector<T> Vector;

    auto const n = Vector::size_type(5);
    auto const t = Vector::value_type(42);

    Vector x(n, t);

    // @todo Проверить, что T является CopyConstructible для Vector
    BOOST_CHECK_EQUAL(n, x.size());

    for(auto i : ural::indices_of(x))
    {
        BOOST_CHECK_EQUAL(x[i], t);
    }
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

BOOST_AUTO_TEST_CASE(vector_operator_less)
{
    typedef int T;
    typedef ural::vector<T> Vector;

    Vector const v1 = {1, 2, 3};
    Vector const v2 = {1, 2, 3, 4};
    Vector const v3 = {1, 4, 3, 4};

    BOOST_CHECK(v1 < v2);
    BOOST_CHECK(v2 < v3);
    BOOST_CHECK(v1 < v3);

    BOOST_CHECK(!(v1 < v1));
    BOOST_CHECK(!(v2 < v2));
    BOOST_CHECK(!(v3 < v3));
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
