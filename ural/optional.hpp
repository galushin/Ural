#ifndef Z_URAL_OPTIONAL_HPP_INCLUDED
#define Z_URAL_OPTIONAL_HPP_INCLUDED

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

/** @file ural/optional.hpp
 @brief Необязательное значение --- обёртка для значения, которое может
 отсутствовать.
*/

#include <ural/defs.hpp>
#include <ural/type_traits.hpp>

#include <cassert>
#include <stdexcept>
#include <type_traits>
#include <ostream>

namespace ural
{
    class nullopt_t{};
    constexpr nullopt_t nullopt{};

    template <class T>
    class optional;

    template <class T>
    void swap(optional<T> & x, optional<T> & y)
    {
        return x.swap(y);
    }

/// @cond false
namespace details
{
    template <class T>
    inline constexpr T * constexpr_addressof(T & x)
    {
        return ((T*)&(char&)x);
    }

    using std::swap;

    template <class T>
    struct has_nothrow_swap
     : std::integral_constant<bool, noexcept(swap(std::declval<T&>(), std::declval<T&>()))>
    {};

    template <class T>
    class optional_base_constexpr
    {
    public:
        static_assert(std::is_trivially_destructible<T>::value,
                      "value is not trivially destructible");

        explicit constexpr optional_base_constexpr()
         : dummy_{}
         , has_value_{false}
        {}

        explicit constexpr optional_base_constexpr(T value)
         : value_(std::move(value))
         , has_value_(true)
        {}

        template <class... Args>
        explicit constexpr optional_base_constexpr(in_place_t, Args && ... args)
         : value_(std::forward<Args>(args)...)
         , has_value_{true}
        {}

        optional_base_constexpr & operator=(nullopt_t)
        {
            if(this->has_value_)
            {
                has_value_ = false;
                // Вызывать деструктор значения не нужно
            }

            return *this;
        }

        template <class U>
        optional_base_constexpr & operator=(U && value)
        {
            if(this->has_value_)
            {
                this->value_ = std::forward<U>(value);
            }
            else
            {
                this->emplace(std::forward<U>(value));
            }

            return *this;
        }

        constexpr bool operator!() const
        {
            return !this->has_value_;
        }

        constexpr T const & value_unsafe() const
        {
            return this->value_;
        }

        T & value_unsafe()
        {
            return this->value_;
        }

        template <class... Args>
        void emplace(Args && ... args)
        {
            static_assert(std::is_trivially_destructible<dummy_type>::value,
                          "dummy is not trivially destructible");

            // Деструктор вызывать не нужно

            new(std::addressof(value_))T(std::forward<Args>(args)...);
            has_value_ = true;
        }

        void swap(optional_base_constexpr & that)
        {
            if (!*this)
            {
                if (!that)
                {
                    // Оба пустые - ничего делать не нужно
                }
                else
                {
                    this->emplace(std::move(that.value_));
                }
            }
            else
            {
                if(!that)
                {
                    that.emplace(std::move(this->value_));
                }
                else
                {
                    using std::swap;
                    swap(this->value_, that.value_);
                }
            }
        }

    private:
        struct dummy_type {};

        union
        {
            T value_;
            dummy_type dummy_;
        };

        bool has_value_;
    };

    template <class T>
    class optional_base
    {
    public:
        explicit optional_base()
         : dummy_{}
         , has_value_{false}
        {}

        explicit optional_base(T value)
         : value_(std::move(value))
         , has_value_{true}
        {}

        template <class... Args>
        explicit optional_base(in_place_t, Args && ... args)
         : value_(std::forward<Args>(args)...)
         , has_value_{true}
        {}

        ~optional_base()
        {
            if(has_value_)
            {
                value_.~T();
            }
        }

        optional_base & operator=(nullopt_t)
        {
            if (this->has_value_)
            {
                value_.~T();
                has_value_ = false;
            }
            return *this;
        }

        template <class U>
        optional_base & operator=(U && value)
        {
            if(this->has_value_)
            {
                value_ = std::forward<U>(value);
            }
            else
            {
                new(std::addressof(dummy_))T{std::forward<U>(value)};
                has_value_ = true;
            }
            return *this;
        }

        bool operator!() const
        {
            return !this->has_value_;
        }

        T const & value_unsafe() const
        {
            return this->value_;
        }

        T & value_unsafe()
        {
            return this->value_;
        }

        template <class... Args>
        void emplace(Args && ... args)
        {
            static_assert(std::is_trivially_destructible<dummy_type>::value,
                          "dummy is not trivially destructible");

            if(this->has_value_)
            {
                value_.~T();
                has_value_ = false;
            }

            new(std::addressof(dummy_))T(std::forward<Args>(args)...);
            has_value_ = true;
        }

        void swap(optional_base & that)
        {
            if(!*this)
            {
                if(!that)
                {}
                else
                {
                    this->emplace(std::move(that.value_));
                }
            }
            else
            {
                if (!that)
                {
                    that.emplace(std::move(this->value_));
                }
                else
                {
                    using std::swap;
                    using ural::swap;
                    swap(this->value_, that.value_);
                }
            }
        }

    private:
        struct dummy_type {};

        union
        {
            T value_;
            dummy_type dummy_;
        };

        bool has_value_;
    };
}
// namespace details
/// @endcond

    /// @brief Тип исключения "доступ к значению пустого @c optional"
    class bad_optional_access
     : public std::logic_error
    {
        typedef std::logic_error Base;

    public:
        bad_optional_access(std::string const & what_arg)
         : Base(what_arg)
        {}

        bad_optional_access(char const * what_arg)
         : Base(what_arg)
        {}

    private:
    };

    template <class T>
    class optional
    {
    public:
        /// @brief Тип значения
        typedef T value_type;

        //@{
        /** @brief Конструктор объекта без значения
        @post <tt> !*this == true </tt>
        */
        constexpr optional() noexcept
         : impl_{}
        {}

        constexpr optional(nullopt_t)
         : impl_{}
        {}
        //@}

        /** @brief Создание на основе значения
        @param value значение
        @post <tt> !*this == false </tt>
        @post <tt> this->value() == value </tt>
        */
        constexpr optional(T const & value)
         : impl_{value}
        {}

        /** @brief Создание на основе временного значения
        @param value значение
        @post <tt> !*this == false </tt>
        @post <tt> this->value() == value_old </tt>, где @c value_old ---
        значение, которое переменная @c value имела до начала выполнения
        конструктора.
        */
        constexpr optional(T && value)
         : impl_{std::move(value)}
        {}

        /** @brief Создаёт значение с помощью конструктора с аргументами @c args
        @param args аргументы
        @post <tt> !*this == false </tt>
        @post <tt> this->value() == T(std::forward<Args>(args)...) </tt>
        */
        template <class... Args>
        constexpr explicit optional(in_place_t, Args &&... args)
         : impl_(in_place_t{}, std::forward<Args>(args)...)
        {}

        // @todo Запретить, если из таких аргументов нельзя создать T
        /** @brief Создание значения "на месте"
        @param tag тэг
        @param ilist список инициализаторов
        @param args прочие аргументы
        @post <tt> this->value() == T(ilist, std::forward<Args>(args)...) </tt>
        */
        template <class U, class... Args>
        constexpr explicit optional(in_place_t tag,
                                    std::initializer_list<U> ilist,
                                    Args&&... args )
         : impl_{tag, ilist, std::forward<Args>(args)...}
        {}

        /** @brief Конструктор копий
        @param x копируемый объект
        @post <tt> *this == x </tt>
        */
        optional(optional const & x)
         : impl_{}
        {
            if(!x)
            {}
            else
            {
                impl_.emplace(*x);
            }
        }

        /** @brief Конструктор перемещения
        @param x объект, содержимое которого должно быть перемещено
        */
        optional(optional && x) noexcept(std::is_nothrow_move_constructible<T>::value)
        : impl_{}
        {
            if(!x)
            {}
            else
            {
                impl_.emplace(std::move(*x));
            }
        }

        /** @brief Присвание объекта, означающего отсутствие значения
        @post <tt> !*this == true </tt>
        @return <tt> *this </tt>
        */
        optional & operator=(nullopt_t) noexcept
        {
            impl_ = nullopt;
            return *this;
        }

        /** @brief Присваивание совместимого значения
        @param value значение
        @pre Объект типа @c T можно создать с помощью конструктора, принимающего
        выражение <tt> std::forward<U>(value) </tt>
        @pre Неконстантному объекту типа @c T можно присвоить значение
        <tt> std::forward<U>(value) </tt>
        @return <tt> *this </tt>
        @post <tt> this->empty() == false </tt>
        @post <tt> this->value() </tt> равно значению, которое @c value, имело
        до начала выполнения присваивания
        */
        template <class U>
        typename std::enable_if<std::is_constructible<T, U>::value && is_assignable<T&, U>::value, optional &>::type
        operator=(U && value)
        {
            impl_ = std::forward<U>(value);
            return *this;
        }

        /** @brief Оператор присваивания
        @param x аргумент
        @return <tt> *this </tt>
        @post <tt> *this == x </tt>
        */
        optional & operator=(optional const & x)
        {
            if(!x)
            {
                return *this = nullopt;
            }
            else
            {
                return *this = *x;
            }
        }

        optional &
        operator=(optional && x) noexcept(std::is_nothrow_move_constructible<T>::value
                                          && std::is_nothrow_move_assignable<T>::value)
        {
            if(!x)
            {
                return *this = nullopt;
            }
            else
            {
                return *this = std::move(*x);
            }
        }

        /** @brief Проверка отсутствия значения
        @return <tt> !bool(*this) </tt>
        */
        constexpr bool operator!() const
        {
            return !impl_;
        }

        /** @brief Явное преобразование в @c bool
        @return Если данный объект содержит значение --- @b true, иначе ---
        @b false.
        */
        constexpr explicit operator bool() const
        {
            return !!*this;
        }

        //@{
        /** @brief Доступ к указателю на значение
        @return @c nullptr, если объект не содержит значение, иначе ---
        <tt> &**this </tt>.
        */
        constexpr const value_type * get_pointer() const
        {
            return !*this ? nullptr
                          : ural::details::constexpr_addressof(**this);
        }

        value_type * get_pointer()
        {
            return !*this ? nullptr : std::addressof(**this);
        }

        constexpr const value_type * operator->() const
        {
            return this->get_pointer();
        }

        value_type * operator->()
        {
            return this->get_pointer();
        }
        //@}

        //@{
        /** @brief Доступ к ссылке на значение
        @pre <tt> !*this == false </tt>
        @return <tt> this->value() </tt>
        */
        constexpr value_type const & operator*() const
        {
            return impl_.value_unsafe();
        }

        value_type & operator*()
        {
            assert(bool(*this) != false);
            return impl_.value_unsafe();
        }
        //@}

        //@{
        constexpr const T & value() const
        {
            return !!*this ? **this
                             : throw bad_optional_access{"optional::value"}, **this;
        }

        T& value()
        {
            if(!*this)
            {
                throw bad_optional_access{"optional::value"};
            }
            else
            {
                return **this;
            }
        }
        //@}

        //@{
        template <class U>
        constexpr T value_or(U && value) const &
        {
            return !*this ? T{std::forward<U>(value)} : *(*this);
        }

        template <class U>
        T value_or(U && value) &&
        {
            return !*this ? T{std::forward<U>(value)} : std::move(**this);
        }
        //@}

        template <class... Args>
        void emplace(Args&&... args)
        {
            return impl_.emplace(std::forward<Args>(args)...);
        }

        template <class U, class... Args>
        void emplace(std::initializer_list<U> ilist, Args &&... args)
        {
            return impl_.emplace(ilist, std::forward<Args>(args)...);
        }

        /** Обменивает содержимое @c x и <tt> *this </tt>
        @brief Обмен
        @param x объект, с которым будет производится обмен
        */
        void swap(optional & x) noexcept(std::is_nothrow_move_constructible<T>::value
                                         && details::has_nothrow_swap<T>::value)
        {
            impl_.swap(x.impl_);
        }

    private:
        typedef typename std::conditional<std::is_trivially_destructible<T>::value,
                                          details::optional_base_constexpr<T>,
                                          details::optional_base<T>>::type Impl;
        Impl impl_;
    };

    /** @brief Специализация для ссылок
    @tparam T тип объекта, на который приозводится ссылка
    Используем тот факт, что ссылка с необязательным значением эквивалентна
    указателю
    @todo Запретить привязки временных объектов
    */
    template <class T>
    class optional<T&>
    {
    public:
        // Типы
        /// @brief Тип значения
        typedef T & value_type;

        // Конструкторы
        //@{
        /** @brief Конструктор объекта без значения
        @post <tt> !*this == true </tt>
        */
        constexpr optional()
         : ptr_{nullptr}
        {}

        constexpr optional(nullopt_t)
         : ptr_{nullptr}
        {}
        //@}

        /// @brief Конструктор копий
        optional(optional const & ) = default;

        /// @brief Конструктор перемещения
        optional(optional && ) = default;

        //@{
        /** @brief Конструктор на основе ссылки
        @param x ссылка
        @post <tt> !*this == false </tt>
        @post <tt> addressof(x) == addressof(this->value()) </tt>
        */
        constexpr optional(T & x)
         : ptr_(details::constexpr_addressof(x))
        {}

        constexpr optional(in_place_t, T & x)
         : ptr_(details::constexpr_addressof(x))
        {}
        //@}

        // Присваивания
        /** @brief Оператор присваивания с @c nullopt_t в правой части
        @post <tt> !*this </tt>
        @return <tt> *this </tt>
        */
        optional & operator=(nullopt_t)
        {
            ptr_ = nullptr;
            return *this;
        }

        /** @brief Оператор копирующего присваивания
        @return <tt> *this </tt>
        */
        optional & operator=(optional const & ) = default;

        /** @brief Оператор присваивания с перемещением
        @return <tt> *this </tt>
        */
        optional & operator=(optional && ) = default;

        // Свойства
        //@{
        /** @brief Проверка отсутствия значения
        @return @b false, если значение отсутствует, иначе --- @b true.
        */
        constexpr bool operator!() const
        {
            return this->ptr_ == nullptr;
        }

        constexpr bool empty() const
        {
            return !*this;
        }
        //@}

        /** @brief Явное преобразование в @с bool -- проверка наличия значения
        @return <tt> !this->empty() </tt>
        */
        constexpr explicit operator bool() const
        {
            return !!*this;
        }

        /** @brief Доступ к текущему значению без проверки
        @pre <tt> !this->empty() </tt>
        @return <tt> *this->get_pointer() </tt>
        */
        constexpr T & operator*() const
        {
            return *this->get_pointer();
        }

        //@{
        /** @brief Указатель на хранимое значение
        @return Если <tt> this->empty() </tt>, то возвращает @c nullptr, иначе
        -- указатель, ссылающийся на тот же объект, что <tt> this->value() </tt>
        */
        constexpr T * operator->() const
        {
            return this->get_pointer();
        }

        constexpr T * get_pointer() const
        {
            return ptr_;
        }
        //@}

        /** @brief Доступ к текущему значению с проверкой
        @return <tt> *(*this) </tt>
        @throw bad_optional_access, если <tt> this->empty() </tt>
        */
        constexpr T & value() const
        {
            return *this ? **this:
                            throw bad_optional_access{"optional::value"}, **this;
        }

        /** @brief Доступ к текущему значению с "запасным" значением на случай,
        если в данном объекте значение отсутствует
        @param other "запасное" значение
        @return Если <tt> this->empty() </tt>, то возвращает @c other, иначе
        -- <tt> **this </tt>
        */
        constexpr T & value_or(T & other) const
        {
            return !*this ? other : **this;
        }

        // Модифицирующие операциии
        /** @brief Размещение новой ссылки
        @param x новая ссылка
        @post <tt> addressof(x) == this->get_pointer() </tt>
        */
        void emplace(T & x)
        {
            ptr_ = std::addressof(x);
        }

        /** Обменивает содержимое @c that и <tt> *this </tt>
        @brief Обмен
        @param that объект, с которым будет производится обмен
        */
        void swap(optional & that) noexcept
        {
            using std::swap;
            using ural::swap;
            swap(this->ptr_, that.ptr_);
        }

    private:
        T * ptr_;
    };

    /** @brief Создание объекта с необязательным значением
    @param value значение
    @return <tt> optional<typename std::decay<T>::type>{std::forward<T>(value)} </tt>
    */
    template <class T>
    constexpr optional<typename std::decay<T>::type>
    make_optional(T && value)
    {
        return optional<typename std::decay<T>::type>{std::forward<T>(value)};
    }

    /** @brief Создание ссылки с необязательным значением
    @param value обёртка для ссылки
    @return <tt> optional<T &>(value.get()) </tt>
    */
    template <class T>
    optional<T &>
    make_optional(std::reference_wrapper<T> value)
    {
        return optional<T &>(value.get());
    }

    //@{
    /** @brief Проверка объекта типа @c optional на равенство с объектом,
    обозначающим отсутствующее значение
    @param x объект с необязательным значением
    @return <tt> !x </tt>
    */
    template <class T>
    constexpr bool operator==(nullopt_t, optional<T> const & x)
    {
        return !x;
    }

    template <class T>
    constexpr bool operator==(optional<T> const & x, nullopt_t)
    {
        return nullopt == x;
    }
    //@}

    //@{
    /** @brief Проверка объекта типа @c optional на равенство со значением
    соответствующего типа
    @param x объект с необязательным значением
    @param a значение
    @return Если @c x не содержит значения, то @b false, иначе --
    <tt> a == *x </tt>
    */
    template <class T1, class T2>
    constexpr bool operator==(optional<T1> const & x, T2 const & a)
    {
        return !x ? false : a == *x;
    }

    template <class T1, class T2>
    constexpr bool operator==(T1 const & a, optional<T2> const & x)
    {
        return x == a;
    }
    //@}

    template <class T>
    constexpr bool
    operator==(optional<T> const & x, optional<T> const & y)
    {
        return !x ? nullopt == y : *x == y;
    }

    template <class T1, class T2>
    constexpr bool operator<(optional<T1> const & x, T2 const & a)
    {
        return !x ? true : *x < a;
    }

    template <class T1, class T2>
    constexpr bool operator<(T1 const & a, optional<T2> const & x)
    {
        return !x ? false : a < *x;
    }

    template <class T>
    constexpr bool operator<(optional<T> const &, nullopt_t)
    {
        // nullopt --- наименьшее: меньше любого значения, равно самому себе
        return false;
    }

    template <class T>
    constexpr bool operator<(nullopt_t, optional<T> const & x)
    {
        // nullopt --- наименьшее: меньше любого значения, равно самому себе
        return static_cast<bool>(x);
    }

    template <class T>
    constexpr bool operator<(optional<T> const & x, optional<T> const & y)
    {
        return !x ? nullopt < y : *x < y;
    }

    template <class T1, class T2>
    constexpr bool operator!=(optional<T1> const & x, T2 const & a)
    {
        return !(x == a);
    }

    template <class T1, class T2>
    constexpr bool operator!=(T1 const & a, optional<T2> const & x)
    {
        return !(a == x);
    }

    template <class T>
    constexpr bool operator!=(optional<T> const & x, nullopt_t)
    {
        return !(x == nullopt);
    }

    template <class T>
    constexpr bool operator!=(nullopt_t, optional<T> const & x)
    {
        return !(x == nullopt);
    }

    template <class T>
    constexpr bool
    operator!=(optional<T> const & x, optional<T> const & y)
    {
        return !(x == y);
    }

    template <class T1, class T2>
    constexpr bool operator>(optional<T1> const & x, T2 const & a)
    {
        return !x ? false : *x > a;
    }

    template <class T1, class T2>
    constexpr bool operator>(T1 const & a, optional<T2> const & x)
    {
        return !x ? true : a > *x;
    }

    template <class T>
    constexpr bool operator>(optional<T> const & x, nullopt_t)
    {
        return static_cast<bool>(x);
    }

    template <class T>
    constexpr bool operator>(nullopt_t, optional<T> const &)
    {
        // nullopt --- наименьшее: меньше любого значения, равно самому себе
        return false;
    }

    template <class T>
    constexpr bool
    operator>(optional<T> const & x, optional<T> const & y)
    {
        return y < x;
    }

    template <class T>
    constexpr bool operator<=(nullopt_t, optional<T> const &)
    {
        // nullopt --- наименьшее: меньше любого значения, равно самому себе
        return true;
    }

    template <class T>
    constexpr bool operator<=(optional<T> const & x, nullopt_t)
    {
        return !x;
    }

    template <class T1, class T2>
    constexpr bool operator<=(T1 const & a, optional<T2> const & x)
    {
        return !(x < a);
    }

    template <class T>
    constexpr bool operator<=(optional<T> const & x, T const & a)
    {
        return !(a < x);
    }

    template <class T>
    constexpr bool
    operator<=(optional<T> const & x, optional<T> const & y)
    {
        return !(y < x);
    }

    template <class T>
    constexpr bool operator>=(optional<T> const &, nullopt_t)
    {
        return true;
    }

    template <class T>
    constexpr bool operator>=(nullopt_t, optional<T> const & x)
    {
        // nullopt --- наименьшее: меньше любого значения, равно самому себе
        return !x;
    }

    template <class T>
    constexpr bool operator>=(optional<T> const & x, T const & a)
    {
        return !(x < a);
    }

    template <class T>
    constexpr bool operator>=(T const & a, optional<T> const & x)
    {
        return !(a < x);
    }

    template <class T>
    constexpr bool
    operator>=(optional<T> const & x, optional<T> const & y)
    {
        return !x ? nullopt >= y : *x >= y;
    }

    template <class Char, class Traits, class T>
    std::ostream & operator<<(std::basic_ostream<Char, Traits> & os,
                              optional<T> const & x)
    {
        os << "{";
        if(x)
        {
            os << *x;
        }
        os << "}";
        return os;
    }
}
// namespace ural

namespace std
{
    template <class T>
    class hash<ural::optional<T>>
     : private std::hash<T>
    {
        typedef std::hash<T> Base;
    public:
        typedef typename Base::result_type result_type;

        constexpr result_type operator()(ural::optional<T> const & x) const
        {
            return !x ? result_type{} : static_cast<Base const&>(*this)(*x);
        }
    };

    template <class T>
    class hash<ural::optional<T &>>
     : private std::hash<T>
    {
        typedef std::hash<T> Base;
    public:
        typedef typename Base::result_type result_type;

        constexpr result_type operator()(ural::optional<T &> const & x) const
        {
            return !x ? result_type{} : static_cast<Base const&>(*this)(*x);
        }
    };
}
// namespace std

#endif
// Z_URAL_OPTIONAL_HPP_INCLUDED
