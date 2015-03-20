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
    // @todo Тест с итераторами ввода
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
