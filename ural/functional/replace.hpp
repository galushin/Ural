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
#include <ural/type_traits.hpp>

#include <ural/functional/cpp_operators.hpp>

namespace ural
{
    /** @brief Функциональный объект, заменяющий значения, удовлетворяющие
    унарному предикату.
    @tparam Predicate тип предиката
    @tparam T тип значения
    */
    template <class Predicate, class T>
    class replace_if_function
    {
    public:
        /// @brief Тип возвращаемого значения
        typedef T const & result_type;

        /** @brief Конструктор
        @param pred предикат
        @param new_value новое значение
        @post <tt> this->predicate() == pred </tt>
        @post <tt> this->new_value() == new_value </tt>
        */
        replace_if_function(Predicate pred, T new_value)
         : members_{std::move(pred), new_value}
        {}

        /** @brief Оператор вычисления значения
        @param x значение
        @return Если @c x не удовлетворяет предикату, то
        <tt> this->new_value() </tt> иначе --- @c x.
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
    replace_if_function<Predicate, typename reference_wrapper_to_reference<T>::type>
    make_replace_if_function(Predicate pred, T const & new_value)
    {
        typedef typename reference_wrapper_to_reference<T>::type T_unwrapped;
        return replace_if_function<Predicate, T_unwrapped>(std::move(pred), new_value);
    }

    /** @brief Функциональный объект, заменяющий заданное значение на новое
    @tparam T_old тип старого значения
    @tparam T тип значения
    @tparam BinaryPredicate бинарный предикат над @c T
    */
    template <class T_old, class T, class BinaryPredicate = ural::equal_to<> >
    class replace_function
    {
    friend constexpr bool
    operator==(replace_function const & x, replace_function const & y)
    {
        return x.members_ == y.members_;
    }

    public:
        /// @brief Тип предиката
        typedef decltype(make_callable(std::declval<BinaryPredicate>())) predicate_type;

        /// @brief Тип возвращаемого значения
        typedef T const & result_type;

        /** @brief Конструктор
        @param old_value заменяемое значение
        @param new_value новое значение
        @post <tt> this->predicate() == predшcate_type{} </tt>
        @post <tt> this->old_value() == old_value </tt>
        @post <tt> this->new_value() == new_value </tt>
        */
        constexpr explicit replace_function(T_old old_value, T new_value)
         : members_{std::move(old_value), std::move(new_value),
                    predicate_type{}}
        {}

        /** @brief Конструктор
        @param old_value заменяемое значение
        @param new_value новое значение
        @param pred используемый бинарный предикат
        @post <tt> this->predicate() == pre{} </tt>
        @post <tt> this->old_value() == old_value </tt>
        @post <tt> this->new_value() == new_value </tt>
        */
        constexpr explicit replace_function(T_old old_value, T new_value,
                                            BinaryPredicate pred)
         : members_{std::move(old_value), std::move(new_value),
                    make_callable(std::move(pred))}
        {}

        // Свойства
        /** @brief Заменяемое значение
        @return Заменяемое значение
        */
        constexpr T_old const & old_value() const
        {
            return members_[ural::_1];
        }

        /** @brief Новое значение
        @return Новое значение
        */
        constexpr T const & new_value() const
        {
            return members_[ural::_2];
        }

        /** @brief Используемый функциональный объект
        @return Используемый функциональный объект
        */
        constexpr predicate_type const & predicate() const
        {
            return members_[ural::_3];
        }

        /** @brief Применение функционального объекта
        @param x значение
        @return Если <tt> this->predicate()(x, this->old_value()) </tt>, то ---
        <tt> this->new_value() </tt>, иначе --- @c x
        */
        constexpr result_type operator()(T const & x) const
        {
            return (this->predicate()(x, this->old_value()))
                   ? this->new_value() : x;
        }

    private:
        ural::tuple<T_old, T, predicate_type> members_;
    };

    /** @brief Функция создания @c replace_function с нестандартным предикатом,
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
    template <class T1, class T2, class BinaryPredicate>
    constexpr replace_function<typename reference_wrapper_to_reference<T1>::type,
                               typename reference_wrapper_to_reference<T2>::type,
                               BinaryPredicate>
    make_replace_function(T1 old_value, T2 new_value, BinaryPredicate pred)
    {
        typedef typename reference_wrapper_to_reference<T1>::type T1_unwrapped;
        typedef typename reference_wrapper_to_reference<T2>::type T2_unwrapped;
        typedef replace_function<T1_unwrapped, T2_unwrapped, BinaryPredicate> Function;

        return Function(std::move(old_value), std::move(new_value),
                        std::move(pred));
    }

    /**
    По умолчанию сохраняются значения, а не ссылки. Чтобы избежать копирования,
    следует обернуть объекты в вызовы std::cref()
    @param old_value заменяемое значение
    @param new_value новое значение
    @return <tt> replace_function<T>(std::move(old_value), std::move(new_value))  </tt>
    */
    template <class T1, class T2>
    constexpr replace_function<typename reference_wrapper_to_reference<T1>::type,
                               typename reference_wrapper_to_reference<T2>::type,
                               ural::equal_to<>>
    make_replace_function(T1 old_value, T2 new_value)
    {
        typedef typename reference_wrapper_to_reference<T1>::type T1_unwrapped;
        typedef typename reference_wrapper_to_reference<T2>::type T2_unwrapped;
        typedef replace_function<T1_unwrapped, T2_unwrapped> Function;
        return Function(std::move(old_value), std::move(new_value));
    }
}
// namespace ural

#endif
// Z_URAL_FUNCTIONAL_REPLACE_HPP_INCLUDED
