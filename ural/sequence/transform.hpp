#ifndef Z_URAL_TRANSFORM_HPP_INCLUDED
#define Z_URAL_TRANSFORM_HPP_INCLUDED

/** @file ural/sequence/transform.hpp
 @todo Параллельная реализация алгоритма
*/

#include <boost/compressed_pair.hpp>

#include <ural/sequence/base.hpp>

namespace ural
{
    /** @brief Реализация для произвольного количества входных
    последовательнсотей
    */
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
         : impl_(std::move(f), std::move(in))
        {}

        bool operator!() const
        {
            return !this->base();
        }

        transform_sequence & operator++()
        {
            ++ input_ref();
            return *this;
        }

        reference operator*()
        {
            return this->functor()(*this->input_ref());
        }

        Input const & base() const
        {
            return impl_.second();
        }

        F const & functor() const
        {
            return impl_.first();
        }

    private:
        Input & input_ref()
        {
            return impl_.second();
        }

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
