#ifndef Z_URAL_TRANSFORM_HPP_INCLUDED
#define Z_URAL_TRANSFORM_HPP_INCLUDED

#include <boost/compressed_pair.hpp>

#include <ural/sequence/base.hpp>

namespace ural
{
    template <class F, class... Inputs>
    class transform_sequence;

    /** @todo Оптимизация размера
    */
    template <class F, class Input>
    class transform_sequence<F, Input>
     : public sequence_base<transform_sequence<F, Input>>
    {
    public:
        typedef decltype(std::declval<F>()(*std::declval<Input>())) reference;

        explicit transform_sequence(F f, Input in)
         : impl(std::move(f), std::move(in))
        {}

        bool operator!() const;

        transform_sequence & operator++();

        reference operator*();

    private:

    private:
        boost::compressed_pair<F, Input> impl_;
    };

    template <class Input, class UnaryFunction>
    auto transform(Input && in, UnaryFunction f)
    -> transform_sequence<decltype(ural::make_functor(std::move(f))),
                          decltype(ural::sequence(std::forward<Input>(in)))>
    {
        typedef transform_sequence<decltype(ural::make_functor(std::move(f))),
                          decltype(ural::sequence(std::forward<Input>(in)))> Result;
        return Result(ural::make_functor(std::move(f)),
                      ural::sequence(std::forward<Input>(in)));
    }
}
// namespace ural

#endif
// Z_URAL_TRANSFORM_HPP_INCLUDED
