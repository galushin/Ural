#ifndef Z_URAL_ALGORITHM_COPY_HPP_INCLUDED
#define Z_URAL_ALGORITHM_COPY_HPP_INCLUDED

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

/** @file ural/algorithm/copy.hpp
 @brief Алгоритм копирования
*/

#include <ural/sequence/make.hpp>
#include <ural/concepts.hpp>
#include <ural/tuple.hpp>

#include <boost/concept/assert.hpp>

namespace ural
{
    class copy_fn
    {
    private:
        template <class Input, class Output>
        static ural::tuple<Input, Output>
        copy_impl(Input in, Output out)
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

    public:
        template <class Input, class Output>
        auto operator()(Input && in, Output && out) const
        -> decltype(::ural::copy_fn::copy_impl(sequence_fwd<Input>(in),
                                               sequence_fwd<Output>(out)))
        {
            return ::ural::copy_fn::copy_impl(sequence_fwd<Input>(in),
                                              sequence_fwd<Output>(out));
        }
    };
}
// namespace ural

#endif
// Z_URAL_ALGORITHM_DETAILS_COPY_HPP_INCLUDED
