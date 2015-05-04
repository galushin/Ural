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

#include <ural/algorithm.hpp>
#include <ural/sequence/iostream.hpp>

#include <string>

namespace ural_test
{
    template <class T>
    class istringstream_helper
    {
        friend
        ::ural::istream_sequence<::ural::use_default, T>
        sequence(istringstream_helper & x)
        {
            return ::ural::istream_sequence<::ural::use_default, T>(x.is_);
        }

    public:
        typedef T value_type;
        typedef value_type const & const_reference;

        istringstream_helper() = default;

        istringstream_helper(std::initializer_list<T> values)
         : is_(istringstream_helper::make(values.begin(), values.end()))
        {}

    private:
        template <class Iterator>
        static std::string make(Iterator first, Iterator last)
        {
            std::ostringstream os;
            ::ural::copy(::ural::make_iterator_sequence(std::move(first), std::move(last)),
                         ::ural::make_ostream_sequence(os, " "));
            return os.str();
        }

    private:
        std::istringstream is_;
    };
}

#endif // Z_URAL_TESTS_DEFS_HPP_INCLUDED
