#ifndef Z_URAL_FUNCTIONAL_MAKE_FUNCTOR_HPP_INCLUDED
#define Z_URAL_FUNCTIONAL_MAKE_FUNCTOR_HPP_INCLUDED

#include <ural/defs.hpp>

/** @file ural/functional/make_functor.hpp
 @brief Классы и функции для преобразования указателей на функции, функции-члены
 и переменные члены.
*/

namespace ural
{
    /** Преобразование в функциональный объект. Перегрузка по умолчанию: просто
    возвращает свой аргумент.
    @brief Преобразование в функциональный объект
    @param f функциональный объект
    @return f
    */
    template <class F>
    constexpr
    typename std::enable_if<std::is_member_pointer<F>::value == false, F>::type
    make_functor(F f)
    {
        return f;
    }

    template <class Signature>
    class function_ptr_functor;

    template <class R, class... Args>
    class function_ptr_functor<R(Args...)>
    {
    public:
        typedef R(*target_type)(Args...);
        typedef R result_type;

        function_ptr_functor(target_type f)
         : target_{f}
        {}

        result_type
        operator()(typename boost::call_traits<Args>::param_type... args) const
        {
            return this->target()(args...);
        }

        target_type target() const
        {
            return this->target_;
        }

    private:
        target_type target_;

    };

    template <class T, class R>
    class function_ptr_functor<R(T::*)>
    {
    public:
        typedef R (T::*target_type);

        explicit function_ptr_functor(target_type mv)
         : mv_(mv)
        {}

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

        template <class U>
        auto operator()(std::reference_wrapper<U> r) const
        -> decltype(std::declval<function_ptr_functor>()(r.get()))
        {
            return (*this)(r.get());
        }

        template <class U>
        auto operator()(U * p) const
        -> decltype(std::declval<function_ptr_functor>()(*p))
        {
            return (*this)(*p);
        }

        target_type target() const
        {
            return mv_;
        }

    private:
        target_type mv_;
    };

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
        typedef typename std::conditional<is_v, Res_v, Res>::type type;
    };

    /** @brief Функциональный объект для указателей на функции-члены
    @tparam R тип возвращаемого значения
    @tparam T тип класса, которому принадлежит функция член, возможно, с
    квалификаторами @b const и @b volatile
    @tparam Args список типов аргументов
    */
    template <class R, class T, class... Args>
    class mem_fn_functor
    {
    public:
        typedef typename declare_mem_fn_ptr_type<R, T, Args...>::type
            target_type;

        typedef R result_type;

        explicit mem_fn_functor(target_type mf)
         : mf_{mf}
        {}

        result_type
        operator()(T & obj,
                   typename boost::call_traits<Args>::param_type... args) const
        {
            return (obj.*mf_)(args...);
        }

        result_type
        operator()(std::reference_wrapper<T> r,
                   typename boost::call_traits<Args>::param_type... args) const
        {
            return (*this)(r.get(), args...);
        }

        template <class Ptr>
        result_type
        operator()(Ptr const & p,
                   typename boost::call_traits<Args>::param_type... args) const
        {
            return (*this)(*p, args...);
        }

        target_type target() const
        {
            return mf_;
        }

    private:
        target_type mf_;
    };

    template <class F>
    struct mem_fn_functor_type;

    template <class T, class R, class... Args>
    struct mem_fn_functor_type<R(T::*)(Args...)>
     : ural::declare_type<mem_fn_functor<R, T, Args...>>
    {};

    template <class T, class R, class... Args>
    struct mem_fn_functor_type<R(T::*)(Args...) const>
     : ural::declare_type<mem_fn_functor<R, T const, Args...>>
    {};

    template <class T, class R, class... Args>
    struct mem_fn_functor_type<R(T::*)(Args...) volatile>
     : ural::declare_type<mem_fn_functor<R, T volatile, Args...>>
    {};

    template <class T, class R, class... Args>
    struct mem_fn_functor_type<R(T::*)(Args...) const volatile>
     : ural::declare_type<mem_fn_functor<R, T const volatile, Args...>>
    {};

    template <class F>
    typename std::enable_if<std::is_member_function_pointer<F>::value,
                            typename mem_fn_functor_type<F>::type>::type
    make_functor(F mf)
    {
        return typename mem_fn_functor_type<F>::type(mf);
    }

    template <class T>
    typename std::enable_if<std::is_member_object_pointer<T>::value,
                            function_ptr_functor<T>>::type
    make_functor(T mv)
    {
        return function_ptr_functor<T>(mv);
    }

    template <class R, class... Args>
    function_ptr_functor<R(Args...)>
    make_functor(R(*f)(Args...))
    {
        return function_ptr_functor<R(Args...)>{f};
    }
}
// namespace ural

#endif
// Z_URAL_FUNCTIONAL_MAKE_FUNCTOR_HPP_INCLUDED
