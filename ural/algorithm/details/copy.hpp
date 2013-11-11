#ifndef Z_URAL_ALGORITHM_DETAILS_COPY_HPP_INCLUDED
#define Z_URAL_ALGORITHM_DETAILS_COPY_HPP_INCLUDED

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
