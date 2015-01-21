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

    /** @brief Оператор "равно"
    @param x левый операнд
    @param y правый операнд
    @return <tt> x() == y() </tt>
    */
    template <class T1, class T2>
    constexpr bool
    operator==(value_functor<T1> const & x, value_functor<T2> const & y)
    {
        return x() == y();
    }

    /** @brief Накопитель для определения наименьшего значения
    @tparam T тип значения
    @tparam Compare функция сравнения
    */
    template <class T, class Compare = ural::less<> >
    class min_element_accumulator
    {
    public:
        // Типы
        /// @brief Тип значения
        typedef T value_type;

        /// @brief Тип функции сравнения
        typedef Compare compare_type;

        // Конструкторы
        /** @brief Конструктор
        @param init_value начальное значение
        @param cmp функция сравнения
        @post <tt> this->result() == init_value </tt>
        @post <tt> this->compare() == cmp </tt>
        */
        explicit min_element_accumulator(T init_value, Compare cmp = Compare{})
         : impl_{std::move(init_value), std::move(cmp)}
        {}

        // Обновление
        /** @brief Обновление статистики
        @param new_value новое значение
        @return <tt> *this </tt>
        */
        template <class Arg>
        min_element_accumulator &
        operator()(Arg && new_value)
        {
            this->update(std::forward<Arg>(new_value));
            return *this;
        }

        /** @brief Обновление статистики
        @param new_value новое значение
        @return @b true, если значение было обновлено, иначе --- @b false.
        */
        template <class Arg>
        bool update(Arg && new_value)
        {
            if(this->compare()(new_value, this->result()))
            {
                ural::get(impl_, ural::_1) = std::forward<Arg>(new_value);
                return true;
            }

            return false;
        }

        // Свойства
        /** @brief Текущее значение статистики
        @return Текущее значение статистики
        */
        value_type const & result() const
        {
            return ural::get(impl_, ural::_1);
        }

        /** @brief Используемая функция сравнения
        @return Ссылка на используемую функцию сравнения
        */
        Compare const & compare() const
        {
            return ural::get(impl_, ural::_2);
        }

    private:
        tuple<value_type, Compare> impl_;
    };

    /** @brief Функция создания накопителя наименьшего значения
    @param init_value начальное значение
    @param pred бинарный предикат
    @return <tt> min_element_accumulator<>(std::move(init_value),
                                           ural::make_functor(std::move(pred)) </tt>
    */
    template <class Value, class BinaryPredicate>
    auto make_min_element_accumulator(Value init_value, BinaryPredicate pred)
    -> min_element_accumulator<Value, decltype(ural::make_functor(std::move(pred)))>
    {
        typedef decltype(ural::make_functor(std::move(pred))) Functor;
        typedef min_element_accumulator<Value, Functor> Result;
        return Result{std::move(init_value), ural::make_functor(std::move(pred))};
    }

    /** @brief Адаптор функционального объекта с двумя аргументами, меняющий
    порядок аргументов.
    @tparam BinaryFunctor Тип функционального объекта с двумя аргументами
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
        constexpr explicit binary_reverse_args_functor(BinaryFunctor f)
         : Base(std::move(f))
        {}

        /** @brief Вычисление значения
        @param x первый аргумент
        @param y второй аргумент
        @return <tt> this->functor()(std::forward<T2>(y), std::forward<T1>(x)) </tt>
        */
        template <class T1, class T2>
        constexpr auto operator()(T1 && x, T2 && y) const
        -> decltype(std::declval<BinaryFunctor>()(std::forward<T2>(y), std::forward<T1>(x)))
        {
            return this->functor()(std::forward<T2>(y), std::forward<T1>(x));
        }

        /** @brief Адаптируемый функциональный объект
        @return Адаптируемый функциональный объект
        */
        constexpr BinaryFunctor const & functor() const
        {
            return static_cast<BinaryFunctor const &>(*this);
        }
    };

    /** @brief Создание адаптора функционального объекта с двумя аргументами,
    меняющего порядок аргументов.
    @param f функциональный объект с двумя аргументами
    @return <tt> Functor{make_functor(std::move(f))} </tt>, где @c Functor ---
    <tt> binary_reverse_args_functor<decltype(make_functor(std::move(f)))> </tt>
    */
    template <class BinaryFunctor>
    constexpr auto make_binary_reverse_args(BinaryFunctor f)
    -> binary_reverse_args_functor<decltype(make_functor(std::move(f)))>
    {
        typedef binary_reverse_args_functor<decltype(make_functor(std::move(f)))>
            Functor;
        return Functor{make_functor(std::move(f))};
    }

    template <class Fun, Fun f>
    class static_fn;

    /** @brief Функциональный объект на основе указателя на функцию-член
    @tparam R тип возвращаемого значения
    @tparam T класс, которому принадлежит функция-член
    @tparam Args типы аргументов
    @tparam f указатель на функцию-член
    */
    template <class R, class T, class... Args, R(T::*f)(Args...)>
    class static_fn<R(T::*)(Args...), f>
    {
    public:
        /// @brief Тип возвращаемого значения
        typedef R result_type;

        /** @brief Оператор применения функционального объекта
        @param obj объект, для которого вызывается функция-член
        @param args аргументы
        @return <tt> (obj.*f)(args...) </tt>
        */
        constexpr result_type
        operator()(T & obj, typename boost::call_traits<Args>::type... args) const
        {
            return (obj.*f)(args...);
        }
    };
}
// namespace ural

#endif
// Z_URAL_FUNCTIONAL_HPP_INCLUDED
