#ifndef Z_URAL_FUNCTIONAL_REPLACE_HPP_INCLUDED
#define Z_URAL_FUNCTIONAL_REPLACE_HPP_INCLUDED

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

/** @file ural/functional/replace.hpp
 @brief Функциональные объекты, заменяющие значения, удовлетворяющие
 определённым условиям.
*/

#include <ural/tuple.hpp>

#include <ural/functional/cpp_operators.hpp>

namespace ural
{
    /** @brief Функциональный объект, заменяющий значения, удовлетворяющие
    унарному предикату.
    @tparam Predicate тип предиката
    @tparam T тип значения
    */
    template <class Predicate, class T>
    class replace_if_functor
    {
    public:
        /// @brief Тип возвращаемого значения
        typedef T const & result_type;

        /** @brief Конструктор
        @param pred предикат
        @tparam new_value новое значение
        @post <tt> this->predicate() == pred </tt>
        @post <tt> this->new_value() == new_value </tt>
        */
        replace_if_functor(Predicate pred, T new_value)
         : members_{std::move(pred), new_value}
        {}

        /** @brief Оператор вычисления значения
        @param x значение
        @return Если @c x не удовлетворяет предикату, то
        <tt> this->new_value() </t>> иначе --- @c x.
        */
        result_type operator()(T const & x) const
        {
            if(this->predicate()(x))
            {
                return this->new_value();
            }
            else
            {
                return x;
            }
        }

        /** @brief Используемый предикат
        @return Используемый предикат
        */
        Predicate const & predicate() const
        {
            return members_.first();
        }

        /** @brief Значение, которое будет возвращено, если аргумент
        удовлетворяет условию
        @return Значение, которое будет возвращено, если аргумент
        удовлетворяет условию
        */
        result_type new_value() const
        {
            return members_.second();
        }

    private:
        boost::compressed_pair<Predicate, T> members_;
    };

    /**
    По умолчанию сохраняются значения, а не ссылки. Чтобы избежать копирования,
    следует обернуть объекты в вызовы std::cref()
    */
    template <class Predicate, class T>
    replace_if_functor<Predicate, T>
    make_replace_if_functor(Predicate pred, T const & new_value)
    {
        return replace_if_functor<Predicate, T>(std::move(pred), new_value);
    }

    template <class T, class BinaryPredicate = ural::equal_to<T> >
    class replace_functor
    {
    public:
        typedef decltype(make_functor(std::declval<BinaryPredicate>())) predicate_type;
        typedef T const & result_type;

        constexpr explicit replace_functor(T old_value, T new_value)
         : members_{std::move(old_value), std::move(new_value),
                    predicate_type{}}
        {}

        constexpr explicit replace_functor(T old_value, T new_value,
                                           BinaryPredicate pred)
         : members_{std::move(old_value), std::move(new_value),
                    make_functor(std::move(pred))}
        {}

        constexpr T const & old_value() const
        {
            return members_[ural::_1];
        }

        constexpr T const & new_value() const
        {
            return members_[ural::_2];
        }

        constexpr predicate_type const & predicate() const
        {
            return members_[ural::_3];
        }

        constexpr result_type operator()(T const & x) const
        {
            return (this->predicate()(x, this->old_value()))
                   ? this->new_value() : x;
        }

    private:
        // @todo Оптимизация размера
        // @todo Выразить через replace_if_functor
        ural::tuple<T, T, predicate_type> members_;
    };

    /** @brief Функция создания @c replace_functor с нестандартным предикатом,
    задающим равенство.

    Создаёт функциональный объект, который заменяет значения @c x,
    удовлетворяющие условию <tt> pred(x, old_value) </tt> на @c new_value,
    а остальные оставляет без изменений.

    @param old_value значение, подлежащее замене
    @param new_value значение, на которое заменяются значения, удовлетворяющие
    условию.
    @param pred предикат
    По умолчанию сохраняются значения, а не ссылки. Чтобы избежать копирования,
    следует обернуть объекты в вызовы std::cref()
    */
    template <class T, class BinaryPredicate>
    constexpr replace_functor<T, BinaryPredicate>
    make_replace_functor(T old_value, T new_value, BinaryPredicate pred)
    {
        return replace_functor<T, BinaryPredicate>{std::move(old_value),
                                                   std::move(new_value),
                                                   std::move(pred)};
    }

    /**
    По умолчанию сохраняются значения, а не ссылки. Чтобы избежать копирования,
    следует обернуть объекты в вызовы std::cref()
    */
    template <class T>
    constexpr replace_functor<T, ural::equal_to<T,T>>
    make_replace_functor(T old_value, T new_value)
    {
        return replace_functor<T, ural::equal_to<T,T>>(std::move(old_value),
                                                       std::move(new_value));
    }
}
// namespace ural

#endif
// Z_URAL_FUNCTIONAL_REPLACE_HPP_INCLUDED
