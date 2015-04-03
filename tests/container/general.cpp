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

#include <ural/container/flat_set.hpp>
#include <ural/container/vector.hpp>

#include <boost/mpl/list.hpp>
#include <boost/test/unit_test.hpp>

// Общие требования к контейнерам (23.2)

namespace
{
    typedef boost::mpl::list<ural::vector<int>, ural::flat_set<int>> Containers_types;
}

// 23.2.1 Общие требования
BOOST_AUTO_TEST_CASE_TEMPLATE(container_types_test, Container, Containers_types)
{
    // Строка 1
    typedef typename Container::value_type T;

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
    static_assert(std::is_same<typename Iterator_traits::value_type, T>::value, "");
    static_assert(std::is_convertible<Iterator, Const_iterator>::value, "");

    // строка 5
    static_assert(std::is_convertible<typename Const_iterator_traits::iterator_category,
                                      std::forward_iterator_tag>::value, "");
    static_assert(std::is_same<typename Const_iterator_traits::value_type, T>::value, "");

    // строка 6
    typedef typename Container::difference_type Difference;

    static_assert(std::is_signed<Difference>::value, "");
    static_assert(std::is_same<Difference, typename Iterator_traits::difference_type>::value, "");
    static_assert(std::is_same<Difference, typename Const_iterator_traits::difference_type>::value, "");

    // строка 7
    typedef typename Container::size_type Size;

    // @todo Может быть отказаться от этого, раз Страуструп считает это ошибкой?
    static_assert(std::is_unsigned<Size>::value, "");

    static_assert(std::is_same<Size, typename std::make_unsigned<Difference>::type>::value, "");

    BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(vector_construct_empty, Container, Containers_types)
{
    // Строка 8
    Container u;
    BOOST_CHECK(u.empty());

    // Строка 9
    BOOST_CHECK(Container().empty());
}

// 23.2.3 Последовательные контейнеры
// 23.2.4 Ассоциативные контейнеры
// 23.2.5 Нуепорядоченные ассоциативные контейнеры
