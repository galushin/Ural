#ifndef Z_URAL_UTILITY_TRACERS_HPP_INCLUDED
#define Z_URAL_UTILITY_TRACERS_HPP_INCLUDED

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

/** @file ural/utility/tracers.hpp
 @brief Классы-обёртки для отслеживания количества выполненных операций и т.д.
*/

#include <ural/defs.hpp>
#include <ural/thread.hpp>
#include <ural/functional.hpp>
#include <ural/operators.hpp>

namespace ural
{
    /** @brief Класс-обёртка для отслеживания количества выполненных операций
    для регулярных объектов
    @tparam T тип значения
    @tparam Threading тип стратегии работы с многопоточностью
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

        /** @brief Количество вызовов конструктора копий
        @return Количество вызовов конструктора копий
        */
        static counter_type copy_ctor_count()
        {
            return copy_ctor_ref();
        }

        /** @brief Количество вызовов конструктора с перемещением
        @return Количество вызовов конструктора с перемещением
        */
        static counter_type move_ctor_count()
        {
            return move_ctor_ref();
        }

        /** @brief Количество вызовов оператора копирующего присваивания
        @return Количество вызовов оператора копирующего присваивания
        */
        static counter_type copy_assignments_count()
        {
            return copy_assign_ref();
        }

        /** @brief Количество вызовов оператора присваивания с перемещением
        @return Количество вызовов оператора присваивания с перемещением
        */
        static counter_type move_assignments_count()
        {
            return move_assign_ref();
        }

        /** @brief Количество вызовов оператора "равно"
        @return Количество вызовов оператора "равно"
        */
        static counter_type equality_count()
        {
            return equality_ref();
        }

        /** @brief Количество вызовов оператора "меньше"
        @return Количество вызовов оператора "меньше"
        */
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

        /** @brief Конструктор копий
        @param x объект, который должен быть скопирован
        @post <tt> *this == x </tt>
        */
        regular_tracer(regular_tracer const & x)
         : value_{x.value_}
        {
            ++ constructed_ref();
            ++ copy_ctor_ref();
        }

        /** @brief Конструктор с перемещением
        @param x объект, содержимое которого будет перемещено
        @post <tt> *this </tt> будет содержать значение, которое до вызова
        содержал @c x.
        */
        regular_tracer(regular_tracer && x)
         : value_{std::move(x.value_)}
        {
            ++ constructed_ref();
            ++ move_ctor_ref();
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

        /** @brief Оператор копирующего присваивания
        @param x объект, содержимое которого будет скопировано
        @return <tt> *this </tt>
        */
        regular_tracer & operator=(regular_tracer const & x)
        {
            this->value_ = x.value_;
            ++ copy_assign_ref();
            return *this;
        }

        /** @brief Оператор присваивания с перемещением
        @param x объект, содержимое которого будет перемещено
        @return <tt> *this </tt>
        */
        regular_tracer & operator=(regular_tracer && x)
        {
            this->value_ = std::move(x.value_);
            ++ move_assign_ref();
            return *this;
        }

        /** @brief Значение
        @return Значение заданное в конструкторе или в результате присваивания
        */
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

        static counter_type & copy_ctor_ref()
        {
            static counter_type inst;
            return inst;
        }

        static counter_type & move_ctor_ref()
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

    template <class Ch, class Tr, class T>
    std::basic_ostream<Ch, Tr> &
    operator<<(std::basic_ostream<Ch, Tr> & os, regular_tracer<T> const & x)
    {
        return os << x.value();
    }

    /** Адаптер функционального объекта, подсчитывающий количество
    вызовов. Подсчитывается количество вызовов на класс. Для более тонкого
    подсчёта можно использовать тэг.
    @brief Адаптер функционального объекта, подсчитывающий количество
    вызовов.
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

        /** @brief Используемый функциональный объект
        @return Используемый функциональный объект
        */
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

    /** @brief Функция создания @c functor_tracer
    @tparam Tag тэг
    @tparam Threading стратегия многопоточности
    @param f функциональный объект
    @return functor_tracer<F>(f)
    */
    template <class F, class Tag = use_default, class Threading = use_default>
    functor_tracer<F, Tag, Threading>
    make_function_tracer(F f)
    {
        return functor_tracer<F, Tag, Threading>(std::move(f));
    }

    /** @todo Синхронизация?
    */
    template <class T, class Alloc = std::allocator<T>>
    class tracing_allocator
    {
        typedef Alloc Base;

    public:
        typedef typename Base::value_type value_type;
        typedef typename Base::size_type size_type;
        typedef typename Base::difference_type difference_type;
        typedef typename Base::const_reference const_reference;
        typedef typename Base::reference reference;
        typedef typename Base::pointer pointer;
        typedef typename Base::const_pointer const_pointer;

        typedef std::true_type propagate_on_container_swap;

        explicit tracing_allocator(int id = 0)
         : id_{id}
        {}

        pointer allocate(size_type n)
        {
            ++ tracing_allocator::get_allocations();

            return a_.allocate(n);
        }

        void deallocate(pointer p, size_type n)
        {
            return a_.deallocate(p, n);
        }

        template <class... Args>
        void construct(pointer p, Args && ... args)
        {
            a_.construct(p, std::forward<Args>(args)...);
        }

        void destroy(pointer p)
        {
            a_.destroy(p);
        }

        template <class U>
        struct rebind
        {
            typedef tracing_allocator<U, Alloc> other;
        };

        int id() const
        {
            return this->id_;
        }

        // Трассировка
        static size_type allocations_count()
        {
            return tracing_allocator::get_allocations();
        }

        static void reset_allocations_count()
        {
            tracing_allocator::get_allocations() = 0;
        }

    private:
        static size_type & get_allocations()
        {
            static size_type instance = 0;
            return instance;
        }

    private:
        int id_;
        Alloc a_;
    };
}
// namespace ural

#endif
// Z_URAL_UTILITY_TRACERS_HPP_INCLUDED
