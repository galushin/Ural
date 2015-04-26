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
#include <ural/functional/make_callable.hpp>
#include <ural/functional/replace.hpp>

#include <ural/tuple.hpp>

namespace ural
{
    /** @brief Класс функционального объекта для создания
    <tt> std::reference_wrapper </tt> без добавления константности.
    */
    class ref_fn
    {
    public:
        //@{
        /** @brief Создание обёртки для ссылки
        @param x ссылка
        @return <tt> std::reference_wrapper<T>(x) </tt>
        */
        template <class T>
        constexpr std::reference_wrapper<T>
        operator()(T & x) const
        {
            return std::reference_wrapper<T>(x);
        }

        template <class T>
        constexpr std::reference_wrapper<T>
        operator=(T & x) const
        {
            return (*this)(x);
        }
        //@}

        //@{
        /** @brief Перегрузка для <tt> std::reference_wrapper </tt>
        @param x обёртка для ссылки
        @return <tt> std::reference_wrapper<T>(x.get()) </tt>
        */
        template <class T>
        constexpr std::reference_wrapper<T>
        operator()(std::reference_wrapper<T> x) const
        {
            return (*this)(x.get());
        }

        template <class T>
        constexpr std::reference_wrapper<T>
        operator=(std::reference_wrapper<T> x) const
        {
            return (*this)(x);
        }
        //@}
    };

    /** @brief Класс функционального объекта для создания
    <tt> std::reference_wrapper </tt> c добавлением константности.
    */
    class cref_fn
    {
    public:
        //@{
        /** @brief Создание обёртки для ссылки
        @param x ссылка
        @return <tt> std::reference_wrapper<T>(x) </tt>
        */
        template <class T>
        constexpr std::reference_wrapper<T const>
        operator()(T const & x) const
        {
            return std::reference_wrapper<T const>(x);
        }

        template <class T>
        constexpr std::reference_wrapper<T const>
        operator=(T const & x) const
        {
            return (*this)(x);
        }
        //@}

        //@{
        /** @brief Перегрузка для <tt> std::reference_wrapper </tt>
        @param x обёртка для ссылки
        @return <tt> std::reference_wrapper<T>(x.get()) </tt>
        */
        template <class T>
        constexpr std::reference_wrapper<T const>
        operator()(std::reference_wrapper<T> x) const
        {
            return (*this)(x.get());
        }

        template <class T>
        constexpr std::reference_wrapper<T const>
        operator=(std::reference_wrapper<T> x) const
        {
            return (*this)(x);
        }
        //@}
    };

    /** @brief Функциональный объект без аргументов, возвращающий фиксированное
    знчение
    @tparam T тип значения

    Если T --- регулярный тип, то <tt> value_functor<T> </tt> --- тоже,
    регулярный.

    Пусть @c f --- это объект типа, <tt> value_functor<T> </tt>, а @c x ---
    типа @c T.
    Выражение <tt> f = x </tt> выглядит не очень логичным. Если нужно изменить
    значение, которое возвращает @c f, то можно воспользоваться кодом вида
    <tt> f = value_functor<T>{x} </tt>.

    Рассматривалась возможность задавать сигнатуру (типы параметров), но она
    была отвергнута, так как лишь усложняет реализацию, а сами параметры вообще
    не используются.
    */
    template <class T>
    class value_functor
    {
    public:
        /// @brief Тип возвращаемого значения
        typedef T const & result_type;

        /** @brief Конструктор
        @param args список аргументов
        @post <tt> (*this)()  == T(std::forward<Args>(args)...) </tt>
        */
        template <class... Args>
        constexpr explicit value_functor(Args &&... args)
         : value_(std::forward<Args>(args)...)
        {}

        /** @brief Оператор вычисления значения
        @return Значение, установленное в конструкторе или в результате
        присваивания
        */
        template <class... Args>
        constexpr result_type operator()(Args &&...) const
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
    constexpr auto
    operator==(value_functor<T1> const & x, value_functor<T2> const & y)
    -> decltype(x() == y())
    {
        return x() == y();
    }

    /** @brief Оператор "меньше"
    @param x левый операнд
    @param y правый операнд
    @return <tt> x() < y() </tt>
    */
    template <class T1, class T2>
    constexpr auto
    operator<(value_functor<T1> const & x, value_functor<T2> const & y)
    -> decltype(x() < y())
    {
        return x() < y();
    }

    /** @brief Функция создания @c value_functor
    @param value начальное значение
    @return <tt> value_functor<T>{value} </tt>
    */
    template <class T>
    constexpr value_functor<T>
    make_value_functor(T const & value)
    {
        return value_functor<T>{value};
    }

    /** @brief Функция создания @c value_functor на основе временного объекта
    @param value начальное значение
    @return <tt> value_functor<T>{std::move(value)} </tt>
    */
    template <class T>
    constexpr value_functor<T>
    make_value_functor(T && value)
    {
        return value_functor<T>{std::move(value)};
    }

    /** @brief Функциональный объект для конструкторов
    @tparam T тип создаваемого объекта
    */
    template <class T>
    class constructor
    {
    public:
        /** @brief Оператор вызова функции
        @param args список аргументов конструктора
        @return <tt> T(std::forward<Args>(args)...) </tt>
        */
        template <class... Args>
        constexpr T operator()(Args && ... args) const
        {
            return T(std::forward<Args>(args)...);
        }
    };

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
                                           ural::make_callable(std::move(pred)) </tt>
    */
    template <class Value, class BinaryPredicate>
    auto make_min_element_accumulator(Value init_value, BinaryPredicate pred)
    -> min_element_accumulator<Value, decltype(ural::make_callable(std::move(pred)))>
    {
        typedef decltype(ural::make_callable(std::move(pred))) Functor;
        typedef min_element_accumulator<Value, Functor> Result;
        return Result{std::move(init_value), ural::make_callable(std::move(pred))};
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
    @return <tt> Functor{make_callable(std::move(f))} </tt>, где @c Functor ---
    <tt> binary_reverse_args_functor<decltype(make_callable(std::move(f)))> </tt>
    */
    template <class BinaryFunctor>
    constexpr auto make_binary_reverse_args(BinaryFunctor f)
    -> binary_reverse_args_functor<decltype(make_callable(std::move(f)))>
    {
        typedef binary_reverse_args_functor<decltype(make_callable(std::move(f)))>
            Functor;
        return Functor{make_callable(std::move(f))};
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

    // Функциональные объекты для операций контейнеров
    /** @brief Функциональный объект, соответствующий функции-члену
    @c pop_front
    */
    class pop_front_fn
    {
    public:
        /** @brief Оператор применения <tt> x.pop_front() </tt>
        @param x аргумент
        */
        template <class T>
        void operator()(T & x) const
        {
            x.pop_front();
        }
    };

    /** @brief Тип функционального объекта для функции @c empty и аналогичной
    функциональности
    */
    class empty_fn
    {
    private:
        template <class Container>
        static bool empty_impl(Container const & x,
                               declare_type<decltype(x.empty())> *)
        {
            return x.empty();
        }

        template <class Container>
        static bool empty_impl(Container const & x, ...)
        {
            return x.size() == 0;
        }

        template <class T, std::size_t N>
        static bool empty_impl(T (&)[N], std::nullptr_t )
        {
            return N == 0;
        }

    public:
        /** @brief Оператор вызова функции
        @param x контейнер @c STL, "почти контейнер" или встроенный массив
        @return Если @c x имеет функцию-член @c empty, то возвращает
        <tt> x.empty </tt>, если @c x -- встроенный C-массив известного размера
        @c N то возвращает @c N, в остальных случаях возвращает
        <tt> x.size() == 0 </tt>
        */
        template <class Container>
        bool operator()(Container const & x) const
        {
            return this->empty_impl(x, nullptr);
        }
    };

    /** @brief Обобщённая реализация операций вида "изменить и вернуть
    копию оригинала в состоянии до изменения"
    */
    class modify_return_old_fn
    {
    public:
        /** @brief Оператор вызова функции
        @param action функциональный объект, выполняющий изменение
        @param x ссылка на объект, который должен быть изменён
        @return Копию @c x до вызова этой функции
        */
        template <class Action, class T>
        T operator()(Action action, T & x) const
        {
            auto old_value = T(x);
            action(x);
            return old_value;
        }
    };

    namespace
    {
        // Обобщённые операции
        constexpr auto const modify_return_old = modify_return_old_fn{};

        // Управление передачей параметров
        constexpr auto const ref = ref_fn{};
        constexpr auto const cref = cref_fn{};

        // Операции контейнеров
        constexpr auto const empty = empty_fn{};
        constexpr auto const pop_front = pop_front_fn{};
    }
}
// namespace ural

#endif
// Z_URAL_FUNCTIONAL_HPP_INCLUDED
