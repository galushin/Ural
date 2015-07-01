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

#include <ural/flex_string.hpp>
#include <ural/container/flat_set.hpp>
#include <ural/container/vector.hpp>

#include <ural/utility/tracers.hpp>

#include <boost/mpl/list.hpp>
#include <boost/mpl/copy.hpp>
#include <boost/mpl/front_inserter.hpp>
#include <boost/test/unit_test.hpp>

// Общие требования к контейнерам (23.2)

namespace
{
    typedef ural::tracing_allocator<char> Char_alloc;
    typedef ural::tracing_allocator<int> Int_alloc;

    typedef boost::mpl::list<ural::vector<int, Int_alloc>,
                             ural::flex_string<char, ural::use_default, Char_alloc>
                            >
        Sequence_containers;

    typedef boost::mpl::list<ural::flat_set<int, ural::use_default, Int_alloc>>
        Assosiative_containers_containers;

    typedef boost::mpl::copy<Assosiative_containers_containers,
                             boost::mpl::front_inserter<Sequence_containers>>::type
        Containers_types;
}

// 23.2.1 Общие требования
// Таблица 96
BOOST_AUTO_TEST_CASE_TEMPLATE(container_types_test, Container, Containers_types)
{
    // Строка 1
    typedef ::ural::ValueType<Container> T;

    // @todo "T is Erasable from X" из таблицы 96

    // строка 2
    static_assert(std::is_same<typename Container::reference, T&>::value, "");

    // строка 3
    static_assert(std::is_same<typename Container::const_reference, T const&>::value, "");

    // итераторы
    typedef typename Container::iterator Iterator;
    typedef std::iterator_traits<Iterator> Iterator_traits;

    typedef typename Container::const_iterator Const_iterator;
    typedef std::iterator_traits<Const_iterator> Const_iterator_traits;

    // строка 4
    static_assert(std::is_convertible<typename Iterator_traits::iterator_category,
                                      std::forward_iterator_tag>::value, "");
    static_assert(std::is_same<::ural::ValueType<Iterator_traits>, T>::value, "");
    static_assert(std::is_convertible<Iterator, Const_iterator>::value, "");

    // строка 5
    static_assert(std::is_convertible<typename Const_iterator_traits::iterator_category,
                                      std::forward_iterator_tag>::value, "");
    static_assert(std::is_same<::ural::ValueType<Const_iterator_traits>, T>::value, "");

    // строка 6
    typedef typename Container::difference_type Difference;

    static_assert(std::is_signed<Difference>::value, "");
    static_assert(std::is_same<Difference, typename Iterator_traits::difference_type>::value, "");
    static_assert(std::is_same<Difference, typename Const_iterator_traits::difference_type>::value, "");

    // строка 7
    typedef typename Container::size_type Size;

    /* @todo Может быть отказаться от этого, раз Страуструп считает это ошибкой?
    Отказаться: более совершенная диагностика
    Сохранить: соответствие существующему стандарту
    Моё предложение: сохранить, но не использовать для индекса
    */
    static_assert(std::is_unsigned<Size>::value, "");

    static_assert(std::is_same<Size, typename std::make_unsigned<Difference>::type>::value, "");

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(container_construct_empty, Container, Containers_types)
{
    // Строка 8
    Container u;
    BOOST_CHECK(u.empty());

    // Строка 9
    BOOST_CHECK(Container().empty());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(container_copy_constructor, Container, Containers_types)
{
    Container const x{1, 2, 3, 4, 5};

    // Строка 10
    BOOST_CHECK(x == Container(x));

    // Строка 11
    Container u(x);

    BOOST_CHECK(u.begin() != x.begin());
    BOOST_CHECK(u.end() != x.end());

    BOOST_CHECK(u == x);

    BOOST_CHECK_EQUAL_COLLECTIONS(x.begin(), x.end(),
                                  u.begin(), u.end());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(container_move_constructor, Container, Containers_types)
{
    Container x{1, 2, 3, 4, 5};

    auto const x_old = x;
    auto const old_begin = x.cbegin();
    auto const old_end = x.cend();

    // Строка 11
    Container const x1(std::move(x));

    BOOST_CHECK_EQUAL(old_begin, x1.cbegin());
    BOOST_CHECK_EQUAL(old_end, x1.cend());

    BOOST_CHECK_EQUAL_COLLECTIONS(x_old.cbegin(), x_old.cend(),
                                  x1.begin(), x1.end());
    BOOST_CHECK(x.empty());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(container_move_assign,
                              Container, Containers_types)
{
    auto x = Container{1, 2, 3, 4, 5};
    auto const x_old = x;

    auto y = Container{1, 1, 2, 3, 5};
    auto const y_old = y;

    auto const y_begin_old = y.cbegin();
    auto const y_end_old = y.cend();

    // Строка 12
    y = std::move(x);

    typedef decltype(y = std::move(x)) Result_type;

    static_assert(std::is_same<Result_type, Container&>::value,
                  "must be reference to Container");

    BOOST_CHECK_EQUAL_COLLECTIONS(x.begin(), x.end(), y_old.begin(), y_old.end());
    BOOST_CHECK_EQUAL_COLLECTIONS(y.begin(), y.end(), x_old.begin(), x_old.end());

    BOOST_CHECK_EQUAL(x.begin(), y_begin_old);
    BOOST_CHECK_EQUAL(x.end(), y_end_old);
}

/* @todo присваивание с перемещением контейнеров в том случае, если
распределители памяти не равны. Здесь нужно два теста: один с меньшим размером,
один - с большим
*/

// @todo строка 13
BOOST_AUTO_TEST_CASE_TEMPLATE(container_destructor_test,
                              Container, Containers_types)
{
    typedef typename Container::allocator_type Alloc;
    Alloc::reset_traced_info();

    BOOST_CHECK_EQUAL(Alloc::allocations_count(), 0U);
    BOOST_CHECK_EQUAL(Alloc::deallocations_count(), 0U);

    {
        auto x = Container{1, 2, 3, 4, 5};

        BOOST_CHECK_GT(Alloc::allocations_count(), 0U);
        BOOST_CHECK_GT(Alloc::constructions_count(), 0U);
    }

    BOOST_CHECK_EQUAL(Alloc::allocations_count(), Alloc::deallocations_count());
    BOOST_CHECK_EQUAL(Alloc::constructions_count(), Alloc::destructions_count());
}

// Строки 14-17: итераторы
BOOST_AUTO_TEST_CASE_TEMPLATE(container_iterators_getters_types,
                              Container, Containers_types)
{
    typedef typename Container::iterator Iterator;
    typedef typename Container::const_iterator CIterator;

    Container v;
    Container const & cr = v;

    // строка 14
    static_assert(std::is_same<decltype(v.begin()), Iterator>::value, "");
    static_assert(std::is_same<decltype(cr.begin()), CIterator>::value, "");

    // строка 15
    static_assert(std::is_same<decltype(v.end()), Iterator>::value, "");
    static_assert(std::is_same<decltype(cr.end()), CIterator>::value, "");

    // строка 16
    static_assert(std::is_same<decltype(v.cbegin()), CIterator>::value, "");
    BOOST_CHECK(v.cbegin() == cr.begin());

    // строка 17
    static_assert(std::is_same<decltype(v.cend()), CIterator>::value, "");
    BOOST_CHECK(v.cend() == cr.end());

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(container_equality_compare, Container, Containers_types)
{
    Container const v1{1, 3, 5, 7};
    Container const v1_copy = v1;
    Container const v2{1, 3, 6, 8, 10};

    // Строка 18
    BOOST_CHECK_EQUAL(v1 == v1, true);
    BOOST_CHECK_EQUAL(v1_copy == v1_copy, true);
    BOOST_CHECK_EQUAL(v2 == v2, true);

    BOOST_CHECK_EQUAL(v1 == v1_copy, true);
    BOOST_CHECK_EQUAL(v1 == v2, false);
    BOOST_CHECK_EQUAL(v1_copy == v2, false);

    // Строка 19
    BOOST_CHECK_EQUAL(v1 != v1, false);
    BOOST_CHECK_EQUAL(v1_copy != v1_copy, false);
    BOOST_CHECK_EQUAL(v2 != v2, false);

    BOOST_CHECK_EQUAL(v1 != v1_copy, false);
    BOOST_CHECK_EQUAL(v1 != v2, true);
    BOOST_CHECK_EQUAL(v1_copy != v2, true);
}

// Строка 20
BOOST_AUTO_TEST_CASE_TEMPLATE(vector_member_swap_test, Container, Containers_types)
{
    typedef typename Container::allocator_type Alloc;

    Container v1({1, 3, 5, 7}, Alloc(1));
    Container v2({2, 4, 6, 8, 10}, Alloc(2));

    auto v1_begin_old = v1.begin();
    auto v1_end_old = v1.end();
    auto v2_begin_old = v2.begin();
    auto v2_end_old = v2.end();

    auto const v1_allocator_old = v1.get_allocator();
    auto const v2_allocator_old = v2.get_allocator();

    v1.swap(v2);

    BOOST_CHECK_EQUAL(v1.begin(), v2_begin_old);
    BOOST_CHECK_EQUAL(v2.begin(), v1_begin_old);
    BOOST_CHECK_EQUAL(v1.end(), v2_end_old);
    BOOST_CHECK_EQUAL(v2.end(), v1_end_old);

    BOOST_CHECK(v1.get_allocator() == v2_allocator_old);
    BOOST_CHECK(v2.get_allocator() == v1_allocator_old);
}

// Строка 21
BOOST_AUTO_TEST_CASE_TEMPLATE(container_non_member_swap_test,
                              Container, Containers_types)
{
    typedef typename Container::allocator_type Alloc;

    static_assert(std::allocator_traits<Alloc>::propagate_on_container_swap::value,
                  "Swap for containers with different allocators is UB");

    Container v1({1, 3, 5, 7}, Alloc(1));
    Container v2({2, 4, 6, 8, 10}, Alloc(2));

    auto v1_data_old = v1.data();
    auto v2_data_old = v2.data();
    auto const v1_allocator_old = v1.get_allocator();
    auto const v2_allocator_old = v2.get_allocator();

    using ural::swap;
    using std::swap;
    swap(v1, v2);

    BOOST_CHECK_EQUAL(v1.data(), v2_data_old);
    BOOST_CHECK_EQUAL(v2.data(), v1_data_old);
    BOOST_CHECK(v1.get_allocator() == v2_allocator_old);
    BOOST_CHECK(v2.get_allocator() == v1_allocator_old);
}

// Строка 22
BOOST_AUTO_TEST_CASE_TEMPLATE(container_copy_assign_bigger,
                              Container, Containers_types)
{
    auto const v1 = Container{1, 1, 2, 3, 5, 8, 13};
    auto v2 = Container{1, 2, 3, 4, 5};

    BOOST_CHECK(v1 != v2);
    BOOST_CHECK_GT(v1.size(), v2.size());

    v2 = v1;

    BOOST_CHECK(v1 == v2);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(container_copy_assign_lesser,
                              Container, Containers_types)
{
    auto const v1 = Container{1, 1, 2, 3, 5, 8, 13};
    auto v2 = Container{1, 2, 3, 4, 5, 5, 6, 7, 7, 8};

    BOOST_CHECK_LT(v1.size(), v2.size());

    BOOST_CHECK(v1 != v2);

    v2 = v1;

    BOOST_CHECK(v1 == v2);
}

// Строка 23
BOOST_AUTO_TEST_CASE_TEMPLATE(container_size_test, Container, Containers_types)
{
    auto const v0 = Container();

    BOOST_CHECK_EQUAL(v0.size(), 0U);

    auto const v1 = Container{1, 1, 2, 3, 5, 8, 13};

    BOOST_CHECK_EQUAL(v1.size(),
                      static_cast<size_t>(std::distance(v1.begin(), v1.end())));
}

// Строка 24
BOOST_AUTO_TEST_CASE_TEMPLATE(container_max_size_test,
                              Container, Containers_types)
{
    typedef typename Container::allocator_type Alloc;

    Container v;
    Alloc a;

    BOOST_CHECK_GE(std::allocator_traits<Alloc>::max_size(a), v.max_size());
}

// Строка 25
BOOST_AUTO_TEST_CASE_TEMPLATE(container_empty_test,
                              Container, Containers_types)
{
    BOOST_CHECK(Container().empty());

    auto const v1 = Container{1, 2, 3, 4, 5, 9};
    BOOST_CHECK(v1.empty() == false);
}

// Таблица 97
BOOST_AUTO_TEST_CASE_TEMPLATE(container_table_97, Container, Containers_types)
{
    typedef typename Container::iterator Iterator;
    typedef typename Container::const_iterator CIterator;

    // Строка 1
    typedef typename Container::reverse_iterator RIterator;
    static_assert(std::is_same<RIterator, std::reverse_iterator<Iterator>>::value, "");

    // Строка 2
    typedef typename Container::const_reverse_iterator CRIterator;
    static_assert(std::is_same<CRIterator, std::reverse_iterator<CIterator>>::value, "");

    // Строки 3-6
    Container x{1, 3, 5, 7, 9};

    BOOST_CHECK(x.rbegin() == RIterator(x.end()));
    BOOST_CHECK(x.rend() == RIterator(x.begin()));

    BOOST_CHECK(x.crbegin() == CRIterator(x.cend()));
    BOOST_CHECK(x.crend() == CRIterator(x.cbegin()));
}

// Таблица 98
BOOST_AUTO_TEST_CASE_TEMPLATE(container_table_98_less_comparable,
                              Container, Containers_types)
{
    Container const v1 = {1, 2, 3};
    Container const v2 = {1, 2, 3, 4};
    Container const v3 = {1, 4, 3, 4};

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
BOOST_AUTO_TEST_CASE_TEMPLATE(container_allocator_constructor,
                              Container, Containers_types)
{
    typedef typename Container::allocator_type Alloc;

    // Строка 1
    static_assert(std::is_same<::ural::ValueType<typename Container::allocator_type>,
                               ::ural::ValueType<Container>>::value,
                  "Allocator for wrong type!");

    // Строка 2
    static_assert(std::is_same<Alloc, decltype(Container().get_allocator())>::value, "");

    // Строка 3.1
    BOOST_CHECK(Container().empty());
    BOOST_CHECK(Container().get_allocator() == Alloc());

    // Строка 3.2
    Container u_0;

    BOOST_CHECK(u_0.empty());
    BOOST_CHECK(u_0.get_allocator() == Alloc());

    // Строка 4
    Alloc alloc(42);
    auto const u_1 = Container(alloc);

    BOOST_CHECK(u_1.empty());
    BOOST_CHECK(u_1.get_allocator() == alloc);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(container_copy_with_other_allocator,
                              Container, Containers_types)
{
    typedef typename Container::allocator_type Alloc;

    // Строка 5
    Container const t = {1, 2, 3, 4, 5};

    Alloc alloc(42);
    Container const u(t, alloc);

    BOOST_CHECK_EQUAL_COLLECTIONS(t.begin(), t.end(), u.begin(), u.end());
    BOOST_CHECK_EQUAL(alloc.id(), u.get_allocator().id());
    BOOST_CHECK(alloc == u.get_allocator());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(container_move_constructor_table_99,
                              Container, Containers_types)
{
    typedef typename Container::allocator_type Alloc;

    // Строка 6
    Container t = {1, 2, 3, 4, 5};

    Container const t_old = t;
    auto const t_old_data = t.data();

    Container const u(std::move(t));

    BOOST_CHECK_EQUAL_COLLECTIONS(t_old.begin(), t_old.end(), u.begin(), u.end());
    BOOST_CHECK_EQUAL(t_old_data, u.data());
    BOOST_CHECK_EQUAL(t_old.get_allocator().id(), u.get_allocator().id());
    BOOST_CHECK(t_old.get_allocator() == u.get_allocator());
}

BOOST_AUTO_TEST_CASE_TEMPLATE(container_move_with_same_allocator,
                              Container, Containers_types)
{
    // Строка 7
    Container t = {1, 2, 3, 4, 5};
    auto const t_old = t;
    typename Container::allocator_type alloc;

    auto const t_data_old = t.data();

    Container const u(std::move(t), alloc);

    BOOST_CHECK_EQUAL(t_data_old, u.data());

    BOOST_CHECK(t.empty());

    BOOST_CHECK_EQUAL_COLLECTIONS(t_old.begin(), t_old.end(),
                                  u.begin(), u.end());
}

// @todo Таблица 99 строки 6, 7, 8,9


// Строка 10: повторяет требование строки 21 таблицы 96

// 23.2.3 Последовательные контейнеры
BOOST_AUTO_TEST_CASE_TEMPLATE(container_assign_n_value_worse_then_iters_regression,
                              Container, Sequence_containers)
{
    static_assert(std::is_signed<::ural::ValueType<Container>>::value, "");

    Container x;
    x.assign(13, 42);

    Container z;
    z.assign(typename Container::size_type(13),
             ::ural::ValueType<Container>(42));

    BOOST_CHECK_EQUAL_COLLECTIONS(x.begin(), x.end(), x.begin(), x.end());
}
// 23.2.4 Ассоциативные контейнеры
// 23.2.5 Нуепорядоченные ассоциативные контейнеры

// Контейнеры с резервированием памяти
namespace
{
    typedef boost::mpl::list<ural::vector<int, Int_alloc>,
                             ural::flat_set<int, ural::use_default, Int_alloc>>
        Reserving_containers;
}

BOOST_AUTO_TEST_CASE_TEMPLATE(container_const_data_test, Container, Reserving_containers)
{
    Container const cs = {1, 3, 5, 7, 9};

    typedef ::ural::ValueType<Container> Value_type;
    typedef decltype(cs.data()) Data_result;

    static_assert(std::is_same<Value_type const *, Data_result>::value, "");

    BOOST_CHECK_EQUAL_COLLECTIONS(cs.begin(), cs.end(),
                                  cs.data(), cs.data() + cs.size());
}

// Качество реализации
BOOST_AUTO_TEST_CASE_TEMPLATE(shrink_to_fit_test, Container, Reserving_containers)
{
    Container cs = {1, 3, 5, 7, 9};
    auto const cs_old = cs;

    cs.reserve(cs.size() * 2);

    BOOST_CHECK_EQUAL_COLLECTIONS(cs.begin(), cs.end(),
                                  cs_old.begin(), cs_old.end());
    BOOST_CHECK_GT(cs.capacity(), cs.size());

    cs.shrink_to_fit();

    BOOST_CHECK_EQUAL_COLLECTIONS(cs.begin(), cs.end(),
                                  cs_old.begin(), cs_old.end());
    BOOST_CHECK_EQUAL(cs.capacity(), cs.size());
}

// @todo распространить это на все контейнеры, кроме flex_string
BOOST_AUTO_TEST_CASE(vector_allocator_constructor_regression)
{
    // Регрессия: при вызове конструктора без параметров память не распределяется

    typedef ural::tracing_allocator<int> Alloc;
    typedef ural::vector<int, Alloc> Container;

    // временный объект
    Alloc::reset_traced_info();

    BOOST_CHECK(Container().empty());
    BOOST_CHECK(Container().get_allocator() == Alloc());
    BOOST_CHECK_EQUAL(Alloc::allocations_count(), 0U);

    // переменная
    Alloc::reset_traced_info();

    Container u_0;

    BOOST_CHECK(u_0.empty());
    BOOST_CHECK(u_0.get_allocator() == Alloc());
    BOOST_CHECK_EQUAL(Alloc::allocations_count(), 0U);
}
