#ifndef Z_URAL_OPTIONAL_HPP_INCLUDED
#define Z_URAL_OPTIONAL_HPP_INCLUDED

#include <stdexcept>
#include <type_traits>
#include <ostream>

namespace ural
{
    class nullopt_t{};
    class in_place_t{};

namespace details
{
    template <class T>
    inline constexpr T * constexpr_addressof(T & x)
    {
        return ((T*)&(char&)x);
    }

    template <class T>
    class optional_base_constexpr
    {
    public:
        explicit constexpr optional_base_constexpr()
         : has_value_(false), dummy_{}
        {}

        explicit constexpr optional_base_constexpr(T value)
         : has_value_(true), value_(std::move(value))
        {}

        template <class... Args>
        explicit constexpr optional_base_constexpr(in_place_t, Args && ... args)
         : has_value_{true}, value_(std::forward<Args>(args)...)
        {}

        constexpr optional_base_constexpr(optional_base_constexpr const & x);
        constexpr optional_base_constexpr(optional_base_constexpr && x);

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

    private:
        bool has_value_;

        struct dummy_type {};

        union
        {
            T value_;
            dummy_type dummy_;
        };
    };

    template <class T>
    class optional_base
    {
    public:
        explicit optional_base();
        explicit optional_base(T value);

        template <class... Args>
        explicit optional_base(in_place_t, Args && ... args);

        bool operator!() const;

        T const & value_unsafe() const;
        T & value_unsafe();

    private:
    };
}
// namespace details

    class bad_optional_access
     : public std::logic_error
    {
        typedef std::logic_error Base;

    public:
        bad_optional_access()
         : Base{"Access to the value of optional objects without value"}
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

        constexpr optional(T const & value)
         : impl_{value}
        {}

        constexpr optional(T && value)
         : impl_{std::move(value)}
        {}

        constexpr optional(optional const & x);

        constexpr optional(optional && x) noexcept(std::is_nothrow_move_constructible<T>::value);

        template <class... Args>
        constexpr explicit optional(in_place_t tag, Args &&... args)
         : impl_(tag, std::forward<Args>(args)...)
        {}

        template <class U, class... Args>
        constexpr explicit optional(in_place_t, std::initializer_list<U> ilist,
                                    Args&&... args );

        optional & operator=(optional const & x);

        optional &
        operator=(optional && x) noexcept(std::is_nothrow_move_constructible<T>::value
                                          && std::is_nothrow_move_assignable<T>::value);

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
                          : ural::details::constexpr_addressof(this->value_unsafe());
        }

        value_type * get_pointer()
        {
            return !*this ? nullptr : std::addressof(this->value_unsafe());
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
            // @todo Проверка через стратегию?
            return impl_.value_unsafe();
        }

        value_type & operator*()
        {
            // @todo Проверка через стратегию?
            return impl_.value_unsafe();
        }

        constexpr const T & value() const
        {
            return !*this ? throw bad_optional_access{} : this->value_unsafe();
        }

        T& value()
        {
            if(!*this)
            {
                throw bad_optional_access{};
            }
            else
            {
                return this->value_unsafe();
            }
        }

        template <class U>
        constexpr T value_or(U && value) const&
        {
            return !*this ? T{std::forward<U>(value)} : this->value_unsafe();
        }

        template <class U>
        T value_or(U && value) &&;

        T & value_unsafe()
        {
            return impl_.value_unsafe();
        }

        constexpr T const & value_unsafe() const
        {
            return impl_.value_unsafe();
        }

        template <class... Args>
        void emplace(Args&&... args);

        template <class U, class... Args>
        void emplace(std::initializer_list<U> ilist, Args &&... args);

        void swap(optional & x);

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
        typedef T & value_type;

        constexpr optional()
         : ptr_{nullptr}
        {}

        constexpr optional(nullopt_t)
         : ptr_{nullptr}
        {}

        constexpr optional(T & x)
         : ptr_(details::constexpr_addressof(x))
        {}

        constexpr optional(in_place_t, T & x);

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
            // @todo Проверка
            return this->value_unsafe();
        }

        constexpr T * operator->() const
        {
            return ptr_;
        }

        constexpr T * get_pointer() const;

        constexpr T& value() const
        {
            // @todo Проверка через стратегию
            return this->value_unsafe();
        }

        constexpr T& value_unsafe() const
        {
            return *ptr_;
        }

        constexpr T & value_or(T & other) const
        {
            return !*this ? other : this->value_unsafe();
        }

        void emplace(T & x)
        {
            ptr_ = std::addressof(x);
        }

        void swap(optional & x);

    private:
        T * ptr_;
    };

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

    constexpr nullopt_t nullopt{};
    constexpr in_place_t inplace{};

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
        return !x ? false : a == x.value_unsafe();
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
        return !x ? nullopt == y : x.value_unsafe() == y;
    }

    template <class T1, class T2>
    constexpr bool operator<(optional<T1> const & x, T2 const & a)
    {
        return !x ? true : x.value_unsafe() < a;
    }

    template <class T1, class T2>
    constexpr bool operator<(T1 const & a, optional<T2> const & x)
    {
        return !x ? false : a < x.value_unsafe();
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
        return !x ? nullopt < y : x.value_unsafe() < y;
    }

    template <class T1, class T2>
    constexpr bool operator!=(optional<T1> const & x, T2 const & a)
    {
        return !x ? true : x.value_unsafe() != a;
    }

    template <class T1, class T2>
    constexpr bool operator!=(T1 const & a, optional<T2> const & x)
    {
        return !x ? true : a != x.value_unsafe();
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
        return !x ? true : x.value_unsafe() > a;
    }

    template <class T1, class T2>
    constexpr bool operator>(T1 const & a, optional<T2> const & x)
    {
        return !x ? true : a > x.value_unsafe();
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
        return !x ? nullopt > y : x.value_unsafe() > y;
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
        return !x ? false : a < x.value_unsafe();
    }

    template <class T>
    constexpr bool operator<=(optional<T> const & x, T const & a)
    {
        return !x ? true : x.value_unsafe() <= a;
    }

    template <class T>
    constexpr bool
    operator<=(optional<T> const & x, optional<T> const & y)
    {
        return !x ? nullopt <= y : x.value_unsafe() <= y;
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
         // nullopt --- наименьшее: меньше любого значения, равно самому себе
        return !x ? false : x.value_unsafe() >= a;
    }

    template <class T>
    constexpr bool operator>=(T const & a, optional<T> const & x)
    {
        return !x ? true : a >= x.value_unsafe();
    }

    template <class T>
    constexpr bool
    operator>=(optional<T> const & x, optional<T> const & y)
    {
        return !x ? nullopt >= y : x.value_unsafe() >= y;
    }

    template <class Char, class Traits, class T>
    std::ostream & operator<<(std::basic_ostream<Char, Traits> & os,
                              optional<T> const & x);
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
            return !x ? result_type{}
                      : static_cast<Base const&>(*this)(x.value_unsafe());
        }
    };

    template <class T>
    class hash<ural::optional<T &>>
     : private std::hash<T*>
    {
        typedef std::hash<T*> Base;
    public:
        typedef typename Base::result_type result_type;

        constexpr result_type operator()(ural::optional<T &> const & x) const
        {
            return static_cast<Base const&>(*this)(x.get_pointer());
        }
    };
}
// namespace std

#endif
// Z_URAL_OPTIONAL_HPP_INCLUDED
