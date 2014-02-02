#ifndef Z_URAL_UTILITY_TRACERS_HPP_INCLUDED
#define Z_URAL_UTILITY_TRACERS_HPP_INCLUDED

/** @file ural/utility/tracers.hpp
 @brief Классы-обёртки для отслеживания количества выполненных операций и т.д.
*/

#include <ural/defs.hpp>
#include <ural/thread.hpp>
#include <ural/functional.hpp>

namespace ural
{
    /** @brief Класс-обёртка для отслеживания количества выполненных операций
    для регулярных объектов
    @tparam T тип значения
    @todo Подсчитывать количество вызовов всех функций, требуемых концепцией
    регулярного типа
    */
    template <class T, class Threading>
    class regular_tracer
    {
    public:
        /// @brief Тип счётчика
        typedef typename Threading::atomic_counter_type counter_type;

        /** @brief Количество активных объектов
        @return Количество объектов, которые были созданы, но ещё не уничтожены
        */
        static counter_type active_objects()
        {
            return constructed_objects() - destroyed_objects();
        }

        /** @brief Количество созданных объектов
        @return Количество созданных объектов
        */
        static counter_type constructed_objects()
        {
            return constructed_ref();
        }

        /** @brief Количество вызовов деструктора
        @return Количество объектов, для которых были вызваны деструкторы
        */
        static counter_type destroyed_objects()
        {
            return destroyed_ref();
        }

        /** @brief Конструктор
        @param init_value начальное значение
        */
        explicit regular_tracer(T init_value)
         : value_{std::move(init_value)}
        {
            ++ constructed_ref();
        }

        /// @brief Деструктор
        ~regular_tracer() noexcept
        {
            try
            {
                ++ destroyed_ref();
            }
            catch(...)
            {}
        }

    private:
        static counter_type & constructed_ref()
        {
            static counter_type inst;
            return inst;
        }

        static counter_type & destroyed_ref()
        {
            static counter_type inst;
            return inst;
        }

    private:
        T value_;
    };

    /** @todo Подсчитывать число вызовов, завершивхися без исключений
    @tparam F тип функционального объекта
    @tparam Tag тэг
    @tparam Threading тип, определяющий стратегию многопоточности
    */
    template <class F, class Tag = use_default, class Threading = use_default>
    class functor_tracer
     : private decltype(ural::make_functor(std::declval<F>()))
    {
        typedef decltype(ural::make_functor(std::declval<F>())) Base_class;
    public:
        /// @brief Стратегия многопоточности
        typedef typename default_helper<Threading, single_thread_policy>::type
            threading_policy;

        /// @brief Тип счётчика
        typedef typename threading_policy::atomic_counter_type counter_type;

        /// @brief Тип функционального объекта
        typedef F target_type;

        /** @brief Конструктор
        @post <tt> this->target() == F{} </tt>
        */
        explicit functor_tracer()
         : Base_class{}
        {}

        /** @brief Конструктор
        @param f используемый функтор
        @post <tt> this->target() == f </tt>
        */
        explicit functor_tracer(F f)
         : Base_class(std::move(f))
        {}

        /** @brief Оператор вызова
        @return this->functor()(std::forward<Args>(args)...);
        */
        template <class... Args>
        auto operator()(Args && ... args) const
        -> decltype(std::declval<F>()(std::forward<Args>(args)...))
        {
            ++ calls_ref();
            return this->functor()(std::forward<Args>(args)...);
        }

        /** @brief Количество вызовов
        @return Количество вызовов оператора () с момента последнего вызова
        @c reset_calls.
        */
        static counter_type calls()
        {
            return calls_ref();
        }

        /** @brief Сбросить счётчик количества вызовов
        @post <tt> this->calls() == 0 </tt>
        */
        static void reset_calls()
        {
            calls_ref() = 0;
        }

        Base_class const & functor() const
        {
            return *this;
        }

    private:
        static counter_type & calls_ref()
        {
            static counter_type var{0};
            return var;
        }
    };
}
// namespace ural

#endif
// Z_URAL_UTILITY_TRACERS_HPP_INCLUDED
