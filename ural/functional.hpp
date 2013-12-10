#ifndef Z_URAL_FUNCTIONAL_HPP_INCLUDED
#define Z_URAL_FUNCTIONAL_HPP_INCLUDED

/** @file ural/functional.hpp
 @brief Функциональные объекты и средства для работы с ними
 @todo Разбить на более мелкие файлы
*/

#include <boost/compressed_pair.hpp>

namespace ural
{
    template <class T>
    class value_functor
    {
    public:
        typedef T const & result_type;

        explicit value_functor(T value)
         : value_(std::move(value))
        {}

        result_type operator()() const
        {
            return this->value_;
        }

    private:
        T value_;
    };

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

    // Негатор
    template <class Predicate>
    class not_functor
     : decltype(make_functor(std::declval<Predicate>()))
    {
    public:
        typedef decltype(make_functor(std::declval<Predicate>())) target_type;

        explicit not_functor(Predicate pred)
         : target_type(std::move(pred))
        {}

        target_type const & target() const
        {
            return *this;
        }

        template <class... Args>
        auto operator()(Args && ... args) const
        -> decltype(!this->target()(std::forward<Args>(args)...))
        {
            return !this->target()(std::forward<Args>(args)...);
        }
    };

    template <class Predicate>
    auto not_fn(Predicate pred)
    -> not_functor<decltype(make_functor(std::move(pred)))>
    {
        typedef not_functor<decltype(make_functor(std::move(pred)))> Functor;
        return Functor{make_functor(std::move(pred))};
    }

    // Функциональные объекты для операторов
    template <class T1 = void, class T2 = T1>
    class plus;

    /// @brief Специализация с выводом типов обоих аргументов
    template <>
    class plus<void, void>
    {
    public:
        /** @brief Оператор вычисления значения
        @param x левый операнд
        @param y правый операнд
        @return <tt> std::forward<T1>(x) + std::forward<T2>(y) </tt>
        */
        template <class T1, class T2>
        constexpr auto operator()(T1 && x, T2 && y) const
        -> decltype(std::forward<T1>(x) + std::forward<T2>(y))
        {
            return std::forward<T1>(x) + std::forward<T2>(y);
        }
    };

    template <class T1 = void, class T2 = T1>
    class minus;

    /// @brief Специализация с выводом типов обоих аргументов
    template <>
    class minus<void, void>
    {
    public:
        /** @brief Оператор вычисления значения
        @param x левый операнд
        @param y правый операнд
        @return <tt> std::forward<T1>(x) - std::forward<T2>(y) </tt>
        */
        template <class T1, class T2>
        constexpr auto operator()(T1 && x, T2 && y) const
        -> decltype(std::forward<T1>(x) - std::forward<T2>(y))
        {
            return std::forward<T1>(x) - std::forward<T2>(y);
        }
    };

    template <class T1 = void, class T2 = T1>
    class multiplies;

    /// @brief Специализация с выводом типов обоих аргументов
    template <>
    class multiplies<void, void>
    {
    public:
        /** @brief Оператор вычисления значения
        @param x левый операнд
        @param y правый операнд
        @return <tt> std::forward<T1>(x) * std::forward<T2>(y) </tt>
        */
        template <class T1, class T2>
        constexpr auto operator()(T1 && x, T2 && y) const
        -> decltype(std::forward<T1>(x) * std::forward<T2>(y))
        {
            return std::forward<T1>(x) * std::forward<T2>(y);
        }
    };

    template <class T1 = void, class T2 = T1>
    class equal_to
    {
    public:
        constexpr auto
        operator()(typename boost::call_traits<T1>::param_type x,
                   typename boost::call_traits<T1>::param_type y) const
        -> decltype(x == y)
        {
            return x == y;
        }
    };

    template <>
    class equal_to<void, void>
    {
    public:
        template <class T1, class T2>
        constexpr auto operator()(T1 const & x, T2 const & y) const
        -> decltype(x == y)
        {
            return x == y;
        }
    };

    template <class T1 = void, class T2 = T1>
    class less
    {
    public:
        constexpr auto operator()(T1 const & x, T2 const & y) const
        -> decltype(x < y)
        {
            return x < y;
        }
    };

    template <>
    class less<void, void>
    {
    public:
        template <class T1, class T2>
        constexpr auto operator()(T1 const & x, T2 const & y) const
        -> decltype(x < y)
        {
            return x < y;
        }
    };

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
            if(impl_.second()(*s, *this->result()))
            {
                impl_.first() = s;
            }

            return *this;
        }

        ForwardSequence const & result() const
        {
            return impl_.first();
        }

    private:
        // @todo Закрытое наследование, а не членство?
        boost::compressed_pair<ForwardSequence, Compare> impl_;
    };
}
// namespace ural

#endif
// Z_URAL_FUNCTIONAL_HPP_INCLUDED
