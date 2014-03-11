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
#include <ural/thread.hpp>
#include <memory>

namespace ural
{
    // Создание объектов в динамической памяти, обёрнутых в unique_ptr
    template <class T, class... Args>
    typename std::enable_if<!std::is_array<T>::value, std::unique_ptr<T>>::type
    make_unique(Args &&... args)
    {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }

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

    template <class T>
    std::unique_ptr<T> make_copy_new(T x)
    {
        return ural::make_unique<T>(std::move(x));
    }

    // Умный указатель с глубоким копированием
    template <class T>
    class default_copy
    {
    public:
        static std::unique_ptr<T> make_copy(T const & x)
        {
            return ural::make_unique<T>(std::move(x));
        }
    };

    template <class T, std::unique_ptr<T>(T::*clone_fn)() const = &T::clone>
    class member_function_copy
    {
    public:
        static std::unique_ptr<T> make_copy(T const & x)
        {
            return (x.*clone_fn)();
        }
    };

    template <class Pointer>
    class default_ptr_checker
    {
    public:
        static void assert_not_null(Pointer p)
        {
            assert(p != nullptr);
        }

    protected:
        ~default_ptr_checker() = default;
    };

    template <class Pointer>
    class throwing_ptr_checker
    {
    public:
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

    /**
    @todo Учесть N3339
    @todo Тесты с полиморфными типами (стратегии копирования и удаления)
    @todo По аналогии с 20.7.1
    @todo По аналогии с 20.7.2
    @todo Функция создания
    @todo Интеграция с unique_ptr и shared_ptr
    @todo Все функции должны быть noexcept?
    @todo Специализация для массивов
    @todo Защита от срезки: как на этапе компиляции (см. shared_ptr), так и во
    время выполнения программы. Проверка, что типы оригинала и копии совпадают
    @todo Произвольный порядок следования стратегий

    Обоснование.

    Конструктор копий не является noexcept, так как копирование указываемого
    объекта может приводить к исключениям.

    Сравнение @c copy_ptr с параметрами разных типов может давать истину только
    если оба они указывают на ноль.
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
        typedef T element_type;
        typedef typename std::add_lvalue_reference<T>::type reference;

        typedef typename default_helper<Cloner, default_copy<element_type>>::type
            cloner_type;
        typedef typename default_helper<Deleter, std::default_delete<element_type>>::type
            deleter_type;

        typedef std::unique_ptr<T, deleter_type> Holder;

        typedef typename Holder::pointer pointer;

        typedef typename default_helper<Checker, default_ptr_checker<pointer>>::type
            checker_type;

        // Конструкторы
        constexpr copy_ptr() noexcept = default;

        explicit copy_ptr(pointer ptr) noexcept
         : holder_{std::move(ptr)}
        {}

        template <class U>
        explicit copy_ptr(std::unique_ptr<U> && p)
         : holder_{std::move(p)}
        {}

        copy_ptr(copy_ptr const & x)
         : holder_{x.make_copy().release(), x.get_deleter()}
        {}

        copy_ptr(copy_ptr &&) = default;

        template <class T1, class C1, class D1, class Ch1>
        copy_ptr(copy_ptr<T1, C1, D1, Ch1> const & x)
         : holder_{x.make_copy().release(), x.get_deleter()}
        {}

        template <class T1, class C1, class D1, class Ch1>
        copy_ptr(copy_ptr<T1, C1, D1, Ch1> && x)
         : holder_{x.release(), std::move(x.get_deleter())}
        {}

        constexpr copy_ptr(nullptr_t) noexcept
         : copy_ptr{}
        {}

        // Присваивание
        copy_ptr & operator=(copy_ptr const & x)
        {
            *this = std::move(copy_ptr(x));

            return *this;
        }

        copy_ptr & operator=(copy_ptr &&) = default;

        // Свойства
        deleter_type const & get_deleter() const
        {
            return holder_.get_deleter();
        }

        explicit operator bool() const
        {
            return static_cast<bool>(holder_);
        }

        pointer get() const
        {
            return holder_.get();
        }

        reference operator*() const
        {
            checker_type::assert_not_null(this->get());
            return *holder_;
        }

        pointer operator->() const
        {
            checker_type::assert_not_null(this->get());
            return holder_.operator->();
        }

        std::unique_ptr<T> make_copy() const
        {
            return !*this ? std::unique_ptr<T>{} : cloner_type::make_copy(**this);
        }

        // Модификаторы
        pointer release()
        {
            return holder_.release();
        }

        void swap(copy_ptr & x) noexcept
        {
            holder_.swap(x.holder_);
        }

    private:
        Holder holder_;
    };

    template <class T1, class C1, class D1, class Ch1,
              class T2, class C2, class D2, class Ch2>
    bool operator==(copy_ptr<T1, C1, D1, Ch1> const & x,
                    copy_ptr<T2, C2, D2, Ch2> const & y)
    {
        return static_cast<void const volatile*>(x.get())
                == static_cast<void const volatile*>(y.get());
    }

    template <class T, class C, class D, class Ch>
    void swap(copy_ptr<T, C, D, Ch> & x,
              copy_ptr<T, C, D, Ch> & y) noexcept
    {
        return x.swap(y);
    }
}
// namespace ural

#endif
// Z_URAL_MEMORY_HPP_INCLUDED
