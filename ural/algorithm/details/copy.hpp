#ifndef Z_URAL_ALGORITHM_DETAILS_COPY_HPP_INCLUDED
#define Z_URAL_ALGORITHM_DETAILS_COPY_HPP_INCLUDED

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

#include <boost/concept/assert.hpp>

#include <ural/concepts.hpp>
#include <ural/tuple.hpp>

namespace ural
{
namespace details
{
    template <class Input, class Output>
    ural::tuple<Input, Output>
    copy(Input in, Output out)
    {
        BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassSequence<Input>));
        BOOST_CONCEPT_ASSERT((ural::concepts::SinglePassSequence<Output>));
        BOOST_CONCEPT_ASSERT((ural::concepts::ReadableSequence<Input>));
        BOOST_CONCEPT_ASSERT((ural::concepts::WritableSequence<Output, typename Input::reference>));

        for(; !!in && !!out; ++ in, ++ out)
        {
            *out = *in;
        }

        return ural::tuple<Input, Output>{in, out};
    }
}
// namespace details
}
// namespace ural

#endif
// Z_URAL_ALGORITHM_DETAILS_COPY_HPP_INCLUDED
