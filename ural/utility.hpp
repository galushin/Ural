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

#include <ural/defs.hpp>
#include <ural/type_traits.hpp>

#include <boost/compressed_pair.hpp>

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
    @todo Придумать пример, в котором используется откат к предыдущему значению

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
        /** @brief Конструктор
        @param value значение
        @post <tt> this->value() == value </tt>
        @post <tt> this->old_value() == value </tt>
        */
        explicit with_old_value(T value)
         : value_{value}
         , old_value_(std::move(value))
        {}

        // Доступ к данным
        ///@{
        /** @brief Текущее значение
        @return Ссылка на текущее значение
        */
        T & value()
        {
            return value_;
        }

        T const & value() const
        {
            return value_;
        }
        //@}

        /** @brief Исходное значение
        @return Значение <tt> this->value() </tt> сразу после конструктора или
        последнего вызова <tt> this->commit() </tt>.
        */
        T const & old_value() const
        {
            return old_value_;
        }

        // Подтверждение и откат
        /** @brief Подтверждение изменений
        @post <tt> this->old_value() == this->value() </tt>
        */
        void commit()
        {
            old_value_ = value_;
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
}
// namespace ural

#endif
// Z_URAL_UTILITY_HPP_INCLUDED
