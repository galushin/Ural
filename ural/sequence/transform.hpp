#ifndef Z_URAL_TRANSFORM_HPP_INCLUDED
#define Z_URAL_TRANSFORM_HPP_INCLUDED

/** @file ural/sequence/transform.hpp
 @brief Последовательность с преобразованием
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

    /** @brief Последовательность с преобразованием
    @tparam F тип функционального объекта
    @tparam Input входная последовательность
    */
    template <class F, class Input>
    class transform_sequence<F, Input>
     : public sequence_base<transform_sequence<F, Input>>
    {
    public:
        /// @brief Тип ссылки
        typedef decltype(std::declval<F>()(*std::declval<Input>())) reference;

        /** @brief Конструктор
        @param f функциональный объект, задающий преобразование
        @param in входная последовательность
        @post <tt> this->base() == in </tt>
        @post <tt> this->functor() == f </tt>
        */
        explicit transform_sequence(F f, Input in)
         : impl_(std::move(f), std::move(in))
        {}

        // Однопроходная последовательность
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

        /** @brief Базовая последовательность
        @return Базовая последовательность
        */
        Input const & base() const
        {
            return impl_.second();
        }

        /** @brief Функциональный объект, задающий преобразование
        @return Функциональный объект, задающий преобразование
        */
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
