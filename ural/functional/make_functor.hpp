#ifndef Z_URAL_FUNCTIONAL_MAKE_FUNCTOR_HPP_INCLUDED
#define Z_URAL_FUNCTIONAL_MAKE_FUNCTOR_HPP_INCLUDED

/** @file ural/functional/make_functor.hpp
 @brief Классы и функции для преобразования указателей на функции, функции-члены
 и переменные члены.
*/

namespace ural
{
    // Преобразование в функциональный объект
    /** Преобразование в функциональный объект. Перегрузка по умолчанию: просто
    возвращает свой аргумент.
    @brief Преобразование в функциональный объект
    @param f функциональный объект
    @return f
    */
    template <class F>
    F make_functor(F f)
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

    // @todo Доступ к указателю на переменную
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
        auto operator()(U * obj) const
        -> decltype(std::declval<function_ptr_functor>()(*obj))
        {
            return (*this)(*obj);
        }

    private:
        target_type mv_;
    };

    // @todo Оптимальные типы параметров
    // @todo Доступ к указателю на функцию
    // @todo Унифицировать
    template <class T, class R, class... Args>
    class function_ptr_functor<R(T::*)(Args...)>
    {
    public:
        typedef R(T::*target_type)(Args...);

        typedef R result_type;

        explicit function_ptr_functor(target_type mf)
         : mf_{mf}
        {}

        result_type operator()(T & obj, Args... args) const
        {
            return (obj.*mf_)(args...);
        }

    private:
        target_type mf_;
    };

    template <class T, class R, class... Args>
    class function_ptr_functor<R(T::*)(Args...) const>
    {
    public:
        typedef R(T::*target_type)(Args...) const;

        typedef R result_type;

        explicit function_ptr_functor(target_type mf)
         : mf_{mf}
        {}

        result_type operator()(T const & obj, Args... args) const
        {
            return (obj.*mf_)(args...);
        }

    private:
        target_type mf_;
    };

    template <class T, class R, class... Args>
    class function_ptr_functor<R(T::*)(Args...) volatile>
    {
    public:
        typedef R(T::*target_type)(Args...) volatile;

        typedef R result_type;

        explicit function_ptr_functor(target_type mf)
         : mf_{mf}
        {}

        result_type operator()(T volatile & obj, Args... args) const
        {
            return (obj.*mf_)(args...);
        }

    private:
        target_type mf_;
    };

    template <class T, class R, class... Args>
    class function_ptr_functor<R(T::*)(Args...) const volatile>
    {
    public:
        typedef R(T::*target_type)(Args...) const volatile;

        typedef R result_type;

        explicit function_ptr_functor(target_type mf)
         : mf_{mf}
        {}

        result_type operator()(T const volatile & obj, Args... args) const
        {
            return (obj.*mf_)(args...);
        }

    private:
        target_type mf_;
    };

    template <class T, class R>
    function_ptr_functor<R(T::*)>
    make_functor(R(T::*mv))
    {
        return function_ptr_functor<R(T::*)>(mv);
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
