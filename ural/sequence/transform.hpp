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
    @tparam F тип функционального объекта
    @tparam Inputs входные последовательности
    */
    template <class F, class... Inputs>
    class transform_sequence
     : public sequence_base<transform_sequence<F, Inputs...>>
    {
    private:
        template <class T>
        static T make_value(T);

        typedef tuple<Inputs...> Bases_tuple;

    public:
        /// @brief Тип ссылки
        typedef decltype(std::declval<F>()(*std::declval<Inputs>()...)) reference;

        /// @brief Тип значения
        typedef decltype(make_value(std::declval<reference>())) value_type;

        typedef typename common_tag<typename Inputs::traversal_tag...>::type
            traversal_tag;

        /** @brief Конструктор
        @param f функциональный объект, задающий преобразование
        @param in входная последовательность
        @post <tt> this->base() == in </tt>
        @post <tt> this->functor() == f </tt>
        */
        explicit transform_sequence(F f, Inputs... in)
         : impl_(std::move(f), Bases_tuple{std::move(in)...})
        {}

        // Однопроходная последовательность
        bool operator!() const
        {
            return this->is_empty(ural::_1, ural::placeholder<sizeof...(Inputs)>{});
        }

        void pop_front()
        {
            return this->pop_fronts(ural::_1, ural::placeholder<sizeof...(Inputs)>{});
        }

        reference front() const
        {
            auto f = [this](Inputs const & ... args)->reference
                     { return this->deref(args...); };

            return apply(f, impl_.second());
        }

        /** @brief Функциональный объект, задающий преобразование
        @return Функциональный объект, задающий преобразование
        */
        F const & functor() const
        {
            return impl_.first();
        }

    private:
        reference deref(Inputs const & ... ins) const
        {
            return this->functor()((*ins)...);
        }

        void pop_fronts(placeholder<sizeof...(Inputs)>, placeholder<sizeof...(Inputs)>)
        {}

        template <size_t I>
        void pop_fronts(placeholder<I> first, placeholder<sizeof...(Inputs)> last)
        {
            impl_.second()[first].pop_front();
            return pop_fronts(placeholder<I+1>{}, last);
        }

        bool is_empty(placeholder<sizeof...(Inputs)>,
                      placeholder<sizeof...(Inputs)>) const
        {
            return false;
        }

        template <size_t I>
        bool is_empty(placeholder<I> first,
                      placeholder<sizeof...(Inputs)> last) const
        {
            return !impl_.second()[first] || is_empty(placeholder<I+1>{}, last);
        }

    private:
        boost::compressed_pair<F, Bases_tuple> impl_;
    };

    template <class UnaryFunction, class... Inputs>
    auto make_transform_sequence(UnaryFunction f, Inputs && ... in)
    -> transform_sequence<decltype(ural::make_functor(std::move(f))),
                          decltype(ural::sequence(std::forward<Inputs>(in)))...>
    {
        typedef transform_sequence<decltype(ural::make_functor(std::move(f))),
                          decltype(ural::sequence(std::forward<Inputs>(in)))...> Result;
        return Result(ural::make_functor(std::move(f)),
                      ural::sequence(std::forward<Inputs>(in))...);
    }
}
// namespace ural

#endif
// Z_URAL_TRANSFORM_HPP_INCLUDED
