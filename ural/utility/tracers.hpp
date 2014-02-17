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
    @tparam Threading тип стратегии работы с многопоточностью
    @todo Подсчитывать количество вызовов: конструктор копий, конструктор
    перемещения
    */
    template <class T, class Threading = single_thread_policy>
    class regular_tracer
    {
    friend bool operator==(regular_tracer const & x, regular_tracer const & y)
    {
        ++ regular_tracer::equality_ref();
        return x.value() == y.value();
    }

    friend bool operator<(regular_tracer const & x, regular_tracer const & y)
    {
        ++ regular_tracer::less_ref();
        return x.value() < y.value();
    }

    public:
        /// @brief Тип счётчика
        typedef typename Threading::atomic_counter_type counter_type;

        // Статистики
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

        static counter_type copy_assignments_count()
        {
            return copy_assign_ref();
        }

        static counter_type move_assignments_count()
        {
            return move_assign_ref();
        }

        static counter_type equality_count()
        {
            return equality_ref();
        }

        static counter_type less_count()
        {
            return less_ref();
        }

        // Регулярный объект
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

        regular_tracer & operator=(regular_tracer const & x)
        {
            this->value_ = x.value_;
            ++ copy_assign_ref();
            return *this;
        }

        regular_tracer & operator=(regular_tracer && x)
        {
            this->value_ = std::move(x.value_);
            ++ move_assign_ref();
            return *this;
        }

        T const & value() const
        {
            return this->value_;
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

        static counter_type & copy_assign_ref()
        {
            static counter_type inst;
            return inst;
        }

        static counter_type & move_assign_ref()
        {
            static counter_type inst;
            return inst;
        }

        static counter_type & equality_ref()
        {
            static counter_type inst;
            return inst;
        }

        static counter_type & less_ref()
        {
            static counter_type inst;
            return inst;
        }

    private:
        T value_;
    };

    /** @tparam F тип функционального объекта
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
