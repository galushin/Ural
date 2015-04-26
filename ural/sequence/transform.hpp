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

#include <ural/meta/list.hpp>
#include <ural/meta/hierarchy.hpp>

#include <ural/functional.hpp>
#include <ural/sequence/make.hpp>
#include <ural/sequence/base.hpp>

#include <boost/iterator/transform_iterator.hpp>
#include <boost/compressed_pair.hpp>

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

        /// @brief Категория обхода
        typedef typename common_tag<typename Inputs::traversal_tag...>::type
            traversal_tag;

        /// @brief Тип расстояния
        typedef typename std::common_type<typename Inputs::distance_type...>::type
            distance_type;

        /// @brief Тип указателя
        typedef typename std::conditional<std::is_lvalue_reference<reference>::value,
                                          typename std::remove_reference<reference>::type *,
                                          void> pointer;

        /** @brief Конструктор
        @param f функциональный объект, задающий преобразование
        @param in входная последовательность
        @post <tt> this->base() == in </tt>
        @post <tt> this->functor() == f </tt>
        */
        explicit transform_sequence(F f, Inputs... in)
         : impl_(std::move(f), Bases_tuple{std::move(in)...})
        {}

        /** @brief Доступ к кортежу базовых последовательностей
        @return Константная ссылка на кортеж базовых последовательностей
        */
        Bases_tuple const & bases() const
        {
            return impl_.second();
        }

        // Однопроходная последовательность
        /** @brief Проверка исчерпания последовательностей
        @return @b true, если последовательность исчерпана, иначе --- @b false.
        */
        bool operator!() const
        {
            return ural::tuples::any_of(impl_.second(), ural::logical_not<>{});
        }

        /** @brief Переход к следующему элементу в передней части
        @pre <tt> !*this == true </tt>
        */
        void pop_front()
        {
            ural::tuples::for_each(impl_.second(), ural::pop_front);
        }

        /** @brief Передний элемент
        @return Ссылка на первый элемент последовательности
        @pre <tt> !*this == true </tt>
        */
        reference front() const
        {
            auto f = [this](Inputs const & ... args)->reference
                     { return this->deref(args...); };

            return apply(f, impl_.second());
        }

        // Прямая последовательность

        // Двусторонняя последовательность

        // Последовательность произвольного доступа
        distance_type size() const
        {
            // @todo Обобщить, реализовать без псевдо-рекурсии
            return this->size_impl(impl_.second()[ural::_1].size(), ural::_2);
        }

        // Адаптор последовательности
        /** @brief Функциональный объект, задающий преобразование
        @return Функциональный объект, задающий преобразование
        */
        F const & functor() const
        {
            return impl_.first();
        }

    private:
        distance_type
        size_impl(distance_type result, placeholder<sizeof...(Inputs)>) const
        {
            return result;
        }

        template <size_t index>
        distance_type
        size_impl(distance_type current, placeholder<index>) const
        {
            return this->size_impl(std::min(current, impl_.second()[ural::_1].size()),
                                   placeholder<index+1>());
        }

        reference deref(Inputs const & ... ins) const
        {
            return this->functor()((*ins)...);
        }

    private:
        boost::compressed_pair<F, Bases_tuple> impl_;
    };

    /** @brief Итератор, задающий начало преобразующей последовательности
    @param s последовательность
    @return <tt> {begin(s.bases()[ural::_1]), s.functor()} </tt>
    */
    template <class UnaryFunction, class Sequence>
    auto begin(transform_sequence<UnaryFunction, Sequence> const & s)
    -> boost::transform_iterator<UnaryFunction, decltype(begin(s.bases()[ural::_1]))>
    {
        return {begin(s.bases()[ural::_1]), s.functor()};
    }

    /** @brief Итератор, задающий конец преобразующей последовательности
    @param s последовательность
    @return <tt> {end(s.bases()[ural::_1]), s.functor()} </tt>
    */
    template <class UnaryFunction, class Sequence>
    auto end(transform_sequence<UnaryFunction, Sequence> const & s)
    -> boost::transform_iterator<UnaryFunction, decltype(begin(s.bases()[ural::_1]))>
    {
        return {end(s.bases()[ural::_1]), s.functor()};
    }

    /** @brief Функция создания @c make_transform_sequence
    @param f функциональный объект, способный принимать <tt> sizeof...(in) </tt>
    аргументов
    @param in список входных последовательностей
    */
    template <class Function, class... Inputs>
    auto make_transform_sequence(Function f, Inputs && ... in)
    -> transform_sequence<decltype(ural::make_callable(std::move(f))),
                          decltype(::ural::sequence_fwd<Inputs>(in))...>
    {
        typedef transform_sequence<decltype(ural::make_callable(std::move(f))),
                          decltype(::ural::sequence_fwd<Inputs>(in))...> Result;
        return Result(ural::make_callable(std::move(f)),
                      ::ural::sequence_fwd<Inputs>(in)...);
    }

    /** @brief Вспомогательный объект для создания @c transform_sequence
    конвейерным синтаксисом
    @tparam Function тип функционального объекта
    */
    template <class Function>
    struct transformed_helper
    {
        /// @brief Функциональный объект
        Function f;
    };

    /** @brief Cоздания @c make_transform_sequence конвейерным синтаксисом
    @param in входная последовательность
    @param helper объект, хранящий функциональный объект
    @return <tt> make_transform_sequence(helper.f, std::forward<Sequence>(in)) </tt>
    */
    template <class Sequence, class Function>
    auto operator|(Sequence && in, transformed_helper<Function> helper)
    -> decltype(make_transform_sequence(helper.f, std::forward<Sequence>(in)))
    {
        return make_transform_sequence(helper.f, std::forward<Sequence>(in));
    }

    /** @brief Создание элемента "конвейера", создающего @c transformed_sequence
    @param f функциональный объект
    @return <tt> {ural::make_callable(std::move(f))} </tt>
    */
    template <class Function>
    auto transformed(Function f)
    -> transformed_helper<decltype(ural::make_callable(std::move(f)))>
    {
        return {ural::make_callable(std::move(f))};
    }
}
// namespace ural

#endif
// Z_URAL_TRANSFORM_HPP_INCLUDED
