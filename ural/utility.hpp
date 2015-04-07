#ifndef Z_URAL_UTILITY_HPP_INCLUDED
#define Z_URAL_UTILITY_HPP_INCLUDED

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

/** @file ural/utility.hpp
 @brief Вспомогательные средства
*/

#include <ural/placeholders.hpp>
#include <ural/defs.hpp>
#include <ural/type_traits.hpp>

#include <boost/compressed_pair.hpp>

#include <cassert>
#include <complex>

namespace ural
{
    /** @brief Последовательность целых чисел времени компиляции
    @tparam T тип целых чисел
    @tparam Ints список целых чисел
    */
    template <class T, T... Ints>
    struct integer_sequence
    {
        /// @brief тип целых чисел
        typedef T value_type;

        /** @brief Количество элементов
        @return Количество чисел в последовательности
        */
        static constexpr std::size_t size() noexcept
        {
            return sizeof...(Ints);
        }
    };

    /** @brief Последовательность индексов времени компиляции
    @tparam Ints список целых чисел без знака (типа @c size_t )
    */
    template<std::size_t... Ints>
    using index_sequence = integer_sequence<std::size_t, Ints...>;

    /// @cond false
    template <class T, T first, T last, T... Rs>
    struct make_int_sequence_helper
     : make_int_sequence_helper<T, first+1, last, Rs..., first>
    {};

    template <class T, T first, T... Rs>
    struct make_int_sequence_helper<T, first, first, Rs...>
     : declare_type<integer_sequence<T, Rs...> >
    {};
    /// @endcond

    /** @brief Создание последовательности целых чисел <tt> [0; N) </tt>
    @tparam T тип целых чисел
    @tparam N количество индексов
    */
    template<class T, T N>
    using make_integer_sequence = typename make_int_sequence_helper<T, 0, N>::type;

    /** @brief Создание последовательности индексов <tt> [0; N) </tt>
    @tparam N количество индексов
    */
    template<std::size_t N>
    using make_index_sequence = make_integer_sequence<std::size_t, N>;

    /** @brief Создание последовательности индексов для списка типов
    @tparam Ts типы-аргументы
    */
    template<class... Ts>
    using index_sequence_for = make_index_sequence<sizeof...(Ts)>;

    struct
    {
    private:
        template <typename F, typename Tuple, size_t... I>
        static constexpr auto impl(F&& f, Tuple&& t, index_sequence<I...>)
            -> decltype(std::forward<F>(f)(std::get<I>(std::forward<Tuple>(t))...))
        {
            return std::forward<F>(f)(std::get<I>(std::forward<Tuple>(t))...);
        }

    public:
        /** @brief Вызов функционального объекта с элементами кортежа в качестве
        аргументов
        @param f функциональный объект
        @param t кортеж аргументов
        @return <tt> f(get<0>(t), ..., get<n-1>(t)) </tt>
        */
        template <typename F, typename Tuple>
        constexpr auto operator()(F&& f, Tuple&& t) const
        -> decltype(impl(std::forward<F>(f), std::forward<Tuple>(t),
                         make_index_sequence<std::tuple_size<decay_t<Tuple>>::value>{}))
        {
            using Indices = make_index_sequence<std::tuple_size<decay_t<Tuple>>::value>;
            return impl(std::forward<F>(f), std::forward<Tuple>(t), Indices{});
        }
    }
    constexpr apply {};

    /** @brief Обобщённая реализация присваивания "Скопировать и обменять"
    @param x объект, которому должно быть присвоено значение
    @param value присваиваемое значение
    @return x
    */
    template <class T>
    T & copy_and_swap(T & x, T value)
    {
        // Копирование правой части производится из-за передачи параметра по
        // значению
        value.swap(x);
        return x;
    }

    /** @brief Обёртка, сохраняющая старое значение
    @tparam T тип значения

    Обоснование

    При копировании (и присваивании) старое значение сохраняется. Альтернатива
    этому: у копии текущее значение совпадает с текущим значением оригиналом,
    а старое значении равно текущему.

    Этот вариант был отклонён, так как для данного шаблона, очевидно, старое
    значение является существенным свойством. А при копировании регулярных типов
    существенные свойства должны сохраняться.

    Кроме того, сделать старое значение равным текущему, можно вызовом функции
    @c commit.
    */
    template <class T>
    class with_old_value
    {
    public:
        // Создание и присваивание
        /** @brief Конструктор без параметров
        @post <tt> this->value() == T{} </tt>
        @post <tt> this->old_value() == T{} </tt>
        */
        constexpr with_old_value() = default;

        /** @brief Конструктор на основе значения
        @param value значение
        @post <tt> this->value() == value </tt>
        @post <tt> this->old_value() == value </tt>
        */
        constexpr explicit with_old_value(T const & value)
         : value_{value}
         , old_value_{value}
        {}

        /** @brief Конструктор на основе временного значения
        @param value значение
        @post <tt> this->value() == value_old </tt>, где @c value_old ---
        значение, которое переменаня @c value принимала до начала вызова
        @post <tt> this->old_value() == value_old </tt>, где @c value_old ---
        значение, которое переменаня @c value принимала до начала вызова
        */
        constexpr explicit with_old_value(T && value)
         : value_{value}
         , old_value_{std::move(value)}
        {}

        /** @brief Создаёт значение с помощью конструктора с аргументами @c args
        @param args аргументы конструктора
        @post <tt> this->value() == T{std::forward<Args>(args)...} </tt>
        @post <tt> this->old_value() == this->old_value() </tt>
        */
        template <class... Args>
        with_old_value(in_place_t, Args &&... args)
         : value_(std::forward<Args>(args)...)
         , old_value_(value_)
        {}

        //@{
        /// @brief Конструкор копий
        with_old_value(with_old_value const &) = default;
        with_old_value(with_old_value &&) = default;
        //@}

        //@{
        /// @brief Оператор присваивания копий
        with_old_value & operator=(with_old_value const &) = default;
        with_old_value & operator=(with_old_value &&) = default;
        //@}

        /** @brief Оператор присваивания значения
        @param new_value новое значение
        @post <tt> this->value() == new_value </tt>
        @post Значение <tt> this->old_value() </tt> не изменяется.
        @return <tt> *this </tt>
        */
        with_old_value & operator=(T const & new_value)
        {
            this->value() = new_value;
            return *this;
        }

        /** @brief Оператор присваивания значения с перемещением
        @param x объект, содержимое которого должно быть передано текущему
        значению
        @post Значение <tt> this->old_value() </tt> не изменяется.
        @post <tt> this->value() == x_old </tt>, где @c x_old --- значение,
        которое переменная @c x принимала до вызова этого оператора
        @return <tt> *this </tt>
        */
        with_old_value & operator=(T && x)
        {
            this->value() = std::move(x);
            return *this;
        }

        // Доступ к данным
        ///@{
        /** @brief Текущее значение
        @return Ссылка на текущее значение
        */
        T & value()
        {
            return value_;
        }

        constexpr T const & value() const
        {
            return value_;
        }
        //@}

        /** @brief Исходное значение
        @return Значение <tt> this->value() </tt> сразу после конструктора или
        последнего вызова <tt> this->commit() </tt>.
        */
        constexpr T const & old_value() const
        {
            return old_value_;
        }

        // Подтверждение и откат
        /** @brief Подтверждение изменений
        @post <tt> this->value() </tt> не меняет значения
        @post <tt> this->old_value() == this->value() </tt>
        */
        void commit()
        {
            old_value_ = value_;
        }

        /** @brief Откат к предыдущему значению.
        @post <tt> this->old_value() </tt> не меняет значения
        @post <tt> this->value() == this->old_value() </tt>
        */
        void rollback()
        {
            value_ = old_value_;
        }

    private:
        // Порядок объявления важен
        T value_;
        T old_value_;
    };

    /** @brief Оператор "равно"
    @param x левый операнд
    @param y правый операнд
    */
    template <class T1, class T2>
    bool operator==(with_old_value<T1> const & x, with_old_value<T2> const & y)
    {
        return x.value() == y.value() && x.old_value() == y.old_value();
    }

    /** @brief Функция создания объекта со старым значением
    @param x начальное значение
    @return <tt> with_old_value<TD>{std::forward<T>(x)} </tt>, где @c TD есть
    <tt> typename std::decay<T>::type </tt>
    */
    template <class T>
    constexpr with_old_value<typename std::decay<T>::type>
    make_with_old_value(T && x)
    {
        return with_old_value<typename std::decay<T>::type>{std::forward<T>(x)};
    }

    /** @brief Функция доступа к значению - реализация для обычных переменных
    @param x переменная
    @return <tt> std::forward<T>(x) </tt>
    */
    template <class T>
    T && get(T && x)
    {
        return std::forward<T>(x);
    }

    ///@{
    /** @brief Функция доступа к значению @c with_old_value
    @param x переменная
    @return <tt> x.value() </tt>
    */
    template <class T>
    T & get(with_old_value<T> & x)
    {
        return x.value();
    }

    template <class T>
    T const & get(with_old_value<T> const & x)
    {
        return x.value();
    }
    //@}

    // Универсальный интерфейс для кортежей, pair, complex, compressed_pair
    // Кортежи
    //@{
    /** @brief Доступ к элементу кортежа по статическому индексу
    @param t кортеж
    @tparam Index номер элемента кортежа
    @return <tt> std::get<Index>(t) </tt>
    */
    template <class... Types, size_t Index>
    constexpr typename std::tuple_element<Index, std::tuple<Types...>>::type &
    get(std::tuple<Types...> & t, placeholder<Index>)
    {
        return std::get<Index>(t);
    }

    template <class... Types, size_t Index>
    constexpr typename std::tuple_element<Index, std::tuple<Types...>>::type const &
    get(std::tuple<Types...> const & t, placeholder<Index>)
    {
        return std::get<Index>(t);
    }

    template <class... Types, size_t Index>
    constexpr typename std::tuple_element<Index, std::tuple<Types...>>::type &&
    get(std::tuple<Types...> && t, placeholder<Index>)
    {
        return std::get<Index>(std::move(t));
    }
    //@}

    // std::piar
    /** @brief Первый элемент пары
    @param x пара
    @return <tt> x.first </tt>
    */
    template <class T1, class T2>
    T1 const & get(std::pair<T1, T2> const & x, ural::placeholder<0>)
    {
        return x.first;
    }

    /** @brief Второй элемент пары
    @param x пара
    @return <tt> x.second </tt>
    */
    template <class T1, class T2>
    T2 const & get(std::pair<T1, T2> const & x, ural::placeholder<1>)
    {
        return x.second;
    }

    // std::complex
    //@{
    /** @brief Доступ к действительной части комплексного числа
    @param x комлпексное числа
    @return Ссылка на действительную часть комплексного числа
    */
    template <class T>
    T & get(std::complex<T> & x, ural::placeholder<0>)
    {
        return reinterpret_cast<T(&)[2]>(x)[0];
    }

    template <class T>
    T const & get(std::complex<T> const & x, ural::placeholder<0>)
    {
        return reinterpret_cast<T const(&)[2]>(x)[0];
    }
    //@}

    //@{
    /** @brief Доступ к мнимой части комплексного числа
    @param x комлпексное числа
    @return Ссылка на мнимую часть комплексного числа
    */
    template <class T>
    T & get(std::complex<T> & x, ural::placeholder<1>)
    {
        return reinterpret_cast<T(&)[2]>(x)[1];
    }

    template <class T>
    T const & get(std::complex<T> const & x, ural::placeholder<1>)
    {
        return reinterpret_cast<T const(&)[2]>(x)[1];
    }
    //@}

    // boost::compressed_pair
    /** @brief Первый элемент сжатой пары <tt> boost::compressed_pair </tt>
    @param x пара
    @return <tt> x.first() </tt>
    */
    template <class T1, class T2>
    T1 const & get(boost::compressed_pair<T1, T2> const & x, ural::placeholder<0>)
    {
        return x.first();
    }

    /** @brief Второй элемент сжатой пары <tt> boost::compressed_pair </tt>
    @param x пара
    @return <tt> x.second() </tt>
    */
    template <class T1, class T2>
    T2 const & get(boost::compressed_pair<T1, T2> const & x, ural::placeholder<1>)
    {
        return x.second();
    }

    /** @brief Тип функционального объекта для доступа к элементу кортежа
    (в широком смысле) с заданным на этапе компиляции индексом
    @tparam Index номер элемента кортежа
    */
    template <size_t Index>
    class tuple_get
    {
    public:
        /** @brief Оператор вызова функции
        @param x кортеж
        @return <tt> get(std::forward<Tuple>(x), ural::placeholder<Index>{}); </tt>
        */
        template <class Tuple>
        constexpr auto operator()(Tuple && x) const
        -> decltype(get(std::forward<Tuple>(x), ural::placeholder<Index>{}))
        {
            return get(std::forward<Tuple>(x), ural::placeholder<Index>{});
        }
    };

    /** @brief Функциональный объект, выполняющий обмен распределителей памяти,
    если это необходимо.
    */
    class swap_allocators
    {
    public:
        /** @brief Выполнение обмена
        @param x первый распределитель памяти
        @param y второй распределитель памяти
        */
        template <class A>
        void operator()(A & x, A & y) const
        {
            auto constexpr tag = typename std::allocator_traits<A>::propagate_on_container_swap{};

            return this->do_swap(x, y, tag);
        }

    private:
        template <class A>
        static void do_swap(A & x, A & y, std::true_type)
        {
            using std::swap;
            swap(x, y);
        }

        template <class A>
        static void do_swap(A & x, A & y, std::false_type)
        {
            // всегда возможно: Таблица 26
            assert(x == y);
        }
    };
}
// namespace ural

#endif
// Z_URAL_UTILITY_HPP_INCLUDED
