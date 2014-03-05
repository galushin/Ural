#ifndef Z_URAL_TRANSFORM_HPP_INCLUDED
#define Z_URAL_TRANSFORM_HPP_INCLUDED

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

/** @file ural/sequence/transform.hpp
 @brief Последовательность с преобразованием
*/

#include <boost/compressed_pair.hpp>

#include <ural/functional.hpp>
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
    private:
        template <class T>
        static T make_value(T);

    public:
        /// @brief Тип ссылки
        typedef decltype(std::declval<F>()(*std::declval<Input>())) reference;

        /// @brief Тип значения
        typedef decltype(make_value(std::declval<reference>())) value_type;

        typedef typename Input::traversal_tag traversal_tag;

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

        void pop_front()
        {
            ++ input_ref();
        }

        reference front() const
        {
            return this->functor()(*this->base());
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
