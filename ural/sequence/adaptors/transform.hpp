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

#include <ural/sequence/adaptor.hpp>
#include <ural/sequence/adaptors/zip.hpp>

#include <ural/meta/list.hpp>
#include <ural/meta/hierarchy.hpp>

#include <ural/functional.hpp>
#include <ural/sequence/make.hpp>
#include <ural/sequence/base.hpp>

#include <boost/iterator/transform_iterator.hpp>

namespace ural
{
    /** @brief Реализация для произвольного количества входных
    последовательнсотей
    @tparam F тип функционального объекта
    @tparam Inputs входные последовательности
    */
    template <class F, class... Inputs>
    class transform_sequence
     : public sequence_adaptor<transform_sequence<F, Inputs...>,
                               zip_sequence<Inputs...>, F>
    {
        using Base = sequence_adaptor<transform_sequence<F, Inputs...>,
                                      zip_sequence<Inputs...>, F>;

    public:
        // Типы
        /// @brief Тип ссылки
        typedef decltype(std::declval<F>()(*std::declval<Inputs>()...)) reference;

        /// @brief Тип значения
        using value_type = typename std::decay<reference>::type;

        /// @brief Тип расстояния
        using typename Base::distance_type;

        /// @brief Тип указателя
        typedef typename std::conditional<std::is_lvalue_reference<reference>::value,
                                          typename std::remove_reference<reference>::type *,
                                          void> pointer;

        /// @brief Кортеж входных последовательностей
        using bases_tuple = tuple<Inputs...>;

        // Конструктор
        /** @brief Конструктор
        @param f функциональный объект, задающий преобразование
        @param in входная последовательность
        @post <tt> this->base() == in </tt>
        @post <tt> this->function() == f </tt>
        */
        explicit transform_sequence(F f, Inputs... in)
         : Base(make_zip_sequence(std::move(in)...), std::move(f))
        {}

        //@{
        /** @brief Доступ к кортежу базовых последовательностей
        @return Константная ссылка на кортеж базовых последовательностей
        */
        bases_tuple const & bases() const &
        {
            return this->base().bases();
        }

        bases_tuple && bases() &&
        {
            return std::move(*this).base().bases();
        }
        //@}

        //@{
        /** @brief Функциональный объект, задающий преобразование
        @return Функциональный объект, задающий преобразование
        */
        F const & function() const &
        {
            return Base::payload();
        }

        F && function() &&
        {
            return std::move(Base::payload());
        }
        //@}

        // Однопроходная последовательность
        /** @brief Передний элемент
        @return Ссылка на первый элемент последовательности
        @pre <tt> !*this == true </tt>
        */
        reference front() const
        {
            return ::ural::apply(this->function(), Base::front());
        }

        // Прямая последовательность

        // Двусторонняя последовательность
        /** @brief Последний элемент
        @return Ссылка на последний элемент последовательности
        @pre <tt> !*this == true </tt>
        */
        reference back() const
        {
            return ::ural::apply(this->function(), Base::back());
        }

        // Последовательность произвольного доступа
        /** @brief Индексированный доступ
        @param n индекс
        @pre <tt> 0 <= n && n < this->size() </tt>
        */
        reference operator[](distance_type n) const
        {
            return ::ural::apply(this->function(),
                                 Base::operator[](std::move(n)));
        }

    private:
        friend Base;

        template <class... OtherInputs>
        transform_sequence<F, OtherInputs...>
        rebind_base(zip_sequence<OtherInputs...> new_base) const
        {
            using Result = transform_sequence<F, OtherInputs...>;
            auto maker = [this](OtherInputs... inputs) -> Result
            {
                return Result(this->function(), std::move(inputs)...);
            };

            return ural::apply(maker, std::move(new_base).bases());
        }

        explicit transform_sequence(zip_sequence<Inputs...> base, F f)
         : Base(std::move(base), std::move(f))
        {}
    };

    /** @brief Итератор, задающий начало преобразующей последовательности
    @param s последовательность
    @return <tt> {begin(s.bases()[ural::_1]), s.function()} </tt>
    */
    template <class UnaryFunction, class Sequence>
    auto begin(transform_sequence<UnaryFunction, Sequence> const & s)
    -> boost::transform_iterator<UnaryFunction, decltype(begin(s.bases()[ural::_1]))>
    {
        return {begin(s.bases()[ural::_1]), s.function()};
    }

    /** @brief Итератор, задающий конец преобразующей последовательности
    @param s последовательность
    @return <tt> {end(s.bases()[ural::_1]), s.function()} </tt>
    */
    template <class UnaryFunction, class Sequence>
    auto end(transform_sequence<UnaryFunction, Sequence> const & s)
    -> boost::transform_iterator<UnaryFunction, decltype(begin(s.bases()[ural::_1]))>
    {
        return {end(s.bases()[ural::_1]), s.function()};
    }

    /// @brief Тип Функционального объекта для создания @c transform_sequence
    struct make_transform_sequence_fn
    {
    public:
        /** @brief Функция создания @c make_transform_sequence
        @param f функциональный объект, способный принимать <tt> sizeof...(in) </tt>
        аргументов
        @param in список входных последовательностей
        @return <tt> { ::ural::make_callable(std::move(f)), ::ural::sequence_fwd<Inputs>(in)...</tt> }
        */
        template <class Function, class... Inputs>
        transform_sequence<FunctionType<Function>, SequenceType<Inputs>...>
        operator()(Function f, Inputs && ... in) const
        {
            using Seq = transform_sequence<FunctionType<Function>,
                                           SequenceType<Inputs>...>;

            return Seq(::ural::make_callable(std::move(f)),
                       ::ural::sequence_fwd<Inputs>(in)...);
        }
    };

    namespace
    {
        /** @brief Функциональный объект для создания @c transform_sequence
        в функциональном стиле.
        */
        constexpr auto const & make_transform_sequence
            = odr_const<make_transform_sequence_fn>;

        /** @brief Функциональный объект для создания @c transform_sequence
        в конвейерном стиле.
        */
        constexpr auto const & transformed
            = odr_const<pipeable_maker<binary_reverse_args_function<make_transform_sequence_fn>>>;
    }
}
// namespace ural

#endif
// Z_URAL_TRANSFORM_HPP_INCLUDED
