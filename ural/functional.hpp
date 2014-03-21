#ifndef Z_URAL_FUNCTIONAL_HPP_INCLUDED
#define Z_URAL_FUNCTIONAL_HPP_INCLUDED

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

/** @file ural/functional.hpp
 @brief Функциональные объекты и средства для работы с ними
*/

#include <boost/compressed_pair.hpp>

#include <ural/functional/adjoin.hpp>
#include <ural/functional/compare_by.hpp>
#include <ural/functional/cpp_operators.hpp>
#include <ural/functional/make_functor.hpp>
#include <ural/functional/replace.hpp>

#include <ural/tuple.hpp>

namespace ural
{
    /** @brief Функциональный объект без аргументов, возвращающий фиксированное
    знчение
    @tparam T тип значения

    Если T --- регулярный тип, то <tt> value_functor<T> </tt> --- тоже,
    регулярный.

    Пусть @c f --- это объект типа, <tt> value_functor<T> </tt>, а @c x ---
    типа @c T.
    Выражение <tt> f = x </tt> выглядит не очень логичным. Если нужно изменить
    значение @c f, то можно воспользоваться кодом вида
    <tt> f = value_functor<T>{x} </tt>.
    */
    template <class T>
    class value_functor
    {
    public:
        /// @brief Тип возвращаемого значения
        typedef T const & result_type;

        /** @brief Конструктор
        @param value значение
        @post <tt> (*this)() == value </tt>
        */
        constexpr explicit value_functor(T value)
         : value_(std::move(value))
        {}

        /** @brief Оператор вычисления значения
        @return Значение, установленное в конструкторе или в результате
        присваивания
        */
        constexpr result_type operator()() const
        {
            return this->value_;
        }

    private:
        T value_;
    };

    template <class T1, class T2>
    constexpr bool
    operator==(value_functor<T1> const & x, value_functor<T2> const & y)
    {
        return x() == y();
    }

    template <class ForwardSequence, class Compare>
    class min_element_accumulator
    {
    public:
        explicit min_element_accumulator(ForwardSequence s, Compare cmp)
         : impl_{std::move(s), std::move(cmp)}
        {}

        min_element_accumulator &
        operator()(ForwardSequence s)
        {
            if(this->compare()(s, this->result()))
            {
                impl_.first() = s;
            }

            return *this;
        }

        ForwardSequence const & result() const
        {
            return impl_.first();
        }

        Compare const & compare() const
        {
            return impl_.second();
        }

    private:
        boost::compressed_pair<ForwardSequence, Compare> impl_;
    };

    /** @brief Адаптор функционального объекта с двумя аргументами, меняющий
    порядок аргументов.
    @tparam BinaryFunctor Тип функционального объекта с двумя аргументами
    @todo constexpr
    */
    template <class BinaryFunctor>
    class binary_reverse_args_functor
     : private BinaryFunctor
    {
        typedef BinaryFunctor Base;

    public:
        /** @brief Конструктор
        @param f функциональный объект
        @post <tt> this->functor() == f </tt>
        */
        explicit binary_reverse_args_functor(BinaryFunctor f)
         : Base(std::move(f))
        {}

        /** @brief Вычисление значения
        @param x первый аргумент
        @param y второй аргумент
        @return <tt> this->functor()(std::forward<T2>(y), std::forward<T1>(x)) </tt>
        */
        template <class T1, class T2>
        auto operator()(T1 && x, T2 && y) const
        -> decltype(std::declval<BinaryFunctor>()(std::forward<T2>(y), std::forward<T1>(x)))
        {
            return this->functor()(std::forward<T2>(y), std::forward<T1>(x));
        }

        /** @brief Адаптируемый функциональный объект
        @return Адаптируемый функциональный объект
        */
        BinaryFunctor const & functor() const
        {
            return static_cast<BinaryFunctor const &>(*this);
        }
    };

    /** @brief Создание адаптора функционального объекта с двумя аргументами,
    меняющего порядок аргументов.
    @param f функциональный объект с двумя аргументами
    */
    template <class BinaryFunctor>
    auto make_binary_reverse_args(BinaryFunctor f)
    -> binary_reverse_args_functor<decltype(make_functor(std::move(f)))>
    {
        typedef binary_reverse_args_functor<decltype(make_functor(std::move(f)))>
            Functor;
        return Functor{make_functor(std::move(f))};
    }
}
// namespace ural

#endif
// Z_URAL_FUNCTIONAL_HPP_INCLUDED
