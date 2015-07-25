#ifndef Z_URAL_MEMORY_HPP_INCLUDED
#define Z_URAL_MEMORY_HPP_INCLUDED

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

/** @file ural/memory.hpp
 @brief Средства для работы с динамической памятью
*/

#include <ural/defs.hpp>
#include <ural/utility.hpp>
#include <ural/operators.hpp>

#include <boost/throw_exception.hpp>

#include <type_traits>
#include <cassert>
#include <memory>

namespace ural
{
    /// @cond false
    namespace details
    {
        template <class A>
        struct allocator_is_always_equal_impl
        {
        private:
            template <class U>
            static typename U::is_always_equal
            impl(...);

            template <class U>
            static typename std::is_empty<U>::type
            impl(void const *);

        public:
            typedef decltype(impl<A>(nullptr)) type;
        };
    }
    // namespace details
    ///@endcond

    /** @brief Временная замена (до С++17) для
    <tt> allocator_traits<A>::is_always_equal </tt>
    @tparam A распределитель памяти
    */
    template <class A>
    struct allocator_is_always_equal
     : public details::allocator_is_always_equal_impl<A>::type
    {};

    // Создание объектов в динамической памяти, обёрнутых в unique_ptr
    /** @brief Создание объекта в динамической памяти, обёрнутого в
    @c unique_ptr
    @tparam T тип создаваемого объекта
    @param args аргументы конструктора
    @return <tt> std::unique_ptr<T>(new T(std::forward<Args>(args)...)) </tt>
    */
    template <class T, class... Args>
    typename std::enable_if<!std::is_array<T>::value, std::unique_ptr<T>>::type
    make_unique(Args &&... args)
    {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }

    /** @brief Создание массива в динамической памяти, обёрнутого в
    @c unique_ptr
    @tparam T тип массива неизвестного размера вида <tt> U[] </tt>
    @param size количество элементов
    @return <tt> std::unique_ptr<T>(new Elem[size]()) </tt>, где @c Elem ---
    <tt> typename std::remove_extent<T>::type </tt>
    */
    template <class T>
    typename std::enable_if<std::is_array<T>::value && std::extent<T>::value == 0,
                            std::unique_ptr<T>>::type
    make_unique(size_t size)
    {
        typedef typename std::remove_extent<T>::type Elem;
        return std::unique_ptr<T>(new Elem[size]());
    }

    template <class T>
    typename std::enable_if<std::is_array<T>::value && std::extent<T>::value != 0,
                            std::unique_ptr<T>>::type
    make_unique(size_t size) = delete;

    /** @brief Тип функционального объекта для создания копии заданного объекта
    в динамической памяти, управляемой с помощью <tt> std::unique_ptr </tt>
    @todo Перегрузки с произвольным числом аргументов
    */
    class to_unqiue_ptr_function
    {
    public:
        /** @brief Оператор вызова функции
        @param value значение
        @return <tt> ::ural::make_unique<T>(std::move(value)) </tt>
        */
        template <class T>
        std::unique_ptr<T> operator()(T value) const
        {
            return ::ural::make_unique<T>(std::move(value));
        }
    };

    namespace
    {
        /** @brief Функциональный объект для создания копии заданного объекта
        в динамической памяти, управляемой с помощью <tt> std::unique_ptr </tt>
        @todo Перегрузки с произвольным числом аргументов
        */
        constexpr auto const & to_unique_ptr
            = odr_const_holder<to_unqiue_ptr_function>::value;
    }

    /** @brief Создание копии в динамической памяти
    @param x копируемое значение
    @return <tt> ural::make_unique<T>(std::move(x)) </tt>
    */
    template <class T>
    std::unique_ptr<T> make_copy_new(T x)
    {
        return ural::make_unique<T>(std::move(x));
    }

    // Умный указатель с глубоким копированием
    /** @brief Стратегия копирования по-умолчанию
    @tparam T тип копируемого объекта
    */
    template <class T>
    class default_copy
    {
    public:
        /** @brief Тип умного указателя, управляющего временем жизни созданного
        объекта.
        */
        typedef std::unique_ptr<T> owner_type;

        /** @brief Создание копии в динамической памяти
        @param x копируемое значение
        @return <tt> make_unique<T>(std::move(x)) </tt>
        */
        static owner_type make_copy(T const & x)
        {
            return ural::make_unique<T>(std::move(x));
        }

    protected:
        ~default_copy() = default;
    };

    /** @brief Стратегия копирования с испльзованием заданной функции-члена
    @tparam T тип копируемого объекта
    @tparam clone_fn функция-член, используемая для копирования
    */
    template <class T, std::unique_ptr<T>(T::*clone_fn)() const = &T::clone>
    class member_function_copy
    {
    public:
        /** @brief Тип умного указателя, управляющего временем жизни созданного
        объекта.
        */
        typedef std::unique_ptr<T> owner_type;

        /** @brief Создание копии в динамической памяти
        @param x копируемое значение
        @return <tt> (x.*clone_fn)() </tt>
        */
        static owner_type make_copy(T const & x)
        {
            return (x.*clone_fn)();
        }

    protected:
        ~member_function_copy() = default;
    };

    /** @brief Стратегия проверки указателей, использующая макрос @c assert.
    @tparam Pointer тип указателя
    */
    template <class Pointer>
    class default_ptr_checker
    {
    public:
        /** @brief Проверка того, что указатель не равен @b nullptr
        @param p указатель
        */
        static void assert_not_null(Pointer p)
        {
            assert(p != nullptr);
        }

    protected:
        ~default_ptr_checker() = default;
    };

    /** @brief Стратегия проверки указателей, возбуждающая исключения.
    @tparam Pointer тип указателя
    */
    template <class Pointer>
    class throwing_ptr_checker
    {
    public:
        /** @brief Проверка того, что указатель не равен @b nullptr
        @param p указатель
        */
        static void assert_not_null(Pointer p)
        {
            if(p == nullptr)
            {
                BOOST_THROW_EXCEPTION(std::logic_error{"unexpected null pointer"});
            }
        }

    protected:
        ~throwing_ptr_checker() = default;
    };

    /** @brief Умный указатель с глубоким копированием
    @tparam T тип указываемого объекта
    @tparam Cloner стратегия копирования
    @tparam Deleter стратегия удаления
    @tparam Checker стратегия проверок
    @todo Учесть N3339
    @todo Тесты с полиморфными типами (стратегии копирования и удаления)
    @todo По аналогии с 20.7.1
    @todo По аналогии с 20.7.2
    @todo Какие функции должны быть noexcept?

    Обоснование.

    Конструктор копий не является noexcept, так как копирование указываемого
    объекта может приводить к исключениям.

    Сравнение @c copy_ptr с параметрами разных типов может давать истину только
    если оба они указывают на ноль.

    Конструирование на основе <tt> std::shared_ptr </tt> не вводится, так как
    эти умные указатели имеют разную семантику владения.
    Конструирование на основе <tt> std::auto_ptr </tt> не вводится, так как
    <tt> std::auto_ptr </tt> объявлен нежелательным.

    Специализация для массивов не предоставляется, так как для управления
    динамической памятью с глубоким копированием можно использовать
    <tt> std::vector </tt>.
    */
    template <class T,
              class Cloner = use_default,
              class Deleter = use_default,
              class Checker = use_default>
    class copy_ptr
    {
        friend bool operator==(copy_ptr const & x, copy_ptr const & y)
        {
            return x.get() == y.get();
        }

        friend bool operator==(copy_ptr const & x, std::nullptr_t)
        {
            return !x;
        }

        friend bool operator==(std::nullptr_t, copy_ptr const & x)
        {
            return !x;
        }

        friend bool operator==(copy_ptr const & x, T const * p)
        {
            return x.get() == p;
        }

        friend bool operator==(T const * p, copy_ptr const & x)
        {
            return p == x.get();
        }

    public:
        // Типы
        /// @brief Тип указываемого объекта
        typedef T element_type;

        /// @brief Тип ссылки
        typedef typename std::add_lvalue_reference<T>::type reference;

        /// @brief Тип стратегии копирования
        typedef typename default_helper<Cloner, default_copy<element_type>>::type
            cloner_type;

        /// @brief Тип стратегии удаления
        typedef typename default_helper<Deleter, std::default_delete<element_type>>::type
            deleter_type;

        static_assert(!std::is_rvalue_reference<deleter_type>::value,
                      "Deleter can't be rvalue reference");

        /// @brief Тип объекта, управляющего владением и удалением
        typedef std::unique_ptr<T, deleter_type> Holder;

        /// @brief Тип указателя
        typedef typename Holder::pointer pointer;

        /// @brief Тип стратегии проверок
        typedef typename default_helper<Checker, default_ptr_checker<pointer>>::type
            checker_type;

        // Конструкторы
        /** @brief Конструктор без параметров
        @post <tt> !*this == true </tt>
        */
        constexpr copy_ptr() noexcept = default;

        /** @brief Конструктор на основе указателя
        @param ptr указатель
        @post <tt> this->get() == ptr </tt>
        */
        template <class U>
        explicit copy_ptr(U * ptr) noexcept
         : holder_{std::move(ptr)}
        {
            static_assert(std::has_virtual_destructor<U>::value
                          || std::is_same<T, U>::value,
                          "Can't be safely destroed");
        }

        /** @brief Конструктор на основе <tt> std::unique_ptr </tt>
        @param p умный указатель
        @post <tt> this->get() </tt> будет иметь то же значение, что @c p имел
        до начала выполнения конструктора.
        */
        template <class U>
        explicit copy_ptr(std::unique_ptr<U> && p)
         : holder_{std::move(p)}
        {}

        /** @brief Конструктор копий
        @param x копируемый объект
        @post <tt> !*this == !x </tt>
        @post Если <tt> x.get() != nullptr </tt>, то <tt> **this == *x </tt>
        */
        copy_ptr(copy_ptr const & x)
         : holder_{x.make_copy().release(), x.get_deleter()}
        {}

        /// @brief Конструктор с перемещением
        copy_ptr(copy_ptr &&) = default;

        /** @brief Копирование умного указателя совместимого типа
        @param x копируемый умный указатель
        @post <tt> **this == *x </tt>
        */
        template <class T1, class C1, class D1, class Ch1>
        explicit copy_ptr(copy_ptr<T1, C1, D1, Ch1> const & x)
         : holder_{x.make_copy().release(), x.get_deleter()}
        {}

        /** @brief Конструктор перемещения умного указателя совместимого типа
        @param x перемещаемый умный указатель
        */
        template <class T1, class C1, class D1, class Ch1>
        explicit copy_ptr(copy_ptr<T1, C1, D1, Ch1> && x)
         : holder_(x.release(), std::move(x.get_deleter()))
        {}

        /** @brief Конструктор на основе @c nullptr_t
        @post <tt> !*this == true </tt>
        */
        constexpr copy_ptr(std::nullptr_t) noexcept
         : copy_ptr{}
        {}

        /** @brief Инициализация заданным указателем и функцией удаления
        @param p указатель на объект, которым будет владеть данный умный
        указатель
        @param d функция удаления
        @post <tt> this->get() == p </tt>
        */
        copy_ptr(pointer p, typename std::add_lvalue_reference<deleter_type const>::type & d)
         : holder_{std::move(p), d}
        {}

        /** @brief Инициализация заданным указателем и функцией удаления
        @param p указатель на объект, которым будет владеть данный умный
        указатель
        @param d функция удаления
        @post <tt> this->get() == p </tt>
        */
        copy_ptr(pointer p, typename std::remove_reference<deleter_type>::type && d)
         : holder_{std::move(p), std::move(d)}
        {
            static_assert(!std::is_reference<deleter_type>::value,
                          "Can't move, if deleter is lvalue reference");
        }

        // Присваивание
        /** @brief Оператор копирующего присваивания
        @param x копируемый объект
        @return *this
        */
        copy_ptr & operator=(copy_ptr const & x)
        {
            return ::ural::copy_and_swap(*this, x);
        }

        /** @brief Присваивание умного указателя совместимого типа
        @param x копируемый умный указатель
        @return <tt> *this </tt>
        @post <tt> **this == *x </tt>
        */
        template <class T1, class C1, class D1, class Ch1>
        copy_ptr & operator=(copy_ptr<T1, C1, D1, Ch1> const & x)
        {
            copy_ptr(x).swap(*this);
            return *this;
        }

        /** @brief Оператор присваивания с перемещением
        @return <tt> *this </tt>
        */
        copy_ptr & operator=(copy_ptr &&) = default;

        /** @brief Присваивание с перемещением на основе умного указателя
        совместимого типа
        @param x объект, содержимое которого должно быть перемещено
        @return <tt> *this </tt>
        */
        template <class T1, class C1, class D1, class Ch1>
        copy_ptr & operator=(copy_ptr<T1, C1, D1, Ch1> && x)
        {
            copy_ptr(std::move(x)).swap(*this);
            return *this;
        }

        /** @brief Присваивание с перемещением из @c unique_ptr
        @param p умный указатель
        @return <tt> *this </tt>
        */
        template <class U>
        copy_ptr & operator=(std::unique_ptr<U> && p)
        {
            holder_ = std::move(p);
            return *this;
        }

        // Свойства
        /** @brief Доступ к стратегии удаления
        @return Константная ссылка на стратегию удаления
        */
        deleter_type const & get_deleter() const
        {
            return holder_.get_deleter();
        }

        /** @brief Явное преобразование в @c bool
        @return @b true, если указатель владеет объектом, иначе --- @c false.
        */
        explicit operator bool() const
        {
            return static_cast<bool>(holder_);
        }

        /** @brief Указатель на указываемый объект
        @return Указатель на указываемый объект
        */
        pointer get() const
        {
            return holder_.get();
        }

        /** @brief Ссылка на указываемый объект
        @pre <tt> !*this == false </tt>
        @return Ссылка на указываемый объект
        */
        reference operator*() const
        {
            checker_type::assert_not_null(this->get());
            return *holder_;
        }

        /** @brief Доступ к членам указывемого объекта
        @pre <tt> !*this == false </tt>
        */
        pointer operator->() const
        {
            checker_type::assert_not_null(this->get());
            return holder_.operator->();
        }

        /** @brief Создание копии указываемого объекта
        @return Умный указатель с эксклюзивным владением, владеющий копией
        указываемого объекта
        */
        typename cloner_type::owner_type
        make_copy() const
        {
            if(!*this)
            {
                return typename cloner_type::owner_type{};
            }
            else
            {
                auto p = cloner_type::make_copy(**this);

                assert(typeid(*p) == typeid(**this));

                return p;
            }
        }

        // Модификаторы
        /** @brief Отказ от владения указываемым объектом
        @return @c unique_ptr, хранящий адрес указываемого объекта
        @post <tt> !*this == true </tt>
        */
        pointer release()
        {
            return holder_.release();
        }

        /** @brief Обмен содержимым с другим объектом
        @param x объект, с которым производится обмен
        */
        void swap(copy_ptr & x) noexcept
        {
            holder_.swap(x.holder_);
        }

    private:
        Holder holder_;
    };

    /** @brief Оператор "равно"
    @param x левый операнд
    @param y правый операнд
    @return <tt> x.get() == y.get() </tt>
    */
    template <class T1, class C1, class D1, class Ch1,
              class T2, class C2, class D2, class Ch2>
    bool operator==(copy_ptr<T1, C1, D1, Ch1> const & x,
                    copy_ptr<T2, C2, D2, Ch2> const & y)
    {
        return static_cast<void const volatile*>(x.get())
                == static_cast<void const volatile*>(y.get());
    }

    /** @brief Обмен значений двух @c copy_ptr
    @param x первый аргумент
    @param y второй аргумент
    */
    template <class T, class C, class D, class Ch>
    void swap(copy_ptr<T, C, D, Ch> & x,
              copy_ptr<T, C, D, Ch> & y) noexcept
    {
        return x.swap(y);
    }

    /** @brief Создание объекта в динамической памяти, обёрнутого в @c copy_ptr
    @tparam T тип создаваемого объекта
    @param args аргументы
    @return <tt> copy_ptr<T>(make_unique<T>(std::forward<Args>(args)...)) </tt>
    */
    template <class T, class... Args>
    copy_ptr<T>
    make_copy_ptr(Args &&... args)
    {
        return copy_ptr<T>(make_unique<T>(std::forward<Args>(args)...));
    }
}
// namespace ural

#endif
// Z_URAL_MEMORY_HPP_INCLUDED
