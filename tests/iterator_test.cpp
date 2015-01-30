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

#include <ural/iterator/move.hpp>

#include <iterator>

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(move_iterator_default_ctor)
{
    typedef std::istream_iterator<int> Iterator;

    ural::move_iterator<Iterator> i;

    static_assert(std::is_same<Iterator, typename decltype(i)::iterator_type>::value, "");

    BOOST_CHECK(i.base() == Iterator{});
}

BOOST_AUTO_TEST_CASE(move_iterator_copy_backward)
{
    std::vector<std::string> src1 = {"one", "two", "three", "four", "five"};
    auto src2 = src1;

    auto const n = 2;

    std::move_backward(src1.begin(), src1.end() - n, src1.end());

    auto first = ural::make_move_iterator(src2.begin());
    auto last = ural::make_move_iterator(src2.end()) - n;
    auto out = src2.end();

    std::copy_backward(first, last, out);

    BOOST_CHECK_EQUAL_COLLECTIONS(src1.begin(), src1.end(),
                                  src2.begin(), src2.end());
}

BOOST_AUTO_TEST_CASE(move_iterator_postfix_minus_minus)
{
    std::vector<int> xs = {1, 2, 3, 4};

    auto iter = xs.end();
    auto m_iter = ural::make_move_iterator(iter);

    auto iter_old = iter--;
    auto m_iter_old = m_iter--;

    BOOST_CHECK(iter == m_iter.base());
    BOOST_CHECK(iter_old == m_iter_old.base());
}

#include <forward_list>
#include <boost/mpl/list.hpp>
namespace
{
    typedef boost::mpl::list<std::forward_list<std::string>,
                             std::list<std::string>,
                             std::vector<std::string>> Containers;
}

BOOST_AUTO_TEST_CASE_TEMPLATE(move_iterator_advance, Container, Containers)
{
    Container xs = {"one", "two", "three", "four", "five"};

    auto const n = std::distance(xs.begin(), xs.end());

    auto mb = ural::make_move_iterator(xs.begin());
    auto me = ural::make_move_iterator(xs.end());

    auto const mn = std::distance(mb, me);

    BOOST_CHECK_EQUAL(mn, n);

    auto b = xs.begin();
    std::advance(b, n / 2);
    std::advance(mb, n / 2);

    BOOST_CHECK(b == mb.base());
}


BOOST_AUTO_TEST_CASE(move_iterator_operator_less)
{
    std::vector<std::string> xs = {"one", "two", "three", "four", "five"};

    auto mb = ural::make_move_iterator(xs.begin());

    BOOST_CHECK(mb + 3 > mb);
}

BOOST_AUTO_TEST_CASE(move_iterator_member_access)
{
    std::vector<std::string> xs = {"one", "two", "three", "four", "five"};

    auto const first = ural::make_move_iterator(xs.begin());
    auto const last = ural::make_move_iterator(xs.begin());

    for(auto i = first; i != last; ++ i)
    {
        BOOST_CHECK_EQUAL(xs[i - first].size(), i->size());
    }
}
