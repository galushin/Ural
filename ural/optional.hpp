#ifndef Z_URAL_OPTIONAL_HPP_INCLUDED
#define Z_URAL_OPTIONAL_HPP_INCLUDED

#include <stdexcept>
#include <type_traits>
#include <ostream>

// @todo Согласованные операторы отношений

namespace ural
{
    class nullopt_t{};
    class in_place_t{};

    constexpr nullopt_t nullopt{};
    constexpr in_place_t inplace{};

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
        static_assert(std::is_trivially_destructible<T>::value,
                      "value is not trivially destructible");

        explicit constexpr optional_base_constexpr()
         : has_value_(false)
         , dummy_{}
        {}

        explicit constexpr optional_base_constexpr(T value)
         : has_value_(true)
         , value_(std::move(value))
        {}

        template <class... Args>
        explicit constexpr optional_base_constexpr(in_place_t, Args && ... args)
         : has_value_{true}, value_(std::forward<Args>(args)...)
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
        bool has_value_;

        struct dummy_type {};

        union
        {
            T value_;
            dummy_type dummy_;
        };
    };

    // @todo Должен отличаться от optional_base_constexpr только отсутствием
    // constexpr
    template <class T>
    class optional_base
    {
    public:
        explicit optional_base()
         : has_value_{false}
         , dummy_{}
        {}

        explicit optional_base(T value)
         : has_value_{true}
         , value_(std::move(value))
        {}

        template <class... Args>
        explicit optional_base(in_place_t, Args && ... args)
         : has_value_{true}
         , value_(std::forward<Args>(args)...)
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

        void swap(optional_base & other);

    private:
        bool has_value_;

        struct dummy_type {};

        union
        {
            T value_;
            dummy_type dummy_;
        };
    };
}
// namespace details

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

        constexpr optional(T const & value)
         : impl_{value}
        {}

        constexpr optional(T && value)
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
                impl_.emplace(x.value_unsafe());
            }
        }

        optional(optional && x) noexcept(std::is_nothrow_move_constructible<T>::value)
        : impl_{}
        {
            if(!x)
            {}
            else
            {
                impl_.emplace(std::move(x.value_unsafe()));
            }
        }

        optional & operator=(nullopt_t) noexcept
        {
            impl_ = nullopt;
            return *this;
        }

        template <class U>
        optional & operator=(U && value)
        {
            impl_ = std::forward<U>(value);
            return *this;
        }

        optional & operator=(optional & x)
        {
            return *this = static_cast<optional const&>(x);
        }

        optional & operator=(optional const & x)
        {
            if(!x)
            {
                return *this = nullopt;
            }
            else
            {
                return *this = x.value_unsafe();
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
                return *this = std::move(x.value_unsafe());
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
            return !!*this ? this->value_unsafe()
                             : throw bad_optional_access{"optional::value"}, this->value_unsafe();
        }

        T& value()
        {
            if(!*this)
            {
                throw bad_optional_access{"optional::value"};
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
        void emplace(Args&&... args)
        {
            return impl_.emplace(std::forward<Args>(args)...);
        }

        template <class U, class... Args>
        void emplace(std::initializer_list<U> ilist, Args &&... args)
        {
            return impl_.emplace(ilist, std::forward<Args>(args)...);
        }

        // @todo задать nothrow
        void swap(optional & x)
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

        constexpr optional(in_place_t, T & x)
         : ptr_(details::constexpr_addressof(x))
        {}

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
            return this->get_pointer();
        }

        constexpr T * get_pointer() const
        {
            return ptr_;
        }

        constexpr T& value() const
        {
            return *this ? this->value_unsafe() :
                            throw bad_optional_access{"optional::value"},
                            this->value_unsafe();
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
        return !x ? false : x.value_unsafe() > a;
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
        return !x ? false : a <= x.value_unsafe();
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
                              optional<T> const & x)
    {
        os << "{";
        if(x)
        {
            os << x.value_unsafe();
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
            return !x ? result_type{}
                      : static_cast<Base const&>(*this)(x.value_unsafe());
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
            return !x ? result_type{}
                      : static_cast<Base const&>(*this)(x.value_unsafe());
        }
    };
}
// namespace std

#endif
// Z_URAL_OPTIONAL_HPP_INCLUDED
