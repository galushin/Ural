#ifndef Z_URAL_FUNCTIONAL_MAKE_FUNCTOR_HPP_INCLUDED
#define Z_URAL_FUNCTIONAL_MAKE_FUNCTOR_HPP_INCLUDED

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
