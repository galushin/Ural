#ifndef Z_URAL_TESTS_DEFS_HPP_INCLUDED
#define Z_URAL_TESTS_DEFS_HPP_INCLUDED

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

#define URAL_STATIC_ASSERT_EQUAL(E, G) static_assert( ((E) == (G)) , "");

#define URAL_CHECK_EQUAL_RANGES(G, E) \
    BOOST_CHECK_EQUAL_COLLECTIONS((G).begin(), (G).end(), (E).begin(), (E).end())

#include <ural/algorithm/core.hpp>
#include <ural/sequence/iostream.hpp>

#include <string>

namespace ural_test
{
    template <class T>
    class istringstream_helper
    {
        typedef std::istringstream IStream;
        typedef ::ural::experimental::istream_cursor<IStream, T> Cursor;

        friend Cursor
        cursor(istringstream_helper const & x)
        {
            return Cursor(IStream(x.src_));
        }

    public:
        typedef T value_type;
        typedef value_type const & const_reference;

        istringstream_helper() = default;

        template <class Sequence>
        explicit istringstream_helper(Sequence && seq)
         : src_(istringstream_helper::make(ural::cursor_fwd<Sequence>(seq)))
        {}

        template <class Iterator>
        istringstream_helper(Iterator first, Iterator last)
         : istringstream_helper(ural::make_iterator_cursor(std::move(first), std::move(last)))
        {}

        istringstream_helper(std::initializer_list<T> values)
         : istringstream_helper(values.begin(), values.end())
        {}

    private:
        template <class Cursor>
        static std::string make(Cursor cur)
        {
            std::ostringstream os;
            ::ural::copy_fn{}(std::move(cur), ::ural::experimental::make_ostream_cursor(os, " "));
            return os.str();
        }

    private:
        std::string src_;
    };
}

#endif // Z_URAL_TESTS_DEFS_HPP_INCLUDED
