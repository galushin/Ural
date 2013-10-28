#ifndef Z_URAL_OPTIONAL_HPP_INCLUDED
#define Z_URAL_OPTIONAL_HPP_INCLUDED

#include <stdexcept>
#include <type_traits>

namespace ural
{
namespace details
{
    template <class T>
    class optional_base_constexpr
    {};

    template <class T>
    class optional_base
    {};
}
// namespace details

    class nullopt_t{};
    class in_place_t{};

    class bad_optional_access
     : public std::logic_error
    {};

    template <class T>
    class optional
    {
    public:
        typedef T value_type;

        constexpr optional();
        constexpr optional(nullopt_t);

        constexpr optional(T const & value);

        constexpr optional(optional const & x);
        constexpr optional(optional && x) noexcept(std::is_nothrow_move_constructible<T>::value);

        template <class... Args>
        constexpr explicit optional(in_place_t, Args &&... args);

        template <class U, class... Args>
        constexpr explicit optional(in_place_t, std::initializer_list<U> ilist,
                                    Args&&... args );

        optional & operator=(optional const & x);

        optional &
        operator=(optional && x) noexcept(std::is_nothrow_move_constructible<T>::value
                                          && std::is_nothrow_move_assignable<T>::value);

        bool operator!() const;
        explicit operator bool() const;

        constexpr const value_type * operator->() const;
        value_type * operator->();

        constexpr value_type const & operator*() const;
        value_type & operator*();

        constexpr const T& value() const;
        T& value();

        template <class U>
        constexpr T value_or(U && value) const&;

        template <class U>
        T value_or(U && value) &&;

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

        optional();
        optional(nullopt_t);
        optional(T & x);
        optional(in_place_t, T & x);

        bool operator!() const;
        explicit operator bool() const;

        T & operator*() const;
        T * operator->() const;

        T& value() const;
        T & value_or(T & other) const;

        void emplace(T & x);

        void swap(optional & x);

    private:
    };

    template <class T>
    constexpr optional<typename std::decay<T>::type>
    make_optional(T && value);

    template <class T>
    optional<T &>
    make_optional(std::reference_wrapper<T> value);

    constexpr nullopt_t nullopt{};
    constexpr in_place_t inplace{};

    template <class T>
    bool operator==(ural::optional<T> const & x, const nullopt_t &);

    template <class T>
    bool operator==(ural::optional<T> const & x, T const & a);

    template <class T>
    bool operator==(ural::optional<T&> const & x, T const & a);

    template <class T>
    bool operator==(T const & a, ural::optional<T> const & x);

    template <class T>
    bool operator==(T const & a, ural::optional<T &> const & x);

    template <class T>
    bool operator!=(ural::optional<T> const & x, T const & a);

    template <class T>
    bool operator!=(ural::optional<T &> const & x, T const & a);

    template <class T>
    bool operator!=(T const & a, ural::optional<T> const & x);

    template <class T>
    bool operator!=(ural::optional<T> const & x, const nullopt_t &);

    template <class T>
    bool operator==(ural::optional<T> const & x, ural::optional<T> const & y);

    template <class T>
    bool operator!=(ural::optional<T> const & x, ural::optional<T> const & y);

    template <class T>
    bool operator<(ural::optional<T> const & x, ural::optional<T> const & y);

    template <class T>
    bool operator<(ural::optional<T> const & x, T const & y);

    template <class T>
    bool operator<(T const & a, ural::optional<T> const & x);

    template <class T>
    bool operator<(ural::optional<T> const & x, T const & a);

    template <class T>
    bool operator<(ural::optional<T &> const & x, T const & a);

    template <class T>
    bool operator>(ural::optional<T> const & x, ural::optional<T> const & y);

    template <class T>
    bool operator>(ural::optional<T> const & x, T const & a);

    template <class T>
    bool operator>(ural::optional<T &> const & x, T const & a);

    template <class T>
    bool operator>(T const & a, ural::optional<T> const & x);

    template <class T>
    bool operator>=(ural::optional<T> const & x, ural::optional<T> const & y);

    template <class T>
    bool operator>=(ural::optional<T> const & x, T const & a);

    template <class T>
    bool operator>=(T const & a, ural::optional<T> const & x);

    template <class T>
    bool operator<=(ural::optional<T> const & x, ural::optional<T> const & y);

    template <class T>
    bool operator<=(ural::optional<T> const & x, T const & a);

    template <class T>
    bool operator<=(T const & a, ural::optional<T> const & x);
}
// namespace ural

namespace std
{
    template <class T>
    class hash<ural::optional<T>>
     : private std::hash<T>
    {
    public:
        typedef typename std::hash<T>::result_type result_type;

        result_type operator()(ural::optional<T> const & x) const;
    };

    template <class T>
    class hash<ural::optional<T &>>
     : private std::hash<T>
    {
    public:
        typedef typename std::hash<T>::result_type result_type;

        result_type operator()(ural::optional<T &> const & x) const;
    };
}
// namespace std

#endif
// Z_URAL_OPTIONAL_HPP_INCLUDED
