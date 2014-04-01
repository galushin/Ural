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

#include <ural/type_traits.hpp>

#include <stdexcept>
#include <type_traits>
#include <ostream>

namespace ural
{
    class nullopt_t{};
    class in_place_t{};

    constexpr nullopt_t nullopt{};
    constexpr in_place_t inplace{};

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
                    using boost::swap;
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
        typedef T value_type;

        constexpr optional()
         : impl_{}
        {}

        constexpr optional(nullopt_t)
         : impl_{}
        {}

        constexpr optional(T value)
         : impl_{std::move(value)}
        {}

        template <class... Args>
        constexpr explicit optional(in_place_t tag, Args &&... args)
         : impl_(tag, std::forward<Args>(args)...)
        {}

        template <class U, class... Args>
        constexpr explicit optional(in_place_t tag,
                                    std::initializer_list<U> ilist,
                                    Args&&... args )
         : impl_{tag, ilist, std::forward<Args>(args)...}
        {}

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

        optional & operator=(nullopt_t) noexcept
        {
            impl_ = nullopt;
            return *this;
        }

        template <class U>
        typename std::enable_if<std::is_constructible<T, U>::value && is_assignable<T&, U>::value, optional &>::type
        operator=(U && value)
        {
            impl_ = std::forward<U>(value);
            return *this;
        }

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

        constexpr bool operator!() const
        {
            return !impl_;
        }

        constexpr explicit operator bool() const
        {
            return !!*this;
        }

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

        constexpr value_type const & operator*() const
        {
            return impl_.value_unsafe();
        }

        value_type & operator*()
        {
            assert(bool(*this) != false);
            return impl_.value_unsafe();
        }

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

    template <class T>
    class optional<T&>
    {
    public:
        // Типы
        typedef T & value_type;

        // Конструкторы
        constexpr optional()
         : ptr_{nullptr}
        {}

        constexpr optional(nullopt_t)
         : ptr_{nullptr}
        {}

        optional(optional const & ) = default;
        optional(optional && ) = default;

        constexpr optional(T & x)
         : ptr_(details::constexpr_addressof(x))
        {}

        constexpr optional(in_place_t, T & x)
         : ptr_(details::constexpr_addressof(x))
        {}

        // Присваивания
        optional & operator=(nullopt_t)
        {
            ptr_ = nullptr;
            return *this;
        }

        optional & operator=(optional const & ) = default;
        optional & operator=(optional && ) = default;

        // Свойства
        constexpr bool operator!() const
        {
            return this->ptr_ == nullptr;
        }

        constexpr bool empty() const
        {
            return !*this;
        }

        constexpr explicit operator bool() const
        {
            return !!*this;
        }

        constexpr T & operator*() const
        {
            return *this->get_pointer();
        }

        constexpr T * operator->() const
        {
            return this->get_pointer();
        }

        constexpr T * get_pointer() const
        {
            return ptr_;
        }

        constexpr T & value() const
        {
            return *this ? **this:
                            throw bad_optional_access{"optional::value"}, **this;
        }

        constexpr T & value_or(T & other) const
        {
            return !*this ? other : **this;
        }

        // Модифицирующие операциии
        void emplace(T & x)
        {
            ptr_ = std::addressof(x);
        }

        void swap(optional & that)
        {
            using std::swap;
            swap(this->ptr_, that.ptr_);
        }

    private:
        T * ptr_;
    };

    template <class T>
    void swap(optional<T> & x, optional<T> & y);

    template <class T>
    constexpr optional<typename std::decay<T>::type>
    make_optional(T && value)
    {
        return optional<typename std::decay<T>::type>{std::forward<T>(value)};
    }

    template <class T>
    optional<T &>
    make_optional(std::reference_wrapper<T> value)
    {
        return optional<T &>(value.get());
    }

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
