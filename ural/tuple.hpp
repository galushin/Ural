#ifndef Z_URAL_TUPLE_HPP_INCLUDED
#define Z_URAL_TUPLE_HPP_INCLUDED

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

/** @file ural/tuple.hpp
 @brief Кортеж --- гетерогенный контейнер фиксированного на этапе компиляции
 размера, а также аналоги стандартных алгоритмов для кортежей.
*/

#include <tuple>

#include <ural/utility.hpp>
#include <ural/placeholders.hpp>

namespace ural
{
inline namespace v0
{
    /** @brief Кортеж
    @tparam Ts типы элементов кортежа
    */
    template <class... Ts>
    class tuple
     : public std::tuple<Ts...>
    {
        typedef std::tuple<Ts...> Base;
    public:
        /** Инициализирует каждый элемент с помощью соответствующих
        конструкторов без аргументов.
        @brief Конструктор без аргументов
        @post <tt> std::get<i>(x) == Ts[i]{} </tt>
        */
        constexpr tuple()
         : Base{}
        {}

        //@{
        /** Инициализирует каждый элемент с помощью конструктора копирования
        на основе соответсвующего аргумента из @c args
        @brief Конструктор
        @param args аргументы
        */
        constexpr tuple(typename std::remove_reference<Ts>::type const &... args)
         : Base(args...)
        {}

        constexpr tuple(typename std::remove_reference<Ts>::type &&... args)
         : Base(std::move(args)...)
        {}

        template <class... Us>
        tuple(Us &&... args)
         : Base(std::forward<Us>(args)...)
        {}
        //@}

        /** @brief Инициализирует элементы кортежа <tt> std::get<i>(x) </tt>
        @param x стандартный кортеж, используемый для инициализации
        */
        template <class... Us>
        constexpr tuple(std::tuple<Us...> const & x)
         : Base(x)
        {}

        /** @brief Инициализирует элементы кортежа
        <tt> std::forward<Ui>(std::get<i>(x)) </tt>
        @param x стандартный кортеж, используемый для инициализации
        */
        template <class... Us>
        constexpr tuple(std::tuple<Us...> && x)
         : Base(std::move(x))
        {}

        //@{
        /** @brief Доступ к элементам по "статическому индексу"
        @tparam Index номер элемента
        @return Ссылка на элемент с номером @c Index
        */
        template <size_t Index>
        constexpr typename std::tuple_element<Index, Base>::type const &
        operator[](ural::placeholder<Index>) const &
        {
            return std::get<Index>(*this);
        }

        template <size_t Index>
        typename std::tuple_element<Index, Base>::type &
        operator[](ural::placeholder<Index>) &
        {
            return std::get<Index>(*this);
        }

        template <size_t Index>
        typename std::tuple_element<Index, Base>::type &&
        operator[](ural::placeholder<Index>) &&
        {
            return std::move(std::get<Index>(*this));
        }
        //@}
    };

    /** @brief Создание кортежа ссылок, пригодных для передачи в функцию
    @param args аргументы
    @return <tt> tuple<Args && ...>(std::forward<Args>(args)...) </tt>
    */
    template <class... Args>
    constexpr tuple<Args && ...>
    forward_as_tuple(Args &&... args) noexcept
    {
        return tuple<Args && ...>(std::forward<Args>(args)...);
    }

    /** @brief Класс функционального объекта для создания кортежей из пачки
    аргументов
    */
    class make_tuple_fn
    {
    private:
        template <class T>
        struct tuple_element_type
         : reference_wrapper_to_reference<typename std::decay<T>::type>
        {};

    public:
        /** @brief Оператор вызова функции
        @param args аргументы
        @return <tt> Tuple(std::forward<Args>(args)...) </tt>, где @c Tuple ---
        это <tt> tuple<typename tuple_element_type<Args>::type...> </tt>
        */
        template <class... Args>
        constexpr
        tuple<typename tuple_element_type<Args>::type...>
        operator()(Args &&... args) const
        {
            typedef tuple<typename tuple_element_type<Args>::type...> Tuple;
            return Tuple(std::forward<Args>(args)...);
        }
    };

namespace
{
    /// @brief Функциональный объект для создания кортежей из пачки аргументов
    constexpr auto const & make_tuple = odr_const<make_tuple_fn>;
}
//namespace

/// @cond false
namespace details
{
    struct tuple_writer
    {
    public:
        template <class OStream, class... Args>
        void operator()(OStream & os, tuple<Args...> const & t) const
        {
            os << "{";
            this->impl(os, t, ural::index_sequence_for<Args...>{});
            os << "}";
        }

    private:
        template <class OStream, class... Args, size_t ... Is>
        void impl(OStream & os, tuple<Args...> const & t,
                  index_sequence<Is...>) const
        {
            using swallow = int[];
            (void)swallow{0, (void(os << (Is == 0? "" : ", ") << std::get<Is>(t)), 0)...};
        }
    };
}
// namespace details
/// @endcond

    /** @brief Вывод кортежа в поток в формате {x1, x2, ... , x3}
    @param os поток вывода
    @param t кортеж
    @return os
    */
    template <class OStream, class... Args>
    OStream operator<<(OStream && os, tuple<Args...> const & t)
    {
        details::tuple_writer{}(os, t);

        return std::forward<OStream>(os);
    }
}
// namespace v0

namespace experimental
{
namespace tuples
{
    /** @brief Тип функционального объекта для применения функции к каждому
    элементу кортежа
    */
    struct for_each_fn
    {
    public:
        /** @brief Применение функционального объекта к каждому из элементов
        котрежа
        @param t кортеж
        @param f функциональный объект
        @return Копия @c f, применённая ко всем элементам @c t
        */
        template <class Tuple, class F>
        F operator()(Tuple && t, F && f) const
        {
            using Raw_tuple = typename std::decay<Tuple>::type;
            constexpr auto const N = std::tuple_size<Raw_tuple>::value;
            return this->impl(std::forward<Tuple>(t), std::forward<F>(f),
                              ural::make_index_sequence<N>{});
        }

    private:
        template <class Tuple, class F, std::size_t... Indices>
        static F impl(Tuple && t, F && f, ural::index_sequence<Indices...>)
        {
            (void)std::initializer_list<int>
            {
                (std::forward<F>(f)(std::get<Indices>(std::forward<Tuple>(t))), 0)...
            };

            return f;
        }
    };

    /** @brief Тип функционального объекта, проверящего, что все элементы
    кортежа удовлетворяют заданному предикату.
    */
    class any_of_fn
    {
    public:
        /** @brief Проверка того, что все элементы кортежа удовлетворяют предикату
        @param x кортеж
        @param pred предикат
        @return @b true, если все элементы кортежа @c x удовлетворяют предикату
        @c pred, иначе --- @b false.
        */
        template <class Tuple, class UnaryPredicate>
        typename std::enable_if<(std::tuple_size<Tuple>::value > 0), bool>::type
        operator()(Tuple const & x, UnaryPredicate pred) const
        {
            return this->impl(x, pred, placeholder<0>{},
                              placeholder<std::tuple_size<Tuple>::value>{});
        }

    private:
        template <class Tuple, class UnaryPredicate, std::size_t First>
        bool impl(Tuple const &, UnaryPredicate,
                  placeholder<First>, placeholder<First>) const
        {
            return false;
        }

        template <class Tuple, class UnaryPredicate, size_t First, size_t Last>
        bool impl(Tuple const & x, UnaryPredicate pred,
                  placeholder<First>, placeholder<Last> stop) const
        {
            return pred(std::get<First>(x))
                   || this->impl(x, std::move(pred),
                                 placeholder<First+1>{}, stop);
        }
    };

namespace
{
    /** @brief Функциональный объект, применяющий функцию к каждому элементу
    кортежа
    */
    constexpr auto const & for_each = ural::odr_const<for_each_fn>;

    /** @brief Функциональный объект, проверящий, что все элементы кортежа
    удовлетворяют заданному предикату.
    */
    constexpr auto const & any_of = ural::odr_const<any_of_fn>;
}
// namespace
}
// namespace tuples
}
// namespace experimental
}
// namespace ural

namespace std
{
    template <class... Ts>
    struct tuple_size<ural::tuple<Ts...>>
     : std::tuple_size<std::tuple<Ts...>>
    {};

    template <size_t I, class... Ts>
    struct tuple_element<I, ural::tuple<Ts...>>
     : std::tuple_element<I, std::tuple<Ts...>>
    {};
}
// namespace std

#endif
// Z_URAL_TUPLE_HPP_INCLUDED
