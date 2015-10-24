#ifndef Z_URAL_FUNCTIONAL_MAKE_CALLABLE_HPP_INCLUDED
#define Z_URAL_FUNCTIONAL_MAKE_CALLABLE_HPP_INCLUDED

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

#include <ural/functional/make_callable.hpp>
#include <ural/type_traits.hpp>
#include <ural/defs.hpp>

#include <boost/call_traits.hpp>

/** @file ural/functional/make_callable.hpp
 @brief Классы и функции для преобразования указателей на функции, функции-члены
 и переменные члены.
*/

namespace ural
{
    template <class Signature>
    class function_ptr_wrapper;

    /** @brief Оператор "равно"
    @param x левый операнд
    @param y правый операнд
    @return <tt> x.target() == y.target() </tt>
    */
    template <class Signature>
    constexpr bool
    operator==(function_ptr_wrapper<Signature> const & x,
               function_ptr_wrapper<Signature> const & y)
    {
        return x.target() == y.target();
    }

    /** @brief Функциональный объект на основе указателя на функцию
    @tparam R тип возвращаемого значения
    @tparam Args типы аргументов
    */
    template <class R, class... Args>
    class function_ptr_wrapper<R(Args...)>
    {
    public:
        /// @brief Тип указателя на функцию
        typedef R(*target_type)(Args...);

        /// @brief Тип возвращаемого значения
        typedef R result_type;

        /** @brief Конструктор
        @param f указатель на функцию
        @post <tt> this->target() == f </tt>
        */
        constexpr
        function_ptr_wrapper(target_type f)
         : target_{f}
        {}

        /** @brief Оператор применения функционального объекта
        @param args аргументы
        @return <tt> this->target()(args...) </tt>
        */
        result_type
        operator()(typename boost::call_traits<Args>::param_type... args) const
        {
            return this->target()(args...);
        }

        /** @brief Указатель на функцию
        @return Заданный указатель на функцию-член
        */
        target_type const & target() const
        {
            return this->target_;
        }

        /** @brief Неявное преобразование в указатель на функцию-член
        @return Заданный указатель на функцию-член
        */
        operator target_type const &() const
        {
            return this->target();
        }

    private:
        target_type target_;

    };

    template <class F>
    constexpr bool operator==(function_ptr_wrapper<F> const & x,
                              typename function_ptr_wrapper<F>::target_type y)
    {
        return x.target() == y;
    }

    template <class F>
    constexpr bool operator==(typename function_ptr_wrapper<F>::target_type x,
                              function_ptr_wrapper<F> const & y)
    {
        return x == y.target();
    }

    /** @brief Функциональный объект на основе указателя на переменную-член
    @tparam T класс, которому принадлежит переменная-член
    @tparam R тип переменной-члена
    */
    template <class T, class R>
    class function_ptr_wrapper<R(T::*)>
    {
    public:
        /// @brief Тип указателя на переменную-член
        typedef R (T::*target_type);

        /** @brief Конструктор
        @param mv указатель на переменную-член
        @post <tt> this->target() == mv </tt>
        */
        explicit function_ptr_wrapper(target_type mv)
         : mv_(mv)
        {}

        //@{
        /** @brief Оператор вычисления значения
        @param obj объект, к которому нужно применить функцию-член
        @return <tt> obj.*mv_ </tt>
        */
        R & operator()(T & obj) const
        {
            return obj.*mv_;
        }

        R const & operator()(T const & obj) const
        {
            return obj.*mv_;
        }

        R volatile & operator()(T volatile & obj) const
        {
            return obj.*mv_;
        }

        R const volatile & operator()(T const volatile & obj) const
        {
            return obj.*mv_;
        }
        //@}

        /** @brief Оператор применения функционального объекта
        @param r обёртка ссылки на объект
        @return <tt> (*this)(r.get()) </tt>
        */
        template <class U>
        auto operator()(std::reference_wrapper<U> r) const
        -> decltype(std::declval<function_ptr_wrapper>()(r.get()))
        {
            return (*this)(r.get());
        }

        /** @brief Оператор применения функционального объекта
        @param p указатель на объект
        @return <tt> (*this)(*p) </tt>
        */
        template <class U>
        auto operator()(U * p) const
        -> decltype(std::declval<function_ptr_wrapper>()(*p))
        {
            return (*this)(*p);
        }

    private:
        R & call(T & obj) const
        {
            return obj.*mv_;
        }

        R const & call(T const & obj) const
        {
            return obj.*mv_;
        }

        R volatile & call(T volatile & obj) const
        {
            return obj.*mv_;
        }

        R const volatile & call(T const volatile & obj) const
        {
            return obj.*mv_;
        }
    public:
        template <class Ptr>
        auto operator()(Ptr const & p) const
        -> decltype(std::declval<function_ptr_wrapper>().call(*p))
        {
            return this->call(*p);
        }

        /** @brief Доступ к указателю на переменную-член
        @return Заданный указатель на переменную-член
        */
        target_type target() const
        {
            return mv_;
        }

    private:
        target_type mv_;
    };

    /** @brief Класс-характеристика, синтезирующий тип указателя на функцию-член
    по типу класса (возможно с квалификаторами @b const и/или @b volatile), типу
    возвращаемого значения и типам аргументов.
    @tparam R тип возвращаемого значения
    @tparam T тип класса
    @tparam Args типы аргументов
    */
    template <class R, class T, class... Args>
    struct declare_mem_fn_ptr_type
    {
    private:
        typedef typename std::remove_cv<T>::type Class_type;
        static constexpr bool is_c = std::is_const<T>::value;
        static constexpr bool is_v = std::is_volatile<T>::value;

        typedef R(Class_type::*Sig)(Args...);
        typedef R(Class_type::*Sig_c)(Args...) const;
        typedef R(Class_type::*Sig_v)(Args...) volatile;
        typedef R(Class_type::*Sig_cv)(Args...) const volatile;

        typedef typename std::conditional<is_c, Sig_c, Sig>::type Res;
        typedef typename std::conditional<is_c, Sig_cv, Sig_v>::type Res_v;

    public:
        /// @brief Тип указателя на функцию
        typedef typename std::conditional<is_v, Res_v, Res>::type type;
    };

    /** @brief Функциональный объект для указателей на функции-члены
    @tparam R тип возвращаемого значения
    @tparam T тип класса, которому принадлежит функция член, возможно, с
    квалификаторами @b const и @b volatile
    @tparam Args список типов аргументов
    */
    template <class R, class T, class... Args>
    class mem_fn_wrapper
    {
    friend bool operator==(mem_fn_wrapper const & x, mem_fn_wrapper const & y)
    {
        return x.target() == y.target();
    }

    public:
        typedef typename declare_mem_fn_ptr_type<R, T, Args...>::type
            target_type;

        /// @brief Тип возвращаемого значения
        typedef R result_type;

        /** @brief Конструктор
        @param mf указатель на функцию-член
        @post <tt> this->target() == mf </tt>
        */
        explicit mem_fn_wrapper(target_type mf)
         : mf_{mf}
        {}

        /** @brief Применение функционального объекта
        @param obj объект, для которого вызывается функция-член
        @param args аргументы
        @return <tt> (obj.*mf_)(args...) </tt>
        */
        result_type
        operator()(T & obj,
                   typename boost::call_traits<Args>::param_type... args) const
        {
            return (obj.*mf_)(args...);
        }

        /** @brief Применение функционального объекта
        @param r обёртка ссылки на объект, для которого вызывается
        функция-член
        @param args аргументы
        @return <tt> (*this)(r.get(), args...) </tt>
        */
        result_type
        operator()(std::reference_wrapper<T> r,
                   typename boost::call_traits<Args>::param_type... args) const
        {
            return (*this)(r.get(), args...);
        }

        /** @brief Применение функционального объекта
        @param p указатель (или умный указатель) на объект, для которого
        вызывается функция-член.
        @param args аргументы
        @return <tt> (*this)(r.get(), args...) </tt>
        */
        template <class Ptr>
        result_type
        operator()(Ptr const & p,
                   typename boost::call_traits<Args>::param_type... args) const
        {
            return (*this)(*p, args...);
        }

        /** @brief Доступ к указателю на функцию-член
        @return mf_ заданный указатель на функцию-член
        */
        target_type target() const
        {
            return mf_;
        }

    private:
        target_type mf_;
    };

    template <class F>
    struct mem_fn_functional_type;

    template <class T, class R, class... Args>
    struct mem_fn_functional_type<R(T::*)(Args...)>
     : ural::declare_type<mem_fn_wrapper<R, T, Args...>>
    {};

    template <class T, class R, class... Args>
    struct mem_fn_functional_type<R(T::*)(Args...) const>
     : ural::declare_type<mem_fn_wrapper<R, T const, Args...>>
    {};

    template <class T, class R, class... Args>
    struct mem_fn_functional_type<R(T::*)(Args...) volatile>
     : ural::declare_type<mem_fn_wrapper<R, T volatile, Args...>>
    {};

    template <class T, class R, class... Args>
    struct mem_fn_functional_type<R(T::*)(Args...) const volatile>
     : ural::declare_type<mem_fn_wrapper<R, T const volatile, Args...>>
    {};

    class make_callable_fn
    {
    public:
        /** @brief Создание функционального объекта на основе указателя на
        функцию-член
        @param mf указатель на функцию-член
        @return <tt> typename mem_fn_functional_type<F>::type(mf) </tt>
        */
        template <class F>
        typename std::enable_if<std::is_member_function_pointer<F>::value,
                                typename mem_fn_functional_type<F>::type>::type
        operator()(F mf) const
        {
            return typename mem_fn_functional_type<F>::type(mf);
        }

        /** @brief Создание функционального объекта на основе указателя на
        переменную-член
        @param mv указатель на переменную-член
        @return <tt> function_ptr_wrapper<T>(mv) </tt>
        */
        template <class T>
        typename std::enable_if<std::is_member_object_pointer<T>::value,
                                function_ptr_wrapper<T>>::type
        operator()(T mv) const
        {
            return function_ptr_wrapper<T>(mv);
        }

        /** @brief Создание функционального объекта на основе указателя на функцию
        @param f указатель на функцию
        @return <tt> function_ptr_wrapper<R(Args...)>{f} </tt>
        */
        template <class R, class... Args>
        function_ptr_wrapper<R(Args...)>
        operator()(R(*f)(Args...)) const
        {
            return function_ptr_wrapper<R(Args...)>{f};
        }

        /** Преобразование в функциональный объект. Перегрузка по умолчанию: просто
        возвращает свой аргумент.
        @brief Преобразование в функциональный объект
        @param f функциональный объект
        @return f
        */
        template <class F>
        constexpr
        typename disable_if<std::is_member_pointer<F>::value, F>::type
        operator()(F f) const
        {
            return f;
        }
    };

    namespace
    {
        /** @brief Функциоанльный объект, преобразующий указатели на
        функции-члены и переменные-члены в функциональные объекты, а остальные
        типы оставляющий без изменения.
        */
        constexpr auto const & make_callable = odr_const<make_callable_fn>;
    }

    template <class T>
    using FunctionType = decltype(::ural::make_callable_fn{}(std::declval<T>()));
}
// namespace ural

#endif
// Z_URAL_FUNCTIONAL_MAKE_CALLABLE_HPP_INCLUDED
