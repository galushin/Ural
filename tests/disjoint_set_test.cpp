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

#include "rnd.hpp"

#include <ural/disjoint_set.hpp>
#include <ural/numeric/numbers_sequence.hpp>

BOOST_AUTO_TEST_CASE(disjoint_set_singletons)
{
    auto const n = 17U;

    ural::disjoint_set<size_t> ds(n);

    BOOST_CHECK_EQUAL(ds.size(), n);

    for(auto i : ural::indices_of(ds))
    {
        BOOST_CHECK_EQUAL(i, ds.root(i));
        BOOST_CHECK_EQUAL(i, ds.parent(i));
        BOOST_CHECK(ds.is_united(i, i));
    }
}

BOOST_AUTO_TEST_CASE(disjoint_set_odd_and_even)
{
    auto const n = 31;

    ural::disjoint_set<size_t> ds(n);

    for(auto i : ural::numbers(2, ds.size()))
    {
        ds.unite(i, i-2);

        BOOST_CHECK(ds.is_united(i, i-2));
    }

    for(auto i : ural::indices_of(ds))
    {
        for(auto j : ural::indices_of(ds))
        {
            BOOST_CHECK_EQUAL(ds.is_united(i, j), i % 2 == j % 2);
            BOOST_CHECK_EQUAL(ds.is_united(j, i), i % 2 == j % 2);
        }
    }
}
// @todo Тест на основе случайной выборки

namespace
{
    size_t height(ural::disjoint_set<size_t> const & ds, size_t index)
    {
        size_t h = 0;

        for(;;)
        {
            auto p = ds.parent(index);

            if(p != index)
            {
                h += 1;
                index = p;
            }
            else
            {
                break;
            }
        }

        return h;
    }
}

BOOST_AUTO_TEST_CASE(disjoint_set_left_to_right)
{
    auto const n = 16;

    ural::disjoint_set<size_t> ds(n);

    for(auto i : ural::indices_of(ds))
    {
        ds.unite(0, i);

        BOOST_CHECK(ds.is_united(0, i));
    }

    for(auto i : ural::indices_of(ds))
    {
        BOOST_CHECK_LE(height(ds, i), 1U);
    }
}

BOOST_AUTO_TEST_CASE(disjoint_set_right_to_left)
{
    auto const n = 16;

    ural::disjoint_set<size_t> ds(n);

    for(size_t i = ds.size(); i > 0; -- i)
    {
        ds.unite(n-1, i-1);

        BOOST_CHECK(ds.is_united(n-1, i-1));
    }

    for(auto i : ural::indices_of(ds))
    {
        BOOST_CHECK_LE(height(ds, i), 1);
    }
}
